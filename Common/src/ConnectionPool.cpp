#include "ConnectionPool.h"

Common::ConnectionPool::ConnectionPool(Connection::SharedPtr_t connectionParameters)
    : _connectionParameters(connectionParameters)
{
}

Common::Connection::SharedPtr_t Common::ConnectionPool::getConnectionForCurrentThread()
{
    auto threadId = std::this_thread::get_id();
    auto connection = _connectionByThreadId.value(threadId);
    if (connection.get())
    {
        // Reusing the old connection
        connection->Connect();
        return connection;
    }

    // Creating a new connection
    connection = _connectionParameters->Clone();
    _connectionByThreadId[threadId] = connection;
    connection->Connect();
    return connection;
}
