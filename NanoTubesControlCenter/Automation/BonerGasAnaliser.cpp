#include "stdafx.h"
#include "BonerGasAnaliser.h"
#include "CommandManager.h"
#include "ModbusTCPConnection.h"

#include "..\ah_errors.h"
#include "..\ah_msgprovider.h"
#include "..\ah_xml.h"



#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

BonerGasAnaliserTest01::BonerGasAnaliserTest01() :  
IdentifiedPhysDevice("Boner", "TEST-01"),
INPUT_ANALOG_CHANNELS_COUNT(2)
{
	
	memset(data, 0, sizeof(double)*2);
}

BonerGasAnaliserTest01::~BonerGasAnaliserTest01(void)
{
	
}

bool BonerGasAnaliserTest01::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	std::string sAtr;

	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
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
		str << "Command manager assigned with port COM" << (int)comPortNo << " not found" << std::ends;
		err.SetLastError( DCE_PORT_NOT_AVAILABLE, str.str() );
		return false;
	}

	comPortNo = port;	
	return true;
}

BonerGasAnaliserTest01* BonerGasAnaliserTest01::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<BonerGasAnaliserTest01> obj ( new BonerGasAnaliserTest01() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool BonerGasAnaliserTest01::Create(std::string name, int portNo, unsigned long updateInterval)
{
	if ( !IdentifiedPhysDevice::Create(name, updateInterval) )
		return false;
	
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	CommandManager* pMan = CommandManager::GetCommandManager(portNo);
	if ( pMan == NULL )
	{
		str << "Command manager assigned with port COM" << (int)comPortNo << " not found" << std::ends;
		err.SetLastError( DCE_PORT_NOT_AVAILABLE, str.str() );
		return false;
	}


	comPortNo = portNo;
	ReadChannels(data);

	return true;
}

//‘ункци€ процесса выпол€етс€ циклически через заданный интервал времени
void BonerGasAnaliserTest01::ControlProc()
{
	ReadChannels(NULL);
}

void BonerGasAnaliserTest01::ReadChannels(double* channelsData) const
{
	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return;

	//ѕолучаем состо€ние регистров MODBUS
	unsigned short table[4];
	memset(table, 0, sizeof(unsigned char)*4 );

	MODBUS_SERIAL_COMMAND cmd;
	cmd.slaveAddress = 0x02;
	cmd.func = 0x04;
	cmd.regCount = 4;
	cmd.regAddress = 0x0034;

	pMan->SendCommand(&cmd, false);

	//ѕреобразуем значени€ регистров в формат с плавающей точкой
	float pData[2];
	memcpy(pData, cmd.response, sizeof(unsigned char) * 4);

	//если задан внешний буфер данных сохран€ем в него
	if ( channelsData != NULL )
	{
		channelsData[0] = pData[0];
		channelsData[1] = pData[1];
	}
	//иначе во внутренний
	else 
	{
		data[0] = pData[0];
		data[1] = pData[1];
	}
	::LeaveCriticalSection(&m_cs);
}

//ѕолучение данных с одного из аналоговых входов (выполн€етс€ запрос на чтение данных)
double  BonerGasAnaliserTest01::ReadChannel(unsigned char channel)  const 
{
	ReadChannels();
	return data[channel];
};

// ѕолучение данных из буфера дл€ одного из аналоговых входов 
// (запрос на чтение данных не выполн€етс€)
double  BonerGasAnaliserTest01::GetChannelData(unsigned char channel)  const
{
	return data[channel];
}

//ѕолучить  информацию об измер€емом диапазоне канала
void BonerGasAnaliserTest01::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	ASSERT(FALSE);
}

//”становить  диапазон  аналогового входа
bool BonerGasAnaliserTest01::SetInputRange(unsigned char ch, unsigned char range)
{
	ASSERT(FALSE);
	return false;
}

//ѕолучить все возможные  диапазоны  измерени€
void BonerGasAnaliserTest01::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	ASSERT(FALSE);
}

