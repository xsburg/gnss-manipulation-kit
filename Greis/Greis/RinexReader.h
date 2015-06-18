#pragma once

#include <QtCore/QString>
#include "Common/File.h"
#include "Common/Logger.h"
#include <Greis/DataChunk.h>
#include <rtklib.h>

namespace Greis
{
    class GnssData
    {
    public:
        SMART_PTR_T(GnssData);

        GnssData()
        {
            obs = { 0 };
            nav = { 0 };
            sta = { "" };
            destroyed = false;
        }

        GnssData(obs_t p_obs, nav_t p_nav)
        {
            obs = p_obs;
            nav = p_nav;
            sta = { "" };
            destroyed = false;
        }

        virtual ~GnssData()
        {
            if (destroyed)
            {
                return;
            }
            if (obs.data)
            {
                free(obs.data);
            }
            if (nav.eph)
            {
                free(nav.eph);
            }
            if (nav.geph)
            {
                free(nav.geph);
            }
            if (nav.seph)
            {
                free(nav.seph);
            }
        }

        virtual obs_t& getObs()
        {
            return obs;
        }

        virtual nav_t& getNav()
        {
            return nav;
        }

        virtual sta_t& getSta()
        {
            return sta;
        }

    protected:
        obs_t obs;
        nav_t nav;
        sta_t sta;
        bool destroyed;
    };

    class RawGnssData : public GnssData
    {
    public:
        SMART_PTR_T(GnssData);

        RawGnssData(raw_t raw) : _raw(raw)
        {
            init_raw(&_raw);
            obs = raw.obs;
            nav = raw.nav;
            sta = raw.sta;
        }

        RawGnssData()
        {
            init_raw(&_raw);
            obs = _raw.obs;
            nav = _raw.nav;
            sta = _raw.sta;
        }

        virtual ~RawGnssData()
        {
            free_raw(&_raw);
            destroyed = true;
        }

        raw_t& getRaw()
        {
            return _raw;
        }
    private:
        raw_t _raw;
    };

    class RinexReader
    {
    public:
        RinexReader();
        
        RinexReader& ReadFile(QString fileName);

        GnssData::SharedPtr_t BuildResult();
    private:
        GnssData::SharedPtr_t _gnssData;
    };

    class RinexWriter
    {
    public:
        RinexWriter(GnssData::SharedPtr_t gnssData);
        RinexWriter& WriteObsFile(QString fileName);
        RinexWriter& WriteNavFile(QString fileName);
    private:
        rnxopt_t createRnxOptions();

        GnssData::SharedPtr_t _gnssData;
    };

    class RtkAdapter
    {
    public:
        GnssData::SharedPtr_t toGnssData(DataChunk* dataChunk);
        DataChunk::SharedPtr_t toMessages(GnssData::SharedPtr_t gnssData);
        std::string opt;
    private:
        void writeEpochMessages(DataChunk* dataChunk, obsd_t* data, int n);
        void encode_RT();
        void encode_RD();
    };
}
