#pragma once

#include <QtCore/QByteArray>
#include "Greis/StdMessage.h"
#include "Greis/EMessageId.h"

namespace Greis
{
    class TrackingTimeStdMessage : public StdMessage
    {
    public:
        SMART_PTR_T(TrackingTimeStdMessage);

        TrackingTimeStdMessage(const char* p_message, int p_length);
        TrackingTimeStdMessage(const std::string& p_id, int p_size);
        TrackingTimeStdMessage();
        TrackingTimeStdMessage(const std::string& p_id);
        
        struct Codes
        {
            static const std::string Code_TT;
        };
        
        virtual std::string ToString() const;
        virtual std::string Id() const { return _id; }
        virtual EMessageId::Type IdNumber() const { return EMessageId::TrackingTime; }
        virtual bool Validate() const;
        virtual bool IsCorrect() const { return _isCorrect; }
        virtual void RecalculateChecksum();
        virtual int BodySize() const { return _bodySize; }
        virtual QByteArray ToByteArray() const;
        
        // tracking time [s]
        const Types::u4& Tt() const { return _tt; }
        Types::u4& Tt() { return _tt; }

        // Checksum
        const Types::u1& Cs() const { return _cs; }
        Types::u1& Cs() { return _cs; }
    private:
        std::string _id;
        int _bodySize;
        bool _isCorrect;

        Types::u4 _tt;
        Types::u1 _cs;
    };
}
