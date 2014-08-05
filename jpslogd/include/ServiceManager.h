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

        struct MessageStats
        {
            static const int WindowSize = 1000;
            int total;
            QQueue<int> last1kOfSizes;

            MessageStats()
            {
                total = 0;
            }

            void Handle(int size)
            {
                total++;
                last1kOfSizes.append(size);
                while (last1kOfSizes.size() > WindowSize)
                {
                    last1kOfSizes.takeFirst();
                }
            }

            int GetAverageSize(int averageSize)
            {
                if (last1kOfSizes.size() > WindowSize / 2 || averageSize == 0)
                {
                    averageSize = 0;
                    for (auto it = last1kOfSizes.constBegin(); it != last1kOfSizes.constEnd(); it++)
                    {
                        averageSize += *it;
                    }
                    averageSize = averageSize / last1kOfSizes.size();
                }
                return averageSize;
            }
        };

        QMap<std::string, MessageStats> messageStatsById;
        int messagesHandled;
        static const int MessageHandlingBufferSize = 1000;
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
            messagesHandled = 0;
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

        void PushMessageStats()
        {
            sLogger.Info("Pushing the message statistics data...");
            QMap<std::string, int> receivedById;
            QMap<std::string, int> averageSizeById;
            QSqlQuery query = _connection->DbHelper()->ExecuteQuery("SELECT `messageId`, `received`, `averageSize` from `messageStatistics`");
            while (query.next())
            {
                QString qmessageId = query.value(0).toString();
                QByteArray amessageId = qmessageId.toAscii();
                std::string messageId = std::string(amessageId.data(), amessageId.size());
                auto received = query.value(1).toInt();
                auto averageSize = query.value(2).toInt();

                if (!messageStatsById.contains(messageId))
                {
                    continue;
                }

                MessageStats& stats = messageStatsById[messageId];
                received += stats.total;
                averageSize = stats.GetAverageSize(averageSize);

                QString insertQuery = "UPDATE `messageStatistics` SET `received` = ?, `averageSize` = ? WHERE `id` = ?";
                QSqlQuery query = _connection->DbHelper()->ExecuteQuery("");
                query.prepare(insertQuery);
                DatabaseHelper::ThrowIfError(query);
                query.addBindValue(received);
                query.addBindValue(averageSize);
                query.addBindValue(qmessageId);
                query.exec();
                DatabaseHelper::ThrowIfError(query);
                sLogger.Info(QString("[id: %1, received: %2, averageSize: %3]").arg(qmessageId).arg(received).arg(averageSize));
                messageStatsById.remove(messageId);
            }

            for (auto it = messageStatsById.begin(); it != messageStatsById.end(); it++)
            {
                auto messageId = it.key();
                QString qmessageId = QString::fromAscii(messageId.c_str(), messageId.size());
                auto received = it->total;
                auto averageSize = it->GetAverageSize(0);

                QString insertQuery = "INSERT INTO `messageStatistics` (`messageId`, `received`, `averageSize`) VALUES (?, ?, ?)";
                QSqlQuery query = _connection->DbHelper()->ExecuteQuery("");
                query.prepare(insertQuery);
                DatabaseHelper::ThrowIfError(query);
                query.addBindValue(qmessageId);
                query.addBindValue(received);
                query.addBindValue(averageSize);
                query.exec();
                DatabaseHelper::ThrowIfError(query);
                sLogger.Info(QString("[id: %1, received: %2, averageSize: %3]").arg(qmessageId).arg(received).arg(averageSize));
                messageStatsById.remove(messageId);
            }

            messagesHandled = 0;
            sLogger.Info("Done.");
        }

        void HandleMessage(Message* msg)
        {
            switch (msg->Kind())
            {
            case EMessageKind::StdMessage:
                {
                    auto stdMsg = (StdMessage*)msg;
                    auto id = stdMsg->Id();
                    MessageStats& stats = messageStatsById[id];
                    stats.Handle(stdMsg->Size());
                    break;
                }
            default:
                {
                    auto id = std::string("OTHERS");
                    int size = msg->Size();
                    MessageStats& stats = messageStatsById[id];
                    stats.Handle(msg->Size());
                    break;
                }
            }
            messagesHandled++;
            if (messagesHandled > MessageHandlingBufferSize)
            {
                PushMessageStats();
            }
        }

        void resetMessageStatistics()
        {
            messageStatsById.clear();
            _connection->DbHelper()->ExecuteQuery("DELETE FROM messageStatistics");
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
