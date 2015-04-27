#pragma once

#include <QObject>
#include <QtCore/QtCore>
#include "Common/SmartPtr.h"
#include "Common/Connection.h"
#include "EServiceLogSeverity.h"
#include <Greis/Message.h>
#include <QtDBus/QtDBus>

using namespace Common;

namespace Platform
{
    class ServiceManager: public QObject
    {
	Q_OBJECT
	
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
                    for (auto it = last1kOfSizes.constBegin(); it != last1kOfSizes.constEnd(); ++it)
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
    private:
	QDBusConnection * DBusPeerConnection;
    protected slots:
        void newDBusConnection(const QDBusConnection &connection);
    public slots:
	Q_SCRIPTABLE bool SetConfigKeyPersistent (const QString &key, const QString &value);
	Q_SCRIPTABLE QString GetConfigKey (const QString &key);
	Q_SCRIPTABLE QString GetStatus ();

    public:
        SMART_PTR_T(ServiceManager);
	
        bool IsRestartRequiredFlag;
        bool IsShutdownRequiredFlag;
        bool IsPausedFlag;
        bool interopEnabled;

        QVariantMap ServiceStatus;

        ServiceManager(Connection::SharedPtr_t connection);

        void PushMessageStats();

        void HandleMessage(Greis::Message* msg);

        void resetMessageStatistics();

        void addLogMessage(const QString& message, EServiceLogSeverity::Type severity);
    };
}
