
#include <gtest/gtest.h>
#include "Utils/BaseTest.h"
#include <Greis/AllStdMessages.h>
#include <Common/Path.h>
#include <QtCore/QtCore>
#include <Common/File.h>
#include <rtklib.h>

using namespace Common;

namespace Greis
{
    namespace Tests
    {
        class RinexTests : public BaseTest
        {
        };
      
               TEST_F(RinexTests, ShouldReadJavadFile)
        {
            // Arrange
            QString fileName = this->ResolvePath("ifz-data-0.jps");

            //FILE* fr = fopen(fileName.toLatin1(), "r");
            auto file = File::OpenReadBinary(fileName);
            auto data = file->readAll();

            raw_t* raw = new raw_t();
            init_raw(raw);
            // Assert
            // If we got here without exceptions or seg-faults, the test is passed
        }

    }
}



