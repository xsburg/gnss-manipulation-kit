#pragma once

#include <gtest/gtest.h>
#include <gpstk/Rinex3ObsStream.hpp>
#include <gpstk/Rinex3ObsBase.hpp>
#include <gpstk/Rinex3ObsHeader.hpp>
#include <gpstk/Rinex3ObsData.hpp>
#include "Utils/BaseTest.h"
#include "Utils/Helpers.h"
#include "Common/SmartPtr.h"
#include "Greis/GreisMessageStream.h"
#include "Greis/FileBinaryStream.h"
#include "Greis/DataChunk.h"
#include "Greis/MySqlSink.h"
#include "Greis/MySqlSource.h"

using namespace Common;
using namespace gpstk;

namespace Greis
{
    namespace Tests
    {
        class RinexTests : public BaseTest
        {
        };

        TEST_F(RinexTests, ShouldOpenTestFile)
        {
            Rinex3ObsStream rin("bahr1620.04o");

            // Create the output file stream
            Rinex3ObsStream rout("bahr1620.04o.new", std::ios::out | std::ios::trunc);

            // Read the RINEX header
            Rinex3ObsHeader head;    //RINEX header object
            rin >> head;
            rout.header = rin.header;
            rout << rout.header;

            // Loop over all data epochs
            Rinex3ObsData data;   //RINEX data object
            while (rin >> data)
            {
                rout << data;
            }

            // Arrange
            //QString fileName = this->ResolvePath("ifz-data-0.jps");
            //auto expectedChunk = DataChunk::FromFile(fileName);

            //// Act
            //{
            //    // Saving to the database
            //    auto sink = make_unique<MySqlSink>(this->Connection().get(), 1000);
            //    sink->AddJpsFile(expectedChunk.get());
            //    sink->Flush();
            //}
            //DataChunk::UniquePtr_t actualChunk;
            //{
            //    // Reading from the database
            //    auto source = make_unique<MySqlSource>(this->Connection().get());
            //    actualChunk = source->ReadAll();
            //}

            //// Assert
            //const int count = 2;
            //int index = 0;
            //std::vector<StdMessage*> headerMessages;
            //for (auto& msg : actualChunk->Head())
            //{
            //    if (msg->Kind() == EMessageKind::StdMessage)
            //    {
            //        headerMessages.push_back(static_cast<StdMessage*>(msg.get()));
            //        index++;
            //        if (index >= count)
            //        {
            //            break;
            //        }
            //    }
            //}
            //ASSERT_EQ(headerMessages[0]->IdNumber(), EMessageId::FileId);
            //ASSERT_EQ(headerMessages[0]->BodySize(), 85);
            //ASSERT_EQ(headerMessages[1]->IdNumber(), EMessageId::MsgFmt);
            //ASSERT_EQ(headerMessages[1]->BodySize(), 9);
        }
    }
}
