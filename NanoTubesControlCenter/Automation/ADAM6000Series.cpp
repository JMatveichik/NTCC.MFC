#include "StdAfx.h"

#include "ADAM6000Series.h"
#include "Converters.h"

#include "..\ah_msgprovider.h"
#include "..\ah_errors.h"
#include "..\ah_xml.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


ADAM6000::ADAM6000(std::string model) :
IdentifiedPhysDevice("Advantech Co.", model),
portNo(0)
{
	
}

ADAM6000::~ADAM6000()
{
}

//Создать объект модуля ADAM
bool ADAM6000::Create(std::string name, unsigned long updateInterval, std::string ip, unsigned port)
{
	if ( !IdentifiedPhysDevice::Create( name, updateInterval ) )
		return  false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//не удалось создать соединение 
	if ( !adamSocket.Establish(devAdress, portNo, &str) )
	{
		str << std::endl << "Modbus/TCP connection " << devAdress.c_str() << ":" << (int)portNo << " failed" << std::ends;
		err.SetLastError( DCE_PORT_NOT_AVAILABLE, str.str() );
		return m_bStatusOk = false;
	}

	adamSocket.EnableCallback(IdentifiedPhysDevice::FailureCallback, this);
	devAdress = devAdress;
	portNo	  = portNo;	

	return OnCreate();
}

//создание объекта из XML узла  
bool ADAM6000::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();
	std::string sAtr;
	if (!appXML.GetNodeAttributeByName(pNode, "ADDRESS", sAtr) )
	{
		str << "IP address must be set for this device type" << std::ends;		
		err.SetLastError(DCE_DEVICE_ADDRESS_NOT_SET, str.str() );
		return false;
	}

	std::string  ip = sAtr;
	if ( !IsIPString( ip ) )
	{
		str << "IP addres format ["<< ip.c_str() << "]" << std::ends;		
		err.SetLastError(DCE_INVALID_DEVICE_ADDRESS, str.str() );
		return false;
	}

	int port = 502;
	if (appXML.GetNodeAttributeByName(pNode, "PORT", sAtr) )
	{
		port = atoi( sAtr.c_str() );

		if ( port <= 0 )
		{
			str << "Invalid TCP port number ["<< port << "]" << std::ends;		
			err.SetLastError(DCE_INVALID_DEVICE_ADDRESS, str.str() );
			return false;
		}
	}

	if (appXML.GetNodeAttributeByName(pNode, "LOG", sAtr) )
	{
		m_logDevs.open(sAtr.c_str());
		if ( !m_logDevs.good() )
		{
			str << "Log file  ["<< sAtr.c_str() << "] creation failed" << std::ends;		
			err.SetLastError(DCE_INVALID_DEVICE_ADDRESS, str.str() );			
		}
	}
	
	//не удалось создать соединение 
	if ( !adamSocket.Establish(ip, (unsigned int)port, &str) )
	{
		str << std::endl << "Modbus/TCP connection " << ip.c_str() << ":" << port << " failed" << std::ends;
		err.SetLastError( DCE_PORT_NOT_AVAILABLE, str.str() );
		return m_bStatusOk = false;
	}

	adamSocket.EnableCallback(IdentifiedPhysDevice::FailureCallback, this);
	devAdress = ip;
	portNo	  = port;

	return OnCreate();
}


bool ADAM6000::OnFailure()
{
	IdentifiedPhysDevice::OnFailure();

	if ( m_nFailures == 1 )
	{
		std::string msg;
		msg = "Device " + m_strName + " does not respond";
		
		std::string title = m_strModel;
		title += " : ";
		title += devAdress;

		AppMessagesProviderHelper::Instance().ShowPopupMessage(msg.c_str(), title.c_str(), COutMessage::MSG_WARNING );
	}

	return false; 
}

bool ADAM6000::SendModbus(MODBUS_COMMAND cmd, const char* data, unsigned data_size, std::vector<unsigned char>& response, std::ostream* pLog)
{
	if ( !(m_bStatusOk = adamSocket.Transact(cmd, data, data_size, response, pLog) ) )
	{
		::SetEvent(m_hStopEvent);

		std::string msg;
		msg = "Device " + m_strName + " does not respond";
		
		std::string title = m_strModel;
		title += " : ";
		title += devAdress;

		AppMessagesProviderHelper::Instance().ShowPopupMessage(msg.c_str(), title.c_str(), COutMessage::MSG_ERROR );

	}
	else {
		m_nFailures = 0;
	}

	return m_bStatusOk;
}


std::string ADAM6000::SendASCII(const char* szData)
{
	vector<unsigned char> response;
	
	unsigned char len = strlen(szData) + 2;	//количество байт данных + возврат кретки + 0	
	short regnum = len / 2;	// число регистров
	short startRegister = 9999;

	vector <unsigned char> send(256);	
	
	*LPWORD(&send[0]) = htons(startRegister);
	*LPWORD(&send[2]) = htons(regnum );
	send[4] = len;
	
	unsigned char i;
	for ( i = 5; i < len + 3; i++ )
		send[i] = szData[i - 5];

	send[i++] = 0x0D;	// возврат каретки для ограгичения команды 
	send[i] = 0x00;  // 0
	

	if ( !SendModbus(PresetMultipleRegisters, (const char*)send.data(), i + 1, response, &m_logDevs) )
		return "ERROR";

	
	*LPWORD(&send[2]) = htons( 126 );
	
	if ( !SendModbus(ReadInputRegisters, (const char*)send.data(), 4, response, &m_logDevs) )
		return "ERROR";
	
	vector <unsigned char>::iterator last = find(response.begin(), response.end(), 0x0D);
	
	if ( last == response.end() )
		return "ERROR";

	std::string res(response.begin(), last); 

	return res;
}

//Получить имя модуля 
std::string ADAM6000::ModuleName()
{
	std::string name = SendASCII("$01M");	
	
	name.erase(0, 3); //удаляем "!01" из ответа 
	return name;
}

