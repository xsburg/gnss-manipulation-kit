#include "AngularVelocityStdMessage.h"
#include <cassert>
#include "ChecksumComputer.h"
#include "Common/Logger.h"

namespace Greis
{
    AngularVelocityStdMessage::AngularVelocityStdMessage( const char* pc_message, int p_length ) 
        : _id(pc_message, 2), _bodySize(p_length - HeadSize())
    {
        char* p_message = const_cast<char*>(pc_message);
        
        p_message += HeadSize();
    
        _serializer.Deserialize(p_message, _time);
        p_message += sizeof(_time);
        _serializer.Deserialize(p_message, _x);
        p_message += sizeof(_x);
        _serializer.Deserialize(p_message, _y);
        p_message += sizeof(_y);
        _serializer.Deserialize(p_message, _z);
        p_message += sizeof(_z);
        _serializer.Deserialize(p_message, _rms);
        p_message += sizeof(_rms);
        _serializer.Deserialize(p_message, _flags);
        p_message += sizeof(_flags);
        _serializer.Deserialize(p_message, _cs);
        p_message += sizeof(_cs);

        _isCorrect = (p_message - pc_message == p_length);
        if (!_isCorrect)
        {
            sLogger.Debug(QString("The message %1 is incorrect. Excepted size is %2 whilst the actual size is %3.")
                .arg(QString::fromStdString(ToString())).arg(p_length).arg(p_message - pc_message));
        }
    }
    
    AngularVelocityStdMessage::AngularVelocityStdMessage( const std::string& p_id, int p_size ) 
        : _id(p_id), _bodySize(p_size - HeadSize())
    {
        _isCorrect = true;
    }

    std::string AngularVelocityStdMessage::ToString() const
    {
        return toString("AngularVelocityStdMessage");
    }
    
    bool AngularVelocityStdMessage::Validate() const
    {
        if (!_isCorrect || !StdMessage::Validate())
        {
            return false;
        }

        auto message = ToByteArray();
        return validateChecksum8Bin(message.data(), message.size());
    }
    
    void AngularVelocityStdMessage::RecalculateChecksum()
    {
        if (!_isCorrect)
        {
            return;
        }
        auto message = ToByteArray();
        _cs = ChecksumComputer::ComputeCs8(message, message.size() - 1);
    }

    QByteArray AngularVelocityStdMessage::ToByteArray() const
    {
        QByteArray result;
        if (!_isCorrect)
        {
            return result;
        }

        result.append(headToByteArray());

        _serializer.Serialize(_time, result);
        _serializer.Serialize(_x, result);
        _serializer.Serialize(_y, result);
        _serializer.Serialize(_z, result);
        _serializer.Serialize(_rms, result);
        _serializer.Serialize(_flags, result);
        _serializer.Serialize(_cs, result);
        
        assert(result.size() == Size());
        return result;
    }
}
