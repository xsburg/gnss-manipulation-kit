#pragma once

#include <gtest/gtest.h>
#include "Utils/BaseTest.h"
#include "Common/SmartPtr.h"
#include "Greis/DataChunk.h"
#include <Platform/ServiceManager.h>
#include <rtklib.h>

using namespace Common;

typedef struct {        /* receiver raw data control type */
    gtime_t time;       /* message time */
    gtime_t tobs;       /* observation data time */
    obs_t obs;          /* observation data */
    obs_t obuf;         /* observation data buffer */
    nav_t nav;          /* satellite ephemerides */
    sta_t sta;          /* station parameters */
    int ephsat;         /* sat number of update ephemeris (0:no satellite) */
    sbsmsg_t sbsmsg;    /* SBAS message */
    char msgtype[256];  /* last message type */
    unsigned char subfrm[MAXSAT][380];  /* subframe buffer */
    lexmsg_t lexmsg;    /* LEX message */
    double lockt[MAXSAT][NFREQ + NEXOBS]; /* lock time (s) */
    double icpp[MAXSAT], off[MAXSAT], icpc; /* carrier params for ss2 */
    double prCA[MAXSAT], dpCA[MAXSAT]; /* L1/CA pseudrange/doppler for javad */
    unsigned char halfc[MAXSAT][NFREQ + NEXOBS]; /* half-cycle add flag */
    char freqn[MAXOBS]; /* frequency number for javad */
    int nbyte;          /* number of bytes in message buffer */
    int len;            /* message length (bytes) */
    int iod;            /* issue of data */
    int tod;            /* time of day (ms) */
    int tbase;          /* time base (0:gpst,1:utc(usno),2:glonass,3:utc(su) */
    int flag;           /* general purpose flag */
    int outtype;        /* output message type */
    unsigned char buff[MAXRAWLEN]; /* message buffer */
    char opt[256];      /* receiver dependent options */
    double receive_time;/* RT17: Reiceve time of week for week rollover detection */
    unsigned int plen;  /* RT17: Total size of packet to be read */
    unsigned int pbyte; /* RT17: How many packet bytes have been read so far */
    unsigned int page;  /* RT17: Last page number */
    unsigned int reply; /* RT17: Current reply number */
    int week;           /* RT17: week number */
    unsigned char pbuff[255 + 4 + 2]; /* RT17: Packet buffer */
} raw_t2;

extern int showmsg(char *format, ...)
{
    va_list arg;
    va_start(arg, format); vfprintf(stderr, format, arg); va_end(arg);
    fprintf(stderr, "\r");
    return 0;
}

namespace Greis
{
    namespace Tests
    {
        class RtklibTests : public BaseTest
        {
        };

        TEST_F(RtklibTests, ShouldReadJavadFile)
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

            rnxopt_t opt = {{0}};
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
    }
}