//Получить версию прошивки модуля
std::string ADAM6000::ModuleFirmwareVersion() 
{
	std::string firmware = SendASCII("$01F");
	
	firmware.erase(0, 3); //удаляем "!01" из ответа
	return firmware;
}

bool ADAM6000::SetInternalFlag(short idx, bool state) const
{
	//GCL Internal	Flag Values 0~15
	if ( idx > 15)
		return false;

	vector<unsigned char> res;
	unsigned short szData[] = 
	{
		htons(304),
		htons(1)
	};

	//Read Aux digital state
	if ( !(const_cast<ADAM6000*>(this))->SendModbus(ReadInputRegisters, (char*)szData, sizeof(szData), res, &m_logDevs))
		return false;
	
	if ( res.empty() )
		return false;

	unsigned short mask = ntohs(*(unsigned short*)res.data());
	//маска канала
	unsigned short msk = 1 << idx; 
	unsigned short stateNew = 0;

	//переключаем в новое состояние
	if ( state ) 
		stateNew = mask | msk;
	else
		stateNew = mask & ~msk;

	unsigned short szData2[] =
	{
		htons(304),
		htons(stateNew)
	};

	if ( !(const_cast<ADAM6000*>(this))->SendModbus(PresetSingleRegister, (char*)szData2, sizeof(szData2), res, &m_logDevs))
		return false;

	return false;
}

bitset<GCL_INTERNAL_FLAGS_COUNT> ADAM6000::GetInternalFlags() const
{
	vector<unsigned char> res;
	unsigned short szData[] =
	{
		htons(304),
		htons(1)
	};

	//Read Aux digital state
	if ( !(const_cast<ADAM6000*>(this))->SendModbus(ReadInputRegisters, (char*)szData, sizeof(szData), res, &m_logDevs))
		return false;

	if (res.empty())
		return false;

	unsigned short mask = (res.size() == 0) ? 0 : ntohs(*(unsigned short*)res.data());	
	return bitset<GCL_INTERNAL_FLAGS_COUNT>(mask);
}

bool ADAM6000::GetInternalFlag(short idx) const
{	
	bitset<GCL_INTERNAL_FLAGS_COUNT> flags = GetInternalFlags();
	return flags.test(idx);
}

/*********************************************/
/*           Модуль ADAM6018                 */
/*********************************************/
#pragma region Реализация модуля ADAM6018

//Конструктор класса
ADAM6018::ADAM6018(std::string model) :  
	ADAM6000(model), 
		channelsState(0),
		AI_CHANNELS_COUNT(8),
		DO_CHANNELS_COUNT(24)
{
	inputRanges.assign(AI_CHANNELS_COUNT, 0xff);
	lastData.assign(AI_CHANNELS_COUNT, 0.0 );
}

ADAM6018::~ADAM6018(void)
{	
}

//Получение общего числа аналоговых входов
//Для данного устройства 8 - входов
int ADAM6018::GetChannelsCountAI() const
{
	return AI_CHANNELS_COUNT;
}

//Получение общего числа цифровых выходов 
// 0-7 - digital output
// 8-23 - aux digital output
int ADAM6018::GetChannelsCountDO() const
{
	return DO_CHANNELS_COUNT;
}

ADAM6018* ADAM6018::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM6018> obj ( new ADAM6018("ADAM-6018") );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();	
}

//Создать объект модуля ADAM6019+
bool ADAM6018::OnCreate()
{
	//тип модуля с адресом deviceAdress не соответствует ожидаемому
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	
	if ( ModuleName() != "6018")
	{
		str << "Device attached to " << devAdress.c_str() << ":" << portNo << " is not " << m_strModel << std::ends;
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() ); 
		return false;
	}
	
	ReadChannels(NULL);
	Sleep(500);
	if ( !UpdateChannelState() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< " attached to  " << devAdress.c_str() << ":" << portNo << std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() ); 
		return false;
	}
	
	UpdateInputRangesInfo();	
	return true;
}

bool ADAM6018::EnableChannel(unsigned char ch, bool enable/* = true*/)
{
	if (ch < 0 || ch >= GetChannelsCountAI())
		return false;

	unsigned char mask = 1 << ch;
	unsigned char newState;
	
	if ( enable ) 
		newState = channelsState | mask;
	else
		newState = channelsState & ~mask;

	std::string respond;
	char szCmd[256];
	sprintf_s(szCmd, 256, "$015%X", newState);
	
	//Команда получения состояния аналоговых входов
	if ( (respond = SendASCII( szCmd )) != "ERROR" )
	{
		
	}
	
	channelsState = newState;	
	return true;	
}

bool ADAM6018::SetInputRange(unsigned char ch, unsigned char range)
{
	char szCmd[MAX_CMD_LEN];

	//Команда получения измеряемого диапазона
	sprintf_s(szCmd, MAX_CMD_LEN, "7C%dR%02X", ch, range);

	std::string respond = SendASCII(szCmd);
	if ( respond.empty() || respond == "FAILURE" )
		return false;

	UpdateInputRangeInfo(ch);
	return true;
}

bool ADAM6018::UpdateChannelState()
{
	//Команда получения состояния аналоговых входов
	std::string st1  = SendASCII("$016");
	st1.erase(0, 3);
	
	if ( st1.empty() || st1.length() != 2)
		return false;

	st1 = "0x" + st1;
	channelsState = unsigned char( strtol(st1.c_str(), NULL, 0) );

	return true;
}


bool ADAM6018::IsChannelEnabled(unsigned char channel)
{
	UpdateChannelState();	
	
	unsigned char mask = 1 << channel;
	if ( mask & channelsState )
		return true;

	return false;
}

// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  ADAM6018::GetChannelData(unsigned char channel)  const
{
	
	double val;
	
	if (channel < 0 || channel >= GetChannelsCountAI())
		return DBL_MIN;
	
	Lock();
	val = lastData[channel];
	Unlock();

	return val;
}

void ADAM6018::ControlProc()
{
	ReadChannels( NULL );

	UpdateChannelsStateDO();

	if ( !m_bStatusOk )
		return; 
}

