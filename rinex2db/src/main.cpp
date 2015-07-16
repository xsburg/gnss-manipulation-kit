#include <QtCore/QtCore>
#include "Common/Logger.h"
#include "Common/Path.h"
#include "Common/Connection.h"
#include "Greis/MySqlSink.h"
#include <Greis/RinexReader.h>

using namespace Common;
using namespace Greis;

int main(int argc, char **argv)
{
    bool wrapIntoTransaction;
    Connection::SharedPtr_t connection;
    try
    {
        // Qt encoding routines
        QCoreApplication a(argc, argv);
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);

        // Setting up command line parsing
        QCommandLineParser parser;
        parser.setApplicationDescription("This program reads data from rinex files and inserts it into the database.");
        QCommandLineOption databaseOption("database", "Database name", "databaseName");
        QCommandLineOption userNameOption("username", "Username to login into the database", "userName");
        QCommandLineOption passwordOption("password", "Password to login into the database", "password");
        QCommandLineOption hostOption("host", "Address where the database is located", "hostName");
        QCommandLineOption portOption("port", "Port used to connect to the database", "port");
        QCommandLineOption verboseOption("verbose", "Verbose level (0 = fatal only, 5 = debug)", "logLevel");
        parser.addHelpOption();
        parser.addVersionOption();
        parser.addOption(databaseOption);
        parser.addOption(userNameOption);
        parser.addOption(passwordOption);
        parser.addOption(hostOption);
        parser.addOption(portOption);
        parser.addOption(verboseOption);
        parser.addPositionalArgument("files", "Rinex files to parse (obs and nav)");
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

        // Setting up database connection
        wrapIntoTransaction = sIniSettings.value("WrapIntoTransaction", false).toBool();
        int inserterBatchSize = sIniSettings.value("inserterBatchSize", 10000).toInt();
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
        QStringList fileNames = parser.positionalArguments();
        RinexReader rinexReader;
        for (auto fileName : fileNames)
        {
            sLogger.Info(QString("Reading rinex file `%1`...").arg(fileName));
            rinexReader.ReadFile(fileName);
        }
        sLogger.Info(QString("Building gnss data bundle..."));
        auto gnssData = rinexReader.BuildResult();
        sLogger.Info(QString("Converting it to messages..."));
        auto dataChunk = RtkAdapter().toMessages(gnssData);
        sLogger.Info(QString("Adding messages to the database..."));
        auto sink = make_unique<MySqlSink>(connection.get(), inserterBatchSize);
        sink->AddJpsFile(dataChunk.get());
        sink->Flush();

        return 0;
    }
    catch (Exception& e)
    {
        if (wrapIntoTransaction)
        {
            connection->Database().rollback();
            sLogger.Info("Transaction has been rolled back.");
        }
        sLogger.Error(e.what());
        return 1;
    }
}
