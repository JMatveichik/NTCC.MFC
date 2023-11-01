#include "StdAfx.h"
#include "IsmatecPump.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

IsmatecPump::IsmatecPump() :
IdentifiedPhysDevice("ISMATECH", "ISM321"), 
DOUT_CHANNELS_COUNT(3),
AOUT_CHANNELS_COUNT(2),
AIN_CHANNELS_COUNT(1)
{

	lastValsAO.assign(AOUT_CHANNELS_COUNT, 0.0);
	lastValsAI.assign(AIN_CHANNELS_COUNT, 0.0);

	
}

IsmatecPump::~IsmatecPump(void)
{
	
}

IsmatecPump* IsmatecPump::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<IsmatecPump> obj ( new IsmatecPump() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool IsmatecPump::CreateFromXMLNode(IXMLDOMNode* pNode)
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

	int adr = atoi(sAtr.c_str());
	devAdr = adr;	

	if (! OnPrepareDevice( ) )
		return false;	

	return true;
}

void IsmatecPump::ControlProc()
{
	ReadChannels();
}

bool IsmatecPump::OnPrepareDevice()
{
	CHECKCMDMANAGER(pMan, portNo);

// 	SetChannelStateDO(DOUT_CH_MODE,   false);
// 	SetChannelStateDO(DOUT_CH_DIRECT, false);
	rngAO.push_back(MEASSURE_RANGE_INFO(0, "Скорость вращения", "об/мин", 0, 1800));

	//формируем конечную команду
	char szCmd[256];
	double maxVal = 0.0;

	//заполняем сруктуру команды
	ASCII_COMMAND cmd;
	cmd.pSender = this;	
	cmd.length = 0;
	strcpy_s(cmd.response, "SEND");
		
	//получаем старое значение
	sprintf_s(szCmd, 256, "%1d?\r", devAdr);
	strcpy_s(cmd.cmd, szCmd);
	if (! pMan->SendCommand(&cmd, true) )
		return false;

	if (strcmp(cmd.response, "FAILURE") == 0)
		return false;

	maxVal = atof( cmd.response );
		
	rngAO.push_back(MEASSURE_RANGE_INFO(1, "Объемный расход", "мл/мин", 0, maxVal));

	
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "Скорость вращения", "об/мин", 0, 1800));
	rngAI.push_back(MEASSURE_RANGE_INFO(1, "Объемный расход", "мл/мин", 1, maxVal));

	return true;
}

//////////////////////////////////////////////////////////////////////////
//IAnalogInput

//Получение общего числа аналоговых входов
int IsmatecPump::GetChannelsCountAI() const
{
	return AIN_CHANNELS_COUNT;
}

//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
void	IsmatecPump::ReadChannels(double* data/*  = NULL*/) const
{	
	
	for (unsigned char i = 0; i < AIN_CHANNELS_COUNT; i++)
	{
		ReadChannel(i);
	}

	if (data != nullptr)
		memcpy(data, lastValsAI.data(), AIN_CHANNELS_COUNT*sizeof(double));

}

//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
double  IsmatecPump::ReadChannel(unsigned char ch)  const
{
	CHECKCMDMANAGER(pMan, portNo);

	std::string ainIDs ("Sf");
		
	//формируем конечную команду
	char szCmd[256];
	sprintf_s(szCmd, 256, "%1d%1c\r", devAdr, ainIDs[ch]);

	//заполняем сруктуру команды
	ASCII_COMMAND cmd;
	strcpy_s(cmd.cmd, szCmd);
	cmd.pSender = this;	
	cmd.length = 0;
	strcpy_s(cmd.response, "SEND");

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	std::string res = cmd.response;
	lastValsAI[ch] = atof(res.c_str());
	
	return lastValsAI[ch];
}

// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  IsmatecPump::GetChannelData(unsigned char ch)  const
{
	return lastValsAI[ch];
}

//Получить  информацию об измеряемом диапазоне канала
void IsmatecPump::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	*lpmci = rngAI[ch];
}

//Установить  диапазон  аналогового входа
bool IsmatecPump::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить все возможные  диапазоны  измерения
void IsmatecPump::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{
	rngs.clear();

	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	rngs.push_back(rngAI[ch]);
}


//////////////////////////////////////////////////////////////////////////
//IDigitalOutput
//Получение общего числа цифровых выходов 
int IsmatecPump::GetChannelsCountDO() const
{
	return DOUT_CHANNELS_COUNT;
}