//Получение данных с одного из 8 аналоговых входов (выполняется запрос на чтение данных)
//номер входа должен быть между 0 и 8
double  ADAM6018::ReadChannel(unsigned char channel)  const
{
	char szCmd[256];
	sprintf_s(szCmd, 256, "#01%d", channel);

	std::string respond = (const_cast<ADAM6018*>(this))->SendASCII(szCmd);
	respond.erase(0,1);

	return atof(respond.c_str());
}

void ADAM6018::ReadChannels(double* pdData/* = NULL*/) const
{
	std::string respond = (const_cast<ADAM6018*>(this))->SendASCII("#01");
	respond.erase(0, 1);

	vector<double> d;
	ExtractData<double>(respond, std::string("+-"), d);

	if ((int)d.size() < GetChannelsCountAI())
		return;

// 	int expected = ( GetChannelsCountAI() + 1) * 8;
// 	if ( respond.length() != expected )
// 		return;

	Lock();

	for (int i = 0; i < GetChannelsCountAI(); i++)
	{
// 		std::string res = respond.substr( i*8, 8 );
// 		double dVal = atof( res.c_str() );
		lastData[i] = d[i];

	}

	if (pdData != NULL)
		memcpy(pdData, lastData.data(), sizeof(double)*GetChannelsCountAI());

	Unlock();
}

//Обновить информацию об измеряемых диапазонов 
bool ADAM6018::UpdateInputRangeInfo( unsigned char channel ) const
{
	char szCmd[256];
	sprintf_s(szCmd, 256, "$01B%02d", channel);

	inputRanges[channel] = 0xFF;

	std::string respond = (const_cast<ADAM6018*>(this))->SendASCII(szCmd);
	if (respond.empty() || respond == "FAILURE")
		return false;

	respond.erase(0, 3);
	respond.insert(0, "0x"); 

	inputRanges[channel] = strtol( respond.c_str(), NULL, 0);
	return true;
}

void ADAM6018::UpdateInputRangesInfo( ) const
{	
	for (int i = 0; i < GetChannelsCountAI(); i++)
		UpdateInputRangeInfo( (unsigned char)i );	
}

void ADAM6018::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	long rngid = inputRanges[channel];
	*lpmci = ranges[ rngid ];
}

//Установить  диапазон  аналогового входа
void ADAM6018::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();

	rngs.push_back(ranges[ 0x0E ]);
	rngs.push_back(ranges[ 0x0F ]);
	rngs.push_back(ranges[ 0x10 ]);
	rngs.push_back(ranges[ 0x11 ]);
	rngs.push_back(ranges[ 0x12 ]);
	rngs.push_back(ranges[ 0x13 ]);
	rngs.push_back(ranges[ 0x14 ]);	
}

//////////////////////////////////////////////////////////////////////////
/// Реализация интерфейса IDigitalOutput

//Обновить текущее состояния дискретных выходов	
bool ADAM6018::UpdateChannelsStateDO() const
{
	vector<unsigned char> response1;
	vector<unsigned char> response2;
	string response;
	unsigned short szBuffer[] =
	{
		htons(16),
		htons(8)
	};

	//Read digital output state
	if ( !(const_cast<ADAM6018*>(this))->SendModbus(ReadCoilStatus, (char*)szBuffer, sizeof(szBuffer), response1, &m_logDevs))
		return false;

	if (response1.empty())
		return false;

	//  [11/18/2014 Johnny A. Matveichik]
	unsigned short szBuffer2[] =
	{
		htons(304),
		htons(1)
	};

	//Read Aux digital state
	if ( !(const_cast<ADAM6018*>(this))->SendModbus(ReadInputRegisters, (char*)szBuffer2, sizeof(szBuffer2), response2, &m_logDevs))
		return false;

	if (response2.empty())
		return false;

	Lock();
 	
	//  [11/18/2014 Johnny A. Matveichik]
	unsigned long long mask1 = *(unsigned char*)response1.data();	
	unsigned short mask2 = ntohs(*(unsigned short*)response2.data());
 	unsigned long long mask  = mask2 << 8;
 	mask |= mask1;

	(const_cast<ADAM6018*>(this))->stateDO = bitset<24>(mask); 
 
	Unlock();

	return true;
}

//Получить состояние цифрового выхода
bool ADAM6018::GetChannelStateDO(unsigned char channel, bool update) const
{
	if ( update )
		UpdateChannelsStateDO();

	Lock();
	bool state = stateDO.test(channel);
	Unlock();
	
	return state;
}

//Установить состояние цифрового выхода 
bool  ADAM6018::SetChannelStateDO(unsigned char channel, bool enable)
{
	///устанавливем цифровые входы номер канала до 8
	if (channel < 8) 
	{
		vector<unsigned char> res;
		unsigned short szData[] =
		{
			htons(16 + channel),
			enable ? htons(0xff00) : htons(0x0000)

		};

		if ( !(const_cast<ADAM6018*>(this))->SendModbus(ForceSingleCoil, (char*)szData, sizeof(szData), res, &m_logDevs))
			return false;
	}
	///устанавливаем внутренние флаги Aux каналы от 8 до 23 
	else 
		return SetInternalFlag(channel - 8, enable);

	
	return true;
}

//Записать данные во все цифровые выходы
void ADAM6018::SetChannelsStateDO(unsigned char val) 
{
	std::string respond;
	/*
	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "00%02X",  val);

	//Команда установки состояния цифровых выходов
	if ( SendCommand('#', szCmd, true, respond) )
	{

	}
	*/
}


#pragma endregion


//Конструктор класса
ADAM6017::ADAM6017() :  
	ADAM6018("ADAM-6017"), 
		DO_CHANNELS_COUNT(18),
		AI_CHANNELS_COUNT(8)
{
	inputRanges.assign(AI_CHANNELS_COUNT, 0xff);
	lastData.assign(AI_CHANNELS_COUNT, 0.0 );
}

ADAM6017::~ADAM6017(void)
{	
}

