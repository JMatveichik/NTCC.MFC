#include "StdAfx.h"
#include "OwenTPM201.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

OwenBase::OwenBase(std::string model) :
IdentifiedPhysDevice("OWEN", model) 
{
	
}

OwenBase::~OwenBase(void)
{
	
}

bool OwenBase::CreateFromXMLNode(IXMLDOMNode* pNode)
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
	comPortNo = port;

	if ( !appXML.GetNodeAttributeByName(pNode, "ADDRESS", sAtr) )
	{
		str << "Address must be set for this device type" << std::ends;		
		err.SetLastError(DCE_DEVICE_ADDRESS_NOT_SET, str.str() );
		return false;
	}

	int adr = atoi(sAtr.c_str());
	devAddres = adr;
	

	if (! OnPrepareDevice( ) )
		return false;	

	return true;
}


bool OwenBase::OnPrepareDevice()
{
	return true;
}


OwenTPM::OwenTPM(std::string model, int coutAI, int coutAO, int coutDO ) :
	OwenBase(model), 
		INPUT_ANALOG_CHANNELS_COUNT(coutAI), 
		INPUT_DIGITAL_CHANNELS_COUNT(16),
		OUTPUT_ANALOG_CHANNELS_COUNT(coutAO),
		OUTPUT_DIGITAL_CHANNELS_COUNT(coutDO)
{
	dTCoef = 1.0;	
	
	dataAI.assign(INPUT_ANALOG_CHANNELS_COUNT, 0.0);
	dataAO.assign(OUTPUT_ANALOG_CHANNELS_COUNT, 0.0);
	dataDO.assign(OUTPUT_DIGITAL_CHANNELS_COUNT, false);

	::InitializeCriticalSection(&m_ModBusCS);
}

OwenTPM::~OwenTPM(void)
{
	::DeleteCriticalSection(&m_ModBusCS);
}

bool OwenTPM::OnPrepareDevice()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return false;

	//подготовка дополнительных параметров последовательного порта
	DCB dcb; 
	pMan->GetComState( dcb );

	dcb.ByteSize = 8;		//длина байта данных
	dcb.fParity = FALSE;	//контроль четности
	dcb.StopBits = TWOSTOPBITS; // число стоповых битов

	if ( !pMan->SetComState(dcb) )
	{
		str << "Initialize port COM" << (int)comPortNo << " failed" << std::ends;
		err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str());
		return false;
	}

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender = this;
	cmd.slaveAddress = devAddres;
	cmd.func = 0x03;
	cmd.regCount = 1;

	//точность вывода температуры
	cmd.regAddress = 0x0201;

	if ( !pMan->SendCommand(&cmd, false) )
	{
		str << "Temperature precition command failed" << std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str());
		return false;
	}

	unsigned short curTPre = cmd.response[0];
	dTCoef = pow(10.0, int(-curTPre) );

	//положение десятичной точки
	cmd.regAddress = 0x0202;
	if ( !pMan->SendCommand(&cmd, false) )
	{
		str << "Decimial point position command failed" << std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str());
		return false;
	}

	unsigned short curPointPos = cmd.response[0];
	dTLimitsCoef = pow(10.0, int(-curPointPos) );


	//Получаем минимальный предел диапазона измеряемой температуры 
	cmd.regAddress = 0x0203;
	if ( !pMan->SendCommand(&cmd, false) )
	{
		str << "Low temperature limit command failed" << std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str());
		return false;
	}

	unsigned short val = cmd.response[0];
	//знаковые значения передаются в обратном коде
	//производим преобразрвание с учетом положения десятичной точки
	unsigned short c = 1 << (sizeof(unsigned short)*8 - 1);
	minT = ((c ^ val) - c) * dTLimitsCoef;
	minT = (minT < 0) ? 0.0 : minT; //отсекаем отрицательные значения


	//Получаем максимальный предел диапазона измеряемой температуры 
	cmd.regAddress = 0x0204;

	if ( !pMan->SendCommand(&cmd, false) )
	{
		str << "Low temperature limit command failed" << std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str());
		return false;
	}

	val = cmd.response[0];

	maxT = ((c ^ val) - c) * dTLimitsCoef;

	//Заполнение информации по измеряемым диапазонам
	BuildMeassureRangeInfo();

	//Заполнение информации по картам регистров
	BuildRegestersMaps();

	//считываем записанные в аналоговые выходы значения
	for (auto it = aoChannelsMap.begin(); it != aoChannelsMap.end(); ++it )
	{
		MODBUS_SERIAL_COMMAND cmd;
		cmd.pSender = this;
		cmd.slaveAddress = devAddres;
		cmd.func = 0x03;
		cmd.regCount = 1;
		cmd.regAddress = (*it).second;

		if ( !pMan->SendCommand(&cmd, false) )
			continue;

		double val = cmd.response[0];
		int ch  = (*it).first;

		PrepareChannelValue(ch, val, false);

		dataAO[ch] = val;
	}

	//обновляем состояние цифровых выходов
	UpdateChannelsStateDO();

	return true;
}


