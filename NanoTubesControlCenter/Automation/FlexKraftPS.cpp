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
	//������ ��������� ��� ���������� �������	
	rngAO.push_back(MEASSURE_RANGE_INFO(0, "�������� ����������", "�", 0, 30));
	aoutRegs.push_back(0x02);

	rngAO.push_back(MEASSURE_RANGE_INFO(1, "�������� ���", "A", 0, 300));
	aoutRegs.push_back(0x03);

	rngAO.push_back(MEASSURE_RANGE_INFO(2, "����� ���������", "", 1, 4));
	aoutRegs.push_back(0x09);

		
	AOUT_CHANNELS_COUNT = 	(int)aoutRegs.size();
	
	//������ ��������� ��� ���������� ������
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "�������� ����������", "�", -30, 30));
	ainRegs.push_back(0x04);

	rngAI.push_back(MEASSURE_RANGE_INFO(1, "�������� ���", "�", -300, 300));
	ainRegs.push_back(0x05);
	
	AIN_CHANNELS_COUNT = (int)ainRegs.size();

	dataAO.assign(AOUT_CHANNELS_COUNT, 0.0);
	dataAI.assign(AIN_CHANNELS_COUNT, 0.0);

	//��������� ��� ����������
	channelFactor.push_back(0.01);

	//��������� ��� ����
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

	//���������� �������������� ���������� ����������������� �����
	DCB dcb; 
	pMan->GetComState( dcb );

	dcb.ByteSize = 8;			//����� ����� ������
	dcb.fParity = TRUE;		//�������� ��������
	dcb.Parity = EVENPARITY;
	dcb.StopBits = ONESTOPBIT;	//����� �������� �����

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

	//�������� ������� ���� ���� ������ ���������� ��������
	channelFactor[1] = dinChState.test(10) ? 0.1 : 1.0;

	return true;
}


///////////////////////////////////////////////////
//���������� ���������� IDigitalInput

// �������� ����� 0-15 ������� ���������
//R Status register b00 Alarm No Alarm		������� ������� ������� ����� ���� ������� � ��������� ALARM
//R Status register b01 Warning No Warning  ������� ������� ��������������	����� ���� ������� � ��������� Warnings
//R Status register b02 Remote Local		� ��������� ������ ���������� �	������� ���������������� ����	����������
//R Status register b03 Running Not running ���� ������, ������ �������������� � ������������ � ��������� �����������
//R Status register b04 Ah>Ah-Limit Ah<Ah-Limit	���������� ����������	������������������ ������� ��	�����-����� (������� � ���	�������� ������)
//R Status register b05 Ah>Ah-Limit Ah<Ah-Limit ���������� ���������� ������������������ ������� �� �����-����� (������� � ��� �������� ������)
//R Status register b06 Timer Timer Limit Timer<Timer Limit ���������� ���������� ������� ��� ������� ���������
//R Status register b07 Voltage Mode Current Mode ����������, ��� ������� � �������� � ������ ����������. �������� �������� ����� ���� �������
//R Status register b08 Voltage Mode Current Mode ����������, ��� ������� � �������� � ������ ����������. �������� �������� ����� ���� �������
//R Status register b09 On or Standby Off ���������� ������ ���������� (���������� �������� ���������� �� ��������)
//R Status register b10 Current Hires Current normal ��� ���������������, ���� ���������� ��� ��������� ���� ����� 0,1� 
//R Status register b11 Running B Not running B ���� Running, ���� �������� � ������������ � ����������� ��� B 
//R Status register b12 Timer>Timer Limit B Timer<Timer Limit B ����������, ���� ����������������� ����� ������� �������� �� ������� B 
//R Status register b13 Auxiliary In A active ��������������� �������� ���� 
//R Status register b14 Auxiliary In B active ��������������� �������� ����
//R Status register b15 ��������������� 

// �������� ����� 16-31 ������� ���������������
//R Warnings register b00 ���������������
//R Warnings register b01 ���������������
//R Warnings register b02 ���������������
//R Warnings register b03 Activated OK ������� �� ������� ������� A
//R Warnings register b04 Activated OK ������� �� ������� ������� B
//R Warnings register b05 Activated OK	���������� �������� �� ������	��������
//R Warnings register b06 Activated OK 	������������ ��������. ���� ��� 	��������� ���������� ������� ������. ��������� ���������
//R Warnings register b07 Activated OK	�������������� � ���������. ����	��� ��������� ������ ������� ���	����������� ��� ����������	������.
//R Warnings register b08 ���������������
//R Warnings register b09 ���������������
//R Warnings register b10 Activated OK 	������� �������� ����������� �������� �����������. ��� 	���������, ��������, ��-�� ����� �	����
//R Warnings register b11 ���������������
//R Warnings register b12 ���������������
//R Warnings register b13 ���������������
//R Warnings register b14 ���������������
//R Warnings register b15 ���������������

