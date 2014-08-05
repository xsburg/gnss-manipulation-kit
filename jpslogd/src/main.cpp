#include <QtCore/QtCore>
#include <clocale>
#include <locale>
#include "Common/Logger.h"
#include "Common/SmartPtr.h"
#include "Common/Path.h"
#include "Common/Connection.h"
#include "Greis/DataChunk.h"
#include "Greis/MySqlSink.h"
#include "Greis/SerialPortBinaryStream.h"
#include "Greis/FileBinaryStream.h"
#include "SerialStreamReader.h"
#include "ChainedSink.h"
#include "ServiceManager.h"

using namespace Common;
using namespace Greis;
using namespace jpslogd;


#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif
void qSleep(int ms)
{
#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

namespace jpslogd
{
    bool startLoop()
    {
        SerialPortBinaryStream::SharedPtr_t serialPort;
        ChainedSink::UniquePtr_t dataCenterSink;
        ChainedSink::UniquePtr_t localSink;

        try
        {
            std::string portName = sIniSettings.value("PortName", "COM3").toString().toStdString();
            unsigned int baudRate = sIniSettings.value("BaudRate", 1500000).toUInt();
            int inserterBatchSize = sIniSettings.value("inserterBatchSize", 250).toInt();
            int dataChunkSize = sIniSettings.value("dataChunkSize", 250).toInt();;
            sLogger.Info("Connecting Javad receiver on "+QString::fromStdString(portName)+" at "+QString::number(baudRate)+"bps...");
            serialPort = std::make_shared<SerialPortBinaryStream>(portName, baudRate);
            // Disable running monitoring
            serialPort->write("\n\n\ndm\n");
            qSleep(1000);
            serialPort->write("dm\r\n\r\ndm\r\n\n\rdm\r\n");
            qSleep(2000);
            serialPort->write("dm\n\r");
            //
            auto serialStream = SerialStreamReader(serialPort);
            // Get recevier data
            serialPort->write("\nprint,/par/rcv/id\n");
            QString _receiverid = serialStream.readLine();
            _receiverid = _receiverid.mid(6,-1);
            serialPort->write("\nprint,/par/rcv/model\n");
            QString _receivermodel = serialStream.readLine();
            _receivermodel = _receivermodel.mid(6,-1);
            serialPort->write("\nprint,/par/rcv/ver/main\n");
            QString _receiverfw = serialStream.readLine();
            _receiverfw = _receiverfw.mid(6,-1);
            serialPort->write("\nprint,/par/rcv/ver/board\n");
            QString _receiverboard = serialStream.readLine();
            _receiverboard = _receiverboard.mid(6,-1);
            sLogger.Info("Connected device is "+_receivermodel+", board "+_receiverboard+" (ID:"+_receiverid+", FW:"+_receiverfw+")");
            // Configure device for data output
            serialPort->write("\nem,,def,/msg/jps/AZ,/msg/jps/r1,/msg/jps/r2,/msg/jps/RD{10.00,0.00,0.00,0x0002},/msg/jps/rc\n");
            GreisMessageStream stream(serialPort, true, false);
            sLogger.Info("Configuring databases...");		
            // Preparing the acquisition sink
            auto dataChunk = make_unique<DataChunk>();
            auto localConnection = Connection::FromSettings("LocalDatabase");
            if(localConnection->Driver=="" || localConnection->Hostname=="" || localConnection->Username==""){
                sLogger.Fatal("Cannot configure local database, check configuraion.");		
                throw new GreisException("Local database configuration missing.");
            } else {
                sLogger.Info("Using local database "+localConnection->DatabaseName+" on "+localConnection->Hostname);
            }
            auto remoteConnection = Connection::FromSettings("RemoteDatabase");
            if(remoteConnection->Driver!=""){
            dataCenterSink = make_unique<ChainedSink>(remoteConnection, inserterBatchSize, nullptr);
            localSink = make_unique<ChainedSink>(localConnection, inserterBatchSize, std::move(dataCenterSink));
            if(!dataCenterSink->IsValid())return true;
            sLogger.Info("Using remote database "+localConnection->DatabaseName+" on "+localConnection->Hostname);
            } else {
            localSink = make_unique<ChainedSink>(localConnection, inserterBatchSize, nullptr);
            }
            if(!localSink->IsValid())return true;
            sLogger.Info("Configuring provisioning via local database...");
            auto serviceManager	 = make_unique<ServiceManager>(localConnection);
            // Set receiver properties
            serviceManager->ServiceStatus["receiverid"]=_receiverid;
            serviceManager->ServiceStatus["receiverfw"]=_receiverfw;
            serviceManager->ServiceStatus["receivermodel"]=_receivermodel;
            serviceManager->ServiceStatus["receiverboard"]=_receiverboard;

            int msgCounter = 0;
            Message::UniquePtr_t msg;
            while((msg = stream.Next()).get())
            {
                serviceManager->HandleMessage(msg.get());

                dataChunk->AddMessage(std::move(msg));
                if (msgCounter++ > dataChunkSize)
                {
                    localSink->Handle(std::move(dataChunk));
                    dataChunk = make_unique<DataChunk>();
                    msgCounter = 0;
                }

                if (msgCounter % 100 == 0)
                {
                    sLogger.Debug("Another 100 has been received.");

                        // Checking for the control commands
                    serviceManager->HandlePendingCommands();
                    if (serviceManager->IsRestartRequiredFlag)
                    {
                        serviceManager->IsRestartRequiredFlag = false;
                        localSink->Flush();
                        return false;
                    }
                    if (serviceManager->IsShutdownRequiredFlag)
                    {
                        serviceManager->IsShutdownRequiredFlag = false;
                        localSink->Flush();
                        return true;
                    }
                    if (serviceManager->IsPausedFlag)
                    {
                        const int sleepIntervalInMilliseconds = 1000;
                        localSink->Flush();
                        while (serviceManager->IsPausedFlag)
                        {
                            qSleep(sleepIntervalInMilliseconds);
                            serviceManager->HandlePendingCommands();
                            if (serviceManager->IsRestartRequiredFlag)
                            {
                                serviceManager->IsRestartRequiredFlag = false;
                                return false;
                            }
                            if (serviceManager->IsShutdownRequiredFlag)
                            {
                                serviceManager->IsShutdownRequiredFlag = false;
                                return true;
                            }
                        }
                    }
                    // Update status table

                }
            }

            localSink->Flush();
            return true;
        }
        catch (GreisException& ex)
        {
            sLogger.Error("An error has occurred: " + ex.what());

            if (localSink.get())
            {
                localSink->Flush();
            }

            if (serialPort.get() && serialPort->isOpen())
            {
                serialPort->write("\ndm\n");
                serialPort->close();
            }

            return false;
        }
        catch (boost::system::system_error& bex)
        {
            if(bex.code().value()==2){
            sLogger.Error("Connection to receiver could not be made.");
            sLogger.Error("The application could not find the port specified.");
            sLogger.Error("It is ether wrong configuration or the receiver moved to another port.");
            } else {
            sLogger.Error("No connection to receiver, code: " + QString::number(bex.code().value()));
            }
            if (localSink.get())
            {
                localSink->Flush();
            }
            return false;
        }
        catch (...)
        {
            sLogger.Error("Something bad has happened. Queueing restart.");
            return false;
        }
    }
}


int main(int argc, char **argv)
{
    try
    {
        //std::setlocale(LC_ALL, "Russian_Russia.1251");
        //std::locale::global(std::locale("Russian_Russia.1251"));

        QCoreApplication a(argc, argv);

        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);
        
        sIniSettings.Initialize(Path::Combine(Path::ApplicationDirPath(), "config.ini"));

        sLogger.Initialize(sIniSettings.value("LogLevel", 5).toInt());

        sLogger.Debug("The following sqldrivers are available:");
        auto sqlDrivers = QSqlDatabase::drivers();
        for (auto it = sqlDrivers.begin(); it != sqlDrivers.end(); it++)
        {
            QString sqlDriverName = *it;
            sLogger.Debug(sqlDriverName);
        }

        QStringList args = a.arguments();
        QString RootPassword = "";
        QRegExp rxArgRootPassword("--root-password=(\\S+)");
        QRegExp rxArgSetup("--setup");
        bool doSetup = false;

        for (int i = 1; i < args.size(); ++i) {
            if (rxArgRootPassword.indexIn(args.at(i)) != -1 ) {   
                RootPassword =  rxArgRootPassword.cap(1);
            }
            else if (rxArgSetup.indexIn(args.at(i)) != -1 ) {   
                sLogger.Warn("Doing first time setup...");
                doSetup = true;
            }
        }
        if(doSetup)
            {
                if(RootPassword!="")
                {
                    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
                    db.setHostName("localhost");
                    db.setUserName("root");
                    db.setPassword(RootPassword);
                    QFile baselineFile(Path::Combine(Path::ApplicationDirPath(), "baseline.sql"));
                    if (!baselineFile.open(QIODevice::ReadOnly | QIODevice::Text))
                        return  0;
                    QFile provisioningFile(Path::Combine(Path::ApplicationDirPath(), "jpslogd.sql"));
                    if (!provisioningFile.open(QIODevice::ReadOnly | QIODevice::Text))
                        return  0;
                    if(db.open()){
                    QSqlQuery query(db);
                    QTextStream in(&baselineFile);
                    QString sql = in.readAll();
                    QStringList sqlStatements = sql.split(';', QString::SkipEmptyParts);
                    
                        
                        query.exec("CREATE DATABASE `jpslogd`;");
                        query.exec("USE `jpslogd`;");
                        foreach(const QString& statement, sqlStatements)
                        {
                            if (statement.trimmed() != "")
                            {
                                if (!query.exec(statement))
                                    sLogger.Error("An error occured during setup: "+query.lastError().text());
                            }
                        }
                        query.exec("GRANT ALL PRIVILEGES ON *.* TO 'jpslogd'@localhost");

                    QTextStream in2(&provisioningFile);
                    sql = in2.readAll();
                    sqlStatements = sql.split(';', QString::SkipEmptyParts);
                        foreach(const QString& statement, sqlStatements)
                        {
                            if (statement.trimmed() != "")
                            {
                                if (!query.exec(statement))
                                    sLogger.Error("An error occured during setup: "+query.lastError().text());
                            }
                        }

                    }
                        return 0;


                } else {
                    sLogger.Fatal("Please specify root password.");
                }
            }


        while (!startLoop())
        {
        
            sLogger.Warn("An error occured, acquisition restart pending.");
            qSleep(1000);
        }

        return 0;
    }
    catch (Exception& e)
    {
        sLogger.Error(e.what());
        return 1;
    }
}
