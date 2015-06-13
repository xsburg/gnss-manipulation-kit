#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <Greis/RinexReader.h>
#include <Common/Exception.h>
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

Greis::GnssData::SharedPtr_t Greis::RtkAdapter::toGnssData(DataChunk* dataChunk)
{
    auto rawGnssData = new RawGnssData();
    GnssData::SharedPtr_t gnssData(rawGnssData);

    auto data = dataChunk->ToByteArray();

    for (size_t i = 0; i < data.size(); i++)
    {
        char c = data.at(i);
        input_javad(&rawGnssData->getRaw(), c);
    }
    
    return gnssData;
}

Greis::DataChunk::SharedPtr_t Greis::RtkAdapter::toMessages(GnssData::SharedPtr_t gnssData)
{
    DataChunk::SharedPtr_t dataChunk;

    encode_RT();
    encode_RD();
    encode_SI();
    encode_NN();

    return dataChunk;
}

void Greis::RtkAdapter::encode_RT()
{
    ReceiverTimeMessage();
    TOD = data.tod(?)
}
