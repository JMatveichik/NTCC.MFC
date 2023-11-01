// Serial.cpp
#include "stdafx.h"
#include "Serial.h"
#include "common.h"

#include <sys/timeb.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

static const unsigned short CRC_16_TABLE []  = 
{
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 
};

static const unsigned short CRC_CCITT_TABLE[256] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

unsigned short CRC16(const unsigned char* data, int size)
{
	unsigned short crc = 0xFFFF;

	for (int i = 0; i < size; i++)
	{
		byte tableIndex = (crc ^ data[i]);
		crc >>= 8;
		crc ^= CRC_16_TABLE[tableIndex];
	}

	return crc;
}

unsigned short CRC_CCITT(const unsigned char* data, int size)
{
	/*unsigned short tmp;
	unsigned short crc = 0x1021;

	for (int i=0; i < size ; i++)
	{
		tmp = (crc >> 8) ^ data[i];
		crc = (crc << 8) ^ CRC_CCITT_TABLE[tmp];
	}

	return crc;*/
	unsigned short crc = 0;
	unsigned short POLY = 0x1021;
	while (size--) {
		crc ^= *data++ << 8;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
		crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
	}
	return crc & 0xffff;
}

SyncSerialPort::SyncSerialPort()
{
	m_hIDComDev = NULL;
	m_bOpened = FALSE;
	portNo = 0;
	baudRate = 0;
}

SyncSerialPort::~SyncSerialPort()
{
	Close();
}

BOOL SyncSerialPort::Open( int nPort, int nBaud )
{
	if ( m_bOpened ) 
		return TRUE;

	char szPort[16];

	sprintf_s(szPort, 16, "\\\\.\\COM%d", nPort);
	
	m_hIDComDev = CreateFile( szPort, 
		GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0/*FILE_ATTRIBUTE_NORMAL*/, NULL );

	if( m_hIDComDev == NULL || m_hIDComDev == INVALID_HANDLE_VALUE) 
		return FALSE;

	
	portNo = nPort;
	baudRate = nBaud;

	DCB dcb;
	dcb.DCBlength = sizeof( DCB );

	GetCommState( m_hIDComDev, &dcb );

	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	dcb.fParity = FALSE;
	dcb.StopBits = ONESTOPBIT;

	if( !SetCommState( m_hIDComDev, &dcb ) )
		return FALSE;
	
	m_bOpened = TRUE;

/*	
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant	= 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;

	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );
*/	

//	dcb.fRtsControl = RTS_CONTROL_TOGGLE;
//	dcb.EvtChar = '\r';	

// 	unsigned char ucSet;
// 	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_DTRDSR  ) != 0 );
// 	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_RTSCTS  ) != 0 );
// 	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_XONXOFF ) != 0 );

/*
	if( !SetCommState( m_hIDComDev, &dcb ) || 
		!SetupComm( m_hIDComDev, 1024, 1024 ) || 
		!SetCommMask(m_hIDComDev, EV_CTS | EV_DSR) )
	{
		LPVOID lpMsgBuf;
		
		FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER |  FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0,  (LPTSTR) &lpMsgBuf, 0, NULL  );
		// Process any inserts in lpMsgBuf.
		// ..
		// Display the string.
		MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );

		CloseHandle( m_hIDComDev );
		return( FALSE );

	}
*/

	

	return m_bOpened;
}

void SyncSerialPort::GetComTimeouts(COMMTIMEOUTS& CommTimeOuts) const
{
	GetCommTimeouts(m_hIDComDev, &CommTimeOuts);
}

BOOL SyncSerialPort::SetComTimeouts(COMMTIMEOUTS CommTimeOuts)
{
	return SetCommTimeouts(m_hIDComDev, &CommTimeOuts);
}

void SyncSerialPort::GetComState(DCB& dcb) const
{
	GetCommState( m_hIDComDev, &dcb );
}

BOOL SyncSerialPort::SetComState(DCB dcb)
{
	return SetCommState( m_hIDComDev, &dcb );
}

