#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <Greis/RinexReader.h>
#include <Common/Exception.h>
#include <rtklib.h>
#include <Greis/AllStdMessages.h>

// Implemented abstract method from rtklib
extern int showmsg(char *format, ...)
{
    va_list arg;
    va_start(arg, format); vfprintf(stderr, format, arg); va_end(arg);
    fprintf(stderr, "\r");
    return 0;
}

Greis::RinexReader::RinexReader() : _gnssData(new GnssData())
{
}

Greis::RinexReader& Greis::RinexReader::ReadFile(QString fileName)
{
    QFileInfo fileInfo(fileName);
    fileName = fileInfo.absoluteFilePath();
    fileName = QDir::toNativeSeparators(fileName);
    int status = readrnx(fileName.toLatin1(), 1, "", &_gnssData->getObs(), &_gnssData->getNav(), &_gnssData->getSta());
    if (status == -1)
    {
        throw new Common::Exception("Failed to read rinex, status == -1.");
    }
    return *this;
}

Greis::GnssData::SharedPtr_t Greis::RinexReader::BuildResult()
{
    sortobs(&_gnssData->getObs());
    uniqnav(&_gnssData->getNav());
    return _gnssData;
}

Greis::RinexWriter::RinexWriter(GnssData::SharedPtr_t gnssData) : _gnssData(gnssData)
{
}

rnxopt_t Greis::RinexWriter::createRnxOptions()
{
    rnxopt_t opt = {{0}};
    opt.rnxver = 2.12;
    strcpy(opt.prog, "GREIFZ v.2.0.99");
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
    opt.navsys = SYS_ALL;
    opt.freqtype = FREQTYPE_ALL;
    opt.obstype = OBSTYPE_ALL;
    opt.tstart = _gnssData->getObs().data[0].time;
    opt.tend = _gnssData->getObs().data[_gnssData->getObs().n - 1].time;
    opt.tint = 1.0; // INTERVAL
    return opt;
}

Greis::RinexWriter& Greis::RinexWriter::WriteObsFile(QString fileName)
{
    rnxopt_t opt = createRnxOptions();

    FILE* fr = fopen(fileName.toLatin1(), "w");
    outrnxobsh(fr, &opt, &_gnssData->getNav());

    int i = 0;
    int j = 0;
    while (i < _gnssData->getObs().n) {
        while (j < _gnssData->getObs().n && timediff(_gnssData->getObs().data[j].time, _gnssData->getObs().data[i].time) <= 0.0)
        {
            j++;
        }
        outrnxobsb(fr, &opt, _gnssData->getObs().data + i, j - i, 0);
        i = j;
    }
    fclose(fr);

    return *this;
}

Greis::RinexWriter& Greis::RinexWriter::WriteNavFile(QString fileName)
{
    rnxopt_t opt = createRnxOptions();

    FILE* fr = fopen(fileName.toLatin1(), "w");
    outrnxnavh(fr, &opt, &_gnssData->getNav());

    for (int i = 0; i < _gnssData->getNav().n; i++)
    {
        outrnxnavb(fr, &opt, _gnssData->getNav().eph + i);
    }

    fclose(fr);

    return *this;
}

void init_obs(obs_t* obs, int nmax = MAXOBS)
{
    obs->data = (obsd_t*)malloc(sizeof(obsd_t)*nmax);
    obs->nmax = nmax;
}

void free_obs(obs_t* obs)
{
    free(obs->data);
    obs->data = NULL;
    obs->n = 0;
}

void init_nav(nav_t* nav, int nmax = MAXSAT, int ngmax = NSATGLO, int nsmax = NSATSBS * 2)
{
    nav->eph = (eph_t *)malloc(sizeof(eph_t)*nmax);
    nav->nmax = nmax;
    nav->geph = (geph_t*)malloc(sizeof(geph_t)*ngmax);
    nav->ngmax = ngmax;
    nav->seph = (seph_t*)malloc(sizeof(seph_t)*nsmax);
    nav->nsmax = nsmax;
    nav->alm = NULL;
}

