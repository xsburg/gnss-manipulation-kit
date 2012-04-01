#include "SatElevationStdMessage.h"
#include <cassert>

namespace Greis
{
    SatElevationStdMessage::SatElevationStdMessage( const char* pc_message, int p_length ) 
        : _id(pc_message, 2), _bodySize(p_length - HeadSize())
    {
        char* p_message = const_cast<char*>(pc_message);
        
        p_message += HeadSize();
    
        int arraySizeInUniformFillFields = (BodySize() - 1) / 1;

        _serializer.Deserialize(p_message, sizeof(std::vector<Types::i1>::value_type) * arraySizeInUniformFillFields, _elev);
        p_message += sizeof(std::vector<Types::i1>::value_type) * arraySizeInUniformFillFields;
        _serializer.Deserialize(p_message, _cs);
        p_message += sizeof(_cs);
        
        assert(p_message - pc_message == p_length);
    }

    std::string SatElevationStdMessage::ToString() const
    {
        return toString("SatElevationStdMessage");
    }
    bool SatElevationStdMessage::Validate() const
    {
        if (!Validate())
        {
            return false;
        }

        auto message = ToByteArray();
        return validateChecksum8Bin(message.data(), message.size());
    }

    QByteArray SatElevationStdMessage::ToByteArray() const
    {
        QByteArray result;
        result.append(headToByteArray());

        _serializer.Serialize(_elev, result);
        _serializer.Serialize(_cs, result);
        
        assert(result.size() == Size());
        return result;
    }
}