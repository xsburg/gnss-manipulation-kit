#pragma once

#include "ProjectBase/SmartPtr.h"
#include "ProjectBase/Connection.h"
#include "EControlCommand.h"
#include "EServiceLogSeverity.h"
#include "ProjectBase/IniSettings.h"
#include <QtCore/QtCore>

using namespace ProjectBase;

namespace jpslogd
{
    class ServiceManager
    {
    private:
        Connection::SharedPtr_t _connection;
        QMap<QString, int> _statisticsCache;
    public:
        SMART_PTR_T(ServiceManager);

        bool IsRestartRequiredFlag;
        bool IsShutdownRequiredFlag;
        bool IsPausedFlag;
        bool interopEnabled;

        QVariantMap ServiceStatus;

        ServiceManager(Connection::SharedPtr_t connection)
        {
            _connection = connection;
            IsRestartRequiredFlag = false;
            IsShutdownRequiredFlag = false;
            IsPausedFlag = false;
            ServiceStatus = QVariantMap();
            if(interopEnabled)sLogger.Info("Provisioning via local database is now active.");
        }

        void HandlePendingCommands()
        {
            // Unconditionally update service status
            
            ServiceStatus["buildnumber"] = "999";
            ServiceStatus["builddate"] = __DATE__;
            ServiceStatus["datacenteruri"] = sIniSettings.value("remoteDatabase.Driver", "").toString()+"://"+sIniSettings.value("remoteDatabase.Hostname", "").toString()+"/"+sIniSettings.value("remoteDatabase.DatabaseName", "").toString();
            ServiceStatus["datacentertransfer"] = (sIniSettings.value("remoteDatabase.Driver", "").toString()=="") ? "0" : "1";
            ServiceStatus["paused"] = IsPausedFlag;
            ServiceStatus["trasferstate"] = "0"; // FIX

            ServiceStatus["receiverport"] = sIniSettings.value("PortName").toString();
            QString insertQuery = "INSERT INTO `status` (`name`, `value`) VALUES (?, ?) ON DUPLICATE KEY UPDATE `value`=VALUES(`value`)";
            QSqlQuery query = _connection->DbHelper()->ExecuteQuery("");
            query.prepare(insertQuery);
            DatabaseHelper::ThrowIfError(query);
            QList<QVariant> names;
            QListIterator<QString> i(ServiceStatus.keys());
            while (i.hasNext()) {
             names << i.next();
            }
            query.addBindValue(names);
            query.addBindValue(ServiceStatus.values());
            query.execBatch();
            DatabaseHelper::ThrowIfError(query);


            query = _connection->DbHelper()->ExecuteQuery("SELECT id, command_id, arguments FROM commandqueue ORDER BY id;");
            while (query.next())
            {
                int id = query.value(0).toInt();
                int commandId = query.value(1).toInt();
                QString arguments = query.value(2).toString();

                switch (commandId)
                {
                case EControlCommand::SetConfigProperties:
                    {
                        auto propertyStrings = arguments.split(";");
                        foreach (QString propertyString, propertyStrings)
                        {
                            auto tokens = propertyString.split("=");
                            if (tokens.count() < 2)
                            {
                                addLogMessage("The service has received an invalid command.", EServiceLogSeverity::Warning);
                                continue;
                            }
                            auto name = tokens[0];
                            tokens.removeAt(0);
                            auto value = tokens.join("=");
                            sIniSettings.setValue(name, value);
                            addLogMessage(QString("The property %1 was assigned the value '%2'. The restart is required to apply the changes.").arg(name).arg(value), EServiceLogSeverity::Info);
                        }
                    }
                    break;
                case EControlCommand::Restart:
                    IsRestartRequiredFlag = true;
                    addLogMessage(QString("The service has received the restart command. The receiving loop is being restarted."), EServiceLogSeverity::Info);
                    break;
                case EControlCommand::Shutdown:
                    IsShutdownRequiredFlag = true;
                    addLogMessage(QString("The service has received the shutdown command and is shutting down."), EServiceLogSeverity::Info);
                    break;
                case EControlCommand::Pause:
                    IsPausedFlag = true;
                    addLogMessage(QString("The service has received the pause command and is getting paused."), EServiceLogSeverity::Info);
                    break;
                case EControlCommand::Resume:
                    IsPausedFlag = false;
                    addLogMessage(QString("The service has received the resume command and is getting back active."), EServiceLogSeverity::Info);
                    break;
                case EControlCommand::UpdateConfigInfo:
                    {
                        _connection->DbHelper()->ExecuteQuery("DELETE FROM `configinfo`");
                        QString insertQuery = "INSERT INTO `configinfo` (`name`, `value`) VALUES (?, ?)";
                        QSqlQuery query = _connection->DbHelper()->ExecuteQuery("");
                        query.prepare(insertQuery);
                        DatabaseHelper::ThrowIfError(query);
                        QVariantList keys;
                        QVariantList values;
                        foreach (auto& k, sIniSettings.settings()->allKeys())
                        {
                            keys.push_back(k);
                            values.push_back(sIniSettings.value(k));
                        }
                        query.addBindValue(keys);
                        query.addBindValue(values);
                        query.execBatch();
                        DatabaseHelper::ThrowIfError(query);
                    }
                    break;
                }

                _connection->DbHelper()->ExecuteQuery(QString("DELETE FROM COMMANDQUEUE WHERE id = %1;").arg(id));
            }
        }

        void resetMessageStatistics()
        {
            _connection->DbHelper()->ExecuteQuery("DELETE FROM messageStatistics");
        }

        void updateMessageStatistics(const QString& messageCode, int incrementedBy)
        {

        }

        void commitMessageStatistics()
        {
            _statisticsCache;
        }

        void addLogMessage(const QString& message, EServiceLogSeverity::Type severity)
        {
            switch (severity)
            {
            case EServiceLogSeverity::Debug:
                sLogger.Debug(message);
                break;
            case EServiceLogSeverity::Info:
                sLogger.Info(message);
                break;
            case EServiceLogSeverity::Warning:
                sLogger.Warn(message);
                break;
            case EServiceLogSeverity::Error:
                sLogger.Error(message);
                break;
            case EServiceLogSeverity::Fatal:
                sLogger.Fatal(message);
                break;
            }

            QSqlQuery query = _connection->DbHelper()->ExecuteQuery("");
            QString insertQuery = "INSERT INTO `servicelog` (`timeStamp`, `message`, `severity_id`, `type`) VALUES (?, ?, ?, 0)";
            query.prepare(insertQuery);
            DatabaseHelper::ThrowIfError(query);
            query.addBindValue(QDateTime::currentDateTimeUtc());
            query.addBindValue(message);
            query.addBindValue((int)severity);
            query.exec();
            DatabaseHelper::ThrowIfError(query);
        }
    };
}
