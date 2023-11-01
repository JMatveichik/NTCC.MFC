// Serial.h
#include <fstream>
#ifndef __SERIAL_PORT_INCLUDED_H__
#define __SERIAL_PORT_INCLUDED_H__

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

const UINT BAUD_RATE [] =  {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};

unsigned short CRC16(const unsigned char* data, int size);
unsigned short CRC_CCITT(const unsigned char* data, int size);

class SyncSerialPort
{

public:
	SyncSerialPort();
	~SyncSerialPort();

	BOOL Open( int nPort = 2, int nBaud = 9600 );
	BOOL Close( void );

	int ReadData( void* buffer, int size) const;

	BOOL ModbusWriteRegs(unsigned char func, unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log = NULL  );
	BOOL ModbusReadRegs(unsigned char func, unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log = NULL  );

	BOOL ModbusReadInputRegs(unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log = NULL);
	BOOL ModbusReadHoldingRegs(unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log = NULL );
	
	BOOL ModbusReadDiscrete(unsigned char func, unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log = NULL  );
	
	BOOL ModbusReadCoils(unsigned char slaveAddress, int regAddress, int coilsCount, unsigned short* pRegTable, std::string* log = NULL  );
	BOOL ModbusReadDiscreteInputs(unsigned char slaveAddress, int regAddress, int coilsCount, unsigned short* pRegTable, std::string* log = NULL  );

	BOOL ModbusWriteSingleCoil(unsigned char func, unsigned char slaveAddress, int regAddress, unsigned short* pRegTable, std::string* log = NULL  );

	int SendData( byte* buffer, int size);
	int ReadDataWaiting( );

	BOOL IsOpened( ) const  {  return( m_bOpened );  }
	
	int PortNo () const  { return portNo; }
	int BaudRate () const  { return baudRate; }

	void GetComState(DCB& dcb) const;
	BOOL SetComState(DCB dcb);

	void GetComTimeouts(COMMTIMEOUTS& CommTimeOuts) const;
	BOOL SetComTimeouts(COMMTIMEOUTS CommTimeOuts);
	
	bool WriteOneByte(unsigned char bt, unsigned char& rb);
	bool ReadOneByte(unsigned char& bt);

protected:	
	void PrintCommStatus();

	void PrintHEX(unsigned char* recv, int size, std::string& str );
	bool CheckCRC(unsigned char* recv, int size);

	HANDLE m_hIDComDev;	
	BOOL m_bOpened;

	int portNo;
	int baudRate;
};

#endif //__SERIAL_PORT_INCLUDED_H__
