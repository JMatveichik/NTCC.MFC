#include "StdAfx.h"
#include "EASunInvertor.h"
#include "CommandManager.h"
#include "Serial.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

EASunInvertor::EASunInvertor() :
IdentifiedPhysDevice("Kraftelectronik", "EASUNINVERTOR")
{
	AIN_CHANNELS_COUNT = 38;
	AOUT_CHANNELS_COUNT = 22;
	DOUT_CHANNELS_COUNT = 13;
	DIN_CHANNELS_COUNT = 1;

	dbi.push_back( DATA_BLOCK_INFO("VFW", 6, 20) );
	dbi.push_back( DATA_BLOCK_INFO("ID", 5, 25)  );
	dbi.push_back( DATA_BLOCK_INFO("MOD", 6, 5)  );
	dbi.push_back( DATA_BLOCK_INFO("PIRI", 7, 88) );
	dbi.push_back( DATA_BLOCK_INFO("GS", 5, 106)  );


	//////////////////////////////////////////////////////////////////////////
	//DIGITAL OUTPUT CHANNELS INFO
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE0, "POPM0", 10));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE0, "PGR", 7));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE0, "PSP", 7));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE0, "POP", 7));

	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "A", 6));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "B", 6));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "C", 6));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "D", 6));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "E", 6));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "F", 6));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "G", 6));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "H", 6));
	doutci.push_back(DOUT_CHANNEL_INFO(DOUT_TYPE1, "I", 6));

	//////////////////////////////////////////////////////////////////////////
	//ANALOG INPUT CHANNELS
	ainci.push_back(AIN_CHANNEL_INFO(PIRI, 0, "AC input voltage", 	0.1,	220,	240,	"V"));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI, 1, "AC input rating current",	0.1, 0.0, 100.0, "А"));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	4,	"AC output rating current",	0.1, 0.0, 100.0, "A"));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	5,	"AC output rating active power", 1.0, 0.0, 100.0, "W"));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	6,	"AC output rating apparent power", 1.0, 0.0, 100.0, "VA"));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	7,	"Battery rating voltage",	0.1, 1.0, 100.0, "V"));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	16,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	19,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	21,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	24,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(PIRI,	25,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	0,	"Grid voltage",	0.1, 0.0, 100.0, "V"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	1,	"Grid frequency",	0.1, 0.0, 100.0, "Hz"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	2,	"Grid output voltage",	0.1, 0.0, 100.0, "V"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	3,	"AC output frequency",	0.1, 0.0, 100.0, "Hz"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	4,	"AC output apparent power",	1, 0.0, 100.0, "VA"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	5,	"AC output active power",	1, 0.0, 100.0, "W"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	6,	"Output load percent",	1, 0.0, 100.0, "%"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	7,	"Battery voltage",	0.1, 0.0, 100.0, "V"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	8,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	9,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	10,	"Battery discharge current", 1.0, 0.0, 100.0, "A"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	11,	"Battery charging current",	1.0, 0.0, 100.0, "A"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	12,	"Battery capacity",	1, 0.0, 100.0, "%"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	13,	"Temperature",	1, 0.0, 100.0, "°C"));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	14,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	15,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	16,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	17,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	18,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	19,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	20,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	21,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	22,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	24,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	25,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	26,	"Unknown",	1, 0.0, 100.0, ""));
	ainci.push_back(AIN_CHANNEL_INFO(GS,	27,	"Unknown",	1, 0.0, 100.0, ""));

	rngAI.assign(ainci.size(), MEASSURE_RANGE_INFO(0, "", "", 0.0, 100.0));
	dataAI.assign(ainci.size(), 0.0);

	for (int i = 0; i < (int)ainci.size(); i++)
	{
		rngAI[i].rngid = i;
		rngAI[i].units = ainci[i].units;
		rngAI[i].desc = ainci[i].description;
		rngAI[i].mins = ainci[i].minValue;
		rngAI[i].maxs = ainci[i].maxValue;
	}


	//////////////////////////////////////////////////////////////////////////
	///ANALOG OUTPUTS
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 18, "^S009PCP0,%1d",		"Charger source priority",									1,	0.0, 2.0, "")); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 15, "^S013MCHGC0,%03d",	"Max. charging current",									1,	10.0, 140.0, "A")); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 14, "^S014MUCHGC0,%03d", "Max. AC charging current",									1,	2.0, 60.0, "A")); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 13, "^S007PBT%1d",		"Battery type",												1,	0.0, 2.0,	"")); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 3,  "^S006F%2d",			"AC output frequency",										1,	50.0, 60.0, "Hz")); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 2,  "^S008V%4d",			"AC output voltage",										10,	220.0, 240.0, "V")); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 8,  "^S014BUCD%03d,%03d0", "Battery re-charged voltage when utility is available",		10,	44.0, 51.0, "V", 9)); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 9,  "^S014BUCD%03d,%03d1", "Battery re-discharged voltage when utility is available",	10,	48.0, 58.0, "V", 8)); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 11, "^S015MCHGV%03d,%03d0", "Bulk charging voltage",									10,	48.0, 58.4, "V", 12)); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 12, "^S015MCHGV%03d,%03d1", "Floating charging voltage",								10,	48.0, 58.4, "V", 11)); 
	aoutci.push_back(AOUT_CHANNEL_INFO("PIRI", 10, "^S010PSDV%3d",		 "Battery cut-off voltage",									10,	40.0, 48.0, "V")); 

	rngAO.assign(aoutci.size(), MEASSURE_RANGE_INFO(0, "", "", 0.0, 100.0));
	dataAO.assign(aoutci.size(), 0.0);

	for (int i = 0; i < (int)aoutci.size(); i++)
	{
		rngAO[i].rngid = i;
		rngAO[i].units = aoutci[i].units;
		rngAO[i].desc = aoutci[i].description;
		rngAO[i].mins = aoutci[i].minValue;
		rngAO[i].maxs = aoutci[i].maxValue;
	}
}


