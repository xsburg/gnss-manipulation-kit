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

namespace Greis
{
    namespace Tests
    {
        class StatisticsTests : public BaseTest
        {
        };

        TEST_F(StatisticsTests, ShouldNotFail)
        {
            // Arrange
            QString filename = this->ResolvePath("ifz-data-0.jps");
            auto dataChunk = DataChunk::FromFile(filename);

            // Act
            auto serviceManager = make_unique<Platform::ServiceManager>(this->Connection());
            for (auto& epoch : dataChunk->Body())
            {
                for (auto& msg : epoch->Messages)
                {
                    serviceManager->HandleMessage(msg.get());
                }
            }
            serviceManager->PushMessageStats();

            // Assert
            // If we got here without exceptions or seg-faults, the test is passed
        }
    }
}
