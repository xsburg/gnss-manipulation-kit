#pragma once

#include <QtCore/QByteArray>
#include "Greis/StdMessage.h"
#include "Greis/EMessageId.h"

namespace Greis
{
    class RcvTimeAccuracyStdMessage : public StdMessage
    {
    public:
        SMART_PTR_T(RcvTimeAccuracyStdMessage);

        RcvTimeAccuracyStdMessage(const char* p_message, int p_length);
        RcvTimeAccuracyStdMessage(const std::string& p_id, int p_size);
        RcvTimeAccuracyStdMessage();
        RcvTimeAccuracyStdMessage(const std::string& p_id);

        virtual std::string ToString() const;
        virtual std::string Id() const { return _id; }
        virtual EMessageId::Type IdNumber() const { return EMessageId::RcvTimeAccuracy; }
        virtual bool Validate() const;
        virtual bool IsCorrect() const { return _isCorrect; }
        virtual void RecalculateChecksum();
        virtual int BodySize() const { return _bodySize; }
        virtual QByteArray ToByteArray() const;
        
        // Accuracy [s]
        const Types::f4& Acc() const { return _acc; }
        Types::f4& Acc() { return _acc; }

        // Checksum
        const Types::u1& Cs() const { return _cs; }
        Types::u1& Cs() { return _cs; }
    private:
        std::string _id;
        int _bodySize;
        bool _isCorrect;

        Types::f4 _acc;
        Types::u1 _cs;
    };
}
