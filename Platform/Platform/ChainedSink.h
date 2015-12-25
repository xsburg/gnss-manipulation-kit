#pragma once

#include "Common/Connection.h"
#include "Common/SmartPtr.h"
#include "Greis/MySqlSink.h"
#include "Greis/DataChunk.h"

using namespace Common;

namespace Platform
{
    class ChainedSink
    {
        Greis::MySqlSink::UniquePtr_t _sink;
        ConnectionPool::SharedPtr_t _connectionPool;
        int _inserterBatchSize;
        bool _isValid;
        QFuture<void> _lastFlush;
    public:
        SMART_PTR_T(ChainedSink);

        ChainedSink(ConnectionPool::SharedPtr_t connectionPool, int inserterBatchSize);

        bool Connect();

        bool Handle(Greis::DataChunk::UniquePtr_t dataChunk);

        void Flush();
    };
}
