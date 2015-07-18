#include "DataChunk.h"
#include "FileBinaryStream.h"
#include "Common/SmartPtr.h"

using namespace Common;

namespace Greis
{
    void DataChunk::FlushEpoch()
    {
        if (_lastEpoch->Messages.size() > 0)
        {
            _lastEpoch->DateTime = _dateTime;
            _body.push_back(std::move(_lastEpoch));
        }
    }

    DataChunk::UniquePtr_t DataChunk::FromFile(QString filename)
    {
        auto dataChunk = make_unique<DataChunk>();
        GreisMessageStream stream(std::make_shared<FileBinaryStream>(filename), false, false);
        
        // Collecting the head
        Message::UniquePtr_t msg;
        int index = 0;

        while((msg = stream.Next()).get())
        {
            if (msg->Kind() == EMessageKind::StdMessage)
            {
                auto stdMsg = dynamic_cast<StdMessage*>(msg.get());
                if (stdMsg->IdNumber() == EMessageId::RcvTime)
                {
                    dataChunk->updateTimePart(dynamic_cast<RcvTimeStdMessage*>(stdMsg));
                    break;
                }
                if (stdMsg->IdNumber() == EMessageId::RcvDate)
                {
        
                    dataChunk->updateDatePart(dynamic_cast<RcvDateStdMessage*>(stdMsg));
                }
            }

            // All messages go to _head until date and time is set
            dataChunk->_head.push_back(std::move(msg));
        }
        // if the head collection was interrupted by a RcvTime message, move it to the first epoch
        if (msg.get())
        {
            dataChunk->_lastEpoch->Messages.push_back(std::move(msg));
        }
        // Collecting a body
        while((msg = stream.Next()).get())
        {
            dataChunk->AddMessage(std::move(msg));
        }
        dataChunk->FlushEpoch();

        sLogger.Trace(QString("%1 epochs have been totally deserialized.").arg(dataChunk->_epochCounter));
        return dataChunk;
    }

    QByteArray DataChunk::ToByteArray() const
    {
        QByteArray result;
        for (const auto& msg : _head)
        {
            result.append(msg->ToByteArray());
        }
        for (const auto& epoch : _body)
        {
            for (const auto& msg : epoch->Messages)
            {
                result.append(msg->ToByteArray());
            }
        }
        return result;
    }

    void DataChunk::AddMessage( Message::UniquePtr_t msg )
    {
        if (msg->Kind() == EMessageKind::StdMessage)
        {
            auto stdMsg = dynamic_cast<StdMessage*>(msg.get());
            if (stdMsg->IdNumber() == EMessageId::RcvTime)
            {
                if (dateTimeIsSet())
                {
                    // This message is a part of a new epoch. 
                    // We should assign the dateTime value to the current epoch and push it to the epochs container.
                    _lastEpoch->DateTime = _dateTime;
                    _body.push_back(std::move(_lastEpoch));
                    // push in the previous epoch
                    if (++_epochCounter % 3000 == 0)
                    {
                        sLogger.Trace(QString("3000 epochs deserialized. %1 total.").arg(_epochCounter));
                    }
                    _lastEpoch = make_unique<Epoch>();
                }

                updateTimePart(dynamic_cast<RcvTimeStdMessage*>(stdMsg));
            }
            if (stdMsg->IdNumber() == EMessageId::RcvDate)
            {
                updateDatePart(dynamic_cast<RcvDateStdMessage*>(stdMsg));
            }
        }
        _lastEpoch->Messages.push_back(std::move(msg));
    }

    DataChunk::DataChunk()
    {
        _dateTime.setTimeSpec(Qt::UTC);
        _epochCounter = 0;
        _dateIsSet = false;
        _timeIsSet = false;
        _lastEpoch = make_unique<Epoch>();
    }
}
