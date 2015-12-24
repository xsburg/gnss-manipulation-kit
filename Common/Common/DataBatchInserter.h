#ifndef DataBatchInserter_h__
#define DataBatchInserter_h__

#include <QtCore/QtCore>
#include <boost/utility.hpp>
#include "ConnectionPool.h"
#include "SmartPtr.h"

namespace Common 
{
    class DataBatchInserter : boost::noncopyable
    {
    public:
        SMART_PTR_T(DataBatchInserter);
    private:
        QString _insertQuery;
        QString _tableName;
        int _rowsAdded;
        int _batchSize;
        ConnectionPool::SharedPtr_t _connectionPool;
        QVector<QVariantList> _boundValues;
        QList<DataBatchInserter::SharedPtr_t> _children;
    public:
        // insertQuery: "INSERT INTO <table name>(<column name>[, <column name>]) VALUES (?, ?, ?)"
        // connection: pointer to connection class
        DataBatchInserter(const QString& insertQuery, int boundColumnsCount, ConnectionPool::SharedPtr_t connectionPool,
                          const QString& tableName = "", int batchSize = 1000);

        ~DataBatchInserter();

        void AddChild(DataBatchInserter::SharedPtr_t child);

        const QList<DataBatchInserter::SharedPtr_t>& GetChildren() const { return _children; }

        bool NeedsFlush();
        void AddRow(const QList<QVariant>& values);
        void Clear();
        // execute all pending queries
        QFuture<void> Flush();

        // returns pending query
        inline const QString& InsertQuery() const
        {
            return _insertQuery;
        }
    };
}

#endif // DataBatchInserter_h__