bool OwenTPM::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !OwenBase::CreateFromXMLNode(pNode) )
		return false;

	return true;
}

//Функция процесса выполяется циклически через заданный интервал времени
void OwenTPM::ControlProc()
{
	ReadChannels(NULL);
	//UpdateChannelsStateDI();
}



//Получение общего числа аналоговых входов
int OwenTPM::GetChannelsCountAI() const 
{
	return INPUT_ANALOG_CHANNELS_COUNT; 
}

//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
void OwenTPM::ReadChannels(double* channelsData/*  = NULL*/) const
{
	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return;

	::EnterCriticalSection(&m_ModBusCS);

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender = this;
	cmd.slaveAddress = devAddres;
	cmd.func = 0x03;
	cmd.regCount = 9;
	cmd.regAddress = 0x0000;

	if ( !pMan->SendCommand(&cmd, false) )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return;
	}

	regs.assign(cmd.response, cmd.response + 9);
	curStatus = regs[0];


	for (auto it = aiChannelsMap.begin(); it != aiChannelsMap.end(); ++it)
	{
		int ch = (*it).first;
		int rn = (*it).second;

		double val = regs[rn];
		PrepareChannelValue(ch, val, false);

		dataAI[ch] = val;

		if (channelsData != NULL)
			channelsData[ch] = dataAI[ch];
	}
	


	//преобразовываем
	::LeaveCriticalSection(&m_ModBusCS);

	
	/*for (unsigned char i = 0; i < INPUT_ANALOG_CHANNELS_COUNT; i++)
	{
		dataAI[i] = ReadChannel(i);
		if (channelsData != NULL)
			channelsData[i] = dataAI[i];
	}
	*/
}

//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
double  OwenTPM::ReadChannel(unsigned char ch)  const
{
	/*CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return dataAI[ch];

	auto it = aiChannelsMap.find(ch);

	if ( it == aiChannelsMap.end() )
		return dataAI[ch];

	::EnterCriticalSection(&m_ModBusCS);

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender = this;
	cmd.slaveAddress = devAddres;
	cmd.func = 0x03;
	cmd.regCount = 1;
	cmd.regAddress = (*it).second;

	if ( !pMan->SendCommand(&cmd, false) )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return dataAI[ch];
	}

	double val = cmd.response[0];

	//преобразовываем 
	PrepareChannelValue(ch, val, false);

	::LeaveCriticalSection(&m_ModBusCS);
	*/
	return GetChannelData(ch);
}

// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  OwenTPM::GetChannelData(unsigned char ch)  const
{
	double val;
	::EnterCriticalSection(&m_ModBusCS);

	val = dataAI[ch];

	::LeaveCriticalSection(&m_ModBusCS);
	return val;
}

//Получить  информацию об измеряемом диапазоне канала
void OwenTPM::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci = mrciIn[ch];	
}

//Установить  диапазон  аналогового входа
bool OwenTPM::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить все возможные  диапазоны  измерения
void OwenTPM::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	rngs.push_back(mrciIn[ch]);
}

/////////////////////////////////////
//Реализация интерфейса IDigitalInput
int OwenTPM::GetChannelsCountDI() const 
{ 
	return INPUT_DIGITAL_CHANNELS_COUNT; 
}

bool OwenTPM::GetChannelStateDI(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDI();

	int mask = 1 << ch;
	bool chState = ((curStatus & mask) == mask);

	return chState;
}

bool OwenTPM::UpdateChannelsStateDI() const
{
	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return false;

	::EnterCriticalSection(&m_ModBusCS);

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender = this;
	cmd.slaveAddress = devAddres;
	cmd.func = 0x03;
	cmd.regCount = 1;
	cmd.regAddress = 0x0000;

	if ( !pMan->SendCommand(&cmd, false) )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return false;
	}

	curStatus = cmd.response[0];	

	::LeaveCriticalSection(&m_ModBusCS);

	return true;
}

