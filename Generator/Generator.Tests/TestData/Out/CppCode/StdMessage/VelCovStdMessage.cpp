#include "VelCovStdMessage.h"
#include <cassert>
#include "ChecksumComputer.h"
#include "Common/Logger.h"

namespace Greis
{
    VelCovStdMessage::VelCovStdMessage( const char* pc_message, int p_length ) 
        : _id(pc_message, 2), _bodySize(p_length - HeadSize())
    {
        char* p_message = const_cast<char*>(pc_message);
        
        p_message += HeadSize();
    
        _serializer.Deserialize(p_message, _xx);
        p_message += sizeof(_xx);
        _serializer.Deserialize(p_message, _yy);
        p_message += sizeof(_yy);
        _serializer.Deserialize(p_message, _zz);
        p_message += sizeof(_zz);
        _serializer.Deserialize(p_message, _tt);
        p_message += sizeof(_tt);
        _serializer.Deserialize(p_message, _xy);
        p_message += sizeof(_xy);
        _serializer.Deserialize(p_message, _xz);
        p_message += sizeof(_xz);
        _serializer.Deserialize(p_message, _xt);
        p_message += sizeof(_xt);
        _serializer.Deserialize(p_message, _yz);
        p_message += sizeof(_yz);
        _serializer.Deserialize(p_message, _yt);
        p_message += sizeof(_yt);
        _serializer.Deserialize(p_message, _zt);
        p_message += sizeof(_zt);
        _serializer.Deserialize(p_message, _solType);
        p_message += sizeof(_solType);
        _serializer.Deserialize(p_message, _cs);
        p_message += sizeof(_cs);

        _isCorrect = (p_message - pc_message == p_length);
        if (!_isCorrect)
        {
            sLogger.Debug(QString("The message %1 is incorrect. Excepted size is %2 whilst the actual size is %3.")
                .arg(QString::fromStdString(ToString())).arg(p_length).arg(p_message - pc_message));
        }
    }
    
    VelCovStdMessage::VelCovStdMessage( const std::string& p_id, int p_size ) 
        : _id(p_id), _bodySize(p_size - HeadSize())
    {
        _isCorrect = true;
    }

    std::string VelCovStdMessage::ToString() const
    {
        return toString("VelCovStdMessage");
    }
    
    bool VelCovStdMessage::Validate() const
    {
        if (!_isCorrect || !StdMessage::Validate())
        {
            return false;
        }

        auto message = ToByteArray();
        return validateChecksum8Bin(message.data(), message.size());
    }
    
    void VelCovStdMessage::RecalculateChecksum()
    {
        if (!_isCorrect)
        {
            return;
        }
        auto message = ToByteArray();
        _cs = ChecksumComputer::ComputeCs8(message, message.size() - 1);
    }

    QByteArray VelCovStdMessage::ToByteArray() const
    {
        QByteArray result;
        if (!_isCorrect)
        {
            return result;
        }

        result.append(headToByteArray());

        _serializer.Serialize(_xx, result);
        _serializer.Serialize(_yy, result);
        _serializer.Serialize(_zz, result);
        _serializer.Serialize(_tt, result);
        _serializer.Serialize(_xy, result);
        _serializer.Serialize(_xz, result);
        _serializer.Serialize(_xt, result);
        _serializer.Serialize(_yz, result);
        _serializer.Serialize(_yt, result);
        _serializer.Serialize(_zt, result);
        _serializer.Serialize(_solType, result);
        _serializer.Serialize(_cs, result);
        
        assert(result.size() == Size());
        return result;
    }
}
