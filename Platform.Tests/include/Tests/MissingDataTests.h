#pragma once

#include <vector>
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include "Utils/BaseTest.h"
#include "Utils/Helpers.h"
#include "Common/SmartPtr.h"
#include "Greis/DataChunk.h"
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
            ASSERT_EQ(databaseSize, 11);
            ASSERT_EQ(rawSize, 16);
        }

        TEST_F(MissingDataTests, Sample1jpslogdEmu)
        {
            // Arrange
            Greis::DataChunk::UniquePtr_t rawDataChunk;
            {
                QString rawDataFileName = this->ResolvePath("samples/1/rawData.jps");
                rawDataChunk = Greis::DataChunk::FromFile(rawDataFileName, true);
                Greis::IBinaryStream::SharedPtr_t deviceBinaryStream = std::make_shared<Greis::FileBinaryStream>(rawDataFileName);
                auto dataChunk = make_unique<Greis::DataChunk>();
                ChainedSink::UniquePtr_t localSink = ChainedSink::UniquePtr_t(new ChainedSink(this->Connection(), 25, ChainedSink::UniquePtr_t(nullptr), false));
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
                        auto dataChunk2 = make_unique<Greis::DataChunk>();
                        for (auto& msg : dataChunk->UnfinishedEpoch()->Messages)
                        {
                            dataChunk2->AddMessage(std::move(msg));
                        }
                        localSink->Handle(std::move(dataChunk));
                        dataChunk = std::move(dataChunk2);
                        msgCounter = 0;
                    }
                }
                localSink->Handle(std::move(dataChunk));
                localSink->Flush();
            }

            // Act
            auto dateStart = QDateTime(QDate(2015, 7, 23), QTime(21, 03, 30), Qt::UTC);
            auto dateEnd = QDateTime(QDate(2015, 7, 23), QTime(21, 11, 0), Qt::UTC);
            auto source = std::make_shared<Greis::MySqlSource>(this->Connection().get());
            auto actualChunk = source->ReadRange(dateStart, dateEnd);

            // Assert
            int expectedSize = 16;
            auto actualSize = actualChunk->Body().size();
            ASSERT_EQ(actualSize, expectedSize);
            int rawStartIndex = findEpochIndex(rawDataChunk.get(), dateStart);
            int rawEndIndex = findEpochIndex(rawDataChunk.get(), dateEnd);
            for (int i = rawStartIndex; i <= rawEndIndex; i++)
            {
                auto& expectedEpoch = rawDataChunk->Body()[i];
                auto &actualEpoch = actualChunk->Body()[i - rawStartIndex];
                sHelpers.assertEpoch(expectedEpoch.get(), actualEpoch.get());
            }
        }
    }
}
