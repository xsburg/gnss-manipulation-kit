#include "GPSAlm0StdMessage.h"
#include <cassert>
#include "Common/Logger.h"
#include "Greis/ChecksumComputer.h"

namespace Greis
{
    GPSAlm0StdMessage::GPSAlm0StdMessage( const char* pc_message, int p_length ) 
        : _id(pc_message, 2), _bodySize(p_length - HeadSize())
    {
        char* p_message = const_cast<char*>(pc_message);
        
        p_message += HeadSize();
    
        _serializer.Deserialize(p_message, _sv);
        p_message += sizeof(_sv);
        _serializer.Deserialize(p_message, _wna);
        p_message += sizeof(_wna);
        _serializer.Deserialize(p_message, _toa);
        p_message += sizeof(_toa);
        _serializer.Deserialize(p_message, _healthA);
        p_message += sizeof(_healthA);
        _serializer.Deserialize(p_message, _healthS);
        p_message += sizeof(_healthS);
        _serializer.Deserialize(p_message, _config);
        p_message += sizeof(_config);
        _serializer.Deserialize(p_message, _af1);
        p_message += sizeof(_af1);
        _serializer.Deserialize(p_message, _af0);
        p_message += sizeof(_af0);
        _serializer.Deserialize(p_message, _rootA);
        p_message += sizeof(_rootA);
        _serializer.Deserialize(p_message, _ecc);
        p_message += sizeof(_ecc);
        _serializer.Deserialize(p_message, _m0);
        p_message += sizeof(_m0);
        _serializer.Deserialize(p_message, _omega0);
        p_message += sizeof(_omega0);
        _serializer.Deserialize(p_message, _argPer);
        p_message += sizeof(_argPer);
        _serializer.Deserialize(p_message, _deli);
        p_message += sizeof(_deli);
        _serializer.Deserialize(p_message, _omegaDot);
        p_message += sizeof(_omegaDot);
        _serializer.Deserialize(p_message, _cs);
        p_message += sizeof(_cs);

        _isCorrect = (p_message - pc_message == p_length);
        if (!_isCorrect)
        {
            sLogger.Debug(QString("The message %1 is incorrect. Excepted size is %2 whilst the actual size is %3.")
                .arg(QString::fromStdString(ToString())).arg(p_length).arg(p_message - pc_message));
        }
    }
    
    GPSAlm0StdMessage::GPSAlm0StdMessage( const std::string& p_id, int p_size ) 
        : _id(p_id), _bodySize(p_size - HeadSize())
    {
        _isCorrect = true;
    }

    std::string GPSAlm0StdMessage::ToString() const
    {
        return toString("GPSAlm0StdMessage");
    }
    
    bool GPSAlm0StdMessage::Validate() const
    {
        if (!_isCorrect || !StdMessage::Validate())
        {
            return false;
        }

        auto message = ToByteArray();
        return validateChecksum8Bin(message.data(), message.size());
    }
    
    void GPSAlm0StdMessage::RecalculateChecksum()
    {
        if (!_isCorrect)
        {
            return;
        }
        auto message = ToByteArray();
        _cs = ChecksumComputer::ComputeCs8(message, message.size() - 1);
    }

    QByteArray GPSAlm0StdMessage::ToByteArray() const
    {
        QByteArray result;
        if (!_isCorrect)
        {
            return result;
        }

        result.append(headToByteArray());

        _serializer.Serialize(_sv, result);
        _serializer.Serialize(_wna, result);
        _serializer.Serialize(_toa, result);
        _serializer.Serialize(_healthA, result);
        _serializer.Serialize(_healthS, result);
        _serializer.Serialize(_config, result);
        _serializer.Serialize(_af1, result);
        _serializer.Serialize(_af0, result);
        _serializer.Serialize(_rootA, result);
        _serializer.Serialize(_ecc, result);
        _serializer.Serialize(_m0, result);
        _serializer.Serialize(_omega0, result);
        _serializer.Serialize(_argPer, result);
        _serializer.Serialize(_deli, result);
        _serializer.Serialize(_omegaDot, result);
        _serializer.Serialize(_cs, result);
        
        assert(result.size() == Size());
        return result;
    }
}