//Получение общего числа аналоговых входов
//Для данного устройства 8 - входов
int ADAM6017::GetChannelsCountAI() const
{
	return AI_CHANNELS_COUNT;
}

//Получение общего числа цифровых выходов 
int ADAM6017::GetChannelsCountDO() const
{ 
	return DO_CHANNELS_COUNT; 
};

ADAM6017* ADAM6017::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM6017> obj ( new ADAM6017() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();	
}

void ADAM6017::ReadChannels(double* pdData/* = NULL*/) const
{
	std::string respond = (const_cast<ADAM6017*>(this))->SendASCII("#01");
	respond.erase(0, 1);

	int count  = 0;
	count  = std::count(respond.begin(), respond.end(), '+');
	count += std::count(respond.begin(), respond.end(), '-');

	int substrlen = respond.length() / count;

	std::vector<double> signals;
	ExtractData<double>(respond, substrlen, signals);
	
 	if ((int)signals.size() < GetChannelsCountAI() )
 		return;

	Lock();
	
	//for (int i = 0; i < GetChannelsCountAI(); i++)
	lastData.assign(signals.begin(), signals.end());	

	if (pdData != NULL)
		memcpy(pdData, lastData.data(), sizeof(double)*GetChannelsCountAI());

	Unlock();
}

//Создать объект модуля ADAM6019+
bool ADAM6017::OnCreate()
{
	//тип модуля с адресом deviceAdress не соответствует ожидаемому
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	
	if ( ModuleName() != "6017")
	{
		str << "Device attached to " << devAdress.c_str() << ":" << portNo << " is not " << m_strModel << std::ends;
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() ); 
		return false;
	}
	ReadChannels(NULL);
	Sleep(250);
	if ( !UpdateChannelState() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< " attached to " << devAdress.c_str() << ":" << portNo << std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() ); 
		return false;
	}
	
	UpdateInputRangesInfo(); 

	///////////////////////////
	/*
	char buf[] = { 
		char(0x00), 
		char(0x02), 
		char(0x00), 
		char(0x04), 
		char(0xEE), 
		char(0xFF) 
	};

	std::vector<unsigned char> response;
	adamSocket.Transact(LoopbackDiagnosis, buf, 4, response);
	///////////////////////////
	*/

	return true;
}

//Получить информацию о всех доступных диапазонах 
void ADAM6017::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();

	rngs.push_back(ranges[ 0x0E ]);
	rngs.push_back(ranges[ 0x0F ]);
	rngs.push_back(ranges[ 0x10 ]);
	rngs.push_back(ranges[ 0x11 ]);
	rngs.push_back(ranges[ 0x12 ]);
	rngs.push_back(ranges[ 0x13 ]);
	rngs.push_back(ranges[ 0x14 ]);	
}

/************************************************************************/
/*      ADAM6000DIDO                                                    */
/************************************************************************/
#pragma region ADAM6000DIDO

ADAM6000DIDO::ADAM6000DIDO(int channelsNumDI, int channelsNumDO, std::string model) : 
ADAM6000(model),
INPUT_DIGITAL_CHANNELS_COUNT(channelsNumDI),	
OUTPUT_DIGITAL_CHANNELS_COUNT(channelsNumDO)
{
	
}

ADAM6000DIDO::~ADAM6000DIDO()
{
}

//Получение общего числа цифровых выходов 
int ADAM6000DIDO::GetChannelsCountDO() const 
{ 
	return OUTPUT_DIGITAL_CHANNELS_COUNT +  GCL_INTERNAL_FLAGS_COUNT; 
}

//Получить число дискретных входов
int ADAM6000DIDO::GetChannelsCountDI() const
{
	return INPUT_DIGITAL_CHANNELS_COUNT;
}

void ADAM6000DIDO::ControlProc() 
{ 
		
	if ( !UpdateChannelsStateDI() )
		return;
	
	UpdateChannelsStateDO();

}

//////////////////////////////////////////////////////////////////////////
/// Реализация интерфейса IDigitalInput


//Обновить состояния дискретных входов
bool ADAM6000DIDO::UpdateChannelsStateDI() const
{
	vector<unsigned char> response;
	unsigned short szBuffer[] =
	{
		htons(0),
		htons(INPUT_DIGITAL_CHANNELS_COUNT)
	};

	//
	if ( !(const_cast<ADAM6000DIDO*>(this))->SendModbus(ReadCoilStatus, (char*)szBuffer, sizeof(szBuffer), response, &m_logDevs))
		return false;

	if ( response.empty() )
		return false;

	Lock();


	unsigned long long mask = *(unsigned char*)response.data();
	(const_cast<ADAM6000DIDO*>(this))->stateDI = bitset<64>(mask); 

	Unlock();
	
	return true;
}

//Получить состояние цифрового выхода
bool ADAM6000DIDO::GetChannelStateDI(unsigned char сhannel, bool update) const 
{
	if ( update )
		UpdateChannelsStateDI();

	Lock();
	
	bool state = stateDI[сhannel];

	Unlock();

	return state;
}

//////////////////////////////////////////////////////////////////////////
/// Реализация интерфейса IDigitalOutput

//Обновить текущее состояния дискретных выходов	
bool ADAM6000DIDO::UpdateChannelsStateDO() const
{
	if ( !m_bStatusOk )
		return false; 	

	vector<unsigned char> response;
	unsigned short szBuffer[] =
	{
		htons(16),
		htons(OUTPUT_DIGITAL_CHANNELS_COUNT)
	};

	if ( !(const_cast<ADAM6000DIDO*>(this))->SendModbus(ReadCoilStatus, (char*)szBuffer, sizeof(szBuffer), response, &m_logDevs))
		return false;

	if ( response.empty() )
		return false;

 	Lock();
 
	//цифровые выходы
 	unsigned long long mask	   = *(unsigned char*)response.data();

	//внутрениие флаги
	unsigned long long maskAux = GetInternalFlags().to_ullong() << OUTPUT_DIGITAL_CHANNELS_COUNT;

	(const_cast<ADAM6000DIDO*>(this))->stateDO = bitset<64>( mask | maskAux ); 

 	Unlock();

	return true;
}