EASunInvertor::~EASunInvertor(void)
{

}

EASunInvertor* EASunInvertor::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<EASunInvertor> obj ( new EASunInvertor() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool EASunInvertor::CreateFromXMLNode(IXMLDOMNode* pNode)
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

void EASunInvertor::ControlProc()
{
	ReadChannels();
	
	/*if ( !ReadDataBlock(PIRI, dataPIRI) )
		TRACE("\nPIRI BLOCK DATA READING FAILED...\n");

	if ( !ReadDataBlock(GS, dataGS) )
		TRACE("\nGS BLOCK DATA READING FAILED...\n");
		*/
}

void TRACE_HEX(const unsigned char* data, int len)
{
	for (int i = 0; i < len; i++)
	{
		TRACE("%02X ", data[i]);
		if (data[i] == 0x0d)
			break;
	}

	TRACE("\n");
}

bool EASunInvertor::ReadDataBlock(unsigned int blockID, std::vector<double>& data) const
{
	CHECKCMDMANAGER(pMan, portNo);

	const DATA_BLOCK_INFO& blockinfo = dbi.at(blockID);	

	//////////////////////////////////////////////////////////////////////////
	//PREPARE READ COMMAND
	boost::format fmt("^P%03d%s");

	fmt % blockinfo.request_length % blockinfo.name.c_str();	 
	std::string s =  boost::str(fmt);

	//calculate CRC 
	unsigned short  crc = CRC_CCITT( (const unsigned char*)s.c_str(), s.size());

	boost::format fmt1("%s%c%c\r");
	fmt1 % s % HIBYTE(crc) % LOBYTE(crc);

	s = boost::str(fmt1);
	
	//////////////////////////////////////////////////////////////////////////
	//SEND COMMAND
	ASCII_COMMAND cmd;
	memset(&cmd, 0, sizeof(ASCII_COMMAND));
	cmd.pSender = this;	
	cmd.length = s.size();

	memcpy(cmd.cmd, s.c_str(), s.size());
	
	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	std::string res = cmd.response;
	
	//////////////////////////////////////////////////////////////////////////
	/// CHECK RESPONSE

	//ПРОВЕРКА ЗАГОЛОВКА ОТВЕТА
	if (res[0] != 0x5E || res[1] != 0x44)
		return false;


	//ПРОВЕРКА КОНТРОЛЬНОЙ СУММЫ ОТВЕТА
	size_t len = res.length();
	
	std::string strCRC = res.substr(len - 3, 2); 
// 	boost::crc_basic<16>  crc_ccitt( 0x1021, 0xFFFF, 0, false, false );
// 	crc_ccitt.process_bytes( (const unsigned char*)res.c_str(), len - 3 );
// 
// 	crc = crc_ccitt.checksum();

	const unsigned char c1 = (const unsigned char)strCRC[0];
	const unsigned char c2 = (const unsigned char)strCRC[1];

	//boost::crc_optimal<16, 0x1021, 0xFFFF, 0, false, false>  crc_ccitt2;
	//boost::crc_ccitt_type crc_ccitt2;
	//crc_ccitt2 = std::for_each( str, str + len - 3, crc_ccitt2 );

	crc = CRC_CCITT( (const unsigned char*)res.c_str(), len - 3);

	/*if ( LOBYTE(crc) != (byte)strCRC[1] ||  HIBYTE(crc) != (byte)strCRC[0])
		return false;*/
	
	//ПРОВЕРКА РАЗМЕРА ДАННЫХ
	int iDataLen = atoi(res.substr(2, 3).c_str() );
	if (iDataLen != blockinfo.response_length )
		return false;


	//////////////////////////////////////////////////////////////////////////
	/// ИЗВЛЕЧЕНИЕ ДАННЫХ ИЗ ОТВЕТА

	std::string strBlockData = res.substr(5, len - 8);
	ExtractData<double>(strBlockData, ",", data);
	
	return true;
}

bool EASunInvertor::OnPrepareDevice()
{

	CHECKCMDMANAGER(pMan, portNo);

	//подготовка дополнительных параметров последовательного порта
	DCB dcb; 
	pMan->GetComState( dcb );

	dcb.ByteSize = 8;			//длина байта данных
	dcb.fParity = FALSE;		//контроль четности
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;	//число стоповых битов

	dcb.BaudRate = 2400;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	if ( !pMan->SetComState(dcb) )
	{
		str << "COM" << (int)portNo<< " initialization failed" << std::ends;
		err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
		return false;
	}

	for (int i = 0; i< 3; i++)
	{
		ReadChannels();
	}
	

	if ( dataPIRI.empty() || dataGS.empty())
	{
		str << "Empty data block. Initialization failed" << std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	int ch = 0;
	for(auto it = aoutci.begin(); it != aoutci.end(); ++it)
	{
		int idx = (*it).blockIdxr;

		if ((*it).blockIDr == "PIRI")
			dataAO[ch] = dataPIRI.at(idx);
		else if ((*it).blockIDr == "GS")
			dataAO[ch] = dataGS.at(idx);
		ch++;
	}

	return true;
}


///////////////////////////////////////////////////
//Реализация интерфейса IDigitalInput


//Получить число дискретных входов
int EASunInvertor::GetChannelsCountDI() const
{
	return DIN_CHANNELS_COUNT;
}

//Обновить состояния дискретных входов
bool EASunInvertor::UpdateChannelsStateDI() const
{
	return true;
}

//Прочитать состояние одного дискретного входа
bool EASunInvertor::GetChannelStateDI(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDI();

	return dinChState.test( ch );
}


////////////////////////////////////////44444//////////////////////////////////
//Реализация интерфейса IDigitalOutput
//Получение общего числа цифровых выходов 
int EASunInvertor::GetChannelsCountDO() const
{
	return DOUT_CHANNELS_COUNT;
}

//Обновить текущее состояния дискретных выходов	
bool EASunInvertor::UpdateChannelsStateDO() const
{
	return true;
}

//Получить состояние цифрового выхода
bool EASunInvertor::GetChannelStateDO(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDO();

	return doutChState.test( ch );
}

//Установить состояние цифрового выхода 
bool  EASunInvertor::SetChannelStateDO(unsigned char ch, bool enable)
{
	if ( ch < 0 || ch >= DOUT_CHANNELS_COUNT )
		return false;

	const DOUT_CHANNEL_INFO& chi = doutci.at(ch);
	CHECKCMDMANAGER(pMan, portNo);
	
	std::string s;
	
	//////////////////////////////////////////////////////////////////////////
	//PREPARE WRITE COMMAND
	switch(chi.channelType)
	{
	
	case DOUT_TYPE0:
		{
			boost::format fmt("^S%03d%s%d");
			fmt % chi.cmdLegth % chi.variableName.c_str() % enable;	 
			s =  boost::str(fmt);
		}
		break;

	case DOUT_TYPE1:
		{
			boost::format fmt("^S%03dP%c%s");
			fmt % chi.cmdLegth % (enable ? 'E': 'D') % chi.variableName.c_str() ;	 
			s =  boost::str(fmt);
		}
		break;

	default:
		return false;

	}

	//calculate CRC 
	unsigned short  crc = CRC_CCITT( (const unsigned char*)s.c_str(), s.size());

	boost::format fmt("%s%c%c\r");
	fmt % s % HIBYTE(crc) % LOBYTE(crc);

	s = boost::str(fmt);

	//////////////////////////////////////////////////////////////////////////
	//SEND COMMAND
	ASCII_COMMAND cmd;
	memset(&cmd, 0, sizeof(ASCII_COMMAND));
	cmd.pSender = this;	
	cmd.length = s.size();

	memcpy(cmd.cmd, s.c_str(), s.size());

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	//////////////////////////////////////////////////////////////////////////
	/// CHECK RESPONSE

	if (cmd.response[0] != 0x5E || cmd.response[1] != 0x31)
		return false;

	doutChState.set(ch, enable);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//interface IAnalogInput
//Получение общего числа аналоговых входов
int EASunInvertor::GetChannelsCountAI() const
{
	return AIN_CHANNELS_COUNT;
}

//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
void EASunInvertor::ReadChannels(double* data/*  = NULL*/) const
{	
	
	if ( !ReadDataBlock(PIRI, dataPIRI) || !ReadDataBlock(GS, dataGS))
	{
		
		return;
	}

	for(int i = 0; i < (int)ainci.size(); i++)
	{
		int idx = ainci[i].blockIdx;
		switch( ainci[i].blockID )
		{
		case PIRI:
			dataAI[i] = dataPIRI[idx];
			break;

		case GS:
			dataAI[i] = dataGS[idx];
			break;
		}
		dataAI[i] *= ainci[i].scale;
	}
	
}

//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
double  EASunInvertor::ReadChannel(unsigned char ch)  const
{	
	return GetChannelData(ch);
}

// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  EASunInvertor::GetChannelData(unsigned char ch)  const
{
	double val = 0.0;
	Lock();
	val = dataAI[ch];
	Unlock();
	return val;
}

//Получить  информацию об измеряемом диапазоне канала
void EASunInvertor::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	*lpmci = rngAI[ch];
}

//Установить  диапазон  аналогового входа
bool EASunInvertor::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить все возможные  диапазоны  измерения
void EASunInvertor::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	rngs.push_back(rngAI[ch]);
}

//////////////////////////////////////////////////////////////////////////
/// IAnalogOutput
//Получение общего числа аналоговых выходов 
int EASunInvertor::GetChannelsCountAO() const
{
	return AOUT_CHANNELS_COUNT;
}

//Записать данные в аналоговый выход 
double  EASunInvertor::WriteToChannel(unsigned char ch, double val)
{
	if ( ch < 0 || ch >= DOUT_CHANNELS_COUNT )
		return false;

	const AOUT_CHANNEL_INFO& chi = aoutci.at(ch);
	CHECKCMDMANAGER(pMan, portNo);

	std::string s;
	int valW = val * chi.scale;
	//////////////////////////////////////////////////////////////////////////
	//PREPARE WRITE COMMAND
	
	std::string scmd = chi.cmd;
	int pos = 0;
	
	if ( chi.seconValueBlockIdxr != -1 )
	{
		std::string spos = scmd.substr(scmd.length()  - 1,  1);
		pos = atoi( spos.c_str() );

		scmd.erase(scmd.length()  - 1, 1);

		int svalW = int(dataPIRI[chi.seconValueBlockIdxr]);
		
		boost::format fmt1(scmd);

		if (pos == 0)
			fmt1 % valW % svalW;
		else if (pos == 1)
			fmt1 % svalW % valW;

		s = boost::str(fmt1);
	}
	else {
		
		boost::format fmt1(scmd);
		fmt1 % valW;
		s = boost::str(fmt1);
	}


	//calculate CRC 
	unsigned short  crc = CRC_CCITT( (const unsigned char*)s.c_str(), s.size());

	boost::format fmt("%s%c%c\r");
	fmt % s % HIBYTE(crc) % LOBYTE(crc);

	s = boost::str(fmt);

	//////////////////////////////////////////////////////////////////////////
	//SEND COMMAND
	ASCII_COMMAND cmd;
	memset(&cmd, 0, sizeof(ASCII_COMMAND));
	cmd.pSender = this;	
	cmd.length = s.size();

	memcpy(cmd.cmd, s.c_str(), s.size());

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	//////////////////////////////////////////////////////////////////////////
	/// CHECK RESPONSE

	if (cmd.response[0] != 0x5E || cmd.response[1] != 0x31)
		return false;

	return true;
}

//Записать данные во все каналы
void EASunInvertor::WriteToAllChannels(const std::vector<double>& vals)
{

}

bool EASunInvertor::UpdateChannelsStateAO() const
{
	return true;
}
//Получить последнее записанное в аналоговый выход значение 
double EASunInvertor::GetChannelStateAO (unsigned char ch) const
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
void EASunInvertor::GetMinMax(unsigned char ch, double& minOut, double& maxOut) const
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
void EASunInvertor::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	*lpmci = rngAO[ch];
}

//Установить  диапазон  аналогового выхода
bool EASunInvertor::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}	

//Получить все возможные  диапазоны  
void EASunInvertor::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();	

	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	rngs.push_back(rngAO[ch]);
}