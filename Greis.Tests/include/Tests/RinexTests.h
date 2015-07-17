#pragma once

#include <gtest/gtest.h>
#include "Utils/BaseTest.h"
#include <Greis/RinexReader.h>
#include <Greis/AllStdMessages.h>

using namespace Common;

namespace Greis
{
    namespace Tests
    {
        class RinexTests : public BaseTest
        {
        };

        /*TEST_F(RinexTests, ShouldOpenTestFile)
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
            }#1#

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
        }*/

        /*TEST_F(RinexTests, ShouldReadJavadFile)
        {
            // Arrange
            /*QString fileName = this->ResolvePath("ifz-data-0.jps");

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
            opt.tint = 1.0; // INTERVAL#1#

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
            convrnx(STRFMT_JAVAD, &opt, fileName.toLatin1(), ofiles);#1#
        /*FILE* fr = fopen((fileName + ".obs").toLatin1(), "w");
        outrnxobsh(fr, &opt, &raw->nav);
        outrnxobsb(fr, &opt, raw->obs.data, raw->obs.n, 0);
        fclose(fr);

        rnxctr_t* rnxctr = new rnxctr_t();
        init_rnxctr(rnxctr);
        readrnx("filename", 1, "", &rnxctr->obs, &rnxctr->nav, &rnxctr->sta);

        std::cout << "messages read: " << msgCount << std::endl;#1#

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
            serviceManager->PushMessageStats();#1#

            // Assert
            // If we got here without exceptions or seg-faults, the test is passed
        }*/

        TEST_F(RinexTests, ShouldReadAndWriteRinexData)
        {
            // Arrange
            QString obsFileNameIn = this->ResolvePath("ifz-data-0.in.14o");
            QString navFileNameIn = this->ResolvePath("ifz-data-0.in.14N");
            QString obsFileNameOut = this->ResolvePath("ifz-data-0.out.14o");
            QString navFileNameOut = this->ResolvePath("ifz-data-0.out.14N");

            // Act
            auto gnssData = RinexReader().ReadFile(obsFileNameIn).ReadFile(navFileNameIn).BuildResult();
            RinexWriter(gnssData).WriteObsFile(obsFileNameOut).WriteNavFile(navFileNameOut);

            // Assert
            // nothing that we can check...
        }

        TEST_F(RinexTests, ShouldExportGreisToRtkRaw)
        {
            // Arrange
            QString fileName = this->ResolvePath("ifz-data-0.jps");
            auto dataChunk = DataChunk::FromFile(fileName);

            // Act
            auto gnssData = RtkAdapter().toGnssData(dataChunk.get());

            // Assert
            // nothing that we can check...
        }

        TEST_F(RinexTests, ShouldImportRtkRawToGreis)
        {
            // Arrange
            QString obsFileNameIn = this->ResolvePath("ifz-data-0.in.14o");
            QString navFileNameIn = this->ResolvePath("ifz-data-0.in.14N");
            auto gnssData = RinexReader().ReadFile(obsFileNameIn).ReadFile(navFileNameIn).BuildResult();

            // Act
            auto dataChunk = RtkAdapter().toMessages(gnssData);

            // Assert
            // nothing that we can check...
        }

        template<typename TMessage>
        TMessage* findMessage(DataChunk* dataChunk, std::string code, int epochIndex = 0)
        {
            TMessage* found = nullptr;
            for (auto& msg : dataChunk->Body()[epochIndex]->Messages)
            {
                if (msg->Kind() != EMessageKind::StdMessage)
                {
                    continue;
                }
                if (static_cast<StdMessage*>(msg.get())->Id() == code)
                {
                    if (found != nullptr)
                    {
                        throw new Exception("Duplicated message.");
                    }
                    found = dynamic_cast<TMessage*>(msg.get());
                }
            }
            return found;
        }

        bool check_xE_Ex(CNR4StdMessage* expected, CNRStdMessage* actual, const QList<int>& omitCheckIndexes = QList<int>())
        {
            if (expected->CnrX4().size() != actual->Cnr().size())
            {
                return false;
            }
            for (int i = 0; i < expected->CnrX4().size(); i++)
            {
                Types::u1 e = expected->CnrX4()[i];
                Types::u1 a = actual->Cnr()[i];
                bool equalData = e / 4 == a || (e == 0xFF && e == a);
                if (!equalData && !omitCheckIndexes.contains(i))
                {
                    return false;
                }
            }
            return true;
        }