//Получить состояние цифрового выхода
bool ADAM6000DIDO::GetChannelStateDO(unsigned char channel, bool update) const
{
	
	Lock();

	if ( update )
		UpdateChannelsStateDO();

	bool state = stateDO.test(channel);

	Unlock();
	
	return state;
}

//Установить состояние цифрового выхода 
bool  ADAM6000DIDO::SetChannelStateDO(unsigned char channel, bool enable)
{
	if ( !m_bStatusOk )
		return false; 	

	if ( channel < OUTPUT_DIGITAL_CHANNELS_COUNT )
	{
		vector<unsigned char> response;
		unsigned short szBuffer[] =
		{
			htons(16 + channel),
			enable ? htons(0xff00) : htons(0x0000)
		};

		if ( !(const_cast<ADAM6000DIDO*>(this))->SendModbus(ForceSingleCoil, (char*)szBuffer, sizeof(szBuffer), response, &m_logDevs))
			return false;
	}
	else 
		return SetInternalFlag(channel - OUTPUT_DIGITAL_CHANNELS_COUNT, enable);

	return true;
}
#pragma endregion

/************************************************************************/
/*      ADAM6050                                                        */
/************************************************************************/
#pragma region Реализация модуля ADAM6050

ADAM6050::ADAM6050() : ADAM6000DIDO(12, 6, "ADAM-6050")
{
	pcs.assign(6, std::make_pair((const BaseConverter*)NULL, (const BaseConverter*)NULL) ); 
	lastVals.assign(6, 0.0 );
	conversionMode.assign(6, NONE);

	//начальный адрес длительности низкого состояния для импульсного режима
	usLowWidthStartAdr = 24;

	//начальный адрес длительности высокого состояния для импульсного режима 
	usHiWidthStartAdr = 36; 

	//начальный адрес для переключения канала в импульсный режим 
	usAbsPulseStartAdr = 48; 	
}

//Конструктор
ADAM6050::ADAM6050(int channelsNumDI, int channelsNumDO) : ADAM6000DIDO (channelsNumDI, channelsNumDO, "ADAM-6050") 
{
	pcs.assign(channelsNumDO, std::make_pair((const BaseConverter*)NULL, (const BaseConverter*)NULL));
	lastVals.assign(channelsNumDO, 0.0 );
	conversionMode.assign(channelsNumDO, NONE);

	//начальный адрес длительности низкого состояния для импульсного режима
	usLowWidthStartAdr = 24;

	//начальный адрес длительности высокого состояния для импульсного режима 
	usHiWidthStartAdr = 36; 

	//начальный адрес для переключения канала в импульсный режим 
	usAbsPulseStartAdr = 48; 	
}

int ADAM6050::GetChannelsCountAO() const 
{ 
	return OUTPUT_DIGITAL_CHANNELS_COUNT; 
};

ADAM6050::~ADAM6050()
{
}

ADAM6050* ADAM6050::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM6050> obj ( new ADAM6050() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM6050::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (!ADAM6000::CreateFromXMLNode(pNode) )
		return false;

	CComPtr<IXMLDOMNodeList> pPulseChannelsList;
	HRESULT hr = pNode->selectNodes(CComBSTR("PULSE"), &pPulseChannelsList);

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( SUCCEEDED(hr) && pPulseChannelsList != NULL )
	{
		long lPulseCount;
		pPulseChannelsList->get_length(&lPulseCount);
		pPulseChannelsList->reset();

		for(long i=0; i < lPulseCount; i++)
		{
			CComPtr<IXMLDOMNode> pChannel;
			pPulseChannelsList->get_item(i, &pChannel);
			
			std::string sVal;
			
			if ( !appXML.GetNodeAttributeByName(pChannel, "CHANNEL", sVal) )
			{
				str << "Channel number must be set" << std::ends;		
				err.SetLastError(DCE_INVALID_CHANNEL_NUMBER, str.str() );
				return false;
			}
			int wire = 	atoi(sVal.c_str());
			if ( wire < 0 || wire >= OUTPUT_DIGITAL_CHANNELS_COUNT  )
			{
				str << "Invalid digital output channel number ["<< wire << "] for " << m_strModel.c_str() << std::ends;		
				err.SetLastError(DCE_INVALID_CHANNEL_NUMBER, str.str() );
				return false;
			}

			if ( !appXML.GetNodeAttributeByName(pChannel, "MODE", sVal) )
			{
				str << "Conversion mode not set" << std::ends;
				err.SetLastError(DCE_INVALID_CHANNEL_NUMBER, str.str() );
				return false;
			}
			
			boost::algorithm::to_upper(sVal);
			if (sVal == "HW_LW")
				conversionMode[wire] = HW_LW;
			else if(sVal == "FQ_DC")
				conversionMode[wire] = FQ_DC;
			else if(sVal == "FQ_HW")
				conversionMode[wire] = FQ_HW;
			else if(sVal == "FQ_LW")
				conversionMode[wire] = FQ_LW;
			else if(sVal == "DC_HW")
				conversionMode[wire] = DC_HW;
			else if(sVal == "DC_LW")
				conversionMode[wire] = DC_LW;
			else
			{
				str << "Unknown conversion mode [" << sVal.c_str() << "]" << std::ends;
				err.SetLastError(DCE_INVALID_CHANNEL_NUMBER, str.str() );
				return false;
			}

			if ( !appXML.GetNodeAttributeByName(pChannel, "CONV2", sVal) )
			{
				str << "Low state width converter must be set for pulse channel" << std::ends;
				err.SetLastError(DCE_INVALID_CHANNEL_NUMBER, str.str() );
				return false;
			}
			const BaseConverter* pLwConv = BaseConverter::GetConverter(sVal);
			if ( pLwConv == NULL )
			{
				str << "Low state width converter ["<< sVal <<"] not found" << std::ends;
				err.SetLastError(DCE_INVALID_CHANNEL_NUMBER, str.str() );
				return false;
			}

			if ( !appXML.GetNodeAttributeByName(pChannel, "CONV1", sVal) )
			{
				str << "High state width converter must be set for pulse channel" << std::ends;
				err.SetLastError(DCE_INVALID_CHANNEL_NUMBER, str.str() );
				return false;
			}

			const BaseConverter* pHwConv = BaseConverter::GetConverter(sVal);
			if ( pHwConv == NULL )
			{
				str << "High state width converter ["<< sVal.c_str() <<"] not found" << std::ends;
				err.SetLastError(DCE_INVALID_CHANNEL_NUMBER, str.str() );
				return false;
			}
			pcs[wire] = std::make_pair(pHwConv, pLwConv);
		}
	}

	return true;
}

