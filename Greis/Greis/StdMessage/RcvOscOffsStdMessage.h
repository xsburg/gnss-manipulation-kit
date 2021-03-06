#pragma once

#include <QtCore/QByteArray>
#include "Greis/StdMessage.h"
#include "Greis/EMessageId.h"

namespace Greis
{
    class RcvOscOffsStdMessage : public StdMessage
    {
    public:
        SMART_PTR_T(RcvOscOffsStdMessage);

        RcvOscOffsStdMessage(const char* p_message, int p_length);
        RcvOscOffsStdMessage(const std::string& p_id, int p_size);

        virtual std::string ToString() const;
        virtual std::string Id() const { return _id; }
        virtual EMessageId::Type IdNumber() const { return EMessageId::RcvOscOffs; }
        virtual bool Validate() const;
        virtual bool IsCorrect() const { return _isCorrect; }
        virtual void RecalculateChecksum();
        virtual int BodySize() const { return _bodySize; }
        virtual QByteArray ToByteArray() const;
        
        // Oscillator offset [s/s]
        const Types::f4& Val() const { return _val; }
        Types::f4& Val() { return _val; }

        // Checksum
        const Types::u1& Cs() const { return _cs; }
        Types::u1& Cs() { return _cs; }
    private:
        std::string _id;
        int _bodySize;
        bool _isCorrect;

        Types::f4 _val;
        Types::u1 _cs;
    };
}
