#include "Logger.h"

#include <QtCore/QtCore>
#include <iostream>

namespace Common
{
    void Logger::Initialize(int logLevel)
    {
        if (logLevel < 0)
        {
            logLevel = 5;
        }

        this->logLevel = logLevel;
    }

    void Logger::Debug(const QString& message)
    {
        if (this->logLevel >= 5)
        {
            _outLock.lock();
            std::cerr << message.toStdString() << std::endl;
            _outLock.unlock();
        }
    }

    void Logger::Trace(const QString& message)
    {
        if (this->logLevel >= 4)
        {
            _outLock.lock();
            std::cerr << message.toStdString() << std::endl;
            _outLock.unlock();
        }
    }

    void Logger::Info(const QString& message)
    {
        if (this->logLevel >= 3)
        {
            _outLock.lock();
            std::cout << message.toStdString() << std::endl;
            _outLock.unlock();
        }
    }

    void Logger::Warn(const QString& message)
    {
        if (this->logLevel >= 2)
        {
            _outLock.lock();
            std::cerr << message.toStdString() << std::endl;
            _outLock.unlock();
        }
    }

    void Logger::Error(const QString& message)
    {
        if (this->logLevel >= 1)
        {
            _outLock.lock();
            std::cerr << message.toStdString() << std::endl;
            _outLock.unlock();
        }
    }

    void Logger::Fatal(const QString& message)
    {
        if (this->logLevel >= 0)
        {
            _outLock.lock();
            std::cerr << "FATAL:" << message.toStdString() << std::endl;
            _outLock.unlock();
        }
    }
}
