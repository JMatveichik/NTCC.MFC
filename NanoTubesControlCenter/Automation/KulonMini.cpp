#include "StdAfx.h"
#include "KulonMini.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

KulonMini::KulonMini() : IdentifiedPhysDevice("RUMICONT", "Kulon Mini") ,
	INPUT_ANALOG_CHANNELS_COUNT(4), 
	INPUT_DIGITAL_CHANNELS_COUNT(7),
	OUTPUT_ANALOG_CHANNELS_COUNT(3),
	OUTPUT_DIGITAL_CHANNELS_COUNT(4)
{
	dataAI.assign(INPUT_ANALOG_CHANNELS_COUNT, 0.0);
	dataAO.assign(OUTPUT_ANALOG_CHANNELS_COUNT, 0.0);
	dataDO.assign(OUTPUT_DIGITAL_CHANNELS_COUNT, false);

	::InitializeCriticalSection(&m_ModBusCS);
}

KulonMini::~KulonMini(void)
{
	::DeleteCriticalSection(&m_ModBusCS);
}

KulonMini* KulonMini::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<KulonMini> obj ( new KulonMini() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();	
}

bool KulonMini::CreateFromXMLNode(IXMLDOMNode* pNode)
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

bool KulonMini::OnPrepareDevice()
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return false;

	//���������� �������������� ���������� ����������������� �����
	DCB dcb; 
	pMan->GetComState( dcb );

	dcb.ByteSize = 8;		//����� ����� ������
	dcb.fParity = TRUE;	//�������� ��������
	dcb.StopBits = ONESTOPBIT; // ����� �������� �����
	dcb.Parity = EVENPARITY; //�������� ��������

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

	//���������� ���������� �� ���������� ����������
	BuildMeassureRangeInfo();

	//���������� ���������� �� ������ ���������
	BuildRegestersMaps();

	//��������� ���������� � ���������� ������ ��������
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

		dataAO[ch] = val;
	}

	
	//��������� ��������� �������� �������
	UpdateChannelsStateDO();

	//
	UpdateChannelsStateDI();

	return true;
}


//������� �������� ���������� ���������� ����� �������� �������� �������
void KulonMini::ControlProc()
{
	ReadChannels(NULL);
	UpdateChannelsStateDI();
}



//��������� ������ ����� ���������� ������
int KulonMini::GetChannelsCountAI() const 
{
	return INPUT_ANALOG_CHANNELS_COUNT; 
}

//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
void KulonMini::ReadChannels(double* channelsData/*  = NULL*/) const
{
	/*CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return;

	::EnterCriticalSection(&m_ModBusCS);

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender = this;
	cmd.slaveAddress = devAddres;
	cmd.func = 0x03;
	cmd.regCount = 1;
	cmd.regAddress = 210;

	if ( !pMan->SendCommand(&cmd, false) )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return;
	}*/
	
	for (int i = 0; i < INPUT_ANALOG_CHANNELS_COUNT; i++)
	{
		dataAI[i] = ReadChannel(i);

		if (channelsData != NULL)
			channelsData[i] = dataAI[i];
	}

// 	int i = 0;
// 	for (auto it = dataAI.begin(); it != dataAI.end(); ++it)
// 	{
// 		double val = cmd.response[i];
// 		(*it) = val;
// 
// 		if (channelsData != NULL)
// 			channelsData[i] = dataAI[i];
// 
// 		i++;
// 	}
// 	
// 
// 	
// 	::LeaveCriticalSection(&m_ModBusCS);
}

//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
double  KulonMini::ReadChannel(unsigned char ch)  const
{

	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return GetChannelData(ch);

	::EnterCriticalSection(&m_ModBusCS);

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender = this;
	cmd.slaveAddress = devAddres;
	cmd.func = 0x03;
	cmd.regCount = 1;
	cmd.regAddress = 210 + ch;

	if ( !pMan->SendCommand(&cmd, false) )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return GetChannelData(ch);
	}

	double val = cmd.response[0];

	::LeaveCriticalSection(&m_ModBusCS);
	return val;
}

// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
// (������ �� ������ ������ �� �����������)
double  KulonMini::GetChannelData(unsigned char ch)  const
{
	double val;
	::EnterCriticalSection(&m_ModBusCS);

	val = dataAI[ch];

	::LeaveCriticalSection(&m_ModBusCS);
	return val;
}

//��������  ���������� �� ���������� ��������� ������
void KulonMini::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci = mrciIn[ch];	
}

//����������  ��������  ����������� �����
bool KulonMini::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  ���������
void KulonMini::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	rngs.push_back(mrciIn[ch]);
}

/////////////////////////////////////
//���������� ���������� IDigitalInput
int KulonMini::GetChannelsCountDI() const 
{ 
	return INPUT_DIGITAL_CHANNELS_COUNT; 
}

bool KulonMini::GetChannelStateDI(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDI();

	int mask = 1 << ch;
	bool chState = ((curStatus & mask) == mask);

	return chState;
}

bool KulonMini::UpdateChannelsStateDI() const
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
	cmd.regAddress = 220;

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
//���������� ���������� IAnalogOutput
//��������� ������ ����� ���������� ������� 
int KulonMini::GetChannelsCountAO() const
{
	return OUTPUT_ANALOG_CHANNELS_COUNT;
}

//�������� ������ � ���������� ����� 
double  KulonMini::WriteToChannel(unsigned char ch, double val)
{
	
	::EnterCriticalSection(&m_ModBusCS);

	if (val == dataAO[ch])
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return val;
	}	

	unsigned short newVal = (unsigned short) val;

	double oldVal = dataAO[ch];

	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan != NULL )
	{
		MODBUS_SERIAL_COMMAND cmd;
		cmd.pSender = this;
		cmd.slaveAddress = devAddres;
		cmd.func = 0x06;
		cmd.regCount = 1;

		//������ ����� �����������
		cmd.regAddress = aoChannelsMap[ch];
		cmd.response[0] = newVal;

		//��������� ����� �������� �������� �����������
		if ( !pMan->SendCommand(&cmd, true) )

		{
			::LeaveCriticalSection(&m_ModBusCS);
			return oldVal;

		}

		dataAO[ch] = val;
	}

	::LeaveCriticalSection(&m_ModBusCS);
	return oldVal;
}

//�������� ������ �� ��� ������
void KulonMini::WriteToAllChannels(const std::vector<double>& vals)
{
	//������ �� ������
}

//�������� ��������� ���������� � ���������� ����� �������� 
double KulonMini::GetChannelStateAO (unsigned char ch) const
{
	double val;
	::EnterCriticalSection(&m_ModBusCS);

	val = dataAO[ch];

	::LeaveCriticalSection(&m_ModBusCS);
	return val;
}

//�������� �������� �������� ������ 
void KulonMini::GetMinMax(unsigned char ch, double& minOut, double& maxOut) const
{
	minOut = mrciOut[ch].mins;
	maxOut = mrciOut[ch].maxs;
}

//��������  ���������� �� ��������� ������
void KulonMini::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci = mrciOut[ch];
}

//����������  ��������  ����������� ������
bool KulonMini::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  
void KulonMini::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{
	rngs.clear();
	rngs.push_back(mrciOut[ch]);

}


/////////////////////////////////////
//���������� ���������� IDigitalOutput
//��������� ������ ����� �������� ������� 
int KulonMini::GetChannelsCountDO() const
{
	return OUTPUT_DIGITAL_CHANNELS_COUNT;
}

//�������� ������� ��������� ���������� �������	
bool KulonMini::UpdateChannelsStateDO() const
{
	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	::EnterCriticalSection(&m_ModBusCS);

	if ( pMan == NULL )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return false;
	}

	for (auto it = doRegs.begin(); it != doRegs.end(); ++it )
	{
		MODBUS_SERIAL_COMMAND cmd;
		cmd.pSender = this;
		cmd.slaveAddress = devAddres;
		cmd.func = 0x03;
		cmd.regCount = 1;
		cmd.regAddress = (*it).first;

		if ( !pMan->SendCommand(&cmd, false) )
			continue;

		(*it).second = cmd.response[0];
	}

	for (auto it = doChannelsMap.begin(); it != doChannelsMap.end(); ++it )
	{	
		unsigned short reg = doRegs[ (*it).second.reg ];

		unsigned int mask = 1 << (*it).second.bit;				
		bool chState = ((reg & mask) == mask);

		dataDO[(*it).first] = chState;
	}

	::LeaveCriticalSection(&m_ModBusCS);
	return true;
}

//�������� ��������� ��������� ������
bool KulonMini::GetChannelStateDO(unsigned char channel, bool update) const
{
	if (update)
		UpdateChannelsStateDO();
	
	bool st;
	::EnterCriticalSection(&m_ModBusCS);

	st = dataDO[channel];
	
	::LeaveCriticalSection(&m_ModBusCS);

	return st;
}

//���������� ��������� ��������� ������ 
bool  KulonMini::SetChannelStateDO(unsigned char ch, bool enable)
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
	cmd.func = 0x06;
	cmd.regCount = 1;

	DO_FULL_ADRESS dofa  = doChannelsMap[ch];
	cmd.regAddress = dofa.reg;
	
	unsigned short reg = doRegs[ dofa.reg ];
	unsigned short mask = 1 << dofa.bit;

	unsigned short stateNew = 0;

	//����������� � ����� ���������
	if ( enable ) 
		stateNew = reg | mask;
	else
		stateNew = reg & ~mask;
	
	cmd.response[0] = stateNew;

	if ( !pMan->SendCommand(&cmd, true) )
	{
		::LeaveCriticalSection(&m_ModBusCS);
		return false;
	}

	dataDO[ch] = enable;
	
	::LeaveCriticalSection(&m_ModBusCS);
	return true;		
}




//���������� ���������� �� ���������� ����������
void KulonMini::BuildMeassureRangeInfo()
{
	MEASSURE_RANGE_INFO msri;
	mrciOut.assign(OUTPUT_ANALOG_CHANNELS_COUNT, msri);
	mrciIn.assign(INPUT_ANALOG_CHANNELS_COUNT, msri);

	mrciOut[0].rngid = 0;				//������������� ����������� ���������
	mrciOut[0].desc  = "�������� �������";//���������� ��������
	mrciOut[0].units = "�";		 //�������	 ���������
	mrciOut[0].mins  = 0;			//����������� �������� ���������� ��������
	mrciOut[0].maxs  = 2;			//������������ �������� ���������� ��������	

	mrciIn[0].rngid = mrciOut[1].rngid = 0;				//������������� ����������� ���������
	mrciIn[0].desc  = mrciOut[1].desc  = "���";//���������� ��������
	mrciIn[0].units = mrciOut[1].units = "�A";		 //�������	 ���������
	mrciIn[0].mins  = mrciOut[1].mins  = 0;			//����������� �������� ���������� ��������
	mrciIn[0].maxs  = mrciOut[1].maxs  = 150;			//������������ �������� ���������� ��������	

	mrciIn[1].rngid = mrciOut[2].rngid = 2;				//������������� ����������� ���������
	mrciIn[1].desc  = mrciOut[2].desc  = "����������";//���������� ��������
	mrciIn[1].units = mrciOut[2].units = "10*�";		 //�������	 ���������
	mrciIn[1].mins  = mrciOut[2].mins = 0;			//����������� �������� ���������� ��������
	mrciIn[1].maxs  = mrciOut[2].maxs = 65;			//������������ �������� ���������� ��������
}


	

//���������� ���������� �� ������ ���������
void KulonMini::BuildRegestersMaps()
{

	//���������� ������
	aoChannelsMap.insert(std::make_pair(0, 100)); //�������� �������
	aoChannelsMap.insert(std::make_pair(1, 101)); //������� ����
	aoChannelsMap.insert(std::make_pair(2, 102)); //������� ����������

	
	//�������� ������
	doChannelsMap.insert(std::make_pair(0, DO_FULL_ADRESS(104, 0))); //������
	doChannelsMap.insert(std::make_pair(1, DO_FULL_ADRESS(104, 1))); //����
	doChannelsMap.insert(std::make_pair(2, DO_FULL_ADRESS(104, 2))); //������
	doChannelsMap.insert(std::make_pair(3, DO_FULL_ADRESS(170, 2))); //����� ������

	
	doRegs.insert(std::make_pair(104, 0) );
	doRegs.insert(std::make_pair(170, 0) );


}