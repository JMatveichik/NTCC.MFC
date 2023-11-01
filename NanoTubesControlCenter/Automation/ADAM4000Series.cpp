
#include "StdAfx.h"

#include "ADAM4000Series.h"
#include "commandmanager.h"
#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

ADAM4000::ADAM4000() :  
IdentifiedPhysDevice("Advantech Co.", "UNKNOWN"),
cmds(NULL), 	
devAdress(0), 
comNo(0)
{	
	
}
//�����������
ADAM4000::ADAM4000(std::string model) :
IdentifiedPhysDevice("Advantech Co.", model),
cmds(NULL), 	
devAdress(0), 
comNo(0)
{

}

ADAM4000::~ADAM4000()
{
}

bool ADAM4000::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

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
	
	cmds = pMan;
	
	//��������� ������� � ����������� ������ 
	if ( cmds == NULL )
	{		
		str << "Command manager assigned with port COM" << (int)port << " not found" << std::ends;
		err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
		return false;
	}
	
	if ( !appXML.GetNodeAttributeByName(pNode, "ADDRESS", sAtr) )
	{
		str << "Address must be set for this device type" << std::ends;		
		err.SetLastError(DCE_DEVICE_ADDRESS_NOT_SET, str.str() );
		return false;
	}

	devAdress = atoi( sAtr.c_str() );
	//�� ������� ������� ���������� 
	
	comNo  = port;

	return OnCreate();
}

//������� ������ ������ ADAM
bool ADAM4000::Create(std::string name, unsigned long updateInterval, unsigned char address, unsigned port)
{
	if ( !IdentifiedPhysDevice::Create( name, updateInterval ) )
		return false;
	
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	cmds = CommandManager::GetCommandManager(comNo);
	
	//��������� ������� � ����������� ������ 
	if ( cmds == NULL )
	{		
		str << "Command manager assigned with port COM" << (int)port << " not found" << std::ends;
		err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
		return false;
	}

	devAdress = address;
	comNo = port;

	return OnCreate();
}


//����������� �������
void ADAM4000::AddBeforeCmdSendHandler(boost::function< void (LPSERIAL_COMMAND)	> handler )
{
	evBeforeCmdSend.connect(handler);
}
void ADAM4000::AddAfterCmdRespondHandler(boost::function< void (LPSERIAL_COMMAND) > handler )
{
	evAfterRespond.connect(handler);
}
void ADAM4000::AddCmdFailureHadler(boost::function< void (LPSERIAL_COMMAND) > handler )
{
	evCmdFailure.connect(handler);
}


//��������� ������� ���������� 
bool ADAM4000::SendCommand(char cmdType, std::string strCommand, bool bHighPriority, std::string &strRespond) const
{
	//���������� ��������� �������
	ASSERT(cmds != NULL);

	//��������� �������� �������
	char szCmd[256];
	sprintf_s(szCmd, 256, "%1c%02X%s\r", cmdType, devAdress, strCommand.c_str());

	//��������� �������� �������
	ASCII_COMMAND command;
	strcpy_s(command.cmd, szCmd);
	command.pSender = this;	
	command.length = 0;
	strcpy_s(command.response, "SEND");

	//���������� ��������� �������
	strLastCmd = szCmd;
	
//	evBeforeCmdSend(&command);	// ������� ������������ ����� ������������ ��������	
		
	//���������� ���������� ������
	if ( !cmds->SendCommand(&command, bHighPriority) )
	{
		SERIAL_COMMAND scmd;
		scmd.size = sizeof(ASCII_COMMAND);
		memcpy(&scmd.ascii, &command, sizeof(ASCII_COMMAND));
		
		evCmdFailure(&scmd);
		return false;
	}
	
	strLastRes = strRespond = command.response;

	//	evAfterRespond(&command);	// ������� ������������ ����� ��������� ������ ��������
	
	return true;
}

std::string ADAM4000::ExtractRespond(std::string fullRespond, std::size_t from, int count) const
{
	if ( fullRespond.empty() || fullRespond[0] == '?' || fullRespond[0] == '\r')
		return "";

	std::size_t len = fullRespond.length();

	if ( len  <= from )
		return "";

	if (count == -1)
	{
		std::size_t endpos = fullRespond.find_first_of("\r\n");
		if (endpos < from)
			return "";

		count = endpos - from;
	}

	if (count == 0)
		return "";
	
	std::string res;
	try {

		res = fullRespond.substr(from, count);
		if (res.empty())
			return "";

		if ( res[ res.length() - 1] == '\r' )
			res.erase(res.length() - 1, 1);
	}
	catch(...)
	{
		return "";
	}

	return res;
}
//��������� ������ ��������� �������
const char* ADAM4000::LastCommand() const 
{ 
	return strLastCmd.c_str(); 
}

//��������� ������ ���������� ������
const char* ADAM4000::LastRespond() const 
{ 
	return strLastRes.c_str();
}

//�������� ����� ������
unsigned char ADAM4000::ModuleAddress() const 
{ 
	return  devAdress; 
} 

//�������� ����� ����������������� �����
unsigned char ADAM4000::Port() const 
{ 
	return  comNo; 
} 	

//�������� ��� ������ 
std::string ADAM4000::ModuleName(bool bUpdate /*= false*/,  int len/* = 4*/) const
{
	if ( ! bUpdate )
		return strName;

	std::string respond;

	//������� ��������� ����� ���������� 
	if ( !SendCommand('$', "M", false, respond) )
		return strName = "";
	
	return 	strName = ExtractRespond(respond, 3, len);
}