// �������� ����� 32-47 ������� ������
//R Warnings register b15
//R Alarms register b00 Activated OK ������� ������� ����������	�����������. ���������� ����	���������. ��������� ������� ����������
//R Alarms register b01	���������������
//R Alarms register b02 Activated OK ����������� ������ � ����� ���	���������� ������ �������.	��������, ���������� �������� ����������� ������
//R Alarms register b03 Activated OK ���������������
//R Alarms register b04 Activated OK ���������������
//R Alarms register b05 Activated OK �������� ����������. ������ � ���������� CFG �������	����������
//R Alarms register b06 Activated OK ������� ������� 1
//R Alarms register b07 ���������������
//R Alarms register b08 ���������������
//R Alarms register b09 ���������������
//R Alarms register b10 ���������������
//R Alarms register b11 ���������������
//R Alarms register b12 ���������������
//R Alarms register b13 ���������������
//R Alarms register b14 ���������������
//R Alarms register b15 ��������������� 


//�������� ����� ���������� ������
int FlexKraftPS::GetChannelsCountDI() const
{
	return DIN_CHANNELS_COUNT;
}

//�������� ��������� ���������� ������
bool FlexKraftPS::UpdateChannelsStateDI() const
{
	CHECKCMDMANAGER(pMan, portNo);

	//������� ������� MODBUS
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

//��������� ��������� ������ ����������� �����
bool FlexKraftPS::GetChannelStateDI(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDI();

	return dinChState.test( ch );
}


////////////////////////////////////////44444//////////////////////////////////
//���������� ���������� IDigitalOutput
//��������� ������ ����� �������� ������� 
int FlexKraftPS::GetChannelsCountDO() const
{
	return DOUT_CHANNELS_COUNT;
}

//�������� ������� ��������� ���������� �������	
bool FlexKraftPS::UpdateChannelsStateDO() const
{
	CHECKCMDMANAGER(pMan, portNo);

	//������� ������� MODBUS
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

//�������� ��������� ��������� ������
bool FlexKraftPS::GetChannelStateDO(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDO();

	return doutChState.test( ch );
}

//���������� ��������� ��������� ������ 
bool  FlexKraftPS::SetChannelStateDO(unsigned char ch, bool enable)
{
	if ( ch < 0 || ch >= DOUT_CHANNELS_COUNT )
		return false;

	CHECKCMDMANAGER(pMan, portNo);

	//������� ������� MODBUS
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 

	cmd.regAddress   = 0x10 + ch; //������� Control
	cmd.func		 = 0x05;

	cmd.response[0]  = enable ? 0xff00 : 0x0000;

	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	Sleep(75);

	//��������� ��������� �������� �������
	UpdateChannelsStateDO();

	return true;
}

//////////////////////////////////////////////////////////////////////////
//interface IAnalogInput
//��������� ������ ����� ���������� ������
int FlexKraftPS::GetChannelsCountAI() const
{
	return AIN_CHANNELS_COUNT;
}

//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
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

//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
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

// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
// (������ �� ������ ������ �� �����������)
double  FlexKraftPS::GetChannelData(unsigned char ch)  const
{
	double val = 0.0;
	//Lock();
	
	val = dataAI[ch];

	//Unlock();
	return val;
}

//��������  ���������� �� ���������� ��������� ������
void FlexKraftPS::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	*lpmci = rngAI[ch];
}

//����������  ��������  ����������� �����
bool FlexKraftPS::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  ���������
void FlexKraftPS::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	rngs.push_back(rngAI[ch]);
}

//////////////////////////////////////////////////////////////////////////
/// IAnalogOutput
//��������� ������ ����� ���������� ������� 
int FlexKraftPS::GetChannelsCountAO() const
{
	return AOUT_CHANNELS_COUNT;
}

//�������� ������ � ���������� ����� 
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

//�������� ������ �� ��� ������
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
//�������� ��������� ���������� � ���������� ����� �������� 
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

//�������� �������� �������� ������ 
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

//��������  ���������� �� ��������� ������
void FlexKraftPS::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	*lpmci = rngAO[ch];
}

//����������  ��������  ����������� ������
bool FlexKraftPS::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}	

//�������� ��� ���������  ���������  
void FlexKraftPS::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();	

	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	rngs.push_back(rngAO[ch]);
}