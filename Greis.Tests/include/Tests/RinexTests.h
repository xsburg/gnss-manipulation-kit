#pragma once

#include <gtest/gtest.h>
#include "Utils/BaseTest.h"
#include "Greis/GreisMessageStream.h"
#include <rtklib.h>
#include <Greis/RinexReader.h>

using namespace Common;

namespace Greis
{
    namespace Tests
    {
        class RinexTests : public BaseTest
        {
        };

        TEST_F(RinexTests, ShouldOpenTestFile)
        {
            /*RinexObsStream rin("ifz-data-0.14o");

            // Create the output file stream
            RinexObsStream rout("bahr1620.04o.new", std::ios::out | std::ios::trunc);

            // Read the RINEX header
            RinexObsHeader head;    //RINEX header object
            rin >> head;
            rout.header = rin.header;
            rout << rout.header;

            // Loop over all data epochs
            RinexObsData data;   //RINEX data object
            while (rin >> data)
            {
                rout << data;
            }*/

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

        TEST_F(RinexTests, ShouldReadJavadFile)
        {
            // Arrange
            QString fileName = this->ResolvePath("ifz-data-0.jps");

            //FILE* fr = fopen(fileName.toLatin1(), "r");
            auto file = File::OpenReadBinary(fileName);
            auto data = file->readAll();

            raw_t* raw = new raw_t();
            init_raw(raw);
            int msgCount = 0;
            for (size_t i = 0; i < data.size(); i++)
            {
                char c = data.at(i);
                input_javad(raw, c);
            }

            rnxopt_t opt = { { 0 } };
            opt.rnxver = 2.12;
            opt.navsys = SYS_ALL;
            strcpy(opt.prog, "JPS2RIN v.2.0.99");
            strcpy(opt.runby, "JAVAD GNSS");
            strcpy(opt.marker, "filename"); // marker name
            strcpy(opt.name[0], "-Unknown-"); // observer
            strcpy(opt.name[1], "-Unknown-"); // agency
            strcpy(opt.rec[0], "00L8XRYFXCC213FJTMVBJAVAD"); // REC #
            strcpy(opt.rec[1], "TR_G3T ALPHA"); // TYPE
            strcpy(opt.rec[2], "3.4.14 Jan,29,2014"); // VERS
            strcpy(opt.ant[0], "-Unknown-"); // ANT #
            strcpy(opt.ant[1], "-Unknown-"); // TYPE
            opt.apppos[0] = opt.apppos[1] = opt.apppos[2] = 0.0; // approx position x/y/z
            opt.antdel[0] = opt.antdel[1] = opt.antdel[2] = 0.0; // antenna delta h/e/n
            // observation types
            opt.freqtype = FREQTYPE_ALL;
            opt.obstype = OBSTYPE_ALL;
            set_obstype(STRFMT_JAVAD, &opt);

            opt.tstart = raw->obs.data[0].time;
            opt.tend = raw->obs.data[raw->obs.n - 1].time;
            opt.tint = 1.0; // INTERVAL

            /*char* ofiles[7] = {
            "",
            "",
            "",
            "",
            "",
            "",
            ""
            };
            opt.scanobs = 1;
            auto file0 = (fileName + ".obs").toLatin1();
            ofiles[0] = file0.data();
            auto file1 = (fileName + ".nav").toLatin1();
            ofiles[1] = file1.data();
            auto file2 = (fileName + ".gnav").toLatin1();
            ofiles[2] = file2.data();
            auto file3 = (fileName + ".hnav").toLatin1();
            ofiles[3] = file3.data();
            auto file4 = (fileName + ".qnav").toLatin1();
            ofiles[4] = file4.data();
            auto file5 = (fileName + ".lnav").toLatin1();
            ofiles[5] = file5.data();
            convrnx(STRFMT_JAVAD, &opt, fileName.toLatin1(), ofiles);*/
            FILE* fr = fopen((fileName + ".obs").toLatin1(), "w");
            outrnxobsh(fr, &opt, &raw->nav);
            outrnxobsb(fr, &opt, raw->obs.data, raw->obs.n, 0);
            fclose(fr);

            rnxctr_t* rnxctr = new rnxctr_t();
            init_rnxctr(rnxctr);
            readrnx("filename", 1, "", &rnxctr->obs, &rnxctr->nav, &rnxctr->sta);

            std::cout << "messages read: " << msgCount << std::endl;

            //auto dataChunk = DataChunk::FromFile(filename);

            // Act
            /*int count = 0;
            bool stop;
            auto serviceManager = make_unique<Platform::ServiceManager>(this->Connection());
            for (auto& epoch : dataChunk->Body())
            {
            for (auto& msg : epoch->Messages)
            {
            ++count;
            serviceManager->HandleMessage(msg.get());
            stop = count == 1000;
            if (stop)
            {
            break;
            }
            }
            if (stop)
            {
            break;
            }
            }
            serviceManager->PushMessageStats();*/

            // Assert
            // If we got here without exceptions or seg-faults, the test is passed
        }

        TEST_F(RinexTests, ShouldReadAndWriteRinexData)
        {
            // Arrange
            QString obsFileName = this->ResolvePath("ifz-data-0.14o");
            QString navFileName = this->ResolvePath("ifz-data-0.14N");

            // Act
            auto gnssData = RinexReader().ReadFile(obsFileName).ReadFile(navFileName).BuildResult();


        }
    }
}



