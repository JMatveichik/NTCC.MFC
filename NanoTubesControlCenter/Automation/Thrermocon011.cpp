#include "StdAfx.h"
#include "Thrermocon011.h"


static unsigned short paramsAdr[] = 
{
	0xda70,	// T_Real
	0xa450, // T_Prog
	0x4500	// Power
};

Thrermocon011::Thrermocon011(void)
{
	memset(&m_values, 0, sizeof(double)*3);
	::InitializeCriticalSection(&m_writeCS);
}

Thrermocon011::~Thrermocon011(void)
{
	SwitchMode(true);
	::DeleteCriticalSection(&m_writeCS);
}

bool Thrermocon011::Create(std::string name, unsigned char portNo)
{
	if ( !IdentifiedPhysDevice::Create(name) )
		return false;

	if ( !comPort.Open(portNo, 600) )
		return false;

	
	comPort.GetComState(dcb0);
	
	dcb0.fParity = TRUE;
	dcb0.Parity = MARKPARITY;
	dcb0.StopBits = TWOSTOPBITS;
	dcb0.ByteSize = 8;

	dcb0.fDtrControl = DTR_CONTROL_DISABLE;
	dcb0.fRtsControl = RTS_CONTROL_DISABLE;

	if ( !comPort.SetComState(dcb0) )
		return false;

	comPort.GetComState(dcb1);
	dcb1.Parity = SPACEPARITY;


	COMMTIMEOUTS comTO;
	comPort.GetComTimeouts(comTO);

	comTO.ReadIntervalTimeout = MAXDWORD;
	comTO.ReadTotalTimeoutMultiplier = MAXDWORD;
	comTO.ReadTotalTimeoutConstant = 200;
	comTO.WriteTotalTimeoutMultiplier = 0;
	comTO.WriteTotalTimeoutConstant = 0;

	if ( !comPort.SetComTimeouts(comTO) )
		return false;

	if ( !SetTemperature(25.0f) )
		return false;

	return true;
}

bool Thrermocon011::Write(unsigned short par, unsigned char data)
{	
	unsigned char cmdByte;
	unsigned char rb;

	cmdByte = 0x80 + (LOBYTE(par) & 0x60);	

	comPort.SetComState(dcb0);
	if ( !comPort.WriteOneByte(cmdByte, rb) )
		return false;	

	comPort.SetComState(dcb1);
	cmdByte = HIBYTE(par);

	if ( !comPort.WriteOneByte(cmdByte, rb) )
		return false;

	cmdByte = data;

	if ( !comPort.WriteOneByte(cmdByte, rb) )
		return false;

	return true;
}

bool Thrermocon011::Read(unsigned short par, unsigned char& data)
{
	unsigned char cmdByte;
	unsigned char rb;

	cmdByte = LOBYTE(par) & 0x60;

	comPort.SetComState(dcb0);
	if ( !comPort.WriteOneByte(cmdByte, rb) )
		return false;	

	cmdByte = HIBYTE(par);
	comPort.SetComState(dcb1);

	if ( !comPort.WriteOneByte(cmdByte, data) )
		return false;

	return true;
}

bool Thrermocon011::SwitchMode(bool bEditMode)
{
	::EnterCriticalSection(&m_writeCS);
	if ( !Write(0xA140, 0x58) )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	if ( !Write(0x3B00, 0x1F) )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	if ( !Write(0x2000, 0x80) )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	const unsigned char mode	 = bEditMode ? 0x02 : 0x03;
	if ( !Write(0xA040, mode) )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	if ( !Write(0x2700, 0x20) )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}
	
	Sleep(600);

	unsigned char curModeMark;
	if ( ! Read(0xA840, curModeMark) )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	const unsigned char modeMark = bEditMode ? 0xED : 0xAE;
	if ( curModeMark != modeMark )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	::LeaveCriticalSection(&m_writeCS);
	return true;
}

