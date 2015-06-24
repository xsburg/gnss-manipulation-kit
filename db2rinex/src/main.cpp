#include <QtCore/QtCore>
#include "Common/Logger.h"
#include "Common/Path.h"
#include "Common/Connection.h"
#include "Greis/MySqlSource.h"
#include <Greis/RinexReader.h>

using namespace Common;
using namespace Greis;

int main(int argc, char **argv)
{
    Connection::SharedPtr_t connection;
    try
    {
        // Qt encoding routines
        QCoreApplication a(argc, argv);
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);

        // Setting up command line parsing
        QCommandLineParser parser;
        parser.setApplicationDescription("This program reads data for a time range from the database and exports it into a set of rinex files. Default range is [now - 1_day, now]");
        QCommandLineOption databaseOption("database", "Database name", "databaseName");
        QCommandLineOption userNameOption("username", "Username to login into the database", "userName");
        QCommandLineOption passwordOption("password", "Password to login into the database", "password");
        QCommandLineOption hostOption("host", "Address where the database is located", "hostName");
        QCommandLineOption portOption("port", "Port used to connect to the database", "port");
        QCommandLineOption dateFromOption("dateFrom", "date range should be specified in either 'yyyy-MM-dd HH:mm:ss' or `msecs since epoch` format in UTC time", "from");
        QCommandLineOption dateToOption("dateTo", "date range should be specified in either 'yyyy-MM-dd HH:mm:ss' or `msecs since epoch` format in UTC time", "to");

        QCommandLineOption verboseOption("verbose", "Verbose level (0 = fatal only, 5 = debug)", "logLevel");
        parser.addHelpOption();
        parser.addVersionOption();
        parser.addOption(databaseOption);
        parser.addOption(userNameOption);
        parser.addOption(passwordOption);
        parser.addOption(hostOption);
        parser.addOption(portOption);
        parser.addOption(verboseOption);
        parser.addOption(dateFromOption);
        parser.addOption(dateToOption);
        parser.addPositionalArgument("outFile", "Output rinex file name (without extension)");
        parser.process(a);

        // Settings initialization
        sIniSettings.Initialize(Path::Combine(Path::ApplicationDirPath(), "config.ini"));

        // Logger initialization
        int logLevel;
        if (parser.isSet(verboseOption))
        {
            logLevel = parser.value(verboseOption).toInt();
        }
        else
        {
            logLevel = sIniSettings.value("LogLevel", 5).toInt();
        }
        sLogger.Initialize(logLevel);

        // Reading date range parameters
        QDateTime from;
        QDateTime to;
        if (!parser.isSet(dateFromOption))
        {
            from = QDateTime::currentDateTimeUtc().addDays(-1);
        }
        else
        {
            bool bOk;
            auto dateFromString = parser.value(dateFromOption);
            unsigned long long val = dateFromString.toULongLong(&bOk);
            if (bOk)
            {
                from = QDateTime::fromMSecsSinceEpoch(val);
            }
            else {
                from = QDateTime::fromString(dateFromString, "yyyy-MM-dd HH:mm:ss");
                from.setTimeSpec(Qt::UTC);
            }
            if (!from.isValid())
            {
                throw Exception("Invalid `dateFrom` argument.");
            }
        }
        if (!parser.isSet(dateToOption))
        {
            to = QDateTime::currentDateTimeUtc();
        }
        else
        {
            bool bOk;
            auto dateToString = parser.value(dateToOption);
            unsigned long long val = dateToString.toULongLong(&bOk);
            if (bOk)
            {
                to = QDateTime::fromMSecsSinceEpoch(val);
            }
            else {
                to = QDateTime::fromString(dateToString, "yyyy-MM-dd HH:mm:ss");
                to.setTimeSpec(Qt::UTC);
            }
            if (!to.isValid())
            {
                throw Exception("Invalid `dateFrom` argument.");
            }
        }

        // Setting up database connection
        connection = Connection::FromSettings("Db");
        if (parser.isSet(databaseOption))
        {
            connection->DatabaseName = parser.value(databaseOption);
        }
        if (parser.isSet(userNameOption))
        {
            connection->Username = parser.value(userNameOption);
        }
        if (parser.isSet(passwordOption))
        {
            connection->Password = parser.value(passwordOption);
        }
        if (parser.isSet(hostOption))
        {
            connection->Hostname = parser.value(hostOption);
        }
        if (parser.isSet(portOption))
        {
            connection->Port = parser.value(portOption).toInt();
        }
        connection->Connect();

        // Parsing
        sLogger.Info(QString("Reading data for the range [`%1`, `%2`] (%3 - %4)...")
            .arg(from.toString("dd.MM.yyyy HH:mm:ss"))
            .arg(to.toString("dd.MM.yyyy HH:mm:ss"))
            .arg(from.toMSecsSinceEpoch())
            .arg(to.toMSecsSinceEpoch()));
        auto mySqlSource = make_unique<MySqlSource>(connection.get());
        auto dataChunk = mySqlSource->ReadRange(from, to);

        QString fileNameTemplate = parser.positionalArguments().at(0);
        QString fileNameObs = fileNameTemplate + ".obs";
        QString fileNameNav = fileNameTemplate + ".nav";
        sLogger.Info("Converting data into the rinex format...");
        auto gnssData = RtkAdapter().toGnssData(dataChunk.get());
        sLogger.Info("Saving data...");
        RinexWriter(gnssData).WriteObsFile(fileNameObs).WriteNavFile(fileNameNav);
        sLogger.Info("Done!");
        sLogger.Info(QString("Data successfully written into: `%1`, `%2`.").arg(fileNameObs).arg(fileNameNav));

        return 0;
    }
    catch (Exception& e)
    {
        sLogger.Error("An application error has occurred");
        sLogger.Error(e.what());
        return 1;
    }
}
