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
            _sink = make_unique<Greis::MySqlSink>(_connection, _inserterBatchSize);
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

        for (auto& epoch : dataChunk->Body())
        {
            _sink->AddEpoch(epoch.get());
            // Here we flush data but perform sync-wait for the last flush
            if (_sink->NeedsFlush())
            {
                _lastFlush.waitForFinished();
                _lastFlush = _sink->FlushAsync();
            }
        }
        if (_autoCommit)
        {
            _connection->Database().commit();
        }
    }
    catch (Exception& e)
    {
        if (_autoCommit)
        {
            _connection->Database().rollback();
            _sink->Clear();
            sLogger.Error("Transaction has been rolled back. All pending data was lost (but it's still consistent!)");
        }
        else
        {
            sLogger.Error("Failed to handle data (transaction is disabled so we do nothing).");
        }
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
