#pragma once

#include <QtCore/QByteArray>
#include "Greis/CustomType.h"
#include "Greis/ECustomTypeId.h"

namespace Greis
{
    class SvData0CustomType : public CustomType
    {
    public:
        SMART_PTR_T(SvData0CustomType);

        SvData0CustomType(const char* p_message, int p_length);
        SvData0CustomType(int p_size);
        
        virtual ECustomTypeId::Type IdNumber() const { return ECustomTypeId::SvData0; }
        virtual QByteArray ToByteArray() const;
        virtual int Size() const { return _size; }
        virtual bool IsCorrect() const { return _isCorrect; }

        // Pseudo-Range Number (PRN)
        const Types::i1& Prn() const { return _prn; }
        Types::i1& Prn() { return _prn; }

        // Counter which is updated upon receiving a
        // new sub-frame for given satellite.
        const Types::u1& Cnt() const { return _cnt; }
        Types::u1& Cnt() { return _cnt; }

        // GPS sub-frame contents. Every 4-bytes word
        // contains 30 LSB of the GPS navigation data.
        const std::vector<Types::u4>& Data() const { return _data; }
        std::vector<Types::u4>& Data() { return _data; }
    private:
        int _size;
        bool _isCorrect;

        Types::i1 _prn;
        Types::u1 _cnt;
        std::vector<Types::u4> _data;
    };
}
