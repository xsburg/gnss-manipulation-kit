#include "ChainedSink.h"

Platform::ChainedSink::ChainedSink(Connection::SharedPtr_t connection, int inserterBatchSize, ChainedSink::UniquePtr_t nextSink, bool autoCommit)
    : _nextChainedSink(std::move(nextSink)), _connection(connection), _inserterBatchSize(inserterBatchSize), _autoCommit(autoCommit)
{
    Connect();
}

bool Platform::ChainedSink::Connect()
{
    try
    {
        if (!_connection->Database().isOpen())
        {
            _connection->Connect();
        }
        if (!_sink.get())
        {
            _sink = make_unique<Greis::MySqlSink>(_connection.get(), _inserterBatchSize);
        }

        _isValid = true;
        return true;
    }
    catch (DatabaseException& ex)
    {
        sLogger.Error("An error occurred while connecting to a database: " + ex.what());
        return _isValid = false;
    }
    catch (...)
    {
        _isValid = false;
        throw;
    }
}

bool Platform::ChainedSink::IsValid() const
{
    return _isValid && _connection->Database().isOpen();
}

bool Platform::ChainedSink::Handle(Greis::DataChunk::UniquePtr_t dataChunk)
{
    try
    {
        if (_autoCommit)
        {
            _connection->Database().transaction();
        }

        for (auto it = dataChunk->Body().begin();
             it != dataChunk->Body().end();
             ++it)
        {
            _sink->AddEpoch((*it)->DateTime);
            for (auto msgIt = (*it)->Messages.begin();
                 msgIt != (*it)->Messages.end();
                 ++msgIt)
            {
                _sink->AddMessage(msgIt->get());
            }
        }
        _sink->Flush(); //Added for testing purposes 19032013 Keir
        if (_autoCommit)
        {
            _connection->Database().commit();
        }
    }
    catch (Exception& e)
    {
        _connection->Database().rollback();
        sLogger.Error("Transaction has been rolled back.");
        sLogger.Error(e.what());
        return false;
    }

    if (_nextChainedSink.get())
    {
        try
        {
            if (_nextChainedSink->Connect())
            {
                _nextChainedSink->Handle(std::move(dataChunk));
            }
        }
        catch (DatabaseException& ex)
        {
            sLogger.Error("An error occurred while adding a data chunk into the data center database: " + ex.what());
        }
    }

    return true;
}

void Platform::ChainedSink::Flush()
{
    if (_autoCommit)
    {
        _connection->Database().transaction();
    }

    _sink->Flush();
    if (_nextChainedSink.get())
    {
        try
        {
            if (_nextChainedSink->Connect())
            {
                _nextChainedSink->Flush();
            }
        }
        catch (DatabaseException& ex)
        {
            sLogger.Error("An error occurred while flushing the data into the data center database: " + ex.what());
        }
    }

    if (_autoCommit)
    {
        _connection->Database().commit();
    }
}
