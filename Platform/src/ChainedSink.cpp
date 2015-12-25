#include "ChainedSink.h"

Platform::ChainedSink::ChainedSink(
    ConnectionPool::SharedPtr_t connectionPool, int inserterBatchSize)
    : _connectionPool(connectionPool), _inserterBatchSize(inserterBatchSize)
{
    Connect();
}

bool Platform::ChainedSink::Connect()
{
    try
    {
        if (!_sink.get())
        {
            _sink = make_unique<Greis::MySqlSink>(_connectionPool, _inserterBatchSize);
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

bool Platform::ChainedSink::Handle(Greis::DataChunk::UniquePtr_t dataChunk)
{
    try
    {
        _sink->AddDataChunk(dataChunk.get());
        _sink->Flush();
    }
    catch (Exception& e)
    {
        sLogger.Error("Failed to handle data (transaction is disabled so we do nothing).");
        sLogger.Error(e.what());
        return false;
    }
    return true;
}

void Platform::ChainedSink::Flush()
{
    _sink->Flush();
}