        bool check_rx_Rx(GnssData* gnssData, SPRStdMessage* expected, PRStdMessage* actual, const QList<int>& omitCheckIndexes = QList<int>())
        {
            if (expected->Spr().size() != actual->Pr().size())
            {
                return false;
            }
            for (int i = 0; i < expected->Spr().size(); i++)
            {
                Types::i4 e = expected->Spr()[i];
                Types::f8 a = actual->Pr()[i];

                obsd_t* data = &gnssData->getObs().data[i];
                int sys;
                if (!(sys = satsys(data->sat, NULL)))
                {
                    continue;
                }
                double prm_e;
                if (sys == SYS_SBS)
                {
                    prm_e = (e*1E-11 + 0.115)*CLIGHT;
                }
                else if (sys == SYS_QZS)
                {
                    prm_e = (e*2E-11 + 0.125)*CLIGHT;
                }
                else if (sys == SYS_CMP)
                {
                    prm_e = (e*2E-11 + 0.105)*CLIGHT;
                }
                else if (sys == SYS_GAL)
                {
                    prm_e = (e*1E-11 + 0.090)*CLIGHT;
                }
                else
                {
                    prm_e = (e*1E-11 + 0.075)*CLIGHT;
                }

                double prm_a = a * CLIGHT;

                bool equalData = prm_e == prm_a;
                if (!equalData && !omitCheckIndexes.contains(i))
                {
                    return false;
                }
            }
            return true;
        }

        bool check_xr_Rx(const std::vector<Types::f8>& prCA, GnssData* gnssData, SRPRStdMessage* expected, PRStdMessage* actual, const QList<int>& omitCheckIndexes = QList<int>())
        {
            if (expected->Srpr().size() != actual->Pr().size())
            {
                return false;
            }
            for (int i = 0; i < expected->Srpr().size(); i++)
            {
                Types::i2 e = expected->Srpr()[i];
                Types::f8 a = actual->Pr()[i];

                if (e == 0x7FFF)
                {
                    continue;
                }

                obsd_t* data = &gnssData->getObs().data[i];
                int sys;
                if (!(sys = satsys(data->sat, NULL)) || prCA[i] == 0.0)
                {
                    continue;
                }

                double prm_e = (e*1E-11 + 2E-7)*CLIGHT + prCA[i] * CLIGHT;
                double prm_a = a * CLIGHT;

                bool equalData = abs(prm_e - prm_a) < 5e-009;
                if (!equalData && !omitCheckIndexes.contains(i))
                {
                    return false;
                }
            }
            return true;
        }

        bool check_xp_Px(char code, const std::vector<Types::f8>& prCA, GnssData* gnssData, RCPRc1StdMessage* expected, CPStdMessage* actual, const QList<int>& omitCheckIndexes = QList<int>())
        {
            if (expected->Rcp().size() != actual->Cp().size())
            {
                return false;
            }
            for (int i = 0; i < expected->Rcp().size(); i++)
            {
                Types::i4 e = expected->Rcp()[i];
                Types::f8 a = actual->Cp()[i];

                if (e == 0x7FFFFFFF)
                {
                    continue;
                }

                obsd_t* data = &gnssData->getObs().data[i];
                int sys;
                if (!(sys = satsys(data->sat, NULL)) || prCA[i] == 0.0)
                {
                    continue;
                }
                int freq;
                int type;
                if ((freq = tofreq(code, sys, &type)) < 0)
                {
                    continue;
                }

                if (sys == SYS_GLO)
                {
                    // we cant check it here now due to lack of freqn data. computation of freqn is too complicated for tests
                    // fn = sys == SYS_GLO ? freq_glo(freq, raw->freqn[i]) : CLIGHT / lam_carr[freq];
                    // cp = (rcp*P2_40 + prCA[i])*fn;
                    continue;
                }

                double fn = CLIGHT / lam_carr[freq];
                double cp_e = (e*P2_40 + prCA[i])*fn;

                double cp_a = a;

                bool equalData = cp_e == cp_a;
                if (!equalData && !omitCheckIndexes.contains(i))
                {
                    return false;
                }
            }
            return true;
        }