bool ADAM6050::OnCreate()
{
	//тип модуля с адресом deviceAdress не соответствует ожидаемому
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	if ( ModuleName() != "6050")
	{
		str << "Device attached to " << devAdress.c_str() << ":" << portNo << " is not " << m_strModel << std::ends;
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	if (!UpdateChannelsStateDO() )
	{
		str << "Update digital output channels state for module " << m_strModel 
			<< " attached to " << devAdress.c_str() << ":" << portNo << " failed!" <<std::ends;

		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	if ( !UpdateChannelsStateDI() )
	{
		str << "Update digital input channels state for module " << m_strModel 
			<< " attached to " << devAdress.c_str() << ":" << portNo << " failed!" <<std::ends;
		
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	return true;
}
//Установить состояние цифрового выхода 
bool  ADAM6050::SetChannelStateDO(unsigned char channel, bool enable)
{
	if ( !m_bStatusOk )
		return false; 	

	std::vector<unsigned char> buffer(256);
	std::vector<unsigned char> response(256);
	unsigned nSize = 9;

	*reinterpret_cast<short*>(&buffer[0]) = htons(48 + channel * 2);
	*reinterpret_cast<short*>(&buffer[2]) = htons(2);
	buffer[4] = 4;
	*reinterpret_cast<short*>(&buffer[5]) = htons(enable ? 0 : 1);
	*reinterpret_cast<short*>(&buffer[7]) = htons(0);
	
	if (!SendModbus(PresetMultipleRegisters, (const char*)buffer.data(), nSize, response, &m_logDevs))
		return false;

	return ADAM6000DIDO::SetChannelStateDO(channel, enable);
}

/////////////////////////////////////////////////
//Реализация интерфейса  IPulseDigitalOutput
bool ADAM6050::SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth)
{
	if ( !m_bStatusOk )
		return false; 	

	std::vector<unsigned char> buffer(256);
	std::vector<unsigned char> response(256);
	
	unsigned nSize = 0;

	// send pulse properties
	unsigned long ulHighWidth = highWidth/* *10 */;
	unsigned long ulLowWidth = lowWidth/* *10 */;
	
	//задание длительности низкого состояния для импульсного режима
	*reinterpret_cast<short*>(&buffer[0]) = htons(usLowWidthStartAdr + channel * 2);
	*reinterpret_cast<short*>(&buffer[2]) = htons(2);
	buffer[4] = 4;
	*reinterpret_cast<short*>(&buffer[5]) = htons(ulLowWidth & 0xFFFF);
	*reinterpret_cast<short*>(&buffer[7]) = htons(ulLowWidth >> 16);
	nSize = 9;
	if (!SendModbus(PresetMultipleRegisters, (const char*)buffer.data(), nSize, response, &m_logDevs))
		return false;

	//задание длительности высокого состояния для импульсного режима 
	*reinterpret_cast<short*>(&buffer[0]) = htons(usHiWidthStartAdr + channel * 2);
	*reinterpret_cast<short*>(&buffer[5]) = htons(ulHighWidth & 0xFFFF);
	*reinterpret_cast<short*>(&buffer[7]) = htons(ulHighWidth >> 16);
	if (!SendModbus(PresetMultipleRegisters, (const char*)buffer.data(), nSize, response, &m_logDevs))
		return false;

	//задание переключения канала в импульсный режим 	
	*reinterpret_cast<short*>(&buffer[0]) = htons(usAbsPulseStartAdr + channel * 2);
	*reinterpret_cast<short*>(&buffer[5]) = htons(highWidth == 0 ? 2 : 0);
	*reinterpret_cast<short*>(&buffer[7]) = htons(0);
	if (!SendModbus(PresetMultipleRegisters, (const char*)buffer.data(), nSize, response, &m_logDevs))
		return false;

	return true;
}

///////////////////////////////////////////////////
//Реализация интерфейса IAnalogOutput
//Получение общего числа аналоговых выходов 

//Записать данные в аналоговый выход 
double  ADAM6050::WriteToChannel(unsigned char channel, double val)
{
	//ofstream log("pulse.log", std::ios::app);

	if ( !m_bStatusOk )
		return UNKNONW_VALUE; 	

	Lock();

	double oldVal = lastVals[channel]; 
	//log << "Предыдущее значение " << oldVal;

	if ( pcs[channel].first != NULL && pcs[channel].second != NULL )
	{
		long loWidth = 0;
		long hiWidth = 0;

		switch ( conversionMode[channel] )
		{
		case HW_LW:
			{
				hiWidth = (long)(pcs[channel].first->Convert(val)/* * 10*/);
				loWidth = (long)(pcs[channel].second->Convert(val)/* * 10*/);

				//log << "Hi (" << hiWidth << ")" << " Low (" << loWidth << ")" << endl;
			}
			break;
		case FQ_DC:
			{
				double fq = pcs[channel].first->Convert(val);
				double dc = pcs[channel].second->Convert(val);

				hiWidth = (long)((1000)*(dc/fq));
				loWidth = (long)((1000)*(1.0 - dc)/fq);
			}
			break;
		case FQ_HW:
			{
				double fq = pcs[channel].first->Convert(val);
				double hw = pcs[channel].second->Convert(val);

				hiWidth = (long)(hw);
				loWidth = (long)((1000)*((1.0 - fq*hw/1000) / fq));
			}
			break;
		case FQ_LW:	
			{
				double fq = pcs[channel].first->Convert(val);
				double lw = pcs[channel].second->Convert(val);

				hiWidth = (long)((1000)*(1.0 - fq*lw/1000) / fq);
				loWidth = (long)((1000)*lw);
			}
			break;
		case DC_HW:
			{
				double dc = pcs[channel].first->Convert(val);
				double hw = pcs[channel].second->Convert(val);

				hiWidth = (long)((1000)*hw);
				loWidth = (long)((1000)*(hw - dc*hw/1000) / dc);
			}
			break;
		case DC_LW:
			{
				double dc = pcs[channel].first->Convert(val);
				double lw = pcs[channel].second->Convert(val);

				hiWidth = (long)((1000)*(dc*lw) / (1-dc));
				loWidth = (long)((1000)*lw);
			}
			break;
		
		case NONE:
		default:
			{

			}
			break;
		}

		if (val != 0.0)
		{
			if ( SetupGenerator(channel, hiWidth, loWidth) )
				lastVals[channel] = val;
		}
		else
			if ( SetupGenerator(channel, 0, 0) )
				lastVals[channel] = 0.0;
	}	
	Unlock();

	return oldVal;
}

//Записать данные во все каналы
void ADAM6050::WriteToAllChannels(const std::vector<double>& vals)
{
}

//Получить последнее записанное в аналоговый выход значение 
double ADAM6050::GetChannelStateAO (unsigned char channel) const
{
	Lock();
	double last = lastVals[channel];	
	Unlock();
	
	return last;
}

//Получить выходной диапазон канала 
void ADAM6050::GetMinMax(unsigned char channel, double& minOut, double& maxOut) const
{
	if ( pcs[channel].first != NULL && pcs[channel].second != NULL )
	{
		pcs[channel].first->GetMinMax(minOut, maxOut);
	}	
}

//Получить  информацию об диапазоне канала
void ADAM6050::GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	lpmci->rngid = 0;
	lpmci->mins = 0;
	lpmci->maxs = 100;
	lpmci->desc = "";
	lpmci->units = "%";

}

//Установить  диапазон  аналогового выхода
bool ADAM6050::SetOutputRange(unsigned char ch, unsigned char range)
{
	//ASSERT(FALSE);
	return false;
}


//Получить все возможные  диапазоны  
void ADAM6050::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{
	ASSERT(FALSE);
}


void ADAM6050::OnExit ()
{

}

#pragma endregion

#pragma region Реализация модуля ADAM6052
//////////////////////////////////////////////////////////////////////////
// Реализация модуля ADAM6052
//////////////////////////////////////////////////////////////////////////
ADAM6052::ADAM6052() : ADAM6050( 8, 8)
{
	m_strModel = "ADAM-6052";

	//начальный адрес длительности низкого состояния для импульсного режима
	usLowWidthStartAdr = 16;

	//начальный адрес длительности высокого состояния для импульсного режима 
	usHiWidthStartAdr = 32; 

	//начальный адрес для переключения канала в импульсный режим 
	usAbsPulseStartAdr = 48;

	data.assign(16, 0.0);
}

ADAM6052::~ADAM6052()
{

}

ADAM6052* ADAM6052::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM6052> obj ( new ADAM6052() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM6052::OnCreate()
{

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//тип модуля с адресом deviceAdress не соответствует ожидаемому
	if ( ModuleName() != "6052")
	{
		str << "Device attached to " << devAdress.c_str() << ":" << portNo << " is not " << m_strModel << std::ends;
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;		
	}

	if (!UpdateChannelsStateDO() )
	{
		str << "Update digital output channels state for module " << m_strModel 
			<< " attached to " << devAdress.c_str() << ":" << portNo << " failed!" <<std::ends;

		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	if ( !UpdateChannelsStateDI() )
	{
		str << "Update digital input channels state for module " << m_strModel 
			<< " attached to " << devAdress.c_str() << ":" << portNo << " failed!" <<std::ends;

		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	return true;
}
/*
bool ADAM6052::SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth)
{
	if ( !m_bStatusOk )
		return false; 	

	std::vector<unsigned char> buffer(256);
	std::vector<unsigned char> response(256);

	unsigned nSize = 0;

	// send pulse properties
	unsigned long ulHighWidth = highWidth;
	unsigned long ulLowWidth = lowWidth;

	*reinterpret_cast<short*>(&buffer[0]) = htons(12 + channel * 2);
	*reinterpret_cast<short*>(&buffer[2]) = htons(2);
	buffer[4] = 4;
	*reinterpret_cast<short*>(&buffer[5]) = htons(ulLowWidth & 0xFFFF);
	*reinterpret_cast<short*>(&buffer[7]) = htons(ulLowWidth >> 16);
	nSize = 9;
	if (!SendModbus(PresetMultipleRegisters, (const char*)buffer.data(), nSize, response, &m_logDevs))
		return false;

	*reinterpret_cast<short*>(&buffer[0]) = htons(24 + channel * 2);
	*reinterpret_cast<short*>(&buffer[5]) = htons(ulHighWidth & 0xFFFF);
	*reinterpret_cast<short*>(&buffer[7]) = htons(ulHighWidth >> 16);
	if (!SendModbus(PresetMultipleRegisters, (const char*)buffer.data(), nSize, response, &m_logDevs))
		return false;

	*reinterpret_cast<short*>(&buffer[0]) = htons(36 + channel * 2);
	*reinterpret_cast<short*>(&buffer[5]) = htons(highWidth == 0 ? 1 : 0);
	*reinterpret_cast<short*>(&buffer[7]) = htons(0);
	if (!SendModbus(PresetMultipleRegisters, (const char*)buffer.data(), nSize, response, &m_logDevs))
		return false;

	return true;
}
*/

void ADAM6052::ControlProc()
{
	UpdateChannelsStateDI();
	UpdateChannelsStateDO();
	ReadChannels();
}

//IAnalogInput
//////////////////////////////////////////////////////////////////////////
//Реализация функций интерфейса IAnalogInput

//Получение общего числа аналоговых входов
//Для данного устройства 16 - входов
//0-7 - counter
//8-15 - frequency
int ADAM6052::GetChannelsCountAI() const 
{ 
	return 16; 
}


//Чтение данных с аналоговых входов во внешний буфер pdData 
//размер буфера должен быть не меньше sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
//При pdData = NULL происходит опрос включеных входов и сохранение во внутреннем буфере класаа  
void ADAM6052::ReadChannels(double *pdData/* = NULL*/) const
{
	vector<unsigned char> response;
	unsigned short szBuffer[] =
	{
		htons(0),
		htons(16*2)
	};

	//
	if ( !(const_cast<ADAM6052*>(this))->SendModbus(ReadInputRegisters, (char*)szBuffer, sizeof(szBuffer), response, &m_logDevs))
		return;

	if ( response.empty() )
		return;

	Lock();
	
	int ch = 0;
	for(int i = 0; i < 32; i += 4)
	{
		short lo = ntohs(*reinterpret_cast<short*>( (&response.data()[i]) ));
		short hi = ntohs(*reinterpret_cast<short*>( (&response.data()[i + 2]) ));
		
		data[ch]	 = hi * 65535 + lo;
		data[ch + 8] = lo / 10.0;
		ch++;		
	}

	Unlock();

	return;
}

//Получение данных с одного из 8 аналоговых входов (выполняется запрос на чтение данных)
// номер входа должен быть от 0 до 8
double  ADAM6052::ReadChannel(unsigned char channel)  const
{
	ReadChannels(NULL);
	return data.at(channel);
}



// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  ADAM6052::GetChannelData(unsigned char channel)  const
{
	return data.at(channel);
}

//Получить  информацию об измеряемом диапазоне канала
void ADAM6052::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{

}

//Установить  диапазон  аналогового входа
bool ADAM6052::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить информациюю о всех доступных диапазонах 
void ADAM6052::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
}


#pragma endregion
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#pragma region Реализация модуля ADAM6066

ADAM6066::ADAM6066() : ADAM6050( 6, 6)
{
	m_strModel = "ADAM-6066";

	//начальный адрес длительности низкого состояния для импульсного режима
	usLowWidthStartAdr = 12;

	//начальный адрес длительности высокого состояния для импульсного режима 
	usHiWidthStartAdr = 24; 

	//начальный адрес для переключения канала в импульсный режим 
	usAbsPulseStartAdr = 36;

	data.assign(12, 0.0);
}

ADAM6066::~ADAM6066()
{

}

ADAM6066* ADAM6066::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM6066> obj ( new ADAM6066() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM6066::OnCreate()
{

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//тип модуля с адресом deviceAdress не соответствует ожидаемому
	if ( ModuleName() != "6066")
	{
		str << "Device attached to " << devAdress.c_str() << ":" << portNo << " is not " << m_strModel << std::ends;
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;		
	}
	
	if (!UpdateChannelsStateDO() )
	{
		str << "Update digital output channels state for module " << m_strModel 
			<< " attached to " << devAdress.c_str() << ":" << portNo << " failed!" <<std::ends;

		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	if ( !UpdateChannelsStateDI() )
	{
		str << "Update digital input channels state for module " << m_strModel 
			<< " attached to " << devAdress.c_str() << ":" << portNo << " failed!" <<std::ends;

		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	return true;
}

void ADAM6066::ControlProc()
{
	UpdateChannelsStateDI();
	UpdateChannelsStateDO();
	ReadChannels();
}

//IAnalogInput
//////////////////////////////////////////////////////////////////////////
//Реализация функций интерфейса IAnalogInput

//Получение общего числа аналоговых входов
//Для данного устройства 12 - входов
//0-5 - counter
//6-11 - frequency
int ADAM6066::GetChannelsCountAI() const 
{ 
	return 12; 
}


//Чтение данных с аналоговых входов во внешний буфер pdData 
//размер буфера должен быть не меньше sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
//При pdData = NULL происходит опрос включеных входов и сохранение во внутреннем буфере класаа  
void ADAM6066::ReadChannels(double *pdData/* = NULL*/) const
{
	vector<unsigned char> response;
	unsigned short szBuffer[] =
	{
		htons(0),
		htons(12*2)
	};

	//
	if ( !(const_cast<ADAM6066*>(this))->SendModbus(ReadInputRegisters, (char*)szBuffer, sizeof(szBuffer), response, &m_logDevs))
		return;

	if ( response.empty() )
		return;

	Lock();
	
	int ch = 0;
	for(int i = 0; i < 24; i += 4)
	{
		short lo = ntohs(*reinterpret_cast<short*>( (&response.data()[i]) ));
		short hi = ntohs(*reinterpret_cast<short*>( (&response.data()[i + 2]) ));
		
		data[ch]	 = hi * 65535 + lo;
		data[ch + 6] = lo / 10.0;
		ch++;		
	}

	Unlock();

	return;
}

//Получение данных с одного из 8 аналоговых входов (выполняется запрос на чтение данных)
// номер входа должен быть от 0 до 8
double  ADAM6066::ReadChannel(unsigned char channel)  const
{
	ReadChannels(NULL);
	return data.at(channel);
}



// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  ADAM6066::GetChannelData(unsigned char channel)  const
{
	return data.at(channel);
}

//Получить  информацию об измеряемом диапазоне канала
void ADAM6066::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{

}

//Установить  диапазон  аналогового входа
bool ADAM6066::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить информациюю о всех доступных диапазонах 
void ADAM6066::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
}

//  [3/3/2016 Johnny A. Matveichik]
ADAM6000Impl::ADAM6000Impl(const CModbusTCPConnection& sock) : 
	adamSocket(sock) 
{

}

ADAM6000Impl::~ADAM6000Impl()
{

}

