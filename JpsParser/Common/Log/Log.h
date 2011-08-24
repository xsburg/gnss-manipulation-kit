#ifndef Log_h__
#define Log_h__

#include <QString>
#include <string>

#include "../Util/singleton.h"

using std::string;

namespace Common
{
    class Log
    {
        SINGLETON_BLOCK(Log)
    public:
        void addError(const QString& message, const QString& details = QString());
        void addWarning(const QString& message, const QString& details = QString());
        void addInfo(const QString& message, const QString& details = QString());
        void addError(const string& message, const string& details = string());
        void addWarning(const string& message, const string& details = string());
        void addInfo(const string& message, const string& details = string());

        void Initialize(bool displayMessages, const QString logFile);
    private:
        bool _displayMessages;
        QString _logFile;
        QTextCodec *_codec;
    private:
        void addToConsole(const QString& message, const QString& details = QString());
        QString asciiToQString(const string& str);
    };
} // Common

#define sLog SINGLETON_INSTANCE(::Common::Log)

#endif // Log_h__
