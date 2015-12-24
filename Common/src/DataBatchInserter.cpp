#include "DataBatchInserter.h"
#include "Logger.h"
#include <QtConcurrent>
#include "InvalidOperationException.h"

namespace Common
{
    DataBatchInserter::DataBatchInserter(
        const QString& insertQuery, 
        int boundColumnsCount, 
        ConnectionPool::SharedPtr_t connectionPool, 
        const QString& tableName,
        int batchSize)
        : _connectionPool(connectionPool)
    {
        _insertQuery = insertQuery;
        _rowsAdded = 0;
        _batchSize = batchSize;
        _boundValues.resize(boundColumnsCount);
        _tableName = tableName;
    }

    DataBatchInserter::~DataBatchInserter()
    {
    }

    void DataBatchInserter::AddChild(DataBatchInserter::SharedPtr_t child)
    {
        _children.push_back(child);
    }

    bool DataBatchInserter::NeedsFlush()
    {
        if (_rowsAdded >= _batchSize)
        {
            return true;
        }
        for (auto& child : _children)
        {
            if (child->NeedsFlush())
            {
                return true;
            }
        }
        return false;
    }

    void DataBatchInserter::AddRow(const QList<QVariant>& values)
    {
        if (values.size() != _boundValues.size())
        {
            throw InvalidOperationException(
                QString("Invalid elements count in `AddRow(values)` array. Actual: %1. Expected: %2.").
                arg(values.size()).arg(_boundValues.size()));
        }

        for (int i = 0; i < _boundValues.size(); ++i)
        {
            _boundValues[i].append(values.at(i));
        }
        _rowsAdded++;
    }

    void DataBatchInserter::Clear()
    {
        foreach(DataBatchInserter::SharedPtr_t child, _children)
        {
            child->Clear();
        }

        if (_rowsAdded == 0)
        {
            return;
        }
        _rowsAdded = 0;
        int size = _boundValues.size();
        _boundValues.clear();
        _boundValues.resize(size);
    }

    QFuture<void> DataBatchInserter::Flush()
    {
        if (_rowsAdded > 0)
        {
            auto childrenFlush = std::make_shared<QFutureSynchronizer<void>>();
            foreach (DataBatchInserter::SharedPtr_t child, _children)
            {
                childrenFlush->addFuture(child->Flush());
            }

            auto insertQueryTmp = _insertQuery;
            auto valuesTmp = QString(",(?");
            for (int i = 1; i < _boundValues.size(); i++)
            {
                valuesTmp.append(",?");
            }
            valuesTmp.append(")");

            for (int i = 1; i < _rowsAdded; i++)
            {
                insertQueryTmp.append(valuesTmp);
            }

            // data copy
            QVector<QVariantList> boundValuesCopy;
            for (auto& valList : _boundValues)
            {
                QVariantList listCopy;
                for (auto& val : valList)
                {
                    listCopy.append(val);
                }
                boundValuesCopy.append(listCopy);
            }
            int rowsAdded = _rowsAdded;

            Clear();

            return QtConcurrent::run([childrenFlush, boundValuesCopy, rowsAdded, insertQueryTmp, this]()
            {
                childrenFlush->waitForFinished();
                // New connection build
                auto connection = _connectionPool->getConnectionForCurrentThread();

                auto query = std::make_shared<QSqlQuery>(connection->Database());
                connection->DbHelper()->ThrowIfError(*query.get());
                //newConnection->Database().transaction();
                //sLogger.Debug(insertQueryTmp);
                query->prepare(insertQueryTmp);
                DatabaseHelper::ThrowIfError(*query.get());
                int varsCount = 0;

                auto colSize = boundValuesCopy.size();
                for (int i = 0; i < rowsAdded; i++)
                {
                    for (int j = 0; j < colSize; j++)
                    {
                        query->addBindValue(boundValuesCopy[j][i]);
                        varsCount++;
                    }
                }
                auto valid = varsCount == boundValuesCopy.size() * rowsAdded;

                // Execution
                query->exec();
                DatabaseHelper::ThrowIfError(*query.get());
                if (_tableName.isEmpty() || _tableName.isNull())
                {
                    sLogger.Trace(QString("%1 records has been added.").arg(rowsAdded));
                }
                else {
                    sLogger.Trace(QString("%1 records has been added into `%2`.").arg(rowsAdded).arg(_tableName));
                }
            });
        }
        else
        {
            return QFuture<void>();
        }
    }
}
