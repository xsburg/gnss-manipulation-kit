#ifndef LatencyStdMessage_h__
#define LatencyStdMessage_h__

#include "StdMessage.h"
#include <QtCore/QByteArray>

namespace Greis
{
    class LatencyStdMessage : public StdMessage
    {
    public:
        SMART_PTR_T(LatencyStdMessage);

        LatencyStdMessage(const char* p_message, int p_length);

        virtual std::string ToString() const;
        virtual std::string Id() const { return _id; }
        virtual bool Validate() const;
        virtual int BodySize() const { return _bodySize; }
        virtual QByteArray ToByteArray() const;
        
        // output latency [ms]
        const Types::u1& Lt() const { return _lt; }
        Types::u1& Lt() { return _lt; }

        // Checksum
        const Types::u1& Cs() const { return _cs; }
        Types::u1& Cs() { return _cs; }
    private:
        std::string _id;
        int _bodySize;

        Types::u1 _lt;
        Types::u1 _cs;
    };
}

#endif // LatencyStdMessage_h__