bool Thrermocon011::SetTemperature(double T)
{

	//переключаемся в режим редактирования
	if ( !SwitchMode(true) )
		return false;

	::EnterCriticalSection(&m_writeCS);
	//ПОДГОТОВКА
	if ( !Write(0xA140, 0x58) )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	if ( !Write(0x2640, 0x1F) )
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	if ( !Write(0x2440, 0x01) )	//номер программы 1
	{
		::LeaveCriticalSection(&m_writeCS);
		return false;
	}

	unsigned short T_ = unsigned short( T * 32);  

	unsigned char data[8];
	data[0] = 0x00; data[1] = 0x08; // через 1 мин 
	data[2] = HIBYTE(T_); data[3] = LOBYTE(T_);

	data[4] = 0xBB; data[5] = 0x80; // бесконечно 
	data[6] = HIBYTE(T_); data[7] = LOBYTE(T_);

	//записываем программу
	for (int i = 0; i < 8; i++)
	{
		if ( !Write(i*0x100 + 0x7440, data[i]) )	//данные
		{
			::LeaveCriticalSection(&m_writeCS);
			return false;
		}
	}


	//переключаемся в автоматический режим
	if ( !SwitchMode(false) )
		return false;

	lastWritenT = T;

	::LeaveCriticalSection(&m_writeCS);

	return true;
}



void Thrermocon011::ControlProc()
{
	ReadChannels(NULL);
}


//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
void	Thrermocon011::ReadChannels(double* data/*  = NULL*/) const
{
	//для каждого параметра (канала)
	for (unsigned char i = 0; i < 3; i++)
		ReadChannel(i);
	
	if (data != NULL)
		memcpy(data, m_values, sizeof(double)*3);

	
}

//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
double  Thrermocon011::ReadChannel(unsigned char channel)  const
{
	//для каждого параметра
	unsigned char cmdByte;
	unsigned char rb;

	::EnterCriticalSection(&m_writeCS);

	cmdByte = LOBYTE(paramsAdr [channel] ) & 0x60;

	comPort.SetComState(dcb0);
	if ( !comPort.WriteOneByte(cmdByte, rb) || rb != cmdByte)
		m_values[channel] = DBL_MIN;	

	cmdByte = HIBYTE(paramsAdr [channel] );

	unsigned char rb1 = 0x00;
	unsigned char rb2 = 0x00;

	comPort.SetComState(dcb1);
	if ( !comPort.WriteOneByte(cmdByte, rb1))
		m_values[channel] = DBL_MIN;

	unsigned short res = 0x00;
	if ( channel < 2 )
	{
		comPort.SetComState(dcb0);
		cmdByte = 0x10;
		if ( !comPort.WriteOneByte(cmdByte, rb2))
			m_values[channel] = DBL_MIN;	
	}

	if ( channel < 2 )
	{
		res = MAKEWORD(rb2, rb1);
		m_values[channel] = res / 32.0;
	}
	else
	{
		res = MAKEWORD(rb1, rb2);
		m_values[channel] = res / 2.56;
	}

	::LeaveCriticalSection(&m_writeCS);

	return m_values[channel];
}

// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  Thrermocon011::GetChannelData(unsigned char channel)  const
{
	double val;
	
	::EnterCriticalSection(&m_dataCS);
	val = m_values[channel];
	::LeaveCriticalSection(&m_dataCS);

	return val;
}

//////////////////////////////////////////////////////////////////////////

//Записать данные в аналоговый выход 
double  Thrermocon011::WriteToChannel(unsigned char channel, double val) const
{
	const_cast<Thrermocon011*>(this)->SetTemperature(val);
	return lastWritenT;
}

//Записать данные во все каналы
void Thrermocon011::WriteToAllChannels(double* vals/* = NULL*/) const 
{
	
}

//Сохранить состояние аналоговых входов
void Thrermocon011::SaveChannelsStateAO() const
{
	storeT = lastWritenT;
}

//Восстановить состояние аналоговых выходов 
void Thrermocon011::RestoreChannelsStateAO() const
{
	const_cast<Thrermocon011*>(this)->SetTemperature(storeT);
}

//Получить последнее записанное в аналоговый выход значение 
double Thrermocon011::GetChannelStateAO (unsigned char channel) const
{
	return lastWritenT;
}

//Получить выходной диапазон канала 
void Thrermocon011::GetMinMax(unsigned char channel, double& minOut, double& maxOut) const
{
	minOut = 25.0;
	maxOut = 1200.0;
}