        bool check_Dx_Dx(DPStdMessage* expected, DPStdMessage* actual, const QList<int>& omitCheckIndexes = QList<int>())
        {
            if (expected->Dp().size() != actual->Dp().size())
            {
                return false;
            }
            for (int i = 0; i < expected->Dp().size(); i++)
            {
                Types::i4 e = expected->Dp()[i];
                Types::i4 a = actual->Dp()[i];

                double dop_e = -e * 1E-4;
                double dop_a = -a * 1E-4;

                bool equalData = abs(dop_e - dop_a) <= 3E-4;
                if (!equalData && !omitCheckIndexes.contains(i))
                {
                    return false;
                }
            }
            return true;
        }

        bool check_xd_Dx(char code, const std::vector<Types::i4>& dpCA, GnssData* gnssData, SRDPStdMessage* expected, DPStdMessage* actual, const QList<int>& omitCheckIndexes = QList<int>())
        {
            if (expected->Srdp().size() != actual->Dp().size())
            {
                return false;
            }
            for (int i = 0; i < expected->Srdp().size(); i++)
            {
                Types::i2 e = expected->Srdp()[i];
                Types::i4 a = actual->Dp()[i];

                if (e == 0x7FFF)
                {
                    continue;
                }

                obsd_t* data = &gnssData->getObs().data[i];
                int sys;
                if (!(sys = satsys(data->sat, NULL)) || dpCA[i] == 0.0)
                {
                    continue;
                }
                int freq;
                int type;
                if ((freq = tofreq(code, sys, &type)) < 0)
                {
                    continue;
                }

                if (sys == SYS_GLO)
                {
                    // we cant check it here now due to lack of freqn data. computation of freqn is too complicated for tests
                    continue;
                }

                double f1 = CLIGHT / lam_carr[0];
                double fn = CLIGHT / lam_carr[freq];
                double dop_e = (-e - dpCA[i]) * fn / f1 * 1E-4;

                double dop_a = -a * 1E-4;

                bool equalData = abs(dop_e - dop_a) <= 3E-4;
                if (!equalData && !omitCheckIndexes.contains(i))
                {
                    return false;
                }
            }
            return true;
        }

