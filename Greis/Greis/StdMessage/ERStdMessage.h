#pragma once

#include <QtCore/QByteArray>
#include "Greis/StdMessage.h"
#include "Greis/EMessageId.h"

namespace Greis
{
    class ERStdMessage : public StdMessage
    {
    public:
        SMART_PTR_T(ERStdMessage);

        ERStdMessage(const char* p_message, int p_length);
        ERStdMessage(const std::string& p_id, int p_size);
        ERStdMessage(int p_size);
        
        struct Codes
        {
            static const std::string Code_ER;
        };
        
        virtual std::string ToString() const;
        virtual std::string Id() const { return _id; }
        virtual EMessageId::Type IdNumber() const { return EMessageId::ER; }
        virtual bool Validate() const;
        virtual bool IsCorrect() const { return _isCorrect; }
        virtual void RecalculateChecksum();
        virtual int BodySize() const { return _bodySize; }
        virtual QByteArray ToByteArray() const;
        
        // Error description
        const std::string& Error() const { return _error; }
        std::string& Error() { return _error; }
    private:
        std::string _id;
        int _bodySize;
        bool _isCorrect;

        std::string _error;
    };
}
