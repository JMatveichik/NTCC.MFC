#include "StdAfx.h"
#include "FlexKraftPS.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

FlexKraftPS::FlexKraftPS() :
IdentifiedPhysDevice("Kraftelectronik", "FLEXKRAFT-PS"),
DIN_CHANNELS_COUNT(48),
DOUT_CHANNELS_COUNT(16)
{
	//адреса регистров для аналоговых выходов	
	rngAO.push_back(MEASSURE_RANGE_INFO(0, "Заданное напряжение", "В", 0, 30));
	aoutRegs.push_back(0x02);

	rngAO.push_back(MEASSURE_RANGE_INFO(1, "Заданный ток", "A", 0, 300));
	aoutRegs.push_back(0x03);

	rngAO.push_back(MEASSURE_RANGE_INFO(2, "Номер программы", "", 1, 4));
	aoutRegs.push_back(0x09);

		
	AOUT_CHANNELS_COUNT = 	(int)aoutRegs.size();
	
	//адреса регистров для аналоговых входов
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "Выходное напряжение", "В", -30, 30));
	ainRegs.push_back(0x04);

	rngAI.push_back(MEASSURE_RANGE_INFO(1, "Выходной ток", "А", -300, 300));
	ainRegs.push_back(0x05);
	
	AIN_CHANNELS_COUNT = (int)ainRegs.size();

	dataAO.assign(AOUT_CHANNELS_COUNT, 0.0);
	dataAI.assign(AIN_CHANNELS_COUNT, 0.0);

	//множитель для напряжения
	channelFactor.push_back(0.01);

	//множитель для тока
	channelFactor.push_back(1.0);

	channelFactor.push_back(1.0);
	
}


FlexKraftPS::~FlexKraftPS(void)
{
/*	for (std::vector<LPMEASSURE_RANGE_INFO>::iterator it = rngAO.begin(); it != rngAO.end(); ++it )
		delete (*it);

	for (std::vector<LPMEASSURE_RANGE_INFO>::iterator it = rngAI.begin(); it != rngAI.end(); ++it )
		delete (*it);
		*/
}

FlexKraftPS* FlexKraftPS::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<FlexKraftPS> obj ( new FlexKraftPS() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool FlexKraftPS::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();

	std::string sAtr;
	if (!appXML.GetNodeAttributeByName(pNode, "PORT", sAtr))
	{
		str << "Port number for device not set (tag <PORT>)" << std::ends;
		err.SetLastError(DCE_PORT_NUMBER_NOT_SET, str.str() );
		return false;	
	}

	int port =  atoi(sAtr.c_str());
	CommandManager* pMan = CommandManager::GetCommandManager(port);
	if ( pMan == NULL )
	{
		str << "Command manager assigned with port COM" << (int)port << " not found" << std::ends;
		err.SetLastError( DCE_PORT_NOT_AVAILABLE, str.str() );
		return false;
	}
	portNo = port;

	if ( !appXML.GetNodeAttributeByName(pNode, "ADDRESS", sAtr) )
	{
		str << "Address must be set for this device type" << std::ends;		
		err.SetLastError(DCE_DEVICE_ADDRESS_NOT_SET, str.str() );
		return false;
	}

	int adr = atoi(sAtr.c_str() );
	devAdr = adr;	

	if (! OnPrepareDevice( ) )
	{
		str << "Device not response" << std::ends;		
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;			
	}

	return true;
}

void FlexKraftPS::ControlProc()
{
	ReadChannels();

	UpdateChannelsStateAO();

	UpdateChannelsStateDI();
}

bool FlexKraftPS::OnPrepareDevice()
{
	CHECKCMDMANAGER(pMan, portNo);

	//подготовка дополнительных параметров последовательного порта
	DCB dcb; 
	pMan->GetComState( dcb );

	dcb.ByteSize = 8;			//длина байта данных
	dcb.fParity = TRUE;		//контроль четности
	dcb.Parity = EVENPARITY;
	dcb.StopBits = ONESTOPBIT;	//число стоповых битов

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	if ( !pMan->SetComState(dcb) )
	{
		str << "COM" << (int)portNo<< " initialization failed" << std::ends;
		err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
		return false;
	}

	if ( !UpdateChannelsStateDI() )
		return false;

	if ( !UpdateChannelsStateDO() )
		return false;

	//точность задания тока если задана повышенная точность
	channelFactor[1] = dinChState.test(10) ? 0.1 : 1.0;

	return true;
}