//Обновить текущее состояния дискретных выходов	
bool IsmatecPump::UpdateChannelsStateDO() const
{
	CHECKCMDMANAGER(pMan, portNo);

	//формируем конечную команду
	char szCmd[256];
	sprintf_s(szCmd, 256, "%1dE\r", devAdr);

	//заполняем сруктуру команды
	ASCII_COMMAND cmd;
	strcpy_s(cmd.cmd, szCmd);
	cmd.pSender = this;	
	cmd.length = 0;
	strcpy_s(cmd.response, "SEND");
	
	if ( !pMan->SendCommand(&cmd, false) )
		return false;
	
	std::string res = cmd.response;
	outChState.set(DOUT_CH_START, (res[0] == '+') ? true : false );

	return true;
}

//Получить состояние цифрового выхода
bool IsmatecPump::GetChannelStateDO(unsigned char ch, bool update) const
{
	return outChState.test( ch );
}

//Установить состояние цифрового выхода 
bool  IsmatecPump::SetChannelStateDO(unsigned char ch, bool enable)
{
	if ( ch < 0 || ch >= DOUT_CHANNELS_COUNT )
		return false;

	std::string on ("HKB");
	std::string off("IJA");

	CHECKCMDMANAGER(pMan, portNo);

	//формируем конечную команду
	char szCmd[256];
	sprintf_s(szCmd, 256, "%1d%1C\r", devAdr, enable ? on[ch] : off[ch]);

	//заполняем сруктуру команды
	ASCII_COMMAND cmd;
	strcpy_s(cmd.cmd, szCmd);
	cmd.pSender = this;	
	cmd.length = 0;
	strcpy_s(cmd.response, "SEND");

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	if (cmd.response[0] == '*')
	{
		outChState.set(ch, enable );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
/// IAnalogOutput
//Получение общего числа аналоговых выходов 
int IsmatecPump::GetChannelsCountAO() const
{
	return AOUT_CHANNELS_COUNT;
}

//Записать данные в аналоговый выход 
double  IsmatecPump::WriteToChannel(unsigned char ch, double val)
{
	CHECKCMDMANAGER(pMan, portNo);

	//формируем конечную команду
	char szCmd[256];
	double oldVal = 0.0;

	//заполняем сруктуру команды
	ASCII_COMMAND cmd;
	cmd.pSender = this;	
	cmd.length = 0;
	strcpy_s(cmd.response, "SEND");

	
	if (ch == AOUT_CH_SPEED)
	{
		//получаем старое значение
		sprintf_s(szCmd, 256, "%1dS\r", devAdr);
		strcpy_s(cmd.cmd, szCmd);
		if ( pMan->SendCommand(&cmd, true) )
			oldVal = atof( cmd.response );
		
		int newVal = (int)val;

		//задаем новое значение
		sprintf_s(szCmd, 256, "%1dS%05d\r", devAdr, newVal);
		strcpy_s(cmd.cmd, szCmd);
		cmd.length = 0;
			 
		if ( !pMan->SendCommand(&cmd, false) )
			return oldVal;
			
		lastValsAO[ch] = val;
		
	}
	else if (ch == AOUT_CH_MLMIN)
	{
		//получаем старое значение
		sprintf_s(szCmd, 256, "%1df\r", devAdr);
		strcpy_s(cmd.cmd, szCmd);
		if ( pMan->SendCommand(&cmd, true) )
			oldVal = atof( cmd.response );

		int e = 3 - (int)log10(val);
		double m = val* pow(10.0, e);

		//задаем новое значение
		sprintf_s(szCmd, 256, "%1df%04.0f%+1d\r", devAdr, m, -1*e);
		strcpy_s(cmd.cmd, szCmd);
		cmd.length = 0;

		if ( !pMan->SendCommand(&cmd, false) )
			return oldVal;

		lastValsAO[ch] = val;
	}

	return oldVal;
}

//Записать данные во все каналы
void IsmatecPump::WriteToAllChannels(const std::vector<double>& vals)
{

}

//Получить последнее записанное в аналоговый выход значение 
double IsmatecPump::GetChannelStateAO (unsigned char ch) const
{
	return lastValsAO[ch];
}

//Получить выходной диапазон канала 
void IsmatecPump::GetMinMax(unsigned char ch, double& minOut, double& maxOut) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		minOut = maxOut = 0;

	minOut = rngAO[ch].mins;
	maxOut = rngAO[ch].maxs;
}

//Получить  информацию об диапазоне канала
void IsmatecPump::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	*lpmci = rngAO[ch];
}

//Установить  диапазон  аналогового выхода
bool IsmatecPump::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить все возможные  диапазоны  
void IsmatecPump::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();	

	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	rngs.push_back(rngAO[ch]);
}