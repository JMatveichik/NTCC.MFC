#include "StdAfx.h"
#include "BatteryMeassureUnit.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

BatteryMeassureUnit::BatteryMeassureUnit() :
IdentifiedPhysDevice("ARTECH", "BMU-001"),
DIN_CHANNELS_COUNT(16)
{
	//������ ��������� ��� ���������� �������
	rngAO.push_back(MEASSURE_RANGE_INFO(0, "����� ��������� ����������������� ��������� ���", "�", 0.0, 2.5));
	aoutRegs.push_back(10);
	
	AOUT_CHANNELS_COUNT = 	(int)aoutRegs.size();
	
	//������ ��������� ��� ���������� ������	
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "���������� ������ ���", "�", 0.0, 2.5)); //������ 1
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "���������� ������ ���", "�", 0.0, 2.5)); //������ 2
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "���������� ������ ���", "�", 0.0, 2.5)); //������ 3
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "���������� ������ ���", "�", 0.0, 2.5)); //������ 4
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "���������� ������ ���", "�", 0.0, 2.5)); //������ 5
	rngAI.push_back(MEASSURE_RANGE_INFO(0, "���������� ������ ���", "�", 0.0, 2.5)); //������ 6
	rngAI.push_back(MEASSURE_RANGE_INFO(1, "����� ��������� ����������������� ��������� ���", "�", 0.0, 2.5)); 	
	rngAI.push_back(MEASSURE_RANGE_INFO(2, "������", "", 0.0, 2.5)); 	

	AIN_CHANNELS_COUNT = (int)rngAI.size();

	dataAO.assign(AOUT_CHANNELS_COUNT, 0.0);
	dataAI.assign(AIN_CHANNELS_COUNT, 0.0);
}

BatteryMeassureUnit::~BatteryMeassureUnit(void)
{

}

BatteryMeassureUnit* BatteryMeassureUnit::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<BatteryMeassureUnit> obj ( new BatteryMeassureUnit() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool BatteryMeassureUnit::CreateFromXMLNode(IXMLDOMNode* pNode)
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

void BatteryMeassureUnit::ControlProc()
{
	ReadChannels();
}

bool BatteryMeassureUnit::OnPrepareDevice()
{
	CHECKCMDMANAGER(pMan, portNo);

	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	cmd.func		 = 0x04;	

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

		double val = (double)cmd.response[0];
		val *= 0.000806;
		dataAO[ch] = val;
	}

	return true;
}


///////////////////////////////////////////////////
//���������� ���������� IDigitalInput

//�������� ����� ���������� ������
int BatteryMeassureUnit::GetChannelsCountDI() const
{
	return DIN_CHANNELS_COUNT;
}

//�������� ��������� ���������� ������
bool BatteryMeassureUnit::UpdateChannelsStateDI() const
{
	CHECKCMDMANAGER(pMan, portNo);

	//
	MODBUS_SERIAL_COMMAND cmd;
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 1; 
	cmd.regAddress   = 0x07; 
	cmd.func		 = 0x04;
	cmd.response[0]  = 0;

	 
	if ( !pMan->SendCommand(&cmd, false) )
		return false;

	unsigned short msk = cmd.response[0];
	std::bitset<16> st(msk);
	dinChState = st;

	return true;
}

//��������� ��������� ������ ����������� �����
bool BatteryMeassureUnit::GetChannelStateDI(unsigned char ch, bool update) const
{
	if (update)
		UpdateChannelsStateDI();

	return dinChState.test( ch );
}

//////////////////////////////////////////////////////////////////////////
//interface IAnalogInput
//��������� ������ ����� ���������� ������
int BatteryMeassureUnit::GetChannelsCountAI() const
{
	return AIN_CHANNELS_COUNT;
}

//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
void BatteryMeassureUnit::ReadChannels(double* data/*  = NULL*/) const
{
	CHECKCMDMANAGER_VOID(pMan, portNo);

	//
	MODBUS_SERIAL_COMMAND cmd;
	memset(&cmd, 0, sizeof(MODBUS_SERIAL_COMMAND));
	cmd.pSender		 = this;
	cmd.slaveAddress = devAdr;
	cmd.regCount	 = 16; 
	cmd.regAddress   = 0x00;
	cmd.func		 = 0x04;	

	if ( !pMan->SendCommand(&cmd, false) )
		return;

	for(int i = 0; i < AIN_CHANNELS_COUNT - 1; i++)
	{
		//Lock();
		double val = (double)cmd.response[i];
		val *= 0.000806;
		dataAI[i] = val;
		//Unlock();
	}

	dataAI[AIN_CHANNELS_COUNT - 1] = (double)cmd.response[8];

	if (data != NULL) 
	{
		Lock();
		memcpy(data, dataAI.data(), sizeof(double)*AIN_CHANNELS_COUNT);
		Unlock();
	}

	//��������� �������� ������	
	std::bitset<16> st(cmd.response[7]);
	dinChState = st;
	
}

//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
double  BatteryMeassureUnit::ReadChannel(unsigned char ch)  const
{
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return 0.0;
	
	ReadChannels();
	return dataAI[ch];
}

// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
// (������ �� ������ ������ �� �����������)
double  BatteryMeassureUnit::GetChannelData(unsigned char ch)  const
{
	double val = 0.0;
	//Lock();
	
	val = dataAI[ch];

	//Unlock();
	return val;
}

//��������  ���������� �� ���������� ��������� ������
void BatteryMeassureUnit::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	*lpmci = rngAI[ch];
}

//����������  ��������  ����������� �����
bool BatteryMeassureUnit::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  ���������
void BatteryMeassureUnit::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	
	if ( ch < 0 || ch >= AIN_CHANNELS_COUNT )
		return;

	rngs.push_back(rngAI[ch]);
}

//////////////////////////////////////////////////////////////////////////
/// IAnalogOutput
//��������� ������ ����� ���������� ������� 
int BatteryMeassureUnit::GetChannelsCountAO() const
{
	return AOUT_CHANNELS_COUNT;
}

//�������� ������ � ���������� ����� 
double  BatteryMeassureUnit::WriteToChannel(unsigned char ch, double val)
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
void BatteryMeassureUnit::WriteToAllChannels(const std::vector<double>& vals)
{

}

//�������� ��������� ���������� � ���������� ����� �������� 
double BatteryMeassureUnit::GetChannelStateAO (unsigned char ch) const
{
	double val = 0.0;
	Lock();

	val = dataAO[ch];

	Unlock();
	return val;
}

//�������� �������� �������� ������ 
void BatteryMeassureUnit::GetMinMax(unsigned char ch, double& minOut, double& maxOut) const
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
void BatteryMeassureUnit::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	*lpmci = rngAO[ch];
}

//����������  ��������  ����������� ������
bool BatteryMeassureUnit::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}	

//�������� ��� ���������  ���������  
void BatteryMeassureUnit::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();	

	if (ch < 0 || ch > AOUT_CHANNELS_COUNT)
		return;

	rngs.push_back(rngAO[ch]);
}