///////////////////////////////////////////////////
//Реализация интерфейса IDigitalInput

// ЦИФРОВЫЕ ВХОДЫ 0-15 РЕГИСТР СОСТОЯНИЯ
//R Status register b00 Alarm No Alarm		Причина сигнала тревоги может быть найдена в регистрах ALARM
//R Status register b01 Warning No Warning  Причина сигнала предупреждения	может быть найдена в регистрах Warnings
//R Status register b02 Remote Local		В локальном режиме управление с	помощью последовательной шины	невозможно
//R Status register b03 Running Not running Если задано, работа осуществляется в соответствии с заданными параметрами
//R Status register b04 Ah>Ah-Limit Ah<Ah-Limit	Показывает достижение	предустановленного предела по	ампер-часам (сторона А для	двойного выхода)
//R Status register b05 Ah>Ah-Limit Ah<Ah-Limit Показывает достижение предустановленного предела по ампер-часам (сторона В для двойного выхода)
//R Status register b06 Timer Timer Limit Timer<Timer Limit Показывает достижение предела для таймера процессов
//R Status register b07 Voltage Mode Current Mode Показывает, что сторона А работает в режиме напряжения. Импеданс нагрузки может быть высоким
//R Status register b08 Voltage Mode Current Mode Показывает, что сторона В работает в режиме напряжения. Импеданс нагрузки может быть высоким
//R Status register b09 On or Standby Off Показывает статус устройства (аналогично зеленому светодиоду на мониторе)
//R Status register b10 Current Hires Current normal Бит устанавливается, если разрешение для регистров тока равно 0,1А 
//R Status register b11 Running B Not running B Если Running, блок работает в соответствии с настройками для B 
//R Status register b12 Timer>Timer Limit B Timer<Timer Limit B Показывает, если предустановленный лимит времени исчерпан на стороне B 
//R Status register b13 Auxiliary In A active Программируемый цифровой вход 
//R Status register b14 Auxiliary In B active Программируемый цифровой вход
//R Status register b15 Зарезервировано 

// ЦИФРОВЫЕ ВХОДЫ 16-31 РЕГИСТР ПЕРЕДУПРЕЖДЕНИЯ
//R Warnings register b00 Зарезервировано
//R Warnings register b01 Зарезервировано
//R Warnings register b02 Зарезервировано
//R Warnings register b03 Activated OK Тревога от реверса полюсов A
//R Warnings register b04 Activated OK Тревога от реверса полюсов B
//R Warnings register b05 Activated OK	Устройство работает на полную	мощность
//R Warnings register b06 Activated OK 	Недопустимый параметр. Один или 	несколько параметров неверно заданы. Проверьте параметры
//R Warnings register b07 Activated OK	Предупреждение о перегреве. Один	или несколько блоков питания уже	выключились или выключатся	вскоре.
//R Warnings register b08 Зарезервировано
//R Warnings register b09 Зарезервировано
//R Warnings register b10 Activated OK 	Попытка передать необходимую мощность провалилась. Это 	произошло, возможно, из-за сбоев в	сети
//R Warnings register b11 Зарезервировано
//R Warnings register b12 Зарезервировано
//R Warnings register b13 Зарезервировано
//R Warnings register b14 Зарезервировано
//R Warnings register b15 Зарезервировано