/////////////////////////////////////
//Реализация интерфейса IAnalogOutput
//Получение общего числа аналоговых выходов 
int OwenTPM::GetChannelsCountAO() const
{
	return OUTPUT_ANALOG_CHANNELS_COUNT;
}

//Записать данные в аналоговый выход 
double  OwenTPM::WriteToChannel(unsigned char ch, double val)
{
	
	::EnterCriticalSection(&m_ModBusCS);

	if (val == dataAO[ch])
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return val;
	}
		

	double oldVal = dataAO[ch];

	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan != NULL )
	{
		MODBUS_SERIAL_COMMAND cmd;
		cmd.pSender = this;
		cmd.slaveAddress = devAddres;
		cmd.func = 0x10;
		cmd.regCount = 1;

		PrepareChannelValue(ch, val, true);
		unsigned short newVal = (unsigned short) val ;

		//задаем новую 
		cmd.regAddress = aoChannelsMap[ch];
		cmd.response[0] = newVal;

		//записывам новое заданное значение температуры
		if ( !pMan->SendCommand(&cmd, true) )

		{
			::LeaveCriticalSection(&m_ModBusCS);
			return oldVal;

		}

		Sleep(200);

		//Обновляем записанное значение
		cmd.func = 0x03;
		cmd.regCount = 1;

		//записывам новое заданное значение температуры
		if ( !pMan->SendCommand(&cmd, true) )

		{
			::LeaveCriticalSection(&m_ModBusCS);
			return oldVal;
		}

		double placedVal = cmd.response[0];
		PrepareChannelValue(ch, placedVal, false);

		/*if (placedVal != val)
			ASSERT(FALSE);*/

		dataAO[ch] = placedVal;
	}
	::LeaveCriticalSection(&m_ModBusCS);
	return oldVal;
}

//Записать данные во все каналы
void OwenTPM::WriteToAllChannels(const std::vector<double>& vals)
{
	//ничего не делаем
}

//Получить последнее записанное в аналоговый выход значение 
double OwenTPM::GetChannelStateAO (unsigned char ch) const
{
	double val;
	::EnterCriticalSection(&m_ModBusCS);

	val = dataAO[ch];

	::LeaveCriticalSection(&m_ModBusCS);
	return val;
}

//Получить выходной диапазон канала 
void OwenTPM::GetMinMax(unsigned char ch, double& minOut, double& maxOut) const
{
	minOut = mrciOut[ch].mins;
	maxOut = mrciOut[ch].maxs;
}

//Получить  информацию об диапазоне канала
void OwenTPM::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci = mrciOut[ch];
}

//Установить  диапазон  аналогового выхода
bool OwenTPM::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить все возможные  диапазоны  
void OwenTPM::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{
	rngs.clear();
	rngs.push_back(mrciOut[ch]);

}


/////////////////////////////////////
//Реализация интерфейса IDigitalOutput
//Получение общего числа цифровых выходов 
int OwenTPM::GetChannelsCountDO() const
{
	return OUTPUT_DIGITAL_CHANNELS_COUNT;
}

//Обновить текущее состояния дискретных выходов	
bool OwenTPM::UpdateChannelsStateDO() const
{
	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	::EnterCriticalSection(&m_ModBusCS);

	if ( pMan == NULL )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return false;
	}

	for (auto it = doChannelsMap.begin(); it != doChannelsMap.end(); ++it )
	{
		MODBUS_SERIAL_COMMAND cmd;
		cmd.pSender = this;
		cmd.slaveAddress = devAddres;
		cmd.func = 0x03;
		cmd.regCount = 1;
		cmd.regAddress = (*it).second;

		if ( !pMan->SendCommand(&cmd, false) )
			continue;
		
		dataDO[(*it).first] = (cmd.response[0] == 0x00) ? false : true;
	}

	::LeaveCriticalSection(&m_ModBusCS);
	return true;
}

//Получить состояние цифрового выхода
bool OwenTPM::GetChannelStateDO(unsigned char channel, bool update) const
{
	if (update)
		UpdateChannelsStateDO();
	
	return dataDO[channel];
}

//Установить состояние цифрового выхода 
bool  OwenTPM::SetChannelStateDO(unsigned char ch, bool enable)
{
	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);
	
	::EnterCriticalSection(&m_ModBusCS);

	if ( pMan == NULL )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return false;
	}

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender = this;
	cmd.slaveAddress = devAddres;
	cmd.func = 0x10;
	cmd.regCount = 1;

	cmd.regAddress = doChannelsMap[ch];
	cmd.response[0] = enable ? 0x01 : 0x00;

	//записывам новое заданное значение температуры
	if ( !pMan->SendCommand(&cmd, true) )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return false;
	}

	dataDO[ch] = enable;
	
	::LeaveCriticalSection(&m_ModBusCS);
	return true;		
}

