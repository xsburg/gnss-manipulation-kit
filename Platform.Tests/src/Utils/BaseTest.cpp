#include "Utils/BaseTest.h"
#include "Common/File.h"
#include <Greis/MySqlSource.h>
#include <Common/ConnectionPool.h>

namespace Platform
{
    namespace Tests
    {
        BaseTest::BaseTest()
        {

        }

        BaseTest::~BaseTest()
        {

        }

        void BaseTest::SetUp()
        {
            sLogger.Info("Connecting to the test database...");
            this->_connectionPool = std::make_shared<Common::ConnectionPool>(Common::Connection::FromSettings("Db"));

            this->Connection()->DbHelper()->ExecuteQuery("SET autocommit=0;");

            sLogger.Info("Starting a new transaction...");
            ASSERT_TRUE(this->Connection()->Database().driver()->hasFeature(QSqlDriver::Transactions));
            bool transactionStarted = this->Connection()->Database().transaction();
            if (!transactionStarted)
            {
                auto errText = this->Connection()->Database().lastError().text();
                throw Common::Exception("Failed to start a database transaction: " + errText);
            }
            sLogger.Info("Testing that database is empty...");
            auto source = std::make_shared<Greis::MySqlSource>(this->Connection());
            ASSERT_EQ(source->ReadAll()->Body().size(), 0);
            sLogger.Info("SetUp Succeeded...");
        }

        void BaseTest::TearDown()
        {
            this->Connection()->Database().rollback();
            sLogger.Info("Transaction has been reverted.");
        }

        const std::shared_ptr<Common::Connection>& BaseTest::Connection() const
        {
            return this->_connectionPool->getConnectionForCurrentThread();
        }

        const std::shared_ptr<Common::ConnectionPool>& BaseTest::ConnectionPool() const
        {
            return this->_connectionPool;
        }

        QString BaseTest::ResolvePath(const QString& fileName) const
        {
            QString baseDir = QCoreApplication::applicationDirPath();
            QString fullPath1(baseDir + "/../../../TestData/" + fileName);
            if (QFile::exists(fullPath1))
            {
                return fullPath1;
            }

            QString fullPath2 = baseDir + "/../../TestData/" + fileName;
            if (QFile::exists(fullPath2))
            {
                return fullPath2;
            }

            throw Common::Exception(QString("File %1 does not exist. Checked pathes: %2 %3.").arg(fileName).arg(fullPath1).arg(fullPath2));
        }

        QByteArray BaseTest::ReadJpsBinary(const QString& fileName) const
        {
            QByteArray binaryData;
            auto file = Common::File::OpenReadBinary(fileName);
            binaryData = file->readAll();
            int i;
            for (i = 0; i < binaryData.size(); i++)
            {
                if (binaryData.at(i) != '\0')
                {
                    break;
                }
            }
            binaryData.remove(0, i);
            return binaryData;
        }

    }
}
