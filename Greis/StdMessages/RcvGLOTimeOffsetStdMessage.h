#ifndef RcvGLOTimeOffsetStdMessage_h__
#define RcvGLOTimeOffsetStdMessage_h__

#include <QtCore/QByteArray>
#include "StdMessage.h"
#include "EMessageId.h"

namespace Greis
{
    class RcvGLOTimeOffsetStdMessage : public StdMessage
    {
    public:
        SMART_PTR_T(RcvGLOTimeOffsetStdMessage);

        RcvGLOTimeOffsetStdMessage(const char* p_message, int p_length);
        RcvGLOTimeOffsetStdMessage(const std::string& p_id, int p_size);

        virtual std::string ToString() const;
        virtual std::string Id() const { return _id; }
        virtual EMessageId::Type IdNumber() const { return EMessageId::RcvGLOTimeOffset; }
        virtual bool Validate() const;
        virtual int BodySize() const { return _bodySize; }
        virtual QByteArray ToByteArray() const;
        
        // (Tn - Tr) [s]
        const Types::f8& Val() const { return _val; }
        Types::f8& Val() { return _val; }

        // Smoothed (Tn - Tr) [s]
        const Types::f8& Sval() const { return _sval; }
        Types::f8& Sval() { return _sval; }

        // Checksum
        const Types::u1& Cs() const { return _cs; }
        Types::u1& Cs() { return _cs; }
    private:
        std::string _id;
        int _bodySize;

        Types::f8 _val;
        Types::f8 _sval;
        Types::u1 _cs;
    };
}

#endif // RcvGLOTimeOffsetStdMessage_h__