void free_nav(nav_t* nav)
{
    free(nav->eph);
    nav->eph = NULL;
    nav->nmax = 0;
    nav->n = 0;
    free(nav->geph);
    nav->geph = NULL;
    nav->ngmax = 0;
    nav->ng = 0;
    free(nav->seph);
    nav->seph = NULL;
    nav->nsmax = 0;
    nav->ns = 0;
}

void copy_nav(nav_t* src, nav_t* dst)
{
    *dst = *src;
    init_nav(dst);
    for (int i = 0; i < src->n; i++)
    {
        dst->eph[i] = src->eph[i];
    }
    for (int i = 0; i < src->ng; i++)
    {
        dst->geph[i] = src->geph[i];
    }
    for (int i = 0; i < src->ns; i++)
    {
    dst->seph[i] = src->seph[i];
    }
}

void copy_obs(obs_t* src, obs_t* dst)
{
    *dst = *src;
    init_obs(dst);
    for (int i = 0; i < src->n; i++)
    {
        dst->data[i] = src->data[i];
    }
}

Greis::GnssData::SharedPtr_t Greis::RtkAdapter::toGnssData(DataChunk* dataChunk)
{
    raw_t* raw = new raw_t();
    init_raw(raw);
    if (this->opt.size() > 0)
    {
        strcpy(raw->opt, this->opt.c_str());
    }

    auto data = dataChunk->ToByteArray();

    std::vector<obs_t> obsEpochs;
    int obsTotalCount = 0;
    std::vector<eph_t> ephAll;

    for (size_t i = 0; i < data.size(); i++)
    {
        char c = data.at(i);
        int ret = input_javad(raw, c);
        switch (ret)
        {
        case 1:
            // input observation data
            obsTotalCount += raw->obs.n;
            obs_t tmp;
            copy_obs(&raw->obs, &tmp);
            obsEpochs.push_back(tmp);
            break;
        case 2:
            // input ephemeris
            ephAll.push_back(raw->nav.eph[raw->ephsat - 1]);
            break;
        }
    }

    free_raw(raw);
    delete raw;
    
    // obs all
    obs_t obsAll;
    init_obs(&obsAll, obsTotalCount);
    obsAll.n = obsTotalCount;
    int index = 0;
    for (auto& obsEp : obsEpochs)
    {
        for (int i = 0; i < obsEp.n; i++)
        {
            obsAll.data[index++] = obsEp.data[i];
        }
        free_obs(&obsEp);
    }
    // nav all
    nav_t navAll;
    init_nav(&navAll, ephAll.size());
    navAll.n = ephAll.size();
    index = 0;
    for (auto& eph : ephAll)
    {
        navAll.eph[index++] = eph;
    }

    GnssData::SharedPtr_t gnssData(new GnssData(obsAll, navAll));
    return gnssData;
}

Greis::DataChunk::SharedPtr_t Greis::RtkAdapter::toMessages(GnssData::SharedPtr_t gnssData)
{
    auto dataChunk = std::make_shared<DataChunk>();

    int i = 0;
    int j = 0;
    while (i < gnssData->getObs().n) {
        while (j < gnssData->getObs().n && timediff(gnssData->getObs().data[j].time, gnssData->getObs().data[i].time) <= 0.0)
        {
            j++;
        }
        writeEpochMessages(dataChunk.get(), gnssData->getObs().data + i, j - i);
        i = j;
    }

    // writeTimeParameters(dataChunk.get(), &gnssData->getNav());

    nav_t *nav = &gnssData->getNav();

    for (int k = 0; k < nav->n; k++)
    {
        writeEphemeris(dataChunk.get(), &nav->eph[k]);
    }

    dataChunk->FlushEpoch();
    return dataChunk;
}