//�������� ������ �������� ������
std::string ADAM4000::ModuleFirmwareVersion() 
{
	std::string respond;

	//������� ��������� ������ �������� ������
	if ( !SendCommand('$', "F", false, respond) )
		return "";

	return ExtractRespond(respond, 3);
}


/*********************************************/
/*           ������ ADAM4019+                */
/*********************************************/

//����������� ������
ADAM4019P::ADAM4019P() :  ADAM4000("ADAM-4019P"), 
	channelsState(0),
	AI_CHANNELS_COUNT(8)
{
	inputRanges.assign(AI_CHANNELS_COUNT, 0xff);
	lastData.assign(AI_CHANNELS_COUNT, 0.0 );	
}

ADAM4019P::ADAM4019P(std::string model) :
	ADAM4000(model), 
	channelsState(0),
	AI_CHANNELS_COUNT(8)
{
	inputRanges.assign(AI_CHANNELS_COUNT, 0xff);
	lastData.assign(AI_CHANNELS_COUNT, 0.0 );	
}

ADAM4019P::~ADAM4019P(void)
{
	
}

//��������� ������ ����� ���������� ������
//��� ������� ���������� 8 - ������
int ADAM4019P::GetChannelsCountAI() const 
{ 
	return AI_CHANNELS_COUNT; 
}

ADAM4019P* ADAM4019P::CreateFromXML( IXMLDOMNode* pNode, UINT& error )
{
	auto_ptr<ADAM4019P> obj ( new ADAM4019P() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

// bool ADAM4019P::CreateFromXMLNode(IXMLDOMNode* pNode)
// {
// 	return ADAM4000::CreateFromXMLNode(pNode);
// }

//������� ������ ������ ADAM4019+
bool ADAM4019P::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName(true, 5) != "4019P")
	{
		str << "Device attached to COM" << (int)comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< (int)devAdress << " is not " << m_strModel << std::ends;
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );

		return false;
	}
	
	if ( !UpdateChannelState() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< "attached to COM" << (int)comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< (int)devAdress<< std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );

		return false;
	}
	
	UpdateInputRangesInfo();
	
	return true;
}

bool ADAM4019P::EnableChannel(unsigned char ch, bool enable/* = true*/)
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
	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "5%X", newState);
	
	//������� ��������� ��������� ���������� ������
	if ( !SendCommand('$', szCmd, false, respond) )
		return false;
	
	channelsState = newState;
	return true;
}

//��������  ���������� �� ���������� ��������� ������
void ADAM4019P::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	long rngid = inputRanges[channel];
	*lpmci = ranges[ rngid ];
}

//����������  ��������  ����������� �����
bool ADAM4019P::SetInputRange(unsigned char ch, unsigned char range)
{
	std::string respond;
	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "7C%dR%02X", ch, range);

	//������� ��������� ��������� ���������� ������
	if ( !SendCommand('$', szCmd, false, respond) )
		return false;

	return true;
}

//�������� ��� ���������  ���������  ���������
void ADAM4019P::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();

	rngs.push_back(ranges[ 0x02 ]);
	rngs.push_back(ranges[ 0x03 ]);
	rngs.push_back(ranges[ 0x04 ]);
	rngs.push_back(ranges[ 0x05 ]);
	rngs.push_back(ranges[ 0x07 ]);
	rngs.push_back(ranges[ 0x08 ]);
	rngs.push_back(ranges[ 0x09 ]);
	rngs.push_back(ranges[ 0x0D ]);
	rngs.push_back(ranges[ 0x0E ]);
	rngs.push_back(ranges[ 0x0F ]);
	rngs.push_back(ranges[ 0x10 ]);
	rngs.push_back(ranges[ 0x11 ]);
	rngs.push_back(ranges[ 0x12 ]);
	rngs.push_back(ranges[ 0x13 ]);
	rngs.push_back(ranges[ 0x14 ]);	
}


bool ADAM4019P::UpdateChannelState() const
{
	std::string respond;

	//������� ��������� ��������� ���������� ������
	if ( !SendCommand('$', "6", false, respond) )
		return false;

	std::string st1 = ExtractRespond(respond, 3, 2);
	
	if ( st1.empty() || st1.length() != 2)
		return false;

	st1 = "0x" + st1;
	channelsState = unsigned char( strtol(st1.c_str(), NULL, 0) );

	return true;
}


bool ADAM4019P::IsChannelEnabled(unsigned char channel) const
{
	UpdateChannelState();	
	
	unsigned char mask = 1 << channel;
	if ( mask & channelsState )
		return true;

	return false;
}

// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
// (������ �� ������ ������ �� �����������)
double  ADAM4019P::GetChannelData(unsigned char channel)  const
{
	if (channel < 0 || channel >= GetChannelsCountAI())
		return UNKNONW_VALUE;
	
	Lock();
	double val = lastData[channel];
	Unlock();

	return val;
}

void ADAM4019P::ControlProc()
{
	ReadChannels( NULL );
}

//��������� ������ � ������ �� 8 ���������� ������ (����������� ������ �� ������ ������)
//����� ����� ������ ���� ����� 0 � 8
double  ADAM4019P::ReadChannel(unsigned char channel)  const
{
	std::string respond;
	char szCh[4];
	sprintf_s(szCh, 4, "%d", channel);
	
	double val = 0.0;

	if (SendCommand('#', szCh, false, respond))
	{
		std::string data = ExtractRespond(respond, 1);
		val = atof( data.c_str() );
	}
	else
		val = DBL_MAX;

	return val;
}

