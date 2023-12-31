#include "StdAfx.h"
#include "EngineControlBlock.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

EngineControlBlock::EngineControlBlock() :
IdentifiedPhysDevice("ARTECH", "WORM-001"),
DOUT_CHANNELS_COUNT(3),
DIN_CHANNELS_COUNT(16)
{
	//������ ��������� ��� ���������� �������
	rngAO.push_back(MEASSURE_RANGE_INFO(0, "���� ���������� ���������", "0.5�", 0, 80));
	aoutRegs.push_back(17);

	rngAO.push_back(MEASSURE_RANGE_INFO(1, "������� �����", "", 0, 250));
	aoutRegs.push_back(18);

	rngAO.push_back(MEASSURE_RANGE_INFO(2, "����� �������", "���", 0, 30000));
	aoutRegs.push_back(19);

	rngAO.push_back(MEASSURE_RANGE_INFO(3, "����� ������",  "10 ��", 0, 800));
	aoutRegs.push_back(27);

	rngAO.push_back(MEASSURE_RANGE_INFO(4, "����������� ������������������ �������� #1",  "", 0, 5000));
	aoutRegs.push_back(29);

	rngAO.push_back(MEASSURE_RANGE_INFO(5, "����������� ������������������ �������� #2",  "", 0, 5000));
	aoutRegs.push_back(40);

	rngAO.push_back(MEASSURE_RANGE_INFO(6, "��������� ������� ��",  "", 0, 63));
	aoutRegs.push_back(52);

	rngAO.push_back(MEASSURE_RANGE_INFO(7, "����������� �������� ��",  "", 128, 191));
	aoutRegs.push_back(53);

	rngAO.push_back(MEASSURE_RANGE_INFO(8, "���������� ������� ��",  "", 192, 223));
	aoutRegs.push_back(54);

	rngAO.push_back(MEASSURE_RANGE_INFO(9, "����� ������ ��",  "", 224, 225));
	aoutRegs.push_back(55);

	rngAO.push_back(MEASSURE_RANGE_INFO(10, "��������������� �������� ��",  "", 226, 255));
	aoutRegs.push_back(56);

	rngAO.push_back(MEASSURE_RANGE_INFO(11, "������ 1",  "", 0, 65535));
	aoutRegs.push_back(59);

	rngAO.push_back(MEASSURE_RANGE_INFO(12, "������ 2",  "", 0, 65535));
	aoutRegs.push_back(60);

	AOUT_CHANNELS_COUNT = 	(int)aoutRegs.size();
	
	//������ ��������� ��� ���������� ������
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "�������� �������� ���� (�����������)", "��/���", 0, 5000));
	ainRegs.push_back(45);

	rngAI.push_back(MEASSURE_RANGE_INFO(1, "������ ���������", "", 0, 50));
	ainRegs.push_back(23);

	rngAI.push_back(MEASSURE_RANGE_INFO(2, "������ �������", "16��/�", 0, 3044));
	ainRegs.push_back(44);

	rngAI.push_back(MEASSURE_RANGE_INFO(3, "����������� �������",  "16��", 0, 2080));
	ainRegs.push_back(25);

	rngAI.push_back(MEASSURE_RANGE_INFO(4, "����������� ���������",  "16��", 0, 2080));
	ainRegs.push_back(26);

	rngAI.push_back(MEASSURE_RANGE_INFO(5, "��������� ����� ������� #1",  "���", 0, 65535));
	ainRegs.push_back(42);

	rngAI.push_back(MEASSURE_RANGE_INFO(6, "��������� ����� ������� #2",  "���", 0, 65535));
	ainRegs.push_back(43);

	rngAI.push_back(MEASSURE_RANGE_INFO(7, "�������� �������� ���� (����������)", "��/���", 0, 5000));
	ainRegs.push_back(14);

	rngAI.push_back(MEASSURE_RANGE_INFO(8, "���������� �� (����� 1)", "�", 0, 4095));
	ainRegs.push_back(57);

	rngAI.push_back(MEASSURE_RANGE_INFO(9, "���������� �� (����� 2)", "�", 0, 4095));
	ainRegs.push_back(58);

	AIN_CHANNELS_COUNT = (int)ainRegs.size();


	//������ ��������� ��� �������� �������
	//doutRegs.push_back(36);
	doutCmds.push_back( std::make_pair(3, 2)   );
	doutCmds.push_back( std::make_pair(41, 40) );
	doutCmds.push_back( std::make_pair(43, 42) );


	dataAO.assign(AOUT_CHANNELS_COUNT, 0.0);
	dataAI.assign(AIN_CHANNELS_COUNT, 0.0);
}

EngineControlBlock::~EngineControlBlock(void)
{
/*	for (std::vector<LPMEASSURE_RANGE_INFO>::iterator it = rngAO.begin(); it != rngAO.end(); ++it )
		delete (*it);

	for (std::vector<LPMEASSURE_RANGE_INFO>::iterator it = rngAI.begin(); it != rngAI.end(); ++it )
		delete (*it);
		*/
}

EngineControlBlock* EngineControlBlock::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<EngineControlBlock> obj ( new EngineControlBlock() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool EngineControlBlock::CreateFromXMLNode(IXMLDOMNode* pNode)
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
		return false;	

	return true;
}

void EngineControlBlock::ControlProc()
{
	ReadChannels();

	UpdateChannelsStateDI();
}

bool EngineControlBlock::OnPrepareDevice()
{
	CHECKCMDMANAGER(pMan, portNo);

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	cmd.func		 = 0x03;	

	for (int ch = 0; ch < AOUT_CHANNELS_COUNT; ch++ )
	{
		cmd.regAddress   = aoutRegs[ch];
		if ( !pMan->SendCommand(&cmd, false) )
		{
			std::strstream str;
			AppErrorsHelper& err = AppErrorsHelper::Instance();

			str << "Device prepare failed." << std::ends;		
			err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
			return false;
		}

		dataAO[ch] = (double)cmd.response[0];
	}

	return true;
}


///////////////////////////////////////////////////
//���������� ���������� IDigitalInput

//�������� ����� ���������� ������
int EngineControlBlock::GetChannelsCountDI() const
{
	return DIN_CHANNELS_COUNT;
}

//�������� ��������� ���������� ������
bool EngineControlBlock::UpdateChannelsStateDI() const
{
	CHECKCMDMANAGER(pMan, portNo);

	//
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	cmd.regAddress   = 0x20; //32 �������
	cmd.func		 = 0x03;
	cmd.response[0]  = 0;

	 
	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	unsigned short msk1 = cmd.response[0];		

	cmd.regAddress   = 0x25; //37 �������
	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	unsigned short msk2 = cmd.response[0];		
	
	unsigned short msk = msk2 << 8; 
	msk |= msk1;

	std::bitset<16> st(msk);
	dinChState = st;

	return true;
}

//��������� ��������� ������ ����������� �����
bool EngineControlBlock::GetChannelStateDI(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDI();

	return dinChState.test( ch );
}

//��������� ������ ����� �������� ������� 
int EngineControlBlock::GetChannelsCountDO() const
{
	return DOUT_CHANNELS_COUNT;
}

//�������� ������� ��������� ���������� �������	
bool EngineControlBlock::UpdateChannelsStateDO() const
{
	return true;
}

//�������� ��������� ��������� ������
bool EngineControlBlock::GetChannelStateDO(unsigned char ch, bool update) const
{
	return doutChState.test( ch );
}

//���������� ��������� ��������� ������ 
bool  EngineControlBlock::SetChannelStateDO(unsigned char ch, bool enable)
{
	if ( ch < 0 || ch >= DOUT_CHANNELS_COUNT )
		return false;

	CHECKCMDMANAGER(pMan, portNo);

	//
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	cmd.regAddress   = 0x24; //36 �������
	cmd.func		 = 0x06;
	cmd.response[0]  = enable ? doutCmds[ch].first : doutCmds[ch].second;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	Sleep(75);

	//������ ��������� ����� � 0
	cmd.func		 = 0x03;
	cmd.response[0]  = 0x00;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	//��������� ��������� �� ����� 
	if ( cmd.response[0] != 0x00)
		return false;

	doutChState.set(ch, enable);

	return false;
}

//////////////////////////////////////////////////////////////////////////
//interface IAnalogInput
//��������� ������ ����� ���������� ������
int EngineControlBlock::GetChannelsCountAI() const
{
	return AIN_CHANNELS_COUNT;
}

//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
void EngineControlBlock::ReadChannels(double* data/*  = NULL*/) const
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

//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
double  EngineControlBlock::ReadChannel(unsigned char ch)  const
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
	cmd.func		 = 0x03;	

	if ( !pMan->SendCommand(&cmd, false) )
		return false;
	
	double val = cmd.response[0];

	return val;
}

// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
// (������ �� ������ ������ �� �����������)
double  EngineControlBlock::GetChannelData(unsigned char ch)  const
{
	double val = 0.0;
	//Lock();
	
	val = dataAI[ch];

	//Unlock();
	return val;
}

//��������  ���������� �� ���������� ��������� ������
void EngineControlBlock::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	*lpmci = rngAI[ch];
}

//����������  ��������  ����������� �����
bool EngineControlBlock::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  ���������
void EngineControlBlock::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	rngs.push_back(rngAI[ch]);
}

//////////////////////////////////////////////////////////////////////////
/// IAnalogOutput
//��������� ������ ����� ���������� ������� 
int EngineControlBlock::GetChannelsCountAO() const
{
	return AOUT_CHANNELS_COUNT;
}

//�������� ������ � ���������� ����� 
double  EngineControlBlock::WriteToChannel(unsigned char ch, double val)
{
	CHECKCMDMANAGER(pMan, portNo);
	
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return 0.0;

	double oldVal = dataAO[ch];
	//
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	cmd.regAddress   = aoutRegs[ch];
	cmd.func		 = 0x06;	

	unsigned short v = (unsigned short)val;
	cmd.response[0] = v;

	if ( !pMan->SendCommand(&cmd, true) ) // BR: changed to "true"
		return false;

	dataAO[ch] = val;	
	return oldVal;
}

//�������� ������ �� ��� ������
void EngineControlBlock::WriteToAllChannels(const std::vector<double>& vals)
{

}

//�������� ��������� ���������� � ���������� ����� �������� 
double EngineControlBlock::GetChannelStateAO (unsigned char ch) const
{
	double val = 0.0;
	Lock();

	val = dataAO[ch];

	Unlock();
	return val;
}

//�������� �������� �������� ������ 
void EngineControlBlock::GetMinMax(unsigned char ch, double& minOut, double& maxOut) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
	{
		minOut = maxOut = 0;
		return;
	}

	minOut = rngAO[ch].mins;
	maxOut = rngAO[ch].maxs;
}

//��������  ���������� �� ��������� ������
void EngineControlBlock::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	*lpmci = rngAO[ch];
}

//����������  ��������  ����������� ������
bool EngineControlBlock::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}	

//�������� ��� ���������  ���������  
void EngineControlBlock::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();	

	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	rngs.push_back(rngAO[ch]);
}