void Greis::RtkAdapter::writeEpochMessages(DataChunk* dataChunk, obsd_t* data, int n)
{
    char* opt = "";//  "-RL1C -RL2C -GL1W -GL1X -GL2X -JL1Z -JL1X";

    // [RT], [RD]
    auto rcvTime = std::make_unique<RcvTimeStdMessage>();
    auto rcvDate = std::make_unique<RcvDateStdMessage>();

    double ep[6] = { 0 };
    // date
    time2epoch(data->time, ep);
    rcvDate->Year() = ep[0];
    rcvDate->Month() = ep[1];
    rcvDate->Day() = ep[2];
    ep[0] = ep[1] = ep[2] = 0;
    // time of day
    gtime_t tod = {0};
    int sec = (int)floor(ep[5]);
    tod.time = (int)ep[3] * 3600 + (int)ep[4] * 60 + sec;
    tod.sec = ep[5] - sec;
    rcvTime->Tod() = (tod.time + tod.sec) * 1000;
    rcvDate->Base() = 0;
    dataChunk->AddMessage(std::move(rcvTime));
    dataChunk->AddMessage(std::move(rcvDate));

    // [SI]
    auto si = std::make_unique<SatIndexStdMessage>(StdMessage::HeadSize() + n + 1);
    std::vector<int> slotData;
    for (int i = 0; i < n; i++)
    {
        obsd_t* satData = data + i;
        auto sat = satData->sat;
        int prn;
        int satSys = satsys(sat, &prn);
        if (satSys == SYS_GLO)
        {
            // Glonass is a special case
            int slot = prn;
            // http://www.novatel.com/support/known-solutions/glonass-slot-number-and-frequency-channel/
            // When a PRN in a log is in the range 38 to 61, then that PRN represents a GLONASS Slot where the Slot shown is the actual GLONASS Slot Number plus 37.
            prn = 37 + slot;
            slotData.push_back(slot);
        }
        if (satSys == SYS_GAL)
        {
            prn += 70;
        }
        if (satSys == SYS_CMP)
        {
            prn += 210;
        }

        si->Usi().push_back(prn);
    }
    dataChunk->AddMessage(std::move(si));

    // [NN]
    if (slotData.size() > 0)
    {
        auto nn = std::make_unique<SatNumbersStdMessage>(StdMessage::HeadSize() + slotData.size() + 1);
        for (auto slot : slotData)
        {
            nn->Osn().push_back(slot);
        }
        dataChunk->AddMessage(std::move(nn));
    }

    char codes[6] = { 'C', '1', '2', '3', '5', 'l' };

    // [EC], [E1], [E2], [E3], [E5], [El]
    std::vector<std::string> cnrCodeIds = {
        CNRStdMessage::Codes::Code_EC,
        CNRStdMessage::Codes::Code_E1,
        CNRStdMessage::Codes::Code_E2,
        CNRStdMessage::Codes::Code_E3,
        CNRStdMessage::Codes::Code_E5,
        CNRStdMessage::Codes::Code_El
    };
    for (int codeIndex = 0; codeIndex < 6; codeIndex++)
    {
        char code = codes[codeIndex];
        std::string codeId = cnrCodeIds[codeIndex];
        
        auto msg = std::make_unique<CNRStdMessage>(codeId, StdMessage::HeadSize() + n + 1);
        msg->Cnr().resize(n);
        for (int i = 0; i < n; i++)
        {
            obsd_t* satData = data + i;

            msg->Cnr()[i] = 0xFF;

            int sys;
            if (!(sys = satsys(satData->sat, NULL)))
            {
                continue;
            }

            int type;
            int freq;
            if ((freq = tofreq(code, sys, &type)) < 0)
            {
                continue;
            }

            int j;
            if ((j = checkpri(opt, sys, type, freq)) >= 0)
            {
                unsigned char cnr = satData->SNR[j] / 4.0;
                if (cnr != 0)
                {
                    msg->Cnr()[i] = cnr;
                }
                else
                {
                    msg->Cnr()[i] = 0xFF;
                }
            }
        }
        dataChunk->AddMessage(std::move(msg));
    }

    // [RC], [R1], [R2], [R3], [R5], [Rl]
    std::vector<std::string> prCodeIds = {
        PRStdMessage::Codes::Code_RC,
        PRStdMessage::Codes::Code_R1,
        PRStdMessage::Codes::Code_R2,
        PRStdMessage::Codes::Code_R3,
        PRStdMessage::Codes::Code_R5,
        PRStdMessage::Codes::Code_Rl
    };
    for (int codeIndex = 0; codeIndex < 6; codeIndex++)
    {
        char code = codes[codeIndex];
        std::string codeId = prCodeIds[codeIndex];
        
        auto msg = std::make_unique<PRStdMessage>(codeId, StdMessage::HeadSize() + 8 * n + 1);
        msg->Pr().resize(n);
        for (int i = 0; i < n; i++)
        {
            obsd_t* satData = data + i;

            msg->Pr()[i] = 0.0;

            int sys;
            if (!(sys = satsys(satData->sat, NULL)))
            {
                continue;
            }

            int type;
            int freq;
            if ((freq = tofreq(code, sys, &type)) < 0)
            {
                continue;
            }

            int j;
            if ((j = checkpri(opt, sys, type, freq)) >= 0)
            {
                Types::f8 pr = satData->P[j] / CLIGHT;
                msg->Pr()[i] = pr;
            }
        }
        dataChunk->AddMessage(std::move(msg));
    }

    // [PC], [P1], [P2], [P3], [P5], [Pl]
    std::vector<std::string> cpCodeIds = {
        CPStdMessage::Codes::Code_PC,
        CPStdMessage::Codes::Code_P1,
        CPStdMessage::Codes::Code_P2,
        CPStdMessage::Codes::Code_P3,
        CPStdMessage::Codes::Code_P5,
        CPStdMessage::Codes::Code_Pl
    };
    for (int codeIndex = 0; codeIndex < 6; codeIndex++)
    {
        char code = codes[codeIndex];
        std::string codeId = cpCodeIds[codeIndex];
        
        auto msg = std::make_unique<CPStdMessage>(codeId, StdMessage::HeadSize() + 8 * n + 1);
        msg->Cp().resize(n);
        for (int i = 0; i < n; i++)
        {
            obsd_t* satData = data + i;

            msg->Cp()[i] = 0.0;

            int sys;
            if (!(sys = satsys(satData->sat, NULL)))
            {
                continue;
            }

            int type;
            int freq;
            if ((freq = tofreq(code, sys, &type)) < 0)
            {
                continue;
            }

            int j;
            if ((j = checkpri(opt, sys, type, freq)) >= 0)
            {
                Types::f8 cp = satData->L[j];
                msg->Cp()[i] = cp;
            }
        }
        dataChunk->AddMessage(std::move(msg));
    }

    // [DC], [D1], [D2], [D3], [D5], [Dl]
    std::vector<std::string> dpCodeIds = {
        DPStdMessage::Codes::Code_DC,
        DPStdMessage::Codes::Code_D1,
        DPStdMessage::Codes::Code_D2,
        DPStdMessage::Codes::Code_D3,
        DPStdMessage::Codes::Code_D5,
        DPStdMessage::Codes::Code_Dl
    };
    for (int codeIndex = 0; codeIndex < 6; codeIndex++)
    {
        char code = codes[codeIndex];
        std::string codeId = dpCodeIds[codeIndex];

        auto msg = std::make_unique<DPStdMessage>(codeId, StdMessage::HeadSize() + 4 * n + 1);
        bool emptyMessage = true;
        msg->Dp().resize(n);
        for (int i = 0; i < n; i++)
        {
            obsd_t* satData = data + i;

            msg->Dp()[i] = 0;

            int sys;
            if (!(sys = satsys(satData->sat, NULL)))
            {
                continue;
            }

            int type;
            int freq;
            if ((freq = tofreq(code, sys, &type)) < 0)
            {
                continue;
            }

            int j;
            if ((j = checkpri(opt, sys, type, freq)) >= 0)
            {
                float dop = satData->D[j];
                Types::i4 dp = -dop / 1E-4;
                msg->Dp()[i] = dp;
                if (dp != 0)
                {
                    emptyMessage = false;
                }
            }
        }
        if (!emptyMessage)
        {
            dataChunk->AddMessage(std::move(msg));
        }
    }

    // [TC] TODO: Not implemented
    {
        auto msg = std::make_unique<TrackingTimeCAStdMessage>(StdMessage::HeadSize() + 2 * n + 1);
        msg->Tt().resize(n);
        for (int i = 0; i < n; i++)
        {
            obsd_t* satData = data + i;

            msg->Tt()[i] = 0xFFFF;

            /*Types::u1 sat = satData->sat;
            tt_p = (unsigned short)raw->lockt[sat - 1][0];

            /* loss-of-lock detected by lock-time counter #1#
            if (tt == 0 || tt<tt_p) {
            raw->obuf.data[i].LLI[0] |= 1;
            }
            raw->lockt[sat - 1][0] = tt;*/
        }
        //dataChunk->AddMessage(std::move(msg));
    }

/*
    + if (!strncmp(p,"~~",2)) return decode_RT(raw); /* receiver time #1#

    + if (strstr(raw->opt, "-NOET")) {
    +     if (!strncmp(p, "::", 2)) return decode_ET(raw); /* epoch time #1#
    + }
    + if (!strncmp(p, "RD", 2)) return decode_RD(raw); /* receiver date #1#
    + if (!strncmp(p, "SI", 2)) return decode_SI(raw); /* satellite indices #1#
    + if (!strncmp(p, "NN", 2)) return decode_NN(raw); /* glonass slot numbers #1#

    + if (!strncmp(p, "GA", 2)) return decode_GA(raw); /* gps almanac #1#
    + if (!strncmp(p, "NA", 2)) return decode_NA(raw); /* glonass almanac #1#
    + if (!strncmp(p, "EA", 2)) return decode_EA(raw); /* galileo almanac #1#
    + if (!strncmp(p, "WA", 2)) return decode_WA(raw); /* sbas almanac #1#
    + if (!strncmp(p, "QA", 2)) return decode_QA(raw); /* qzss almanac (ext) #1#

    if (!strncmp(p, "GE", 2)) return decode_GE(raw); /* gps ephemeris #1#
    if (!strncmp(p, "NE", 2)) return decode_NE(raw); /* glonass ephemeris #1#
    if (!strncmp(p, "EN", 2)) return decode_EN(raw); /* galileo ephemeris #1#
    if (!strncmp(p, "WE", 2)) return decode_WE(raw); /* waas ephemeris #1#
    if (!strncmp(p, "QE", 2)) return decode_QE(raw); /* qzss ephemeris (ext) #1#
    if (!strncmp(p, "CN", 2)) return decode_CN(raw); /* beidou ephemeris (ext) #1#

    ~+ if (!strncmp(p, "UO", 2)) return decode_UO(raw); /* gps utc time parameters #1#
    + if (!strncmp(p, "NU", 2)) return decode_NU(raw); /* glonass utc and gps time par #1#
    + if (!strncmp(p, "EU", 2)) return decode_EU(raw); /* galileo utc and gps time par #1#
    + if (!strncmp(p, "WU", 2)) return decode_WU(raw); /* waas utc time parameters #1#
    + if (!strncmp(p, "QU", 2)) return decode_QU(raw); /* qzss utc and gps time par #1#
    (todo) if (!strncmp(p, "IO", 2)) return decode_IO(raw); /* ionospheric parameters #1#

    if (!strncmp(p, "GD", 2)) return decode_nD(raw, SYS_GPS); /* raw navigation data #1#
    if (!strncmp(p, "QD", 2)) return decode_nD(raw, SYS_QZS); /* raw navigation data #1#
    if (!strncmp(p, "gd", 2)) return decode_nd(raw, SYS_GPS); /* raw navigation data #1#
    if (!strncmp(p, "qd", 2)) return decode_nd(raw, SYS_QZS); /* raw navigation data #1#
    if (!strncmp(p, "ED", 2)) return decode_nd(raw, SYS_GAL); /* raw navigation data #1#
    if (!strncmp(p, "cd", 2)) return decode_nd(raw, SYS_CMP); /* raw navigation data #1#
    + if (!strncmp(p, "LD", 2)) return decode_LD(raw); /* glonass raw navigation data #1#
    if (!strncmp(p, "lD", 2)) return decode_lD(raw); /* glonass raw navigation data #1#
    if (!strncmp(p, "WD", 2)) return decode_WD(raw); /* sbas raw navigation data #1#

    - if (!strncmp(p, "TC", 2)) return decode_TC(raw); /* CA/L1 continuous track time #1#

    + if (p[0] == 'R') return decode_Rx(raw, p[1]); /* pseudoranges #1#
    + if (p[0] == 'r') return decode_rx(raw, p[1]); /* short pseudoranges #1#
    + if (p[1] == 'R') return decode_xR(raw, p[0]); /* relative pseudoranges #1#
    + if (p[1] == 'r') return decode_xr(raw, p[0]); /* short relative pseudoranges #1#
    + if (p[0] == 'P') return decode_Px(raw, p[1]); /* carrier phases #1#
    + if (p[0] == 'p') return decode_px(raw, p[1]); /* short carrier phases #1#
    + if (p[1] == 'P') return decode_xP(raw, p[0]); /* relative carrier phases #1#
    + if (p[1] == 'p') return decode_xp(raw, p[0]); /* relative carrier phases #1#
    + if (p[0] == 'D') return decode_Dx(raw, p[1]); /* doppler #1#
    + if (p[1] == 'd') return decode_xd(raw, p[0]); /* short relative doppler #1#
    + if (p[0] == 'E') return decode_Ex(raw, p[1]); /* carrier to noise ratio #1#
    + if (p[1] == 'E') return decode_xE(raw, p[0]); /* carrier to noise ratio x 4 #1#
    + if (p[0] == 'F') return decode_Fx(raw, p[1]); /* signal lock loop flags #1#    
*/
}

