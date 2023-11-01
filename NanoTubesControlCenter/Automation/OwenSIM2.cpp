#include "StdAfx.h"
#include "OwenSIM2.h"

#include "CommandManager.h"
#include "DataSrc.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

#define MAX_WORD_LEN 8



OwenSIM2::OwenSIM2() : OwenBase( "SIM2" ), AOUT_CHANNELS_COUNT(4), DOUT_CHANNELS_COUNT(5), dMinDiff(5.0)
{
	rngAO.push_back(MEASSURE_RANGE_INFO(0, "Код режима", "", -999, 9999));
	rngAO.push_back(MEASSURE_RANGE_INFO(1, "Код предупреждения", "", -999, 9999));
	rngAO.push_back(MEASSURE_RANGE_INFO(2, "Код ошибки", "", -999, 9999));
	rngAO.push_back(MEASSURE_RANGE_INFO(3, "Произвольное значение",  "", -999, 9999));

	aoData.assign(AOUT_CHANNELS_COUNT, 0.0);
	doData.assign(DOUT_CHANNELS_COUNT, false);
}


OwenSIM2::~OwenSIM2(void)
{
	Write("wait");
}

void OwenSIM2::string2chara(std::string word, char* pszOut)
{
	//size = word.length() > 8 ? 8 : word.length();
	memset(pszOut, 0x20, MAX_WORD_LEN); //забиваем все пробелами

	for ( int i = 0; i < MAX_WORD_LEN; i++ )
	{
		if ( i >= (int)word.length())
			break;

		char ch = word[i];
		if ( 
			(ch >= 0x41 || ch <= 0x5A) ||	// "A-Z"
			(ch >= 0x61 || ch <= 0x7A) ||	// "a-z")
			(ch == 0x20 || ch == 0x2E || ch == 0x2D || ch == 0x5F) // пробел, точка, тире или нижнее подчеркивание
			)
			pszOut[i] = ch;
	}	
}

OwenSIM2* OwenSIM2::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<OwenSIM2> obj ( new OwenSIM2() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();	
}

bool OwenSIM2::OnPrepareDevice()
{
	CHECKCMDMANAGER(pMan, comPortNo)

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//подготовка дополнительных параметров последовательного порта
	DCB dcb; 
	pMan->GetComState( dcb );

	dcb.ByteSize = 8;			//длина байта данных
	dcb.fParity = FALSE;		//контроль четности
	dcb.StopBits = ONESTOPBIT;	// число стоповых битов

	if ( !pMan->SetComState(dcb) )
	{
		str << "Initialize port COM" << (int)comPortNo << " failed" << std::ends;
		err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str());
		return false;
	}	

	Blink(true);
 	if ( !Write("init") )
 		return false;
	
	curValInd = -1;

	return true;
}

bool OwenSIM2::PostConfigProcessing(IXMLDOMNode* pNode)
{
	Lock();
	
	AppXMLHelper& appXml = AppXMLHelper::Instance();

	std::vector<std::string> names;
	appXml.GetSubNodesAtributes(pNode, "ITEM", "DSID", names);	
	appXml.GetSubNodesAtributes(pNode, "ITEM", "DISPLAY", outNames);

	for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end();++it)
	{
		const DataSrc* pDS = DataSrc::GetDataSource((*it));
		if ( pDS != NULL)
			outValues.push_back(pDS);
	}
	
	Unlock();

	return true;
}

bool OwenSIM2::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !OwenBase::CreateFromXMLNode(pNode) )
		return false;

	return true;
}

bool OwenSIM2::DisplayValueType(unsigned short vt)
{
	CHECKCMDMANAGER(pMan, comPortNo)

	MODBUS_SERIAL_COMMAND cmd;
	InitCmd(&cmd, 0x06, 1, 0x0011);

	cmd.response[0] = vt;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	return true;
}

bool OwenSIM2::Blink(bool blink) 
{
	CHECKCMDMANAGER(pMan, comPortNo)

	MODBUS_SERIAL_COMMAND cmd;
	InitCmd(&cmd, 0x06, 1, 0x0023);	
	
	//мигание текстовых значений
	cmd.response[0] = blink ? 0x00BB : 0x0000;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	return true;
}

void OwenSIM2::InitCmd(LPMODBUS_SERIAL_COMMAND lpmbsc, unsigned char fn, int regCount, int regAddr) const
{
	lpmbsc->pSender = this;
	lpmbsc->slaveAddress = devAddres;
	lpmbsc->func = fn;
	lpmbsc->regCount = regCount;
	lpmbsc->regAddress = regAddr;
}

bool OwenSIM2::Write(float val)
{
	CHECKCMDMANAGER(pMan, comPortNo)

	MODBUS_SERIAL_COMMAND cmd;
	InitCmd(&cmd, 0x10, 2, 0x001B);

	unsigned short d[2];
	memcpy(d, &val, sizeof(unsigned short) * 2);
	
	cmd.response[0] = d[1];
	cmd.response[1] = d[0];
 	
  	if ( !pMan->SendCommand(&cmd, false) )
 		return false;

	return DisplayValueType(DVT_FLT);
}

