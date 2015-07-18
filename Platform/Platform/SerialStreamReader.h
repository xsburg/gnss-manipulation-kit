#pragma once

#include "Greis/SerialPortBinaryStream.h"

using namespace Greis;

namespace Platform
{
    class SerialStreamReader
    {
        SerialPortBinaryStream::SharedPtr_t _binaryStream;

    public:
        explicit SerialStreamReader(SerialPortBinaryStream::SharedPtr_t stream)
        {
            _binaryStream = stream;
        }

        QString readLine()
        {
            QString line = "";
            QByteArray data(1, 0);
            while (true)
            {
                _binaryStream->read(data.data(), 1);
                if (line.size() == 0 && (data.data()[0] == 0x0a || data.data()[0] == 0x0d))
                {
                    continue;
                }
                if (data.data()[0] == 0x0a || data.data()[0] == 0x0d)
                {
                    break;
                }

                line.append(data);
            }
            return line;
        }
    };
}