void Greis::RtkAdapter::writeTimeParameters(DataChunk* dataChunk, nav_t* nav)
{
    // [UO] gps utc time parameters
    {
        auto msg = std::make_unique<GpsUtcParamStdMessage>();

        msg->Utc() = std::make_unique<UtcOffsCustomType>(23);
        msg->Utc()->A0() = nav->utc_gps[0];
        msg->Utc()->A1() = nav->utc_gps[1];
        msg->Utc()->Tot() = nav->utc_gps[2];
        msg->Utc()->Wnt() = nav->utc_gps[3];
        msg->Utc()->Dtls() = nav->leaps;

        int w;
        (void)time2gpst(utc2gpst(timeget()), &w);
        if (w<1560) w = 1560; /* use 2009/12/1 if time is earlier than 2009/12/1 */




        double week = (nav->utc_gps[3] * 1024 * 1024 - w - 512) / (1024 * 1024 - 1);

        //auto w = msg->Utc()->Wnt();

        /*int w;
        (void)time2gpst(utc2gpst(timeget()), &w);
        if (w<1560) w = 1560; /* use 2009/12/1 if time is earlier than 2009/12/1 #1#
        return week + (w - week + 512) / 1024 * 1024;*/
        /*unsigned char *p = raw->buff + 5;
        raw->nav.utc_gps[0] = R8(p); p += 8;
        raw->nav.utc_gps[1] = R4(p); p += 4;
        raw->nav.utc_gps[2] = U4(p); p += 4;
        raw->nav.utc_gps[3] = adjgpsweek((int)U2(p)); p += 2;
        raw->nav.leaps = I1(p);
        return 9;*/

        dataChunk->AddMessage(std::move(msg));
    }
}

