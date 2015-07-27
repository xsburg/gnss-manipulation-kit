#pragma once

#include <QtCore/QtCore>
#include "Common/Singleton.h"
#include "Common/SmartPtr.h"
#include "Common/Exception.h"
#include "Common/Connection.h"
#include <Greis/Epoch.h>
#include <Greis/DataChunk.h>

using namespace Common;

namespace Platform
{
    namespace Tests
    {
        class Helpers
        {
            SINGLETON_BLOCK(Helpers)
        public:
            Helpers();
            ~Helpers();

            void assertBinaryArray(const QByteArray& expected, const QByteArray& actual);
            void assertMessages(const std::vector<Greis::Message::UniquePtr_t>& expectedMessages, const std::vector<Greis::Message::UniquePtr_t>& actualMessages);
            void assertEpoch(Greis::Epoch* expected, Greis::Epoch* actual);
            void assertDataChunk(Greis::DataChunk* expected, Greis::DataChunk* actual, bool assertHead);
            void saveToFile(const QString& fileName, Greis::DataChunk* data);
        };
    }
}

#define sHelpers SINGLETON_INSTANCE(::Platform::Tests::Helpers)