void ADAM4019P::ReadChannels(double* pdData/* = NULL*/) const
{
	std::string respond;

	
	if (SendCommand('#', "", false, respond) )
	{	
		if (respond.empty() || respond[0] != '>')
			return;

			
		std::string data = ExtractRespond(respond, 1, -1);				
		
		int expected = GetChannelsCountAI() * 7;
		if ( data.length() != expected )
			return;

		int count = (int)data.length() / 7;

		
		Lock();

		for (int i = 0; i < count; i++)
		{
			std::string res = data.substr( i*7, 7 );
			double dVal = atof( res.c_str() );
						
			if ( dVal != -999999.0 && dVal != 999999.0 && dVal != -888888.0 && dVal != 888888.0)
				lastData[i] = dVal;
		}

		if (pdData != NULL)
			memcpy(pdData, lastData.data(), sizeof(double)*GetChannelsCountAI());

		Unlock();
		
	}
}

//�������� ���������� �� ���������� ���������� 
bool ADAM4019P::UpdateInputRangeInfo( unsigned char channel ) const
{
	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "8C%d", channel);

	inputRanges[channel] = 0xff;

	std::string respond;

	if ( !SendCommand('$', szCmd, false, respond) )
		return false;

	std::string rng = ExtractRespond(respond, 6);

	if ( rng.empty() ) {
		inputRanges[channel] = 0xff;
		return false;
	}

	rng = "0x" + rng;				
	inputRanges[channel] = strtol( rng.c_str(), NULL, 0);
	
	return true;
}

void ADAM4019P::UpdateInputRangesInfo( ) const
{	
	for ( int i = 0; i < GetChannelsCountAI(); i++ )
		UpdateInputRangeInfo( (unsigned char)i );	
}

/*********************************************/
/*           ������ ADAM4018+                */
/*********************************************/
ADAM4018P::ADAM4018P() : 
	ADAM4019P("ADAM-4018P"), 
	AI_CHANNELS_COUNT(8)
{	
}

ADAM4018P::~ADAM4018P()
{

}

//��������� ������ ����� ���������� ������
//��� ������� ���������� 8 - ������
int ADAM4018P::GetChannelsCountAI()
{
	return AI_CHANNELS_COUNT;
}

ADAM4018P* ADAM4018P::CreateFromXML( IXMLDOMNode* pNode, UINT& error )
{
	auto_ptr<ADAM4018P> obj ( new ADAM4018P() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

// bool ADAM4018P::CreateFromXMLNode(IXMLDOMNode* pNode)
// {
// 	return ADAM4019P::CreateFromXMLNode(pNode);
// }

bool ADAM4018P::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������	
	if ( ModuleName() != "4018P")
	{
		str << "Device attached to COM" << comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress << " is not " << m_strModel << std::ends;
		
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	if ( !UpdateChannelState() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< "attached to COM" << comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress<< std::ends;

		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	UpdateInputRangesInfo();

	return true;
}

//�������� ��� ���������  ���������  ���������
void ADAM4018P::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{
	rngs.clear();

	rngs.push_back(ranges[ 0x0C ]);
	rngs.push_back(ranges[ 0x0B ]);
	rngs.push_back(ranges[ 0x0A ]);
	rngs.push_back(ranges[ 0x09 ]);
	rngs.push_back(ranges[ 0x08 ]);
	rngs.push_back(ranges[ 0x07 ]);
	rngs.push_back(ranges[ 0x0D ]);	

}

/*********************************************/
/*           ������ ADAM4015                 */
/*********************************************/

ADAM4011::ADAM4011() :
ADAM4019P("ADAM-4011"),
	AI_CHANNELS_COUNT(1)
{	
}

ADAM4011::~ADAM4011()
{

}

//��������� ������ ����� ���������� ������
//��� ������� ���������� 1 - ������
int ADAM4011::GetChannelsCountAI() const
{
	return AI_CHANNELS_COUNT;
}

ADAM4011* ADAM4011::CreateFromXML( IXMLDOMNode* pNode, UINT& error )
{
	auto_ptr<ADAM4011> obj ( new ADAM4011() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

void ADAM4011::ReadChannels(double* pdData/* = NULL*/) const
{
	std::string respond;
	
	if (SendCommand('#', "", false, respond) )
	{	
		if (respond.empty() || respond[0] != '>')
			return;
		
		std::string data = ExtractRespond(respond, 1);				

		Lock();

		double dVal = atof( data.c_str() );
		lastData[0] = dVal;		

		if (pdData != NULL)
			memcpy(pdData, lastData.data(), sizeof(double)*GetChannelsCountAI());

		Unlock();

	}
}

bool ADAM4011::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4011")
	{
		str << "Device attached to COM" << comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress << " is not " << m_strModel << std::ends;

		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	UpdateInputRangesInfo();

	return true;
}


//�������� ��� ���������  ���������  ���������
void ADAM4011::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
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

//�������� ���������� �� ���������� ���������� 
bool ADAM4011::UpdateInputRangeInfo( unsigned char channel ) const
{
	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "%d", 2);

	std::string respond;

	if ( !SendCommand('$', szCmd, false, respond) )
		return false;

	std::string rng = ExtractRespond(respond, 0, 2);

	if ( rng.empty() ) {
		inputRanges[channel] = 0xff;
		return false;
	}

	rng = "0x" + rng;				
	inputRanges[channel] = strtol( rng.c_str(), NULL, 0);

	return true;
}

/*********************************************/
/*           ������ ADAM4015                 */
/*********************************************/

ADAM4015::ADAM4015() :
	ADAM4019P("ADAM-4015"),
	AI_CHANNELS_COUNT(6)
{	
}

ADAM4015::~ADAM4015()
{

}

//��������� ������ ����� ���������� ������
//��� ������� ���������� 6 - ������
int ADAM4015::GetChannelsCountAI() const
{
	return AI_CHANNELS_COUNT;
}

ADAM4015* ADAM4015::CreateFromXML( IXMLDOMNode* pNode, UINT& error )
{
	auto_ptr<ADAM4015> obj ( new ADAM4015() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM4015::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4015")
	{
		str << "Device attached to COM" << comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress << " is not " << m_strModel << std::ends;
		
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	if ( !UpdateChannelState() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< "attached to COM" << comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress<< std::ends;

		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	UpdateInputRangesInfo();

	return true;
}



/*******************************************************/
/*                     ������ ADAM4521                 */
/*******************************************************/
ADAM4521::ADAM4521():
delim(char(255))
{	
	 m_strModel = "ADAM-4521";
}

ADAM4521::~ADAM4521()
{

}

ADAM4521* ADAM4521::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM4521> obj ( new ADAM4521() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM4521::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4521")
	{
		str << "Device attached to COM" << (int)comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< (int)devAdress << " is not " << m_strModel << std::ends;
		
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	if ( GetDelimiter() == char(255) )
	{
		str << "Unable get delimiter character for module " << m_strModel 
			<< "attached to COM" << comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress<< std::ends;
		
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false; 
	}

	return true;
}

bool ADAM4521::SetID(std::string strID) const
{
	std::string respond;
	char szCmd[64];

	sprintf_s(szCmd, 64, "%d%s", 6, strID.c_str());

	if ( SendCommand('$', szCmd, false, respond) )
		return true;
	else
		return false;
}

std::string ADAM4521::GetID() const
{
	std::string respond;

	if ( SendCommand('$', "7", false, respond))
		strID = ExtractRespond(respond, 3);

	return strID;
}

char ADAM4521::GetDelimiter() const
{
	std::string respond;	

	if ( SendCommand('$', "D", false, respond) )
		delim = respond[3];

	return delim;
}

std::string ADAM4521::DataPass(const IdentifiedPhysDevice* pSender, std::string outerCmd, bool bHigthPriority) const
{
	std::string respond;

	SendCommand(delim, outerCmd, bHigthPriority, respond);

	return respond;
}

void ADAM4521::ControlProc()
{

}

//////////////////////////////////////////////////////////////////////////
//  ���������� ������   ADAM4024
//////////////////////////////////////////////////////////////////////////

ADAM4024::ADAM4024() : ADAM4000("ADAM-4024"),
	OUTPUT_ANALOG_CHANNELS_COUNT(4),
	INPUT_DIGITAL_CHANNELS_COUNT(4),
	stateDI(0)
{
	lastValues.assign(OUTPUT_ANALOG_CHANNELS_COUNT, 0.0);
	outputRanges.assign(OUTPUT_ANALOG_CHANNELS_COUNT, 0xFF);
}

ADAM4024::~ADAM4024()
{
	
}

//��������� ������ ����� ���������� ������� 
int ADAM4024::GetChannelsCountAO() const 
{ 
	return OUTPUT_ANALOG_CHANNELS_COUNT; 
}

int ADAM4024::GetChannelsCountDI() const 
{ 
	return INPUT_DIGITAL_CHANNELS_COUNT; 
} 

ADAM4024* ADAM4024::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM4024> obj ( new ADAM4024() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM4024::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4024")
	{
		str << "Device attached to COM" << (int)comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< (int)devAdress << " is not " << m_strModel << std::ends;
		
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	for (unsigned char ch = 0; ch < GetChannelsCountAO(); ch++)
	{
		std::string respond;
		char szCmd[MAX_CMD_LEN];
		
		sprintf_s(szCmd, MAX_CMD_LEN, "6C%d", ch);

		if ( !SendCommand('$', szCmd, false, respond) )
			lastValues[ch] = DBL_MIN;
		else
			lastValues[ch] = atof(ExtractRespond(respond, 3).c_str());		
	}

	UpdateOutputRangesInfo( );

	return true;
}

void ADAM4024::ControlProc() 
{	
	UpdateChannelsStateDI();
}

/////////////////////////////////////
//���������� ���������� IDigitalInput
bool ADAM4024::GetChannelStateDI(unsigned char �hannel, bool update) const
{
	unsigned char msk = 1 << �hannel;

	::EnterCriticalSection(&m_cs);

	if ( update )
		UpdateChannelsStateDI();

	bool bEnable = (stateDI & msk) ? true : false;

	::LeaveCriticalSection(&m_cs);

	return bEnable;
}

//�������� ��������� �������� ������
bool ADAM4024::UpdateChannelsStateDI() const
{
	std::string respond;

	//������� ��������� ��������� �������� ������
	if ( SendCommand('$', "I", false, respond) )
	{
		std::string st = ExtractRespond(respond, 3);
		if ( !st.empty() )
		{
			st = "0x" + st;
			stateDI = unsigned char(strtol(st.c_str(), NULL, 0));
		}
	}
	else
		return false;

	return true;
}

//********************************************************

//�������� ������ � ���������� ����� 
double  ADAM4024::WriteToChannel(unsigned char ch, double val)
{
	std::string respond;
	char szCmd[MAX_CMD_LEN];

	sprintf_s(szCmd, MAX_CMD_LEN, "C%d%+07.3f", ch, val);

	if ( !SendCommand('#', szCmd, true, respond) )
		return DBL_MIN;	
	 
	return lastValues[ch] = val;
}

//�������� ������ �� ��� ������
void ADAM4024::WriteToAllChannels(const std::vector<double>& vals) 
{ 
	int ch = 0;
	for (std::vector<double>::const_iterator it = vals.begin(); it != vals.end();  ++it )
		WriteToChannel( ch++, (*it) );
}

//�������� ��������� ���������� � ���������� ����� �������� 
double ADAM4024::GetChannelStateAO (unsigned char ch) const
{
	return lastValues[ch];	
}

//�������� �������� �������� ������ 
void ADAM4024::GetMinMax(unsigned char channel, double& minOut, double& maxOut) const
{
	
	MEASSURE_RANGE_INFO msri;
	GetOutputRangeInfo(channel, &msri);

	minOut = msri.mins;
	maxOut = msri.maxs;
}

//��������  ���������� �� ��������� ������
void ADAM4024::GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmsri) const
{
	long rngid = outputRanges[channel];
	*lpmsri = ranges[ rngid ];
}

//����������  ��������  ����������� ������
bool ADAM4024::SetOutputRange(unsigned char ch, unsigned char range)
{
	std::string respond;
	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "7C%dR%X", ch, range);

	if ( !SendCommand('$', szCmd,  false, respond) )
		return false;

	return true;
}

//�������� ��� ���������  ���������  
void ADAM4024::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{
	rngs.clear();
	rngs.push_back( ranges[0x30] );
	rngs.push_back( ranges[0x31] );
	rngs.push_back( ranges[0x32] );
}


//�������� ���������� �� ��������� ������
bool ADAM4024::UpdateOutputRangeInfo( unsigned char channel ) const
{
	std::string respond;
	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "8C%d", channel);

	outputRanges[channel] = 0xFF;

	if ( !SendCommand('$', szCmd,  false, respond) )
		return false;
	
	std::string range = ExtractRespond(respond, 5);
	if (range.empty() || range.length() != 2)
		return false;
	
	range = "0x" + range;
	outputRanges[channel] = LOBYTE( strtol(range.c_str(), 0, 0) );

	return true;
}

//�������� ���������� � ���������� 
void ADAM4024::UpdateOutputRangesInfo( ) const
{
	for (int i=0; i<GetChannelsCountAO(); i++)
		UpdateOutputRangeInfo( (unsigned char) i );
}


//������� ���������� ��������� ��� ������ ������
bool ADAM4024::SetStartupValue(unsigned char channel, double val) const
{
	std::string respond;
	char szCmd[MAX_CMD_LEN];

	sprintf_s(szCmd, MAX_CMD_LEN, "SC%d%+07.3f", channel, val);

	if ( !SendCommand('#', szCmd, false, respond) )
		return false;

	return true;
}

//������� ���������� ��������� ��� ���� ������
bool ADAM4024::SetStartupValue(double* vals) const
{
	for (int ch = 0; ch < OUTPUT_ANALOG_CHANNELS_COUNT; ch++)
		SetStartupValue(ch, vals == NULL ? 0.0 : vals[ch]);

	return true;
}

//������� ���������� ��������� ��� ������ ������ 
bool ADAM4024::SetEmergencyValue(unsigned char channel, double val) const
{
	std::string respond;
	char szCmd[MAX_CMD_LEN];

	sprintf_s(szCmd, MAX_CMD_LEN, "EC%d%+07.3f", channel, val);

	if ( !SendCommand('#', szCmd, false, respond) )
		return false;

	return true;
}
//������� ���������� ��������� ��� ���� ������� 
bool ADAM4024::SetEmergencyValue(double* vals) const
{
	for (unsigned char ch = 0; ch < OUTPUT_ANALOG_CHANNELS_COUNT; ch++)
		SetEmergencyValue(ch, vals == NULL ? 0.0 : vals[ch]);
	
	return true;
}

//������������ ����� ��� ������������ ���������� ��������� ��� ������ ������ 
bool ADAM4024::SetEmergencyFlag(unsigned char channel, bool enable) const
{
	std::string respond;
	char szCmd[MAX_CMD_LEN];

	sprintf_s(szCmd, MAX_CMD_LEN, "AC%d%d", channel, enable);

	if ( !SendCommand('$', szCmd, false,respond) )
		return false;

	return true;
}

//��������� ����� ��� ������������ ���������� ��������� ��� ������ ������ 
bool ADAM4024::GetEmergencyFlag(unsigned char channel) const
{
	std::string respond;
	char szCmd[MAX_CMD_LEN];
	
	sprintf_s(szCmd, MAX_CMD_LEN, "BC%d", channel);

	if ( !SendCommand('$', szCmd, false, respond) )
		return false;

	std::string st = ExtractRespond(respond, 5);
	
	return atoi(st.c_str()) == 1;
}



/************************************************************************/
/*      ADAM4050                                                        */
/************************************************************************/

ADAM4050::ADAM4050() :
	OUTPUT_DIGITAL_CHANNELS_COUNT(8),
	INPUT_DIGITAL_CHANNELS_COUNT(7)
{
	m_strModel = "ADAM-4050";
}

ADAM4050::~ADAM4050()
{
}

//�������� ����� ���������� ������
int ADAM4050::GetChannelsCountDI() const 
{ 
	return INPUT_DIGITAL_CHANNELS_COUNT; 
};

//��������� ������ ����� �������� ������� 
int ADAM4050::GetChannelsCountDO() const 
{ 
	return OUTPUT_DIGITAL_CHANNELS_COUNT; 
};

ADAM4050* ADAM4050::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM4050> obj ( new ADAM4050() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM4050::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4050")
	{
		str << "Device attached to COM" << (int)comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< (int)devAdress << " is not " << m_strModel << std::ends;
		
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	
	if ( !UpdateChannelsStateDI() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< "attached to COM" << (int)comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< (int)devAdress<< std::ends;
		
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	return true;
}

void ADAM4050::ControlProc() 
{ 
	UpdateChannelsStateDI(); 
};

//////////////////////////////////////////////////////////////////////////
/// ���������� ���������� IDigitalInput


//�������� ��������� ���������� ������
bool ADAM4050::UpdateChannelsStateDI() const
{
	std::string respond;

	//������� ��������� ��������� �������� ������ � �������
	if ( SendCommand('$', "6", false, respond) )
	{
		std::string stDI = "0x" + ExtractRespond(respond, 3, 2);
		std::string stDO = "0x" + ExtractRespond(respond, 1, 2);

		unsigned char newStateDI = unsigned char(strtol(stDI.c_str(), NULL, 0)); //��������� �������� ������
		unsigned char newStateDO = unsigned char(strtol(stDO.c_str(), NULL, 0)); //��������� �������� �������

		//����������� ��������� ��������� 
		if ( newStateDI != stateDI || newStateDO != stateDO )
		{
			if ( SendCommand('$', "6", false, respond) )
			{
				stDI = "0x" + ExtractRespond(respond, 3, 2);
				stDO = "0x" + ExtractRespond(respond, 1, 2);
				
				newStateDI = unsigned char(strtol(stDI.c_str(), NULL, 0)); //��������� �������� ������
				newStateDO = unsigned char(strtol(stDO.c_str(), NULL, 0)); //��������� �������� �������
			}
		}

		::EnterCriticalSection(&m_cs);
		
		stateDI = newStateDI;
		stateDO = newStateDO;

		::LeaveCriticalSection(&m_cs);
	}
	else
		return false;

	return true;
}

//�������� ��������� ��������� ������
bool ADAM4050::GetChannelStateDI(unsigned char �hannel, bool update) const 
{
	unsigned char msk = 1 << �hannel;
	
	::EnterCriticalSection(&m_cs);

	if ( update )
		UpdateChannelsStateDI();

	bool bEnable = (stateDI & msk) == msk; 

	::LeaveCriticalSection(&m_cs);
	
	return bEnable;
}

//////////////////////////////////////////////////////////////////////////
/// ���������� ���������� IDigitalOutput

//�������� ������� ��������� ���������� �������	
bool ADAM4050::UpdateChannelsStateDO() const
{
	//������� ��������� ��������� � �������� ������ � �������
	return UpdateChannelsStateDI();
}

//�������� ��������� ��������� ������
bool ADAM4050::GetChannelStateDO(unsigned char channel, bool update) const
{
	unsigned char msk = 1 << channel;

	::EnterCriticalSection(&m_cs);

	if ( update )
		UpdateChannelsStateDO();

	bool bEnable = (stateDO & msk) == msk;

	::LeaveCriticalSection(&m_cs);

	return bEnable;
}

//���������� ��������� ��������� ������ 
bool  ADAM4050::SetChannelStateDO(unsigned char channel, bool enable)
{
	bool bOldState;

	unsigned char msk = 1 << channel;
	unsigned char stateNew = 0;
	
	::EnterCriticalSection(&m_cs);
	
	bOldState = GetChannelStateDO(channel, true);

	if ( enable ) 
		stateNew = stateDO | msk;
	else
		stateNew = stateDO & ~msk;

	::LeaveCriticalSection(&m_cs);

	SetChannelsStateDO(stateNew);

	return bOldState;
}

//�������� ������ �� ��� �������� ������
void ADAM4050::SetChannelsStateDO(unsigned char val)
{
	std::string respond;

	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "00%02X",  val);

	//������� ��������� ��������� �������� �������
	if ( SendCommand('#', szCmd, true, respond) )
	{

	}
}

//�������� ��������� �������� �������
unsigned char ADAM4050::GetChannelsStateDO()
{
	UpdateChannelsStateDO();
	return stateDO;	//��������� �������� ������	
}

//�������� ��������� �������� �������
unsigned char ADAM4050::GetChannelsStateDI()
{
	UpdateChannelsStateDI();
	return stateDI;	
}


//////////////////////////////////////////////////////////////////////////
//
//�����������
ADAM4053::ADAM4053() : 
ADAM4000("ADAM-4053"), 
INPUT_DIGITAL_CHANNELS_COUNT(16)
{

}

//����������
ADAM4053::~ADAM4053() 
{

}

ADAM4053* ADAM4053::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM4053> obj ( new ADAM4053() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

///////////////////////////////////////////////////
//���������� ���������� IDigitalInput

//�������� ����� ���������� ������
int ADAM4053::GetChannelsCountDI() const 
{ 
	return INPUT_DIGITAL_CHANNELS_COUNT; 
};


bool ADAM4053::GetStateDI(std::bitset<16>& state) const
{
	std::string respond;
	
	try {
		//������� ��������� ��������� �������� ������ � �������
		if ( SendCommand('$', "6", false, respond) )
		{
			std::string stDI = "0x" + ExtractRespond(respond, 1, 4);
			state = (long)boost::lexical_cast< hex_to<long> >( stDI.c_str() );
		}
	}
	catch(boost::bad_lexical_cast& e)
	{
		e;
		return false;
	}

	return true;
}

//�������� ��������� ���������� ������
bool ADAM4053::UpdateChannelsStateDI() const
{	
	//�������� ��������� ������
	std::bitset<16> stateNew;
	
	if ( !GetStateDI(stateNew) )
		return false;

	Lock();
	
	//��������� ��������� ��������� �������
	std::bitset<16> mask = stateNew ^ stateDI;

	//���� ���������� ��������� ������ � ���������� ����
	//������������� ��������� ��������� ������
	if ( mask.any() && changesConfirm )
	{
		 if ( !GetStateDI(stateNew) )
		 {
			 Unlock();
			 return false;
		 }
	}

	const_cast<ADAM4053*>(this)->stateDI = stateNew;

	Unlock();

	return true;
}

//��������� ��������� ������ ����������� �����
bool ADAM4053::GetChannelStateDI(unsigned char ch, bool update) const
{
	if ( update )
		UpdateChannelsStateDI();

	Lock();
	
	bool b = stateDI[ch];

	Unlock();

	return b;
}

bool ADAM4053::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4053")
	{
		str << "Device attached to COM" << comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress << " is not " << m_strModel << std::ends;

		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	if ( !UpdateChannelsStateDI() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< "attached to COM" << comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress<< std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	return true;
}

void ADAM4053::ControlProc()
{
	UpdateChannelsStateDI();
}

//////////////////////////////////////////////////////////////////////////
//
//�����������
ADAM4052::ADAM4052() : 
ADAM4000("ADAM-4052"), 
	INPUT_DIGITAL_CHANNELS_COUNT(8)
{

}

//����������
ADAM4052::~ADAM4052() 
{

}

ADAM4052* ADAM4052::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM4052> obj ( new ADAM4052() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

///////////////////////////////////////////////////
//���������� ���������� IDigitalInput

//�������� ����� ���������� ������
int ADAM4052::GetChannelsCountDI() const 
{ 
	return INPUT_DIGITAL_CHANNELS_COUNT; 
};


bool ADAM4052::GetStateDI(std::bitset<8>& state) const
{
	std::string respond;

	try {
		//������� ��������� ��������� �������� ������ � �������
		if ( SendCommand('$', "6", false, respond) )
		{
			std::string stDI = "0x" + ExtractRespond(respond, 1, 2);
			state = (long)boost::lexical_cast< hex_to<long> >( stDI.c_str() );
		}
	}
	catch(boost::bad_lexical_cast& e)
	{
		e;
		return false;
	}

	return true;
}

//�������� ��������� ���������� ������
bool ADAM4052::UpdateChannelsStateDI() const
{	
	//�������� ��������� ������
	std::bitset<8> stateNew;

	if ( !GetStateDI(stateNew) )
		return false;

	Lock();

	//��������� ��������� ��������� �������
	std::bitset<8> mask = stateNew ^ stateDI;

	//���� ���������� ��������� ������ � ���������� ����
	//������������� ��������� ��������� ������
	if ( mask.any() && changesConfirm )
	{
		if ( !GetStateDI(stateNew) )
		{
			Unlock();
			return false;
		}
	}

	const_cast<ADAM4052*>(this)->stateDI = stateNew;

	Unlock();

	return true;
}

//��������� ��������� ������ ����������� �����
bool ADAM4052::GetChannelStateDI(unsigned char ch, bool update) const
{
	if ( update )
		UpdateChannelsStateDI();

	Lock();

	bool b = stateDI[ch];

	Unlock();

	return b;
}

bool ADAM4052::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4052")
	{
		str << "Device attached to COM" << comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress << " is not " << m_strModel << std::ends;

		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	if ( !UpdateChannelsStateDI() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< "attached to COM" << comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress<< std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	return true;
}

void ADAM4052::ControlProc()
{
	UpdateChannelsStateDI();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
ADAM4069::ADAM4069() :
	OUTPUT_DIGITAL_CHANNELS_COUNT_4069(8)
{
	m_strModel = "ADAM-4069";
}

ADAM4069::~ADAM4069()
{

}
//��������� ������ ����� �������� ������� 
int ADAM4069::GetChannelsCountDO() const 
{ 
	return OUTPUT_DIGITAL_CHANNELS_COUNT_4069; 
}

ADAM4069* ADAM4069::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM4069> obj ( new ADAM4069() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM4069::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4069")
	{
		str << "Device attached to COM" << comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress << " is not " << m_strModel << std::ends;
		
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	if ( !UpdateChannelsStateDO() )
	{
		str << "Unable to update channels state for module " << m_strModel 
			<< "attached to COM" << comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress<< std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}
	
	return true;
}

void ADAM4069::ControlProc()
{
	UpdateChannelsStateDO();
}

//�������� ������� ��������� ���������� �������	
bool ADAM4069::UpdateChannelsStateDO() const
{
	std::string respond;

	//������� ��������� ��������� �������� ������ � �������
	if ( SendCommand('$', "6", false, respond) )
	{
		if ( respond == "FAILURE" )
			return false;

		std::string stDO = "0x" + ExtractRespond(respond, 1, 2);

		::EnterCriticalSection(&m_cs);

		stateDO = unsigned char(strtol(stDO.c_str(), NULL, 0)); //��������� �������� �������

		::LeaveCriticalSection(&m_cs);
	}
	else
		return false;

	return true;
}

//�������� ��������� ��������� ������
bool ADAM4069::GetChannelStateDO(unsigned char channel, bool update) const
{
	unsigned char msk = 1 << channel;

	::EnterCriticalSection(&m_cs);
	
	if ( update )
		UpdateChannelsStateDO();

	bool bEnable = (stateDO & msk) ? true : false;

	::LeaveCriticalSection(&m_cs);

	return bEnable;
}

//���������� ��������� ��������� ������ 
bool  ADAM4069::SetChannelStateDO(unsigned char channel, bool enable)
{
	//bool bOldState = GetChannelStateDO(channel, true);

	unsigned char msk = 1 << channel;
	unsigned char stateNew = 0;

	::EnterCriticalSection(&m_cs);

	if ( enable ) 
		stateNew = stateDO | msk;
	else
		stateNew = stateDO & ~msk;

	::LeaveCriticalSection(&m_cs);

	//if ( stateNew != stateDO )
		SetChannelsStateDO(stateNew);

	return enable;
}

//�������� ������ �� ��� ������
void ADAM4069::SetChannelsStateDO(unsigned char val)
{
	std::string respond;

	char szCmd[MAX_CMD_LEN];
	sprintf_s(szCmd, MAX_CMD_LEN, "00%02X",  val);

	//������� ��������� ��������� �������� �������
	if ( SendCommand('#', szCmd, true, respond) )
	{
		UpdateChannelsStateDO();
	}
}

//��������� ��������� �������� �������
/*
void ADAM4069::SaveChannelsStateDO() const
{
	::EnterCriticalSection(&m_cs);

	stateStoreDO = stateDO;

	::LeaveCriticalSection(&m_cs);
	
}

//������������ ��������� �������� ������� 
void ADAM4069::RestoreChannelsStateDO() const
{
	SetChannelsStateDO( stateStoreDO );
}

*/

//�����������
ADAM4068::ADAM4068() :
OUTPUT_DIGITAL_CHANNELS_COUNT_4068(8)
{
	m_strModel = "ADAM-4068";
}

//����������
ADAM4068::~ADAM4068()
{
	
}

//��������� ������ ����� �������� ������� 
int ADAM4068::GetChannelsCountDO() const 
{ 
	return OUTPUT_DIGITAL_CHANNELS_COUNT_4068; 
};

ADAM4068* ADAM4068::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM4068> obj ( new ADAM4068() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool ADAM4068::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4068")
	{
		str << "Device attached to COM" << comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress << " is not " << m_strModel << std::ends;
		
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );
		return false;
	}

	if ( !UpdateChannelsStateDO() )
	{	
		str << "Unable to update channels state for module " << m_strModel 
			<< "attached to COM" << comNo 
			<< " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< devAdress<< std::ends;
		
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	return true;
}


//�����������
ADAM4080::ADAM4080() : ADAM4000("ADAM-4080"), INPUT_ANALOG_CHANNELS_COUNT_4080(2)
{
	dataBuffer.assign(INPUT_ANALOG_CHANNELS_COUNT_4080, 0.0);
}

//����������
ADAM4080::~ADAM4080()
{

}


ADAM4080* ADAM4080::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ADAM4080> obj ( new ADAM4080() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();

}

bool ADAM4080::OnCreate()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	//��� ������ � ������� devAdress �� ������������� ����������
	if ( ModuleName() != "4080")
	{
		str << "Device attached to COM" << (int)comNo << " at address 0x" << std::setw(2) << std::setfill('0') << std::hex  
			<< (int)devAdress << " is not " << m_strModel << std::ends;
		err.SetLastError(DCE_ANOTHER_MODULE_TYPE, str.str() );

		return false;
	}
	
	ReadChannels();

	return true;
}

void ADAM4080::ControlProc()
{
	ReadChannels();
}
//////////////////////////////////////////////////////////////////////////
//���������� ������� ���������� IAnalogInput

//��������� ������ ����� ���������� ������
int ADAM4080::GetChannelsCountAI() const 
{ 
	return INPUT_ANALOG_CHANNELS_COUNT_4080; 
};

//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
void ADAM4080::ReadChannels(double* data/*  = NULL*/) const
{
	for (unsigned char ch = 0; ch < INPUT_ANALOG_CHANNELS_COUNT_4080; ch++)
	{
		ReadChannel(ch);
	}

	if (data != NULL)
		memcpy(data, dataBuffer.data(), sizeof(double)*GetChannelsCountAI());

}

//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
double  ADAM4080::ReadChannel(unsigned char ch)  const
{
	//��������� �������� �������
	std::strstream str;
	str << (unsigned int)ch << ends;

	std::string respond;
	SendCommand('#', str.str(), false, respond);

	std::string data = "0x" + ExtractRespond(respond, 1);

	//����� ������ 8 ��������
	if ( data.length() != 10 )
		return UNKNONW_VALUE;

	//str << "0x"<< data << std::ends;
	//TRACE("\n\t%s", data.c_str() );
	Lock();

	double val = (double)boost::lexical_cast< hex_to<long> >(data.c_str()); 
	const_cast<ADAM4080*>(this)->dataBuffer[ch] = val;

	Unlock();

	return val;
}

// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
// (������ �� ������ ������ �� �����������)
double  ADAM4080::GetChannelData(unsigned char ch)  const
{
	double val;
	Lock();
	val = const_cast<ADAM4080*>(this)->dataBuffer[ch];
	Unlock();

	return dataBuffer[ch];
}

//��������  ���������� �� ���������� ��������� ������
void ADAM4080::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{

}

//����������  ��������  ����������� �����
bool ADAM4080::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  ���������
void ADAM4080::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{

}



	
