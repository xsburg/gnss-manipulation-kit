#pragma once

#include <vector>
#include <cmath>
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include "Utils/BaseTest.h"
#include "Common/SmartPtr.h"
#include "Greis/DataChunk.h"
#include <Platform/ServiceManager.h>

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

        TEST_F(MissingDataTests, Sample1)
        {
            // Arrange
            QString databaseDataFileName = this->ResolvePath("samples/1/databaseData.jps");
            QString rawDataFileName = this->ResolvePath("samples/1/rawData.jps");

            auto databaseDataChunk = Greis::DataChunk::FromFile(databaseDataFileName);
            auto rawDataChunk = Greis::DataChunk::FromFile(rawDataFileName, true);

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


            // Act

            // Assert
        }
    }
}
