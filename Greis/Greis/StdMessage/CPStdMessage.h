#pragma once

#include <QtCore/QByteArray>
#include "Greis/StdMessage.h"
#include "Greis/EMessageId.h"

namespace Greis
{
    class CPStdMessage : public StdMessage
    {
    public:
        SMART_PTR_T(CPStdMessage);

        CPStdMessage(const char* p_message, int p_length);
        CPStdMessage(const std::string& p_id, int p_size);
        
        struct Codes
        {
            static const std::string Code_PC;
            static const std::string Code_P1;
            static const std::string Code_P2;
            static const std::string Code_P3;
            static const std::string Code_P5;
            static const std::string Code_Pl;
        };
        
        virtual std::string ToString() const;
        virtual std::string Id() const { return _id; }
        virtual EMessageId::Type IdNumber() const { return EMessageId::CP; }
        virtual bool Validate() const;
        virtual bool IsCorrect() const { return _isCorrect; }
        virtual void RecalculateChecksum();
        virtual int BodySize() const { return _bodySize; }
        virtual QByteArray ToByteArray() const;
        
        // CP, [cycles]
        const std::vector<Types::f8>& Cp() const { return _cp; }
        std::vector<Types::f8>& Cp() { return _cp; }

        // Checksum
        const Types::u1& Cs() const { return _cs; }
        Types::u1& Cs() { return _cs; }
    private:
        std::string _id;
        int _bodySize;
        bool _isCorrect;

        std::vector<Types::f8> _cp;
        Types::u1 _cs;
    };
}