//////////////////////////////////////////////////////////////////////////
/// OWEN TPM201
OwenTPM201::OwenTPM201() : OwenTPM("TPM201", 1, 1, 1)
{
	
}

OwenTPM201::~OwenTPM201()
{

}
OwenTPM201* OwenTPM201::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<OwenTPM201> obj ( new OwenTPM201() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();	
}

//Заполнение информации по измеряемым диапазонам
void OwenTPM201::BuildMeassureRangeInfo()
{
	MEASSURE_RANGE_INFO msri;
	mrciOut.assign(OUTPUT_ANALOG_CHANNELS_COUNT, msri);
	mrciIn.assign(INPUT_ANALOG_CHANNELS_COUNT, msri);

	mrciIn[0].rngid = mrciOut[0].rngid = 0;				//идентификатор измеряемого диапазона
	mrciIn[0].desc  = mrciOut[0].desc  = "Температура";//измеряемая величина
	mrciIn[0].units = mrciOut[0].units = "­°C";		 //единицы	 измерения
	mrciIn[0].mins  = mrciOut[0].mins  = minT;			//минимальное значение измеряемой величины
	mrciIn[0].maxs  = mrciOut[0].maxs  = maxT;			//максимальное значение измеряемой величины	
}

//Заполнение информации по картам регистров
void OwenTPM201::BuildRegestersMaps()
{
	//аналоговые входы
	aiChannelsMap.insert(std::make_pair(0, 0x0001));

	//аналоговые выходы
	aoChannelsMap.insert(std::make_pair(0, 0x0002));
}

//Преобразовать данные канала с учетом текущих настроек положения плавающей точки
void OwenTPM201::PrepareChannelValue(unsigned char channel, double& val, bool forWrite ) const
{
	switch (channel)
	{
	case 0:
		val = forWrite ? (val / dTLimitsCoef) : (val * dTLimitsCoef);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
/// OWEN TPM210
OwenTPM210::OwenTPM210() : OwenTPM ("TPM210", 2, 2, 1)
{
	
	
	
}

OwenTPM210::~OwenTPM210()
{
}

OwenTPM210* OwenTPM210::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<OwenTPM210> obj ( new OwenTPM210() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();	
}

//Заполнение информации по измеряемым диапазонам
void OwenTPM210::BuildMeassureRangeInfo()
{
	MEASSURE_RANGE_INFO msri;
	mrciOut.assign(OUTPUT_ANALOG_CHANNELS_COUNT, msri);
	mrciIn.assign(INPUT_ANALOG_CHANNELS_COUNT, msri);

	mrciIn[0].rngid = mrciOut[0].rngid = 0;				//идентификатор измеряемого диапазона
	mrciIn[0].desc  = mrciOut[0].desc  = "Температура";//измеряемая величина
	mrciIn[0].units = mrciOut[0].units = "­°C";		 //единицы	 измерения
	mrciIn[0].mins  = mrciOut[0].mins  = minT;			//минимальное значение измеряемой величины
	mrciIn[0].maxs  = mrciOut[0].maxs  = maxT;			//максимальное значение измеряемой величины	

	mrciOut[1].rngid = 2;				//идентификатор измеряемого диапазона
	mrciOut[1].desc  = "Выходная мощность";//измеряемая величина
	mrciOut[1].units = "%­";		 //единицы	 измерения
	mrciOut[1].mins = 0;			//минимальное значение измеряемой величины
	mrciOut[1].maxs = 100;			//максимальное значение измеряемой величины
}

//Заполнение информации по картам регистров
void OwenTPM210::BuildRegestersMaps()
{
	//аналоговые входы
	aiChannelsMap.insert(std::make_pair(0, 0x0001));
	aiChannelsMap.insert(std::make_pair(1, 0x0004));

	//аналоговые выходы
	aoChannelsMap.insert(std::make_pair(0, 0x0002));
	aoChannelsMap.insert(std::make_pair(1, 0x030C));

	//аналоговые выходы
	doChannelsMap.insert(std::make_pair(0, 0x0007));
	
}

//Преобразовать данные канала с учетом текущих настроек положения плавающей точки
void OwenTPM210::PrepareChannelValue(unsigned char channel, double& val, bool forWrite ) const
{
	switch (channel)
	{
	case 0:
		val = forWrite ? (val / dTLimitsCoef) : (val * dTLimitsCoef);
		break;
	}
}