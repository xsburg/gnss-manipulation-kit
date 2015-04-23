#include "ServiceManager.h"
#include <EControlCommand.h>
#include <Greis/StdMessage.h>

Platform::ServiceManager::ServiceManager(Connection::SharedPtr_t connection)
{
    _connection = connection;
    IsRestartRequiredFlag = false;
    IsShutdownRequiredFlag = false;
    IsPausedFlag = false;
    ServiceStatus = QVariantMap();
    messagesHandled = 0;
    if(interopEnabled)sLogger.Info("Provisioning via local database is now active.");
}

void Platform::ServiceManager::HandlePendingCommands()
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

void Platform::ServiceManager::PushMessageStats()
{
    sLogger.Info("Pushing the message statistics data...");
    QMap<std::string, int> receivedById;
    QMap<std::string, int> averageSizeById;
    QSqlQuery query = _connection->DbHelper()->ExecuteQuery("SELECT `messageId`, `received`, `averageSize` from `messageStatistics`");
    while (query.next())
    {
        QString qmessageId = query.value(0).toString();
        QByteArray amessageId = qmessageId.toLatin1();
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
        QString qmessageId = QString::fromLatin1(messageId.c_str(), messageId.size());
        auto received = it->total;
        auto averageSize = it->GetAverageSize(0);
        auto currentDate = QDateTime::currentDateTimeUtc();

        QString insertQuery = "INSERT INTO `messageStatistics` (`messageId`, `received`, `averageSize`, `startedAt`) VALUES (?, ?, ?, ?)";
        QSqlQuery query = _connection->DbHelper()->ExecuteQuery("");
        query.prepare(insertQuery);
        DatabaseHelper::ThrowIfError(query);
        query.addBindValue(qmessageId);
        query.addBindValue(received);
        query.addBindValue(averageSize);
        query.addBindValue(currentDate);
        query.exec();
        DatabaseHelper::ThrowIfError(query);
        sLogger.Info(QString("[id: %1, received: %2, averageSize: %3]").arg(qmessageId).arg(received).arg(averageSize));
    }
    messageStatsById.clear();

    messagesHandled = 0;
    sLogger.Info("Done.");
}

void Platform::ServiceManager::HandleMessage(Greis::Message* msg)
{
    switch (msg->Kind())
    {
    case Greis::EMessageKind::StdMessage:
        {
            auto stdMsg = static_cast<Greis::StdMessage*>(msg);
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
            stats.Handle(size);
            break;
        }
    }
    messagesHandled++;
    if (messagesHandled > MessageHandlingBufferSize)
    {
        PushMessageStats();
    }
}

void Platform::ServiceManager::resetMessageStatistics()
{
    messageStatsById.clear();
    _connection->DbHelper()->ExecuteQuery("DELETE FROM messageStatistics");
}

void Platform::ServiceManager::addLogMessage(const QString& message, EServiceLogSeverity::Type severity)
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
    query.addBindValue(int(severity));
    query.exec();
    DatabaseHelper::ThrowIfError(query);
}