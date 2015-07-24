#pragma once

#include <vector>
#include <cmath>
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include "Utils/BaseTest.h"
#include "Common/SmartPtr.h"
#include "Greis/DataChunk.h"
#include <Platform/ServiceManager.h>
#include <Platform/ChainedSink.h>
#include <Greis/FileBinaryStream.h>
#include <Greis/MySqlSource.h>

using namespace Common;

namespace Platform
{
    namespace Tests
    {
        class MissingDataTests : public BaseTest
        {
        };

        int findEpochIndex(Greis::DataChunk* dataChunk, QDateTime dateTime)
        {
            for (int i = 0; i < dataChunk->Body().size(); i++)
            {
                auto dt = dataChunk->Body()[i]->DateTime;
                if (dt == dateTime)
                {
                    return i;
                }
            }
        }

        TEST_F(MissingDataTests, Sample1Failed)
        {
            // Arrange
            QString databaseDataFileName = this->ResolvePath("samples/1/databaseData.jps");
            QString rawDataFileName = this->ResolvePath("samples/1/rawData.jps");

            // Act
            auto databaseDataChunk = Greis::DataChunk::FromFile(databaseDataFileName);
            auto rawDataChunk = Greis::DataChunk::FromFile(rawDataFileName, true);

            // Assert
            int databaseStartIndex = databaseDataChunk->Body().size() - 20;
            int databaseEndIndex = databaseDataChunk->Body().size() - 10;
            QDateTime databaseStart = databaseDataChunk->Body()[databaseStartIndex]->DateTime;
            QDateTime databaseEnd = databaseDataChunk->Body()[databaseEndIndex]->DateTime;
            auto databaseStartString = databaseStart.toString(Qt::ISODate);
            auto databaseEndString = databaseEnd.toString(Qt::ISODate);
            auto databaseSize = databaseEndIndex - databaseStartIndex + 1;

            int rawStartIndex = findEpochIndex(rawDataChunk.get(), databaseStart);
            int rawEndIndex = findEpochIndex(rawDataChunk.get(), databaseEnd);
            auto rawStart = rawDataChunk->Body()[rawStartIndex]->DateTime;
            auto rawEnd = rawDataChunk->Body()[rawEndIndex]->DateTime;
            auto rawSize = rawEndIndex - rawStartIndex + 1;
            auto rawStartString = rawStart.toString(Qt::ISODate);
            auto rawEndString = rawEnd.toString(Qt::ISODate);

            ASSERT_NE(databaseSize, rawSize);
        }

        TEST_F(MissingDataTests, Sample1jpslogdEmu)
        {
            // Arrange
            {
                auto source = std::make_shared<Greis::MySqlSource>(this->Connection().get());
                ASSERT_EQ(source->ReadAll()->Body().size(), 0);
            }
            {
                QString rawDataFileName = this->ResolvePath("samples/1/rawData.jps");
                Greis::IBinaryStream::SharedPtr_t deviceBinaryStream = std::make_shared<Greis::FileBinaryStream>(rawDataFileName);
                auto dataChunk = make_unique<Greis::DataChunk>();
                ChainedSink::UniquePtr_t localSink = make_unique<ChainedSink>(this->Connection(), 25, nullptr, false);
                int dataChunkSize = 250;
                ASSERT_TRUE(localSink->IsValid());
                Greis::GreisMessageStream::SharedPtr_t messageStream = std::make_shared<Greis::GreisMessageStream>(deviceBinaryStream, true, false);

                int msgCounter = 0;
                Greis::Message::UniquePtr_t msg;
                while ((msg = messageStream->Next()).get())
                {
                    dataChunk->AddMessage(std::move(msg));
                    if (msgCounter++ > dataChunkSize)
                    {
                        localSink->Handle(std::move(dataChunk));
                        dataChunk = make_unique<Greis::DataChunk>();
                        msgCounter = 0;
                    }
                }
                localSink->Flush();
            }

            // Act
            auto source = std::make_shared<Greis::MySqlSource>(this->Connection().get());
            auto actualChunk = source->ReadRange(QDateTime(QDate(2015, 7, 23), QTime(21, 03, 30), Qt::UTC), QDateTime(QDate(2015, 7, 23), QTime(21, 11, 0), Qt::UTC));

            // Assert
            int expectedRawSize = 16;
            int expectedFailedDbSize = 11;
            auto actualSize = actualChunk->Body().size();
            ASSERT_EQ(actualSize, expectedFailedDbSize);
        }
    }
}
