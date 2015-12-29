#pragma once

#include <vector>
#include <cmath>
#include <gtest/gtest.h>
#include <boost/thread.hpp>
#include "Common/Exception.h"
#include "Common/ConnectionPool.h"

namespace Greis
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

            std::shared_ptr<Common::Connection> Connection();
            std::shared_ptr<Common::ConnectionPool>& ConnectionPool();

            QString ResolvePath(const QString& fileName) const;

            QByteArray ReadJpsBinary(const QString& fileName) const;
        };
    }
}
