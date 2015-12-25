#pragma once

#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include "Common/Exception.h"
#include "Common/Connection.h"
#include <Common/ConnectionPool.h>

namespace Platform
{
    namespace Tests
    {
        class BaseTest : public ::testing::Test
        {
        private:
            std::shared_ptr<Common::ConnectionPool> _connectionPool;
        protected:
            BaseTest();

            virtual ~BaseTest();

            virtual void SetUp();
            virtual void TearDown();

            const std::shared_ptr<Common::Connection>& Connection() const;
            const std::shared_ptr<Common::ConnectionPool>& ConnectionPool() const;
            QString ResolvePath(const QString& fileName) const;

            QByteArray ReadJpsBinary(const QString& fileName) const;
        };
    }
}