// ЦИФРОВЫЕ ВХОДЫ 32-47 РЕГИСТР АВАРИЙ
//R Warnings register b15
//R Alarms register b00 Activated OK Слишком большая внутренняя	температура. Устройство было	выключено. Проверьте систему охлаждения
//R Alarms register b01	Зарезервировано
//R Alarms register b02 Activated OK Критическая ошибка в одном или	нескольких блоках питания.	Возможно, необходимо провести технический осмотр
//R Alarms register b03 Activated OK Зарезервировано
//R Alarms register b04 Activated OK Зарезервировано
//R Alarms register b05 Activated OK Контроль полярности. Ошибка в настройках CFG реверса	полярности
//R Alarms register b06 Activated OK Внешняя тревога 1
//R Alarms register b07 Зарезервировано
//R Alarms register b08 Зарезервировано
//R Alarms register b09 Зарезервировано
//R Alarms register b10 Зарезервировано
//R Alarms register b11 Зарезервировано
//R Alarms register b12 Зарезервировано
//R Alarms register b13 Зарезервировано
//R Alarms register b14 Зарезервировано
//R Alarms register b15 Зарезервировано 


//Получить число дискретных входов
int FlexKraftPS::GetChannelsCountDI() const
{
	return DIN_CHANNELS_COUNT;
}

//Обновить состояния дискретных входов
bool FlexKraftPS::UpdateChannelsStateDI() const
{
	CHECKCMDMANAGER(pMan, portNo);

	//ГОТОВИМ КОМАНДУ MODBUS
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regAddress   = 0x10;
	cmd.regCount	 = 48; //coils count
	cmd.func		 = 0x02;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	unsigned long long uCoilsStatus = 0;
	int byteCount = (int)ceil( cmd.regCount / 8.0 );
	
	for (int i = 0; i < byteCount; i++)
	{
		byte bt = LOBYTE( cmd.response[i] );
		unsigned long long msk = bt;
		msk = msk <<  8 * i;		
		uCoilsStatus |= msk;
	}

	std::bitset<48> st(uCoilsStatus);
	dinChState = st;

	return true;
}

//Прочитать состояние одного дискретного входа
bool FlexKraftPS::GetChannelStateDI(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDI();

	return dinChState.test( ch );
}


////////////////////////////////////////44444//////////////////////////////////
//Реализация интерфейса IDigitalOutput
//Получение общего числа цифровых выходов 
int FlexKraftPS::GetChannelsCountDO() const
{
	return DOUT_CHANNELS_COUNT;
}

//Обновить текущее состояния дискретных выходов	
bool FlexKraftPS::UpdateChannelsStateDO() const
{
	CHECKCMDMANAGER(pMan, portNo);

	//ГОТОВИМ КОМАНДУ MODBUS
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regAddress   = 0x10;
	cmd.regCount	 = 16; //coils count
	cmd.func		 = 0x01;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	unsigned long long uCoilsStatus = 0;
	int byteCount = (int)ceil( cmd.regCount / 8.0 );

	for (int i = 0; i < byteCount; i++)
	{
		byte bt = LOBYTE( cmd.response[i] );
		unsigned long long msk = bt;
		msk = msk <<  8 * i;		
		uCoilsStatus |= msk;
	}

	std::bitset<16> st(uCoilsStatus);
	doutChState = st;

	return true;
}

//Получить состояние цифрового выхода
bool FlexKraftPS::GetChannelStateDO(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDO();

	return doutChState.test( ch );
}

//Установить состояние цифрового выхода 
bool  FlexKraftPS::SetChannelStateDO(unsigned char ch, bool enable)
{
	if ( ch < 0 || ch >= DOUT_CHANNELS_COUNT )
		return false;

	CHECKCMDMANAGER(pMan, portNo);

	//Готовим команду MODBUS
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 

	cmd.regAddress   = 0x10 + ch; //регистр Control
	cmd.func		 = 0x05;

	cmd.response[0]  = enable ? 0xff00 : 0x0000;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	Sleep(75);

	//Обновляем состояние цифровых выходов
	UpdateChannelsStateDO();

	return true;
}

//////////////////////////////////////////////////////////////////////////
//interface IAnalogInput
//Получение общего числа аналоговых входов
int FlexKraftPS::GetChannelsCountAI() const
{
	return AIN_CHANNELS_COUNT;
}

