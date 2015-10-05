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
        std::unique_ptr<ChainedSink> _nextChainedSink;
        Connection::SharedPtr_t _connection;
        int _inserterBatchSize;
        bool _isValid;
        bool _autoCommit;
    public:
        SMART_PTR_T(ChainedSink);

        ChainedSink(Connection::SharedPtr_t connection, int inserterBatchSize, ChainedSink::UniquePtr_t nextSink, bool autoCommit);

        bool Connect();

        bool IsValid() const;

        bool Handle(Greis::DataChunk::UniquePtr_t dataChunk);

        void Flush();
    };
}