void Greis::RtkAdapter::writeEphemeris(DataChunk* dataChunk, eph_t* eph)
{
    int sys;
    int prn;
    if (!(sys = satsys(eph->sat, &prn)))
    {
        return;
    }
    if (sys == SYS_GLO)
    {
        // Glonass is a special case
        int slot = prn;
        // http://www.novatel.com/support/known-solutions/glonass-slot-number-and-frequency-channel/
        // When a PRN in a log is in the range 38 to 61, then that PRN represents a GLONASS Slot where the Slot shown is the actual GLONASS Slot Number plus 37.
        prn = 37 + slot;
    }
    if (sys == SYS_GAL)
    {
        prn += 70;
    }
    if (sys == SYS_CMP)
    {
        prn += 210;
    }

    auto reqData = std::make_unique<GpsEphReqDataCustomType>(122);

    reqData->Sv() = prn;
    //reqData->Tow()  <= below in the code
    //reqData->Flags() <= below in the code
    reqData->Iodc() = eph->iodc;
    //reqData->Toc()  <= below in the code
    reqData->Ura() = eph->sva;
    reqData->HealthS() = eph->svh;
    //reqData->Wn()  <= below in the code
    reqData->Tgd() = eph->tgd[0];
    reqData->Af2() = eph->f2;
    reqData->Af1() = eph->f1;
    reqData->Af0() = eph->f0;
    reqData->Toe() = eph->toes;
    reqData->Iode() = eph->iode;
    reqData->RootA() = sqrt(eph->A);
    reqData->Ecc() = eph->e;
    reqData->M0() = eph->M0 / SC2RAD;
    reqData->Omega0() = eph->OMG0 / SC2RAD;
    reqData->Inc0() = eph->i0 / SC2RAD;
    reqData->ArgPer() = eph->omg / SC2RAD;
    reqData->Deln() = eph->deln / SC2RAD;
    reqData->OmegaDot() = eph->OMGd / SC2RAD;
    reqData->IncDot() = eph->idot / SC2RAD;
    reqData->Crc() = eph->crc;
    reqData->Crs() = eph->crs;
    reqData->Cuc() = eph->cuc;
    reqData->Cus() = eph->cus;
    reqData->Cic() = eph->cic;
    reqData->Cis() = eph->cis;

    switch (sys)
    {
    case SYS_GPS:
    case SYS_QZS:
        // GPS, QZSS
        {
            // flag
            Types::u1 flag = 0;
            if (eph->fit)
            {
                flag |= 0x1;
            }
            flag |= (eph->code & 3) << 2;
            flag |= (eph->flag & 1) << 1;
            reqData->Flags() = flag;

            // tow, toc, wn
            int week;
            double tow = time2gpst(eph->ttr, &week);
            reqData->Tow() = tow;
            reqData->Wn() = week - 1024;
            double toc = time2gpst(eph->toc, &week);
            reqData->Toc() = toc;

            if (sys == SYS_GPS)
            {
                auto gpsMsg = std::make_unique<GPSEphemeris0StdMessage>(StdMessage::HeadSize() + 123);
                gpsMsg->Req() = std::move(reqData);
                dataChunk->AddMessage(std::move(gpsMsg));
            }
            else if (sys == SYS_QZS)
            {
                auto qzsMsg = std::make_unique<QZSSEphemerisStdMessage>(StdMessage::HeadSize() + 123);
                qzsMsg->Gps() = std::make_unique<GPSEphemeris1CustomType>(122);
                qzsMsg->Gps()->Req() = std::move(reqData);
                dataChunk->AddMessage(std::move(qzsMsg));
            }
        }
        break;
    case SYS_GAL:
        // Galileo
        break;
    case SYS_CMP:
        // BeiDou
        break;
    }
    

    /*switch (sys)
    {
    case SYS_GPS:
    {GpsEphReqDataCustomType
            auto msg = std::make_unique<GPSEphemeris0StdMessage>(fullSize);
        }
        break;
    case SYS_GAL:
    {GpsEphReqDataCustomType
            auto msg = std::make_unique<GALEphemerisStdMessage>();
        }
        break;
    case SYS_QZS:
    {GPSEphemeris1CustomType, GpsEphReqDataCustomType
            auto msg = std::make_unique<QZSSEphemerisStdMessage>(fullSize);
        }
        break;
    case SYS_CMP:
    {GpsEphReqDataCustomType
            auto msg = std::make_unique<BeiDouEphemerisStdMessage>();
        }
        break;
    }*/


    // decode_GE -> gps SYS_GPS
    // decode_EN -> galileo SYS_GAL
    // decode_QE -> qzss SYS_QZS
    // decode_CN -> beidou SYS_CMP
    /*eph_t eph = { 0 };
    double toc, sqrtA, tt;
    char *msg;
    int prn, tow, flag, week;
    unsigned char *p = raw->buff + 5;

    prn = U1(p);        p += 1;
    tow = U4(p);        p += 4;
    flag = U1(p);        p += 1;
    eph.iodc = I2(p);        p += 2;
    toc = I4(p);        p += 4;
    eph.sva = I1(p);        p += 1;
    eph.svh = U1(p);        p += 1;
    week = I2(p);        p += 2;
    eph.tgd[0] = R4(p);        p += 4;
    eph.f2 = R4(p);        p += 4;
    eph.f1 = R4(p);        p += 4;
    eph.f0 = R4(p);        p += 4;
    eph.toes = I4(p);        p += 4;
    eph.iode = I2(p);        p += 2;
    sqrtA = R8(p);        p += 8;
    eph.e = R8(p);        p += 8;
    eph.M0 = R8(p)*SC2RAD; p += 8;
    eph.OMG0 = R8(p)*SC2RAD; p += 8;
    eph.i0 = R8(p)*SC2RAD; p += 8;
    eph.omg = R8(p)*SC2RAD; p += 8;
    eph.deln = R4(p)*SC2RAD; p += 4;
    eph.OMGd = R4(p)*SC2RAD; p += 4;
    eph.idot = R4(p)*SC2RAD; p += 4;
    eph.crc = R4(p);        p += 4;
    eph.crs = R4(p);        p += 4;
    eph.cuc = R4(p);        p += 4;
    eph.cus = R4(p);        p += 4;
    eph.cic = R4(p);        p += 4;
    eph.cis = R4(p);        p += 4;
    eph.A = sqrtA*sqrtA;

    if (raw->outtype) {
        msg = raw->msgtype + strlen(raw->msgtype);
        sprintf(msg, " prn=%3d iode=%3d iodc=%3d toes=%6.0f", prn, eph.iode,
            eph.iodc, eph.toes);
    }
    if (sys == SYS_GPS || sys == SYS_QZS) {
        if (!(eph.sat = satno(sys, prn))) {
            trace(2, "javad ephemeris satellite error: sys=%d prn=%d\n", sys, prn);
            return -1;
        }
        eph.flag = (flag >> 1) & 1;
        eph.code = (flag >> 2) & 3;
        eph.fit = flag & 1;
        eph.week = adjgpsweek(week);
        eph.toe = gpst2time(eph.week, eph.toes);

        /* for week-handover problem #1#
        tt = timediff(eph.toe, raw->time);
        if (tt<-302400.0) eph.week++;
        else if (tt> 302400.0) eph.week--;
        eph.toe = gpst2time(eph.week, eph.toes);

        eph.toc = gpst2time(eph.week, toc);
        eph.ttr = adjweek(eph.toe, tow);
    }
    else if (sys == SYS_GAL) {
        if (!(eph.sat = satno(sys, prn))) {
            trace(2, "javad ephemeris satellite error: sys=%d prn=%d\n", sys, prn);
            return -1;
        }
        eph.tgd[1] = R4(p); p += 4;    /* BGD: E1-E5A (s) #1#
        eph.tgd[2] = R4(p); p += 4 + 13; /* BGD: E1-E5B (s) #1#
        eph.code = U1(p);          /* navtype: 0:E1B(INAV),1:E5A(FNAV) #1#
        /*          3:GIOVE E1B,4:GIOVE E5A #1#

        /* gst week -> gps week #1#
        eph.week = week + 1024;
        eph.toe = gpst2time(eph.week, eph.toes);

        /* for week-handover problem #1#
        tt = timediff(eph.toe, raw->time);
        if (tt<-302400.0) eph.week++;
        else if (tt> 302400.0) eph.week--;
        eph.toe = gpst2time(eph.week, eph.toes);

        eph.toc = gpst2time(eph.week, toc);
        eph.ttr = adjweek(eph.toe, tow);
    }
    else if (sys == SYS_CMP) {
        if (!(eph.sat = satno(sys, prn))) {
            trace(2, "javad ephemeris satellite error: sys=%d prn=%d\n", sys, prn);
            return -1;
        }
        eph.tgd[1] = R4(p); p += 4;    /* TGD2 (s) #1#
        eph.code = U1(p);          /* type of nav data #1#

        eph.week = week;
        eph.toe = bdt2time(week, eph.toes); /* bdt -> gpst #1#
        eph.toc = bdt2time(week, toc);      /* bdt -> gpst #1#
        eph.ttr = adjweek(eph.toe, tow);
    }
    else return 0;

    if (!strstr(raw->opt, "-EPHALL")) {
        if (raw->nav.eph[eph.sat - 1].iode == eph.iode&&
            raw->nav.eph[eph.sat - 1].iodc == eph.iodc) return 0; /* unchanged #1#
    }
    raw->nav.eph[eph.sat - 1] = eph;
    raw->ephsat = eph.sat;
    return 2;*/
}