//////////////////////////////////////////////////////////////////////////
//
BonerGasAnaliserTest14::BonerGasAnaliserTest14() : IdentifiedPhysDevice("Boner", "Test-1.4") ,
INPUT_ANALOG_CHANNELS_COUNT(4), comPortNo(-1), pModbusTCP(NULL)
{
	memset(data, 0, sizeof(double) * 4);

	//адреса регистров дл€ аналоговых входов
	rngAI.push_back(MEASSURE_RANGE_INFO(0, " онцентраци€ CH4", "%", 0, 40));
	rngAI.push_back(MEASSURE_RANGE_INFO(1, " онцентраци€ H2", "%", 0, 40));
	rngAI.push_back(MEASSURE_RANGE_INFO(2, " онцентраци€ CO", "%", 0, 40));
	rngAI.push_back(MEASSURE_RANGE_INFO(3, " онцентраци€ CO2", "%", 0, 40));
	
}

BonerGasAnaliserTest14::~BonerGasAnaliserTest14(void)
{
	delete pModbusTCP;
}

bool BonerGasAnaliserTest14::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	std::string sAtr;

	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if (!appXML.GetNodeAttributeByName(pNode, "PORT", sAtr))
	{
		str << "Port number for device not set (tag <PORT>)" << std::ends;
		err.SetLastError(DCE_PORT_NUMBER_NOT_SET, str.str() );
		return false;	
	}
	int port =  atoi(sAtr.c_str());

	//////////////////////////////////////////////////////////////////////////
	if ( appXML.GetNodeAttributeByName(pNode, "ADDRESS", sAtr) )
	{
		auto_ptr<CModbusTCPConnection> pTCP(new CModbusTCPConnection()) ;
		std::string strIP = sAtr;

		if ( !pTCP->Establish(strIP, port) )
		{
			str << "Boner gasanalizer at IP address " << strIP.c_str() << " does not respond" << std::ends;
			err.SetLastError(DCE_INVALID_DEVICE_ADDRESS, str.str() );
			return false;
		}

		devAdress = strIP;

		pModbusTCP = pTCP.release();
		//pModbusTCP->SetTimeout(5000, 1000, 10000);
		pModbusTCP->EnableCallback(IdentifiedPhysDevice::FailureCallback, this);
	}
	else
	{
		CommandManager* pMan = CommandManager::GetCommandManager(port);
		if ( pMan == NULL )
		{
			str << "Command manager assigned with port COM" << (int)port<< " not found" << std::ends;
			err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
			return false;
		}
		//подготовка дополнительных параметров последовательного порта
		DCB dcb; 
		pMan->GetComState( dcb );

		dcb.BaudRate = 9600;
		dcb.ByteSize = 8;			//длина байта данных
		dcb.fParity = FALSE;		//контроль четности
		dcb.StopBits = ONESTOPBIT;	//число стоповых битов

		if ( !pMan->SetComState(dcb) )
		{
			str << "COM" << (int)port<< " initialization failed" << std::ends;
			err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
			return false;
		}

		comPortNo = port;
	}

	
	return true;
}

BonerGasAnaliserTest14* BonerGasAnaliserTest14::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<BonerGasAnaliserTest14> obj ( new BonerGasAnaliserTest14() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}


