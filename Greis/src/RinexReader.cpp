#include <Greis/RinexReader.h>
#include <rtklib.h>

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


    return *this;
}

Greis::GnssData::SharedPtr_t Greis::RinexReader::BuildResult()
{
    return _gnssData;
}