bool OwenSIM2::Write(short int val)
{
	CHECKCMDMANAGER(pMan, comPortNo)

	MODBUS_SERIAL_COMMAND cmd;
	InitCmd(&cmd, 0x06, 1, 0x0019);
	
	cmd.response[0] = val;			
	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	return DisplayValueType(DVT_INT);
}

bool OwenSIM2::Write(std::string word)
{
	CHECKCMDMANAGER(pMan, comPortNo)

	MODBUS_SERIAL_COMMAND cmd;
	InitCmd(&cmd, 0x10, MAX_WORD_LEN / 2, 0x001D);

	char szOut[MAX_WORD_LEN];
	string2chara(word, szOut);

	for (int i = 0; i < MAX_WORD_LEN; i += 2)
	{
		reinterpret_cast<char*>(cmd.response)[i]     = szOut[i + 1];
		reinterpret_cast<char*>(cmd.response)[i + 1] = szOut[i];
	}
	
	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	return DisplayValueType(DVT_STR4);
}

//Функция процесса выполяется циклически через заданный интервал времени
void OwenSIM2::ControlProc()
{
	int mode = 0;
	int warn = 0;
	int err  = 0;
	float val = 0.0;

	Lock();

	mode = (int)aoData.at(AOUT_CH_MODE);
	warn = (int)aoData.at(AOUT_CH_WARNING);
	err  = (int)aoData.at(AOUT_CH_ERRORS);
	val  = (float)aoData.at(AOUT_CH_VALUE);

	Unlock();

	std::stringstream str;
	
	Blink( doData[DOUT_CH_BLINK] );

	if ( doData[DOUT_CH_MODE] )
	{
		//Вывод информации o текущем  режиме
		str << "P." << setw(3)  << setfill('0')  << mode << ends; 
		Write( str.str() );
		Sleep(m_nInterval / AOUT_CHANNELS_COUNT);
	}

	//Если есть пердупреждения выводим код предупреждения
	if (doData[DOUT_CH_WARNING] )
	{
		str.str("");
		str << "L." << setw(3)  << setfill('0')  << warn << ends;
		
		Write( str.str() );	
		Sleep(m_nInterval / AOUT_CHANNELS_COUNT);
	}
	

	//Если есть ошибки выводим код ошибки
	if ( doData[DOUT_CH_ERRORS] )
	{
		str.str("");
		str << "E." << setw(3)  << setfill('0')  << err << ends; 
		Write( str.str() );		
		Sleep(m_nInterval / AOUT_CHANNELS_COUNT);
	}

	if ( doData[DOUT_CH_VALUE] )
	{
		Write( val );
		Sleep(m_nInterval / AOUT_CHANNELS_COUNT);
	}
	
}

//////////////////////////////////////////////////////////////////////////
/// IAnalogOutput
//Получение общего числа аналоговых выходов 
int OwenSIM2::GetChannelsCountAO() const
{
	return AOUT_CHANNELS_COUNT;
}

//Записать данные в аналоговый выход 
double OwenSIM2::WriteToChannel(unsigned char channel, double val)
{
	double old;
	Lock();

	old = (double)aoData.at(channel);
	
	//if ( fabs(old - val) > dMinDiff )
		aoData.at(channel) = (int)val;

	Unlock();

	return old;
}

//Записать данные во все каналы
void OwenSIM2::WriteToAllChannels(const std::vector<double>& vals) 
{

}

//Получить последнее записанное в аналоговый выход значение 
double OwenSIM2::GetChannelStateAO (unsigned char channel) const
{
	if (channel < 0 || channel > AOUT_CHANNELS_COUNT)
		return 0.0;

	return aoData.at((int)channel);
}
//Получить выходной диапазон канала 
void OwenSIM2::GetMinMax(unsigned char channel, double& minOut, double& maxOut) const
{
	if (channel < 0 || channel > AOUT_CHANNELS_COUNT)
		return;

	minOut = rngAO[channel].mins;
	maxOut = rngAO[channel].maxs;
}

//Получить  информацию об диапазоне канала
void OwenSIM2::GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci = rngAO[channel];
}
//Установить  диапазон  аналогового выхода
bool OwenSIM2::SetOutputRange(unsigned char channel, unsigned char range)
{
	return false;
}

//Получить все возможные  диапазоны  
void OwenSIM2::EnumOutputRanges(unsigned char channel, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();	

	if (channel < 0 || channel > AOUT_CHANNELS_COUNT)
		return;

	rngs.push_back(rngAO[channel]);
}

//////////////////////////////////////////////////////////////////////////
/// IDigitalOutput
//Получение общего числа цифровых выходов 
int OwenSIM2::GetChannelsCountDO() const
{
	return DOUT_CHANNELS_COUNT;
}

//Обновить текущее состояния дискретных выходов	
bool OwenSIM2::UpdateChannelsStateDO() const
{
	return true;
}

//Получить состояние цифрового выхода
bool OwenSIM2::GetChannelStateDO(unsigned char channel, bool update) const
{
	return doData.at((int)channel);
}

//Установить состояние цифрового выхода 
bool  OwenSIM2::SetChannelStateDO(unsigned char channel, bool enable)
{
	bool old = doData[channel];
	doData[channel] = enable;
	return old;
}