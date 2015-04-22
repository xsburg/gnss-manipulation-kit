#include "ChainedSink.h"

Platform::ChainedSink::ChainedSink(Connection::SharedPtr_t connection, int inserterBatchSize, ChainedSink::UniquePtr_t nextSink)
    : _nextChainedSink(std::move(nextSink)), _connection(connection), _inserterBatchSize(inserterBatchSize)
{
    Connect();
}

bool Platform::ChainedSink::Connect()
{
    try
    {
        if (IsValid())
        {
            return _isValid = true;
        }

        _connection->Connect();
        _sink = make_unique<Greis::MySqlSink>(_connection.get(), _inserterBatchSize);
        return _isValid = true;
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
        _connection->Database().transaction();

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
        _connection->Database().commit();
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
    _connection->Database().transaction();

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

    _connection->Database().commit();
}