        TEST_F(RinexTests, ShouldImportOneRawEpoch)
        {
            // Arrange
            QString jpsFileIn = this->ResolvePath("javad_20110115-cut.jps");
            auto dataChunkIn = DataChunk::FromFile(jpsFileIn);
            RtkAdapter adapter;
            adapter.opt = "-NOET"; // "-NOET -RL1C -RL2C -GL1W -GL1X -GL2X -JL1Z -JL1X";
            auto gnssDataIn = adapter.toGnssData(dataChunkIn.get());
            auto msg_rt_e = findMessage<RcvTimeStdMessage>(dataChunkIn.get(), RcvTimeStdMessage::Codes::Code);
            auto msg_rd_e = findMessage<RcvDateStdMessage>(dataChunkIn.get(), RcvDateStdMessage::Codes::Code_RD);
            auto msg_si_e = findMessage<SatIndexStdMessage>(dataChunkIn.get(), SatIndexStdMessage::Codes::Code_SI);
            auto msg_nn_e = findMessage<SatNumbersStdMessage>(dataChunkIn.get(), SatNumbersStdMessage::Codes::Code_NN);
            // SNR
            auto msg_CE_e = findMessage<CNR4StdMessage>(dataChunkIn.get(), CNR4StdMessage::Codes::Code_CE);
            auto msg_1E_e = findMessage<CNR4StdMessage>(dataChunkIn.get(), CNR4StdMessage::Codes::Code_1E);
            auto msg_2E_e = findMessage<CNR4StdMessage>(dataChunkIn.get(), CNR4StdMessage::Codes::Code_2E);
            auto msg_3E_e = findMessage<CNR4StdMessage>(dataChunkIn.get(), CNR4StdMessage::Codes::Code_3E);
            auto msg_5E_e = findMessage<CNR4StdMessage>(dataChunkIn.get(), CNR4StdMessage::Codes::Code_5E);
            auto msg_lE_e = findMessage<CNR4StdMessage>(dataChunkIn.get(), CNR4StdMessage::Codes::Code_lE);
            // Preudoranges
            auto msg_rc_e = findMessage<SPRStdMessage>(dataChunkIn.get(), SPRStdMessage::Codes::Code_rc);
            auto msg_1r_e = findMessage<SRPRStdMessage>(dataChunkIn.get(), SRPRStdMessage::Codes::Code_1r);
            auto msg_2r_e = findMessage<SRPRStdMessage>(dataChunkIn.get(), SRPRStdMessage::Codes::Code_2r);
            auto msg_3r_e = findMessage<SRPRStdMessage>(dataChunkIn.get(), SRPRStdMessage::Codes::Code_3r);
            auto msg_5r_e = findMessage<SRPRStdMessage>(dataChunkIn.get(), SRPRStdMessage::Codes::Code_5r);
            auto msg_lr_e = findMessage<SRPRStdMessage>(dataChunkIn.get(), SRPRStdMessage::Codes::Code_lr);
            // Carrier phases
            auto msg_cp_e = findMessage<RCPRc1StdMessage>(dataChunkIn.get(), RCPRc1StdMessage::Codes::Code_cp);
            auto msg_1p_e = findMessage<RCPRc1StdMessage>(dataChunkIn.get(), RCPRc1StdMessage::Codes::Code_1p);
            auto msg_2p_e = findMessage<RCPRc1StdMessage>(dataChunkIn.get(), RCPRc1StdMessage::Codes::Code_2p);
            auto msg_3p_e = findMessage<RCPRc1StdMessage>(dataChunkIn.get(), RCPRc1StdMessage::Codes::Code_3p);
            auto msg_5p_e = findMessage<RCPRc1StdMessage>(dataChunkIn.get(), RCPRc1StdMessage::Codes::Code_5p);
            auto msg_lp_e = findMessage<RCPRc1StdMessage>(dataChunkIn.get(), RCPRc1StdMessage::Codes::Code_lp);
            // Doppler
            auto msg_DC_e = findMessage<DPStdMessage>(dataChunkIn.get(), DPStdMessage::Codes::Code_DC);
            auto msg_1d_e = findMessage<SRDPStdMessage>(dataChunkIn.get(), SRDPStdMessage::Codes::Code_1d);
            auto msg_2d_e = findMessage<SRDPStdMessage>(dataChunkIn.get(), SRDPStdMessage::Codes::Code_2d);
            auto msg_3d_e = findMessage<SRDPStdMessage>(dataChunkIn.get(), SRDPStdMessage::Codes::Code_3d);
            auto msg_5d_e = findMessage<SRDPStdMessage>(dataChunkIn.get(), SRDPStdMessage::Codes::Code_5d);
            auto msg_ld_e = findMessage<SRDPStdMessage>(dataChunkIn.get(), SRDPStdMessage::Codes::Code_ld);
            // ephemeris
            auto msg_GE_e = findMessage<GPSEphemeris0StdMessage>(dataChunkIn.get(), GPSEphemeris0StdMessage::Codes::Code_GE);
            auto msg_NE_e = findMessage<GLOEphemerisStdMessage>(dataChunkIn.get(), GLOEphemerisStdMessage::Codes::Code_NE);
            auto msg_EN_e = findMessage<GALEphemerisStdMessage>(dataChunkIn.get(), GALEphemerisStdMessage::Codes::Code_EN);
            auto msg_WE_e = findMessage<SBASEhemerisStdMessage>(dataChunkIn.get(), SBASEhemerisStdMessage::Codes::Code_WE);
            auto msg_QE_e = findMessage<QZSSEphemerisStdMessage>(dataChunkIn.get(), QZSSEphemerisStdMessage::Codes::Code_QE);
            auto msg_CN_e = findMessage<BeiDouEphemerisStdMessage>(dataChunkIn.get(), BeiDouEphemerisStdMessage::Codes::Code_CN);

            auto dataChunkOut = RtkAdapter().toMessages(gnssDataIn);
            auto msg_rt_a = findMessage<RcvTimeStdMessage>(dataChunkOut.get(), RcvTimeStdMessage::Codes::Code);
            auto msg_rd_a = findMessage<RcvDateStdMessage>(dataChunkOut.get(), RcvDateStdMessage::Codes::Code_RD);
            auto msg_si_a = findMessage<SatIndexStdMessage>(dataChunkOut.get(), SatIndexStdMessage::Codes::Code_SI);
            auto msg_nn_a = findMessage<SatNumbersStdMessage>(dataChunkOut.get(), SatNumbersStdMessage::Codes::Code_NN);
            // SNR
            auto msg_EC_a = findMessage<CNRStdMessage>(dataChunkOut.get(), CNRStdMessage::Codes::Code_EC);
            auto msg_E1_a = findMessage<CNRStdMessage>(dataChunkOut.get(), CNRStdMessage::Codes::Code_E1);
            auto msg_E2_a = findMessage<CNRStdMessage>(dataChunkOut.get(), CNRStdMessage::Codes::Code_E2);
            auto msg_E3_a = findMessage<CNRStdMessage>(dataChunkOut.get(), CNRStdMessage::Codes::Code_E3);
            auto msg_E5_a = findMessage<CNRStdMessage>(dataChunkOut.get(), CNRStdMessage::Codes::Code_E5);
            auto msg_El_a = findMessage<CNRStdMessage>(dataChunkOut.get(), CNRStdMessage::Codes::Code_El);
            // Preudoranges
            auto msg_RC_a = findMessage<PRStdMessage>(dataChunkOut.get(), PRStdMessage::Codes::Code_RC);
            auto msg_R1_a = findMessage<PRStdMessage>(dataChunkOut.get(), PRStdMessage::Codes::Code_R1);
            auto msg_R2_a = findMessage<PRStdMessage>(dataChunkOut.get(), PRStdMessage::Codes::Code_R2);
            auto msg_R3_a = findMessage<PRStdMessage>(dataChunkOut.get(), PRStdMessage::Codes::Code_R3);
            auto msg_R5_a = findMessage<PRStdMessage>(dataChunkOut.get(), PRStdMessage::Codes::Code_R5);
            auto msg_Rl_a = findMessage<PRStdMessage>(dataChunkOut.get(), PRStdMessage::Codes::Code_Rl);
            // Carrier phases
            auto msg_PC_a = findMessage<CPStdMessage>(dataChunkOut.get(), CPStdMessage::Codes::Code_PC);
            auto msg_P1_a = findMessage<CPStdMessage>(dataChunkOut.get(), CPStdMessage::Codes::Code_P1);
            auto msg_P2_a = findMessage<CPStdMessage>(dataChunkOut.get(), CPStdMessage::Codes::Code_P2);
            auto msg_P3_a = findMessage<CPStdMessage>(dataChunkOut.get(), CPStdMessage::Codes::Code_P3);
            auto msg_P5_a = findMessage<CPStdMessage>(dataChunkOut.get(), CPStdMessage::Codes::Code_P5);
            auto msg_Pl_a = findMessage<CPStdMessage>(dataChunkOut.get(), CPStdMessage::Codes::Code_Pl);
            // Doppler
            auto msg_DC_a = findMessage<DPStdMessage>(dataChunkOut.get(), DPStdMessage::Codes::Code_DC);
            auto msg_D1_a = findMessage<DPStdMessage>(dataChunkOut.get(), DPStdMessage::Codes::Code_D1);
            auto msg_D2_a = findMessage<DPStdMessage>(dataChunkOut.get(), DPStdMessage::Codes::Code_D2);
            auto msg_D3_a = findMessage<DPStdMessage>(dataChunkOut.get(), DPStdMessage::Codes::Code_D3);
            auto msg_D5_a = findMessage<DPStdMessage>(dataChunkOut.get(), DPStdMessage::Codes::Code_D5);
            auto msg_Dl_a = findMessage<DPStdMessage>(dataChunkOut.get(), DPStdMessage::Codes::Code_Dl);
            // ephemeris
            auto msg_GE_a = findMessage<GPSEphemeris0StdMessage>(dataChunkOut.get(), GPSEphemeris0StdMessage::Codes::Code_GE);
            auto msg_NE_a = findMessage<GLOEphemerisStdMessage>(dataChunkOut.get(), GLOEphemerisStdMessage::Codes::Code_NE);
            auto msg_EN_a = findMessage<GALEphemerisStdMessage>(dataChunkOut.get(), GALEphemerisStdMessage::Codes::Code_EN);
            auto msg_WE_a = findMessage<SBASEhemerisStdMessage>(dataChunkOut.get(), SBASEhemerisStdMessage::Codes::Code_WE);
            auto msg_QE_a = findMessage<QZSSEphemerisStdMessage>(dataChunkOut.get(), QZSSEphemerisStdMessage::Codes::Code_QE);
            auto msg_CN_a = findMessage<BeiDouEphemerisStdMessage>(dataChunkOut.get(), BeiDouEphemerisStdMessage::Codes::Code_CN);

            // [RT]
            ASSERT_EQ(msg_rt_e->Tod(), msg_rt_a->Tod());
            // [RD]
            ASSERT_EQ(msg_rd_e->Year(), msg_rd_a->Year());
            ASSERT_EQ(msg_rd_e->Month(), msg_rd_a->Month());
            ASSERT_EQ(msg_rd_e->Day(), msg_rd_a->Day());
            ASSERT_EQ(msg_rd_e->Base(), msg_rd_a->Base());
            // [SI]
            ASSERT_EQ(msg_si_e->Usi().size(), msg_si_a->Usi().size());
            for (int i = 0; i < msg_si_e->Usi().size(); i++)
            {
                auto e = msg_si_e->Usi()[i];
                auto a = msg_si_a->Usi()[i];
                bool equalIndexes = e == a;
                bool glonassStubIndex = a > 37 && a < 71;
                ASSERT_TRUE(equalIndexes || glonassStubIndex);
            }
            // [NN]
            ASSERT_EQ(msg_nn_e->Osn().size(), msg_nn_a->Osn().size());
            for (int i = 0; i < msg_nn_e->Osn().size(); i++)
            {
                bool equalData = msg_nn_e->Osn()[i] == msg_nn_a->Osn()[i];
                ASSERT_TRUE(equalData);
            }
            // [CE/EC]
            ASSERT_TRUE(check_xE_Ex(msg_CE_e, msg_EC_a, QList<int>({ 2, 3, 4, 15, 16 })));
            // [1E/E1]
            ASSERT_TRUE(check_xE_Ex(msg_1E_e, msg_E1_a, QList<int>({ 0, 1, 5, 6 ,7, 8, 9, 10, 11, 12, 13, 14, 17, 18, 19, 20 })));
            // [2E/E2]
            ASSERT_TRUE(check_xE_Ex(msg_2E_e, msg_E2_a));
            // [3E/E3]
            ASSERT_TRUE(check_xE_Ex(msg_3E_e, msg_E3_a, QList<int>({ 2, 3, 4, 9, 13, 15, 16 })));
            // [5E/E5]
            ASSERT_TRUE(check_xE_Ex(msg_5E_e, msg_E5_a));
            // [lE/El]
            ASSERT_TRUE(check_xE_Ex(msg_lE_e, msg_El_a, QList<int>({ 19 })));

            // [rc/RC]
            ASSERT_TRUE(check_rx_Rx(gnssDataIn.get(), msg_rc_e, msg_RC_a, QList<int>({ 2, 3, 4, 15, 16, 20 })));
            // [1r/R1]
            ASSERT_TRUE(check_xr_Rx(msg_RC_a->Pr(), gnssDataIn.get(), msg_1r_e, msg_R1_a, QList<int>({ 0, 1, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18, 19, 20 })));
            // [2r/R2]
            ASSERT_TRUE(check_xr_Rx(msg_RC_a->Pr(), gnssDataIn.get(), msg_2r_e, msg_R2_a, QList<int>({ 17, 18, 19, 20 })));
            // [3r/R3]
            ASSERT_TRUE(check_xr_Rx(msg_RC_a->Pr(), gnssDataIn.get(), msg_3r_e, msg_R3_a, QList<int>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20 })));
            // [5r/R5]
            ASSERT_TRUE(check_xr_Rx(msg_RC_a->Pr(), gnssDataIn.get(), msg_5r_e, msg_R5_a));
            // [lr/Rl]
            ASSERT_TRUE(check_xr_Rx(msg_RC_a->Pr(), gnssDataIn.get(), msg_lr_e, msg_Rl_a, QList<int>({ 19 })));
            
