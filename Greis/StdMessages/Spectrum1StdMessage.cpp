#include "Spectrum1StdMessage.h"
#include <cassert>
#include "ChecksumComputer.h"

namespace Greis
{
    Spectrum1StdMessage::Spectrum1StdMessage( const char* pc_message, int p_length ) 
        : _id(pc_message, 2), _bodySize(p_length - HeadSize())
    {
        char* p_message = const_cast<char*>(pc_message);
        
        p_message += HeadSize();
    
        throw ProjectBase::NotImplementedException();
        
        assert(p_message - pc_message == p_length);
    }
    
    Spectrum1StdMessage::Spectrum1StdMessage( const std::string& p_id, int p_size ) 
        : _id(p_id), _bodySize(p_size - HeadSize())
    {
    }

    std::string Spectrum1StdMessage::ToString() const
    {
        return toString("Spectrum1StdMessage");
    }
    
    bool Spectrum1StdMessage::Validate() const
    {
        if (!StdMessage::Validate())
        {
            return false;
        }

        auto message = ToByteArray();
        return validateChecksum8Bin(message.data(), message.size());
    }
    
    void Spectrum1StdMessage::RecalculateChecksum()
    {
        auto message = ToByteArray();
        _cs = ChecksumComputer::ComputeCs8(message, message.size() - 1);
    }

    QByteArray Spectrum1StdMessage::ToByteArray() const
    {
        QByteArray result;
        result.append(headToByteArray());

        _serializer.Serialize(_currFrq, result);
        _serializer.Serialize(_finalFrq, result);
        _serializer.Serialize(_n, result);
        _serializer.Serialize(_m, result);
        _serializer.Serialize(_s, result);
        _serializer.Serialize(_cs, result);
        
        assert(result.size() == Size());
        return result;
    }
}