bool BonerGasAnaliserTest14::Create(std::string name, unsigned long updateInterval, int port, std::string strIP)
{
	if ( !IdentifiedPhysDevice::Create(name, updateInterval) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	if ( strIP.empty() )
	{
		CommandManager* pMan = CommandManager::GetCommandManager(port);
		if ( pMan == NULL )
		{
			str << "Command manager assigned with port COM" << (int)port<< " not found" << std::ends;
			err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
			return false;
		}
		//подготовка дополнительных параметров последовательного порта
		DCB dcb; 
		pMan->GetComState( dcb );

		dcb.BaudRate = 9600;
		dcb.ByteSize = 8;			//длина байта данных
		dcb.fParity = FALSE;		//контроль четности
		dcb.StopBits = ONESTOPBIT;	//число стоповых битов

		if ( !pMan->SetComState(dcb) )
		{
			str << "COM" << (int)port<< " initialization failed" << std::ends;
			err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
			return false;
		}

		comPortNo = port;
	}
	else
	{
		str << "Device address not set (tag <ADDRESS>)" << std::ends;

		auto_ptr<CModbusTCPConnection> pTCP(new CModbusTCPConnection()) ;
		
		if ( !pTCP->Establish(strIP, port) )
		{
			str << "IP address " << strIP.c_str() << " does not respond" << std::ends;
			err.SetLastError(DCE_INVALID_DEVICE_ADDRESS, str.str() );
			return false;
		}
		
		devAdress = strIP;
		pModbusTCP = pTCP.release();		
		pModbusTCP->EnableCallback(IdentifiedPhysDevice::FailureCallback, this);
	}

	ReadChannels(data);

	return true;
}

bool BonerGasAnaliserTest14::SendModbus(MODBUS_COMMAND cmd, const char* data, unsigned data_size, std::vector<unsigned char>& response, std::ostream* pLog)
{
	if ( !(m_bStatusOk = pModbusTCP->Transact(cmd, data, data_size, response, pLog) ) )
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

bool BonerGasAnaliserTest14::OnFailure()
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

//‘ункци€ процесса выпол€етс€ циклически через заданный интервал времени
void BonerGasAnaliserTest14::ControlProc()
{
	ReadChannels(NULL);
}


void BonerGasAnaliserTest14::ReadChannels(double* channelsData) const
{
	
	::EnterCriticalSection(&m_cs);

	//ѕолучаем состо€ние регистров MODBUS
	unsigned short regState[2];
	float X[4] = {0.0, 0.0, 0.0, 0.0};	

	if ( pModbusTCP )
	{
		unsigned short szBuf[] = { htons(0), htons(16) };
	
		vector<unsigned char> response;

		if ( (const_cast<BonerGasAnaliserTest14*>(this) )->SendModbus(ReadInputRegisters, (char*)szBuf, sizeof(szBuf), response, &m_logDevs) )
		{
			if ( response.size() == 32 )
			{
				for (int i = 0, adr=0x02; i < 4; i++, adr += 0x04 )
				{
					regState[0] = ntohs(((unsigned short*)&response[0])[adr]);
					regState[1] = ntohs(((unsigned short*)&response[0])[adr + 1]);
					memcpy(&X[i], regState, sizeof(regState));
					
					data[i] = X[i];
					////TRACE("Channel %d addres 0x%04X - %04.2f\n", i, adr, data[i]);
				}
			}
			else 
				data[0] = data[1] = data[2] = data[3] = -1.0;
		}
		else 
				data[0] = data[1] = data[2] = data[3] = -2.0;
		
	}
	else
	{
		CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

		if ( pMan == NULL )
			return;

		for (int i = 0, adr=0x02; i < 4; i++, adr += 0x04 )
		{

			MODBUS_SERIAL_COMMAND cmd;
			cmd.slaveAddress = 0x01;
			cmd.func = 0x04;
			cmd.regCount = 2;

			//точность вывода температуры
			cmd.regAddress = adr;

			pMan->SendCommand(&cmd, false);


			//pMan->ComPort().ModbusReadInputRegs(0x01, adr, 2, regState);

			memcpy(&X[i], cmd.response, sizeof(unsigned short)*cmd.regCount);

			data[i] = X[i];
			////TRACE("Channel %d addres 0x%04X - %04.2f\n", i, adr, data[i]);
		}

	}	
	
	::LeaveCriticalSection(&m_cs);
}

//ѕолучение данных с одного из аналоговых входов (выполн€етс€ запрос на чтение данных)
double  BonerGasAnaliserTest14::ReadChannel(unsigned char channel)  const 
{
	ReadChannels();
	return data[channel];
};

// ѕолучение данных из буфера дл€ одного из аналоговых входов 
// (запрос на чтение данных не выполн€етс€)
double  BonerGasAnaliserTest14::GetChannelData(unsigned char channel)  const
{
	return data[channel];
}

//ѕолучить  информацию об измер€емом диапазоне канала
void BonerGasAnaliserTest14::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci = rngAI.at(ch);
}

//”становить  диапазон  аналогового входа
bool BonerGasAnaliserTest14::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//ѕолучить все возможные  диапазоны  измерени€
void BonerGasAnaliserTest14::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	rngs.push_back(rngAI.at(ch));
}