            // [cp/PC]
            ASSERT_TRUE(check_xp_Px('c', msg_RC_a->Pr(), gnssDataIn.get(), msg_cp_e, msg_PC_a));
            // [1p/P1]
            ASSERT_TRUE(check_xp_Px('1', msg_RC_a->Pr(), gnssDataIn.get(), msg_1p_e, msg_P1_a, QList<int>({ 0, 1, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18, 19, 20 })));
            // [2p/P2]
            ASSERT_TRUE(check_xp_Px('2', msg_RC_a->Pr(), gnssDataIn.get(), msg_2p_e, msg_P2_a));
            // [3p/P3]
            ASSERT_TRUE(check_xp_Px('3', msg_RC_a->Pr(), gnssDataIn.get(), msg_3p_e, msg_P3_a, QList<int>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20 })));
            // [5p/P5]
            ASSERT_TRUE(check_xp_Px('5', msg_RC_a->Pr(), gnssDataIn.get(), msg_5p_e, msg_P5_a));
            // [lp/Pl]
            ASSERT_TRUE(check_xp_Px('l', msg_RC_a->Pr(), gnssDataIn.get(), msg_lp_e, msg_Pl_a, QList<int>({ 19 })));

            // [DC/DC]
            ASSERT_TRUE(check_Dx_Dx(msg_DC_e, msg_DC_a, QList<int>({ 2, 3, 4, 15, 16 })));
            // [1d/D1]
            ASSERT_TRUE(msg_1d_e == NULL);
            ASSERT_TRUE(msg_D1_a == NULL);
            // [2d/D2]
            ASSERT_TRUE(check_xd_Dx('2', msg_DC_a->Dp(), gnssDataIn.get(), msg_2d_e, msg_D2_a));
            // [3d/D3]
            ASSERT_TRUE(check_xd_Dx('3', msg_DC_a->Dp(), gnssDataIn.get(), msg_3d_e, msg_D3_a, QList<int>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20 })));
            // [5d/D5]
            ASSERT_TRUE(check_xd_Dx('5', msg_DC_a->Dp(), gnssDataIn.get(), msg_5d_e, msg_D5_a));
            // [ld/Dl]
            ASSERT_TRUE(msg_Dl_a == NULL); // missing index 19

            // GPS Ephemeris
            EXPECT_TRUE(msg_GE_e->Req()->Sv() == msg_GE_a->Req()->Sv());
            EXPECT_TRUE(msg_GE_e->Req()->Tow() == msg_GE_a->Req()->Tow());
            {
                Types::u1 flag_e = msg_GE_e->Req()->Flags();
                Types::u1 flag_a = msg_GE_a->Req()->Flags();
                EXPECT_TRUE((flag_e & flag_a) == flag_a);
            }
            EXPECT_TRUE(msg_GE_e->Req()->Iodc() == msg_GE_a->Req()->Iodc());
            EXPECT_TRUE(msg_GE_e->Req()->Toc() == msg_GE_a->Req()->Toc());
            EXPECT_TRUE(msg_GE_e->Req()->Ura() == msg_GE_a->Req()->Ura());
            EXPECT_TRUE(msg_GE_e->Req()->HealthS() == msg_GE_a->Req()->HealthS());
            EXPECT_TRUE(msg_GE_e->Req()->Wn() == msg_GE_a->Req()->Wn());
            EXPECT_TRUE(msg_GE_e->Req()->Tgd() == msg_GE_a->Req()->Tgd());
            EXPECT_TRUE(msg_GE_e->Req()->Af2() == msg_GE_a->Req()->Af2());
            EXPECT_TRUE(msg_GE_e->Req()->Af1() == msg_GE_a->Req()->Af1());
            EXPECT_TRUE(msg_GE_e->Req()->Af0() == msg_GE_a->Req()->Af0());
            EXPECT_TRUE(msg_GE_e->Req()->Toe() == msg_GE_a->Req()->Toe());
            EXPECT_TRUE(msg_GE_e->Req()->Iode() == msg_GE_a->Req()->Iode());
            EXPECT_TRUE(msg_GE_e->Req()->RootA() == msg_GE_a->Req()->RootA());
            EXPECT_TRUE(msg_GE_e->Req()->Ecc() == msg_GE_a->Req()->Ecc());
            EXPECT_TRUE(msg_GE_e->Req()->M0() == msg_GE_a->Req()->M0());
            EXPECT_TRUE(msg_GE_e->Req()->Omega0() == msg_GE_a->Req()->Omega0());
            EXPECT_TRUE(msg_GE_e->Req()->Inc0() == msg_GE_a->Req()->Inc0());
            EXPECT_TRUE(msg_GE_e->Req()->ArgPer() == msg_GE_a->Req()->ArgPer());
            EXPECT_TRUE(msg_GE_e->Req()->Deln() == msg_GE_a->Req()->Deln());
            EXPECT_TRUE(msg_GE_e->Req()->OmegaDot() == msg_GE_a->Req()->OmegaDot());
            EXPECT_TRUE(msg_GE_e->Req()->IncDot() == msg_GE_a->Req()->IncDot());
            EXPECT_TRUE(msg_GE_e->Req()->Crc() == msg_GE_a->Req()->Crc());
            EXPECT_TRUE(msg_GE_e->Req()->Crs() == msg_GE_a->Req()->Crs());
            EXPECT_TRUE(msg_GE_e->Req()->Cuc() == msg_GE_a->Req()->Cuc());
            EXPECT_TRUE(msg_GE_e->Req()->Cus() == msg_GE_a->Req()->Cus());
            EXPECT_TRUE(msg_GE_e->Req()->Cic() == msg_GE_a->Req()->Cic());
            EXPECT_TRUE(msg_GE_e->Req()->Cis() == msg_GE_a->Req()->Cis());


            /*ASSERT_EQ(msg_CE_e->CnrX4().size(), msg_EC_a->Cnr().size());
            for (int i = 0; i < msg_CE_e->CnrX4().size(); i++)
            {
                Types::u1 e = msg_CE_e->CnrX4()[i];
                Types::u1 a = msg_EC_a->Cnr()[i];
                bool equalData = e / 4 == a;
                ASSERT_TRUE(equalData);
            }
            // [1E/E1]
            ASSERT_EQ(msg_1E_e->CnrX4().size(), msg_EC_a->Cnr().size());
            for (int i = 0; i < msg_CE_e->CnrX4().size(); i++)
            {
                Types::u1 e = msg_CE_e->CnrX4()[i];
                Types::u1 a = msg_EC_a->Cnr()[i];
                bool equalData = e / 4 == a;
                ASSERT_TRUE(equalData);
            }*/
            
            /*
            auto msg_rt_e_data = msg_rt_e->ToByteArray();
            int size_e = msg_rt_e_data.size();
            auto msg_rt_a_data = msg_rt_a->ToByteArray();
            int size_a = msg_rt_a_data.size();
            ASSERT_EQ(size_e, size_a);
            int neq = memcmp(msg_rt_e_data.data(), msg_rt_a_data.data(), size_e);
            ASSERT_EQ(neq, 0);*/
            //auto gnssDataOut = RtkAdapter().toGnssData(dataChunkOut.get());
            /*QString navFileNameIn = this->ResolvePath("ifz-data-0.in.14N");
            auto gnssData = RinexReader().ReadFile(obsFileNameIn).ReadFile(navFileNameIn).BuildResult();

            // Act
            auto dataChunk = RtkAdapter().toMessages(gnssData);*/

            // Assert
            // nothing that we can check...
        }
    }
}


