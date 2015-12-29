#include "Utils/BaseTest.h"
#include "Common/File.h"
#include <Greis/MySqlSource.h>
#include <Common/Path.h>

namespace Greis
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
            // Connecting
            sLogger.Info("Connecting to the test database...");
            this->_connectionPool = std::make_shared<Common::ConnectionPool>(Common::Connection::FromSettings("Db"));

            auto connection = this->Connection();
            auto connectionPool = this->ConnectionPool();

            connection->DbHelper()->ExecuteQuery("SET autocommit=0;");

            // Resetting the test database
            sLogger.Info("Database reset...");
            QString baseDir = QCoreApplication::applicationDirPath();
            QFile baselineFile(Path::Combine(Path::ApplicationDirPath(), "../../../Generator/Output/baseline.sql"));
            if (!baselineFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                throw Common::Exception("Failed to open baseline.sql: " + baselineFile.errorString());
            }

            QSqlQuery query(connection->Database());
            QTextStream in(&baselineFile);
            QString sql = in.readAll();
            QStringList sqlStatements = sql.split(';', QString::SkipEmptyParts);

            foreach(const QString& statement, sqlStatements)
            {
                if (!statement.trimmed().isEmpty())
                {
                    if (!query.exec(statement))
                    {
                        connection->DbHelper()->ThrowIfError(query);
                    }
                }
            }

            sLogger.Info("Asserting features...");
            ASSERT_TRUE(connection->Database().driver()->hasFeature(QSqlDriver::Transactions));

            sLogger.Info("Testing that database is empty...");
            auto source = std::make_shared<Greis::MySqlSource>(connection);
            ASSERT_EQ(source->ReadAll()->Body().size(), 0);
            sLogger.Info("SetUp Succeeded...");
        }

        void BaseTest::TearDown()
        {
        }

        std::shared_ptr<Common::Connection> BaseTest::Connection()
        {
            auto connection = this->_connectionPool->getConnectionForCurrentThread();
            return connection;
        }

        std::shared_ptr<Common::ConnectionPool>& BaseTest::ConnectionPool()
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