//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
void FlexKraftPS::ReadChannels(double* data/*  = NULL*/) const
{
	
	
	for(int i = 0; i < AIN_CHANNELS_COUNT; i++)
	{
		//Lock();
		dataAI[i] = ReadChannel(i);
		//Unlock();
	}

	if (data != NULL) 
	{
		Lock();
		memcpy(data, dataAI.data(), sizeof(double)*AIN_CHANNELS_COUNT);
		Unlock();
	}
	

	
}

//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
double  FlexKraftPS::ReadChannel(unsigned char ch)  const
{
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return 0.0;

	CHECKCMDMANAGER(pMan, portNo);

	//
	MODBUS_SERIAL_COMMAND cmd;
	memset(&cmd, 0, sizeof(MODBUS_SERIAL_COMMAND));
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	cmd.regAddress   = ainRegs[ch];
	cmd.func		 = 0x04;	

	if ( !pMan->SendCommand(&cmd, false) )
		return false;
	
	double val = (double)((short)cmd.response[0]);
	
	val *= channelFactor[ch];

	return val;
}

// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  FlexKraftPS::GetChannelData(unsigned char ch)  const
{
	double val = 0.0;
	//Lock();
	
	val = dataAI[ch];

	//Unlock();
	return val;
}

//Получить  информацию об измеряемом диапазоне канала
void FlexKraftPS::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	*lpmci = rngAI[ch];
}

//Установить  диапазон  аналогового входа
bool FlexKraftPS::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить все возможные  диапазоны  измерения
void FlexKraftPS::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	rngs.push_back(rngAI[ch]);
}

//////////////////////////////////////////////////////////////////////////
/// IAnalogOutput
//Получение общего числа аналоговых выходов 
int FlexKraftPS::GetChannelsCountAO() const
{
	return AOUT_CHANNELS_COUNT;
}

//Записать данные в аналоговый выход 
double  FlexKraftPS::WriteToChannel(unsigned char ch, double val)
{
	CHECKCMDMANAGER(pMan, portNo);
	
	if ( ch < 0 || ch >= AOUT_CHANNELS_COUNT )
		return 0.0;

	double oldVal = dataAO[ch];
	//
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	cmd.regAddress   = aoutRegs[ch];
	cmd.func		 = 0x06;	

	unsigned short v = (unsigned short)(val/channelFactor[ch]);
	cmd.response[0] = v;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	dataAO[ch] = val;	
	return oldVal;
}

//Записать данные во все каналы
void FlexKraftPS::WriteToAllChannels(const std::vector<double>& vals)
{

}

bool FlexKraftPS::UpdateChannelsStateAO() const
{
	
	CHECKCMDMANAGER(pMan, portNo);

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	
	cmd.func		 = 0x03;	
	for (unsigned int ch = 0; ch < AOUT_CHANNELS_COUNT; ch++) 
	{
		cmd.regAddress   = aoutRegs[ch];
		if ( !pMan->SendCommand(&cmd, false) )
			return false;

		double val = (double)cmd.response[0];
		val *= channelFactor[ch];

		Lock();

		dataAO[ch] = val;

		Unlock();
	}

	return true;
}
//Получить последнее записанное в аналоговый выход значение 
double FlexKraftPS::GetChannelStateAO (unsigned char ch) const
{
	

	if ( ch < 0 || ch >= AOUT_CHANNELS_COUNT )
		return 0.0;
	
	double val;

	Lock();

	val = dataAO[ch];

	Unlock();

	return val;
}

//Получить выходной диапазон канала 
void FlexKraftPS::GetMinMax(unsigned char ch, double& minOut, double& maxOut) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
	{
		minOut = maxOut = 0;
		return;
	}

	minOut = rngAO[ch].mins;
	maxOut = rngAO[ch].maxs;
}

//Получить  информацию об диапазоне канала
void FlexKraftPS::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	*lpmci = rngAO[ch];
}

//Установить  диапазон  аналогового выхода
bool FlexKraftPS::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}	

//Получить все возможные  диапазоны  
void FlexKraftPS::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();	

	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	rngs.push_back(rngAO[ch]);
}