BOOL SyncSerialPort::Close( )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  TRUE;

	CloseHandle( m_hIDComDev );

	m_bOpened = FALSE;
	m_hIDComDev = NULL;

	return TRUE;
}
void SyncSerialPort::PrintCommStatus()
{
	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	if (dwErrorFlags & CE_BREAK)
	{
		//TRACE("Status : The hardware detected a break condition.\n");
	} 
	else if (dwErrorFlags & CE_FRAME )	
	{
		//TRACE("Status : The hardware detected a framing error.\n"); 
	}
	else if (dwErrorFlags & CE_IOE )	
	{
		//TRACE("Status : An I/O error occurred during communications with the device.\n"); 
	}
	else if (dwErrorFlags & CE_MODE ) 
	{
		//TRACE("Status : The requested mode is not supported, or the hFile parameter is invalid.\n"); 
		//TRACE("Status : If this value is specified, it is the only valid error.\n");
	}
	else if (dwErrorFlags & CE_OVERRUN)  
	{
		//TRACE("Status : A character-buffer overrun has occurred.\n"); 
		//TRACE("Status : The next character is lost.\n");
	}
	else if (dwErrorFlags & CE_RXOVER) 
	{
		//TRACE("Status : An input buffer overflow has occurred. There is either no room in the\n");
		//TRACE("Status : input buffer, or a character was received after the end-of-file (EOF) character.\n");
	}
	else if (dwErrorFlags & CE_RXPARITY) 
	{
		//TRACE("Status : The hardware detected a parity error.\n");
	}
	else if (dwErrorFlags & CE_TXFULL) 
	{
		//TRACE("Status : The application tried to transmit a character, but the output buffer was full.\n");
	}
	else
	{
		//TRACE("Status : ОК\n");
	}
}

int SyncSerialPort::SendData( byte *buffer, int size )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  0;

	DWORD dwBytesWritten = 0;
	
	PurgeComm(m_hIDComDev, PURGE_TXCLEAR); //ACTION 4 -> Clear serial output buffer

	if ( !WriteFile( m_hIDComDev, buffer, size, &dwBytesWritten, NULL))
	{
		//TRACE("\nWRITING ERROR\n", buffer);
	}

	//buffer[0] = 0x0A;
	//WriteFile( m_hIDComDev, buffer, 1, &dwBytesWritten, NULL);
	
	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); // Clear serial input buff
/*	
	BOOL bres = EscapeCommFunction(m_hIDComDev, CLRDTR);

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
*/
	return  (int) dwBytesWritten;
}

int SyncSerialPort::ReadDataWaiting( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  0;

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	return  (int)ComStat.cbInQue;

}

/*
int SynchSerialPort::ReadData( void *buffer, int limit)
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  0;

 	BOOL bReadStatus;
 	DWORD dwBytesRead, dwErrorFlags;
 	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	bool bFrameComplete = false;
	std::string str;

	int counter = 0;
	while ( !bFrameComplete )
	{
		char ch = 0x00;
		bReadStatus = ReadFile( m_hIDComDev, &ch, 1, &dwBytesRead, NULL);

		if ( dwBytesRead != 1 )
		{

			if ( counter++  > 10 )	//!!!!!!!!!
				return 0;

			Sleep(10);
			continue;
		}

		if ( !bReadStatus )
			return 0;		
		
		if (ch == '\r')
			bFrameComplete = true;
		else
			str += ch;
	}
	
	memcpy(buffer, str.c_str(), str.length() + 1);

	return  (int)str.length();
}
*/
int SyncSerialPort::ReadData( void *buffer, int limit) const
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  0;

	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	
	//PurgeComm(m_hIDComDev, PURGE_RXCLEAR); //ACTION 8 -> Clear serial input buffer	
	Sleep( int(115200.0 / baudRate * 30) + 10 );
	
	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
		

	int len = 0;
	unsigned char recv[MAX_RESPONSE_LEN];
	recv[0] = 0;
	char ch = -1;

	/*struct _timeb timebuffer;
	_ftime_s( &timebuffer );
	__int64 start = timebuffer.time*1000 + timebuffer.millitm;

	char  szEol[] = "\r\n";
	for(;;)
	{
		ReadFile( m_hIDComDev, &ch, 1, &dwBytesRead, NULL);

		if (strchr(szEol, ch) == NULL )
		{
			recv[len++] = ch;
		}
		else
			break;
	
		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );	

		_ftime_s( &timebuffer );

		if (timebuffer.time*1000 + timebuffer.millitm - start >= 1000)
			break;
	}*/


	while ( ComStat.cbInQue > 0 )
	{
		ReadFile( m_hIDComDev, &ch, 1, &dwBytesRead, NULL);

		/*if (ch == '\r')
		{
			recv[len++] = 0;
			break;
		}*/

		if (ch != -1)
			recv[len++] = ch;

		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );		
	}

/*	char szBuffer[MAX_RESPONSE_LEN];
	szBuffer[0] = 0;
	len = 0;
	while ( ComStat.cbInQue > 0 )
	{
		ReadFile( m_hIDComDev, &szBuffer, ComStat.cbInQue, &dwBytesRead, NULL);

		memcpy(&recv[len], szBuffer, dwBytesRead);
		len += dwBytesRead;
		Sleep(100);
		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	}
*/
	recv[len/* + 1*/] = 0;
	memcpy(buffer, recv, len + 1);
	//memcpy(buffer, recv.c_str(), recv.length());

	PurgeComm(m_hIDComDev, PURGE_TXCLEAR); //ACTION 4 -> Clear serial output buffer

	return  len;
}

void SyncSerialPort::PrintHEX(unsigned char* recv, int size, std::string& str )
{
	for (int i = 0; i < size; i++)
	{
		char bt[4];
		sprintf_s(bt, "%02X ", recv[i]);
		str += bt;
	}
}
bool SyncSerialPort::CheckCRC(unsigned char* recv, int size)
{
	//Проверка контрольной суммы
	unsigned short crcRecive = MAKEWORD(recv[size - 2], recv[size - 1]);
	unsigned short crcReciveCalc = CRC16(recv, size - 2);

	if ( crcRecive != crcReciveCalc)
		return false; 

	return true;
}

//func
// * 1 (0x01) — чтение значений из нескольких регистров флагов (Read Coil Status)
// * 2 (0x02) — чтение значений из нескольких дискретных регистров (Read Discrete Inputs)
BOOL SyncSerialPort::ModbusReadDiscrete(unsigned char func, unsigned char slaveAddress, int regAddress, int coilsCount, unsigned short* pRegTable, std::string* log/* = NULL*/  )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  FALSE;

	unsigned char cmd[8];

	cmd[0] = slaveAddress;	//адрес подчиененного устройства
	cmd[1] = func;			//номер стандартной функции MODBUS (Read Coil Status)

	cmd[2] = HIBYTE(regAddress);  // базовый адрес таблицы регистров, старший байт  
	cmd[3] = LOBYTE(regAddress);  // базовый адрес таблицы регистров, младший  байт

	cmd[4] = HIBYTE(coilsCount);	  // количество регистров для чтения, старший байт  
	cmd[5] = LOBYTE(coilsCount);	  // количество регистров для чтения, младший байт  

	unsigned short crc = CRC16(cmd, 6);

	cmd[6] = LOBYTE(crc);	  // контрольная сумма CRC-16,  младший байт   
	cmd[7] = HIBYTE(crc);	  // контрольная сумма CRC-16,  старший байт  

	DWORD dwBytesWritten = 0;


	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); //ACTION 8 -> Clear serial input buffer	
	PurgeComm(m_hIDComDev, PURGE_TXCLEAR); //ACTION 4 -> Clear serial output buffer

	if ( log != NULL ) 
	{
		*log = "SEND : ";		
		PrintHEX(cmd, 8, *log);
	}

	if ( !WriteFile( m_hIDComDev, cmd, 8, &dwBytesWritten, NULL) )
	{
		if ( log != NULL ) 
			*log += "\tCOMMAND WRITING FAILED";
		return FALSE;
	}


	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); // Clear serial input buff.., one more time.
	Sleep(75);

	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	int ind = 0;
	unsigned char recv[MAX_RESPONSE_LEN];
	char ch = -1;

	while ( ComStat.cbInQue > 0 )
	{
		ReadFile( m_hIDComDev, &ch, 1, &dwBytesRead, NULL);

		//if (ch != -1)
		recv[ind++] = ch;

		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	}

	if (ind == 0)
	{
		if ( log != NULL ) 
			*log += "\tRECIVE : EMPTY";

		return FALSE;
	}

	if ( log != NULL ) 
	{
		*log += "\tRECIVE : ";
		PrintHEX(recv, ind, *log);
	}

	if ( !CheckCRC(recv, ind ) )
	{
		if ( log != NULL ) 
			*log += "\tCRC ERROR";

		return FALSE;
	}

	//Проверка адреса устройства и номера функции 
	if (recv[0] != cmd[0] ||  recv[1] != cmd[1])
		return FALSE;

	int bytesCount = recv[2];
	int dataOffset = 3;
	for (byte i = 0; i < bytesCount; i++ )
	{
		pRegTable[i] = recv[dataOffset + i];
	}

	return  TRUE;
}


// * 1 (0x01) — чтение значений из нескольких регистров флагов (Read Coil Status)
BOOL SyncSerialPort::ModbusReadCoils(unsigned char slaveAddress, int regAddress, int coilsCount, unsigned short* pRegTable, std::string* log/* = NULL  */)
{
	return ModbusReadDiscrete(0x01, slaveAddress, regAddress, coilsCount, pRegTable, log);
}

// * 2 (0x02) — чтение значений из нескольких дискретных регистров (Read Discrete Inputs)
BOOL SyncSerialPort::ModbusReadDiscreteInputs(unsigned char slaveAddress, int regAddress, int coilsCount, unsigned short* pRegTable, std::string* log/* = NULL  */)
{
	return ModbusReadDiscrete(0x02, slaveAddress, regAddress, coilsCount, pRegTable, log);
}

// * 3 (0x03) — чтение значений из нескольких регистров хранения (Read Holding Registers)
// * 4 (0x04) — чтение значений из нескольких регистров ввода (Read Input Registers)
BOOL SyncSerialPort::ModbusReadRegs(unsigned char func, unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log/* = NULL */ )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  FALSE;

	unsigned char cmd[8];

	cmd[0] = slaveAddress;	//адрес подчиененного устройства
	cmd[1] = func;			//номер стандартной функции MODBUS

	cmd[2] = HIBYTE(regAddress);  // базовый адрес таблицы регистров, старший байт  
	cmd[3] = LOBYTE(regAddress);  // базовый адрес таблицы регистров, младший  байт

	cmd[4] = HIBYTE(regCount);	  // количество регистров для чтения, старший байт  
	cmd[5] = LOBYTE(regCount);	  // количество регистров для чтения, младший байт  

	unsigned short crc = CRC16(cmd, 6);

	cmd[6] = LOBYTE(crc);	  // контрольная сумма CRC-16,  младший байт   
	cmd[7] = HIBYTE(crc);	  // контрольная сумма CRC-16,  старший байт  

	DWORD dwBytesWritten = 0;


	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); //ACTION 8 -> Clear serial input buffer	
	PurgeComm(m_hIDComDev, PURGE_TXCLEAR); //ACTION 4 -> Clear serial output buffer

	if ( log != NULL ) 
	{
		*log = "SEND : ";		
		PrintHEX(cmd, 8, *log);
	}

	if ( !WriteFile( m_hIDComDev, cmd, 8, &dwBytesWritten, NULL) )
	{
		if ( log != NULL ) 
			*log += "\tCOMMAND WRITING FAILED";
		return FALSE;
	}


	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); // Clear serial input buff.., one more time.
	Sleep(75);

	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	int ind = 0;
	unsigned char recv[MAX_RESPONSE_LEN];
	char ch = -1;

	while ( ComStat.cbInQue > 0 )
	{
		ReadFile( m_hIDComDev, &ch, 1, &dwBytesRead, NULL);

		//if (ch != -1)
		recv[ind++] = ch;

		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	}
	
	if (ind == 0)
	{
		if ( log != NULL ) 
			*log += "\tRECIVE : EMPTY";

		return FALSE;
	}
		
	if ( log != NULL ) 
	{
		*log += "\tRECIVE : ";
		PrintHEX(recv, ind, *log);
	}

	if ( !CheckCRC(recv, ind ) )
	{
		if ( log != NULL ) 
			*log += "\tCRC ERROR";

		return FALSE;
	}

	//Проверка адреса устройства и номера функции 
	if (recv[0] != cmd[0] ||  recv[1] != cmd[1])
		return FALSE;

	int loOffset = 4;
	int hiOffset = 3;

	for (byte i = 0; i < regCount; i++ )
	{
		byte lo = recv[loOffset + i*2];
		byte hi = recv[hiOffset + i*2];
		pRegTable[i] = MAKEWORD(lo, hi);
	}

	return  TRUE;
}




//func
// *  (0x05) — чтение значений из нескольких регистров флагов (Read Coil Status)
// *  (0x06) — чтение значений из нескольких дискретных регистров (Read Discrete Inputs)
// *  (0x0F) — чтение значений из нескольких регистров хранения (Read Holding Registers)
// *  (0x10) — чтение значений из нескольких регистров ввода (Read Input Registers)
BOOL SyncSerialPort::ModbusWriteRegs(unsigned char func, unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log/* = NULL */ )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  FALSE;

	unsigned char cmd[32];
	int byteNo = 0;
	cmd[byteNo++] = slaveAddress;	//адрес подчиененного устройства
	cmd[byteNo++] = func;			//номер стандартной функции MODBUS

	cmd[byteNo++] = HIBYTE(regAddress);  // базовый адрес таблицы регистров, старший байт  
	cmd[byteNo++] = LOBYTE(regAddress);  // базовый адрес таблицы регистров, младший  байт

	if ( func == 0x10 )
	{

		cmd[byteNo++] = HIBYTE(regCount);	  // количество регистров для записи, старший байт  
		cmd[byteNo++] = LOBYTE(regCount);	  // количество регистров для записи, младший байт

		cmd[byteNo++] = LOBYTE(regCount*2);	  // количество последующих байт данных 	
	}

	for( int i = 0; i < regCount; i++ )
	{
		cmd[byteNo++] = HIBYTE( pRegTable[i] );
		cmd[byteNo++] = LOBYTE( pRegTable[i] );
	}

	unsigned short crc = CRC16(cmd, byteNo);

	cmd[byteNo++] = LOBYTE(crc);	  // контрольная сумма CRC-16,  младший байт   
	cmd[byteNo++] = HIBYTE(crc);	  // контрольная сумма CRC-16,  старший байт  

	DWORD dwBytesWritten = 0;

	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); //ACTION 8 -> Clear serial input buffer	
	PurgeComm(m_hIDComDev, PURGE_TXCLEAR); //ACTION 4 -> Clear serial output buffer
	
	if ( log != NULL ) 
	{
		*log = "SEND : ";

		for (int i = 0; i < byteNo; i++)
		{
			char bt[4];
			sprintf_s(bt, "%02X ", cmd[i]);
			*log += bt;
		}
	}

	if ( !WriteFile( m_hIDComDev, cmd, byteNo, &dwBytesWritten, NULL) )
	{
		if ( log != NULL ) 
			*log += "\tCOMMAND WRITING FAILED";
		return FALSE;		
	}
	
	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); // Clear serial input buff.., one more time.
	Sleep(75);

	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	int ind = 0;
	unsigned char recv[256];
	char ch = -1;

	while ( ComStat.cbInQue > 0 )
	{
		ReadFile( m_hIDComDev, &ch, 1, &dwBytesRead, NULL);

		//if (ch != -1)
			recv[ind++] = ch;

		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	}

	if (ind == 0)
	{
		if ( log != NULL ) 
			*log += "\tRECIVE : EMPTY";

		return FALSE;
	}
		
	if ( log != NULL ) 
	{
		*log += "\tRECIVE : ";
		
		for (int i = 0; i < ind; i++)
		{
			char bt[4];
			sprintf_s(bt, "%02X ", recv[i]);
			*log += bt;
		}
		
	}

	//Проверка контрольной суммы
	unsigned short crcRecive = MAKEWORD(recv[ind - 2], recv[ind - 1]);
	unsigned short crcReciveCalc = CRC16(recv, ind - 2);

	if ( crcRecive != crcReciveCalc)
	{
		if ( log != NULL ) 
			*log += "\tCRC ERROR";

		return FALSE;
	}

	//Проверка адреса устройства и номера функции 
	if (recv[0] != cmd[0] ||  recv[1] != cmd[1])
		return FALSE;

	int regAddr  =  MAKEWORD(recv[3], recv[2]);
	int regCnt   =  MAKEWORD(recv[5], recv[4]);

	return  TRUE;
}



BOOL SyncSerialPort::ModbusWriteSingleCoil(unsigned char func, unsigned char slaveAddress, int regAddress, unsigned short* pRegTable, std::string* log/* = NULL*/  )
{
	if( !m_bOpened || m_hIDComDev == NULL ) 
		return  FALSE;

	unsigned char cmd[32];
	int byteNo = 0;
	cmd[byteNo++] = slaveAddress;	//адрес подчиененного устройства
	cmd[byteNo++] = func;			//номер стандартной функции MODBUS

	cmd[byteNo++] = HIBYTE(regAddress);  // базовый адрес таблицы регистров, старший байт  
	cmd[byteNo++] = LOBYTE(regAddress);  // базовый адрес таблицы регистров, младший  байт

	cmd[byteNo++] = HIBYTE( pRegTable[0] );
	cmd[byteNo++] = LOBYTE( pRegTable[0] );

	unsigned short crc = CRC16(cmd, byteNo);

	cmd[byteNo++] = LOBYTE(crc);	  // контрольная сумма CRC-16,  младший байт   
	cmd[byteNo++] = HIBYTE(crc);	  // контрольная сумма CRC-16,  старший байт  

	DWORD dwBytesWritten = 0;

	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); //ACTION 8 -> Clear serial input buffer	
	PurgeComm(m_hIDComDev, PURGE_TXCLEAR); //ACTION 4 -> Clear serial output buffer

	if ( log != NULL ) 
	{
		*log = "SEND : ";
		PrintHEX(cmd, byteNo, *log);		
	}

	if ( !WriteFile( m_hIDComDev, cmd, byteNo, &dwBytesWritten, NULL) )
	{
		if ( log != NULL ) 
			*log += "\tCOMMAND WRITING FAILED";
		return FALSE;		
	}

	PurgeComm(m_hIDComDev, PURGE_RXCLEAR); // Clear serial input buff.., one more time.
	Sleep(75);

	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	int ind = 0;
	unsigned char recv[256];	
	char ch = -1;

	while ( ComStat.cbInQue > 0 )
	{
		ReadFile( m_hIDComDev, &ch, 1, &dwBytesRead, NULL);
		recv[ind++] = ch;

		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	}

	if (ind == 0)
	{
		if ( log != NULL ) 
			*log += "\tRECIVE : EMPTY";

		return FALSE;
	}

	if ( log != NULL ) 
	{
		*log += "\tRECIVE : ";
		PrintHEX(recv, ind, *log);
	}

	//Проверка контрольной суммы
	unsigned short crcRecive = MAKEWORD(recv[ind - 2], recv[ind - 1]);
	unsigned short crcReciveCalc = CRC16(recv, ind - 2);

	if ( crcRecive != crcReciveCalc)
	{
		if ( log != NULL ) 
			*log += "\tCRC ERROR";

		return FALSE;
	}

	//Проверка адреса устройства и номера функции 
	if (recv[0] != cmd[0] ||  recv[1] != cmd[1])
		return FALSE;
	
	return  TRUE;
}


BOOL SyncSerialPort::ModbusReadInputRegs(unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log/* = NULL*/  )
{
	return  ModbusReadRegs(0x03, slaveAddress, regAddress, regCount, pRegTable, log  );
}

BOOL SyncSerialPort::ModbusReadHoldingRegs(unsigned char slaveAddress, int regAddress, int regCount, unsigned short* pRegTable, std::string* log/* = NULL  */)
{
	return  ModbusReadRegs(0x04, slaveAddress, regAddress, regCount, pRegTable,  log);
}



/////////////////////////////////////////////////////////////////////////////
bool SyncSerialPort::WriteOneByte(unsigned char wb, unsigned char& rb)
{
	DWORD dwBW;
	if ( !WriteFile( m_hIDComDev, &wb, 1, &dwBW, NULL) || dwBW != 1)
		return false;
	
	if ( !ReadOneByte(rb) /*|| rb != wb */)
		return false;

	return true;
}



bool SyncSerialPort::ReadOneByte(unsigned char& bt)
{
	DWORD dwBR;
	EscapeCommFunction(m_hIDComDev, SETRTS);

	if ( !ReadFile( m_hIDComDev, &bt, 1, &dwBR, NULL) || dwBR != 1)
		return false;

	return true;
}