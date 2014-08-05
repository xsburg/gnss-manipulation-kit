#include "SerialPortBinaryStream.h"

using namespace Greis;

namespace jpslogd
{
	class SerialStreamReader
	{
	private:
		SerialPortBinaryStream::SharedPtr_t _BinaryStream;

	public:
		SerialStreamReader(SerialPortBinaryStream::SharedPtr_t stream)
		{
			_BinaryStream = stream;
		}
		QString readLine()
		{
			QString line="";
			QByteArray data(1,0);
			while(true){
			_BinaryStream->read(data.data(), 1);
			if(line.size()==0 && (data.data()[0]==0x0a || data.data()[0]==0x0d)) continue;
			if(data.data()[0]==0x0a || data.data()[0]==0x0d)break;
			
			line.append(data);
			}
			return line;	
		}
	};
}