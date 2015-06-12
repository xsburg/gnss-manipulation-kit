#pragma once

#include <QtCore/QString>
#include "Common/File.h"
#include "Common/Logger.h"
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
        }

        ~GnssData()
        {
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

        obs_t obs;
        nav_t nav;
        sta_t sta;
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
    private:
        GnssData::SharedPtr_t _gnssData;
    };
}
