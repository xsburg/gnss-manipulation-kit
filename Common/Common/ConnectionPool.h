// ***********************************************************************
// <author>Stepan Burguchev</author>
// <copyright company="Comindware">
//   Copyright (c) Comindware 2010-2015. All rights reserved.
// </copyright>
// <summary>
//   ConnectionPool.h
// </summary>
// ***********************************************************************
#pragma once

#include "SmartPtr.h"
#include "Connection.h"

namespace Common
{
    class ConnectionPool
    {
        QMap<std::thread::id, Connection::SharedPtr_t> _connectionByThreadId;
        Connection::SharedPtr_t _connectionParameters;
    public:
        SMART_PTR_T(ConnectionPool);

        explicit ConnectionPool(Connection::SharedPtr_t connectionParameters);

        Connection::SharedPtr_t getConnectionForCurrentThread();
    };
}
