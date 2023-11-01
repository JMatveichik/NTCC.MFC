#include "StdAfx.h"
#include "InfracarGasAnalyser.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_msgprovider.h"
#include "..\ah_xml.h"

char crc(LPASCII_COMMAND pCmd, int count)
{
	char cr = 0x00;
	for (int i = 1; i < 10; i++)
		cr += pCmd->cmd[i];

	cr = cr & 0xFF;
	return cr;
}

InfracarGasAnalyser::InfracarGasAnalyser(void) :
IdentifiedPhysDevice("��������", "INFRACAR-M"),
INPUT_ANALOG_CHANNELS_COUNT(7)
{
	ranges.push_back(MEASSURE_RANGE_INFO(0, "������������ �������� ���� (CO)", "%", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(1, "������������ ������������� (CH)", "ppm", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(2, "������������ ����������� ���� (CO2)", "%", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(3, "������������ ��������� (O2)", "%", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(4, "������� �������� ����", "��/���", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(5, "������", "��/���", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(6, "������������ ������� ����� (NOx)", "ppm", 0, 30));

}


InfracarGasAnalyser::~InfracarGasAnalyser(void)
{
	//ranges.clear();
}

bool InfracarGasAnalyser::CreateFromXMLNode(IXMLDOMNode* pNode)
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
	
	/*
	0xCA,0x00,0x05,0x07,0x00,0x00,0x03,0x05,0x00,0xFF,0x13
	
		����� ���������������:
	0xFA,0xFF,0x05,0x07,0x02,0x00,0x11,0x05,0xC3,0xC0,0xC7,0xCE,0xC0,0xCD,0xC0,0xCB,
		0xC8,0xC7,0xC0,0xD2,0xCE,0xD0,0x00,0x02,0x14
		� ���� ��������� � ��� ���������� 0x02, � ���� ������ ������������ ���������� ��������������� ��������-̒.
	*/

	ASCII_COMMAND aCmd;
	aCmd.pSender = this;
	
	aCmd.cmd[0] = char(0xCA); //0xCA � ������������� �������
	aCmd.cmd[1] = char(0x00);	//0x00 � ����������� ����������
	aCmd.cmd[2] = char(0x05); //0x05 � ������ ��������� ������
	aCmd.cmd[3] = char(0x07); //0x07 � ������������� ������� ������ �������� ����������
	aCmd.cmd[4] = char(0x00); //0x00 � ���� ���������� �������
	aCmd.cmd[5] = char(0x00); //0x00 � ���� ���������� �������
	aCmd.cmd[6] = char(0x03); //0x03 � ���������� ���� ���� ������
	aCmd.cmd[7] = char(0x05); //0x05 � ������ ���� ����������	
	aCmd.cmd[8] = char(0x00); //0x00 � ����������� ��������
	aCmd.cmd[9] = char(0xFF); //0xFF � ������������� ���������� ��������� ������ (��)
	aCmd.cmd[10] = crc(&aCmd, 10); // � ����������� �����
	
	aCmd.length = 11;

	if ( pMan->SendCommand(&aCmd, false) )
	{
		int len = aCmd.length;
		TRACE("\n����� %d ����\n", len );
	}
	/*
	0xFA - �			//���������� ����� �� ����������. ����������� �����
	0x05 - 			//������ ����������
	0x07 - 			//��� �������������� ������
	0x02 - 			//������������� �������
	0x00 -			//������� ���� ���������� �������
	0x23 - #			//������� ���� ���������� �������
	0x05 - 			//������ ������� ������
	0xC3 - �
	0xC0 - �
	0xC7 - �
	0xCE - �
	0xC0 - �
	0xCD - �
	0xC0 - �
	0xCB - �
	0xC8 - �
	0xC7 - �
	0xC0 - �
	0xD2 - �
	0xCE - �
	0xD0 - �
	0x20 -  
	0xCC - �
	0xCD - �
	0xCE - �
	0xC3 - �
	0xCE - �
	0xCA - �
	0xCE - �
	0xCC - �
	0xCF - �
	0xCE - �
	0xCD - �
	0xC5 - �
	0xCD - �
	0xD2 - �
	0xCD - �
	0xDB - �
	0xC9 - �
	0x00 - 
	0x02 - 
	0xE1 - �
	*/
	unsigned char res[1024];
	memcpy(res, aCmd.response, aCmd.length);

	for(int i =0; i < aCmd.length; i++)
	{
		TRACE("\n0x%02X - %c", res[i], res[i]);
	}
	return true;

}

InfracarGasAnalyser* InfracarGasAnalyser::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<InfracarGasAnalyser> obj ( new InfracarGasAnalyser() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
void	InfracarGasAnalyser::ReadChannels(double* data /* = NULL*/) const
{
	for (int i = 0; i < INPUT_ANALOG_CHANNELS_COUNT; i++)
	{
		data[i] = ReadChannel(i);		
	}
	
}

//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
double  InfracarGasAnalyser::ReadChannel(unsigned char channel)  const
{
	/*
	������ ���������� (0x2A) �������� ���������: 
	CO (1),CH(2),CO2(3), O2(4), n(5), tt(6), Nox(7).
	������:
	������ ���������� ���������� ��������� ��������� CO
	0xCA,0x02,0x2A,0x07,0x00,0x00,0x04,0x01,0x01,0x00,0xFF,0x38
	
	����� ����������
	0xFA,0xFF,0x2A,0x07,0x2C,0x01,0x09,0x01,0x01,0x33,0x2E,0x30,0x30,0x31,0x00,0x02, 0x5C
	*/
	
// 		26355 30/09/2014 09:25:42 IRP_MJ_WRITE UP STATUS_SUCCESS ca 00 2a 07 45 00 03 01 01 ff 7a  �.*.E....�z 11 
// 		26369 30/09/2014 09:25:42 IRP_MJ_WRITE UP STATUS_SUCCESS ca 00 2a 07 45 00 03 02 01 ff 7b  �.*.E....�{ 11 
// 		26387 30/09/2014 09:25:42 IRP_MJ_WRITE UP STATUS_SUCCESS ca 00 2a 07 45 00 03 03 01 ff 7c  �.*.E....�| 11 

	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);
	ASCII_COMMAND aCmd;
	aCmd.pSender = this;
	aCmd.cmd[0] = char(0xCA); //0xCA � ������������� �������
	aCmd.cmd[1] = char(0x00);	//0x02 � ������������� ��������������� ��������-�
	aCmd.cmd[2] = char(0x2A); //0x2A � ������������� ������ ����������
	aCmd.cmd[3] = char(0x07); //0x07 � ������������� ������� ������ �������� ����������
	aCmd.cmd[4] = char(0x45); //0x00 � ���� ���������� �������
	aCmd.cmd[5] = char(0x00); //0x00 � ���� ���������� �������
	aCmd.cmd[6] = char(0x03); //0x04 � ���������� ���� ���� ������
	aCmd.cmd[7] = char(channel + 1); //0x01 � ������ ��������� ������ (channel)
	aCmd.cmd[8] = char(0x01); //0x01 � ������ ���� ��������� ���������
	aCmd.cmd[9] = char(0xFF); //0xFF � ������������� ���������� ��������� ������ (��)
	aCmd.cmd[10] = crc(&aCmd, 10); // � ����������� �����
						
	aCmd.length = 11;
	
	unsigned char res[32];

	if ( pMan->SendCommand(&aCmd, false) )
	{
		int byteNo = 0;
		int dataStartByteNo = 8;
		for(int i = dataStartByteNo; i < aCmd.length; i++)
		{
			res[byteNo++] = aCmd.response[i];
			if ( aCmd.response[i] == 0)
				break;
		}
	}
	return atof((const char*)res);
}

// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
// (������ �� ������ ������ �� �����������)
double  InfracarGasAnalyser::GetChannelData(unsigned char channel)  const
{
	return data[channel];
}

//��������  ���������� �� ���������� ��������� ������
void InfracarGasAnalyser::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci =  ranges[channel];
}

//����������  ��������  ����������� �����
bool InfracarGasAnalyser::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  ���������
void InfracarGasAnalyser::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.push_back(ranges[ch]);
}

//������� �������� ���������� ���������� ����� �������� �������� �������
void InfracarGasAnalyser::ControlProc()
{
	ReadChannels(data);
}