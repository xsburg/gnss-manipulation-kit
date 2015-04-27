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

QString Platform::ServiceManager::GetStatus()
{
   sLogger.Debug("DBUS: getstatus");
}

QString Platform::ServiceManager::GetConfigKey (const QString &key)
{
   sLogger.Debug(QString("DBUS: getconfig: %1").arg(key));
}

bool Platform::ServiceManager::SetConfigKeyPersistent (const QString &key, const QString &value)
{

}

void Platform::ServiceManager::newDBusConnection(const QDBusConnection &connection) {
   DBusPeerConnection = new QDBusConnection(connection);
   DBusPeerConnection->registerService("ru.ifz.jpsutils.jpslogd"); 
   DBusPeerConnection->registerObject("/",this,QDBusConnection::ExportAllSlots);
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
