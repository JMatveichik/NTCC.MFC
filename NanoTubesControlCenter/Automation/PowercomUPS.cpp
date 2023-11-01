#include "StdAfx.h"
#include "PowercomUPS.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

PowercomUPS::PowercomUPS(void) : IdentifiedPhysDevice("Powercom", "VGD-2000 RM"),
	INPUT_ANALOG_CHANNELS_COUNT(5), comPortNo(0)
{
	lastData.assign(INPUT_ANALOG_CHANNELS_COUNT, 0.0);
}


PowercomUPS::~PowercomUPS(void)
{
}

PowercomUPS* PowercomUPS::CreateFromXML(IXMLDOMNode* pNode, UINT& error )
{
	auto_ptr<PowercomUPS> obj ( new PowercomUPS() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool PowercomUPS::CreateFromXMLNode(IXMLDOMNode* pNode)
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
		err.SetLastError(DCE_DEVICE_ADDRESS_NOT_SET, str.str() );
		return false;
			
	}
	int nPort = atoi(sAtr.c_str());

		CommandManager* pMan = CommandManager::GetCommandManager(nPort);
	if ( pMan == NULL )
	{
		str << "Command manager assigned with port COM" << (int)nPort << " not found" << std::ends;
		err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str());
		return false;
	}

	comPortNo = nPort;

	//���������� �������������� ���������� ����������������� �����
	DCB dcb; 
	pMan->GetComState( dcb );

	dcb.BaudRate = 2400;
	dcb.ByteSize = 8;			//����� ����� ������
	dcb.fParity = FALSE;		//�������� ��������
	dcb.StopBits = ONESTOPBIT;	// ����� �������� �����

	if ( !pMan->SetComState(dcb) )
	{
		str << "Initialize port COM" << (int)comPortNo  << " failed" << std::ends;
		err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str());
		return false;
	}

	int nInterval = 3000;
	if (appXML.GetNodeAttributeByName(pNode, "TIME", sAtr))
		nInterval = atoi(sAtr.c_str() );


	return true; 
}

void PowercomUPS::ControlProc()
{
	ReadChannels(NULL);
}


//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
void	PowercomUPS::ReadChannels(double* data) const
{
	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);

	if ( pMan == NULL )
		return;

	char szCmd[] = "Q1\n\r";
	//��������� �������� �������
	ASCII_COMMAND command;
	strcpy_s(command.cmd, szCmd);
	command.pSender = this;	
	command.length = 0;
	strcpy_s(command.response, "SEND");

	if ( !pMan->SendCommand(&command, false) )
	{

	}

	std::string response = command.response;
	
	if ( response == "FAILURE" || response == "SEND")
	{
		SERIAL_COMMAND scmd;
		scmd.size = sizeof(ASCII_COMMAND);
		memcpy(&scmd.ascii, &command, sizeof(ASCII_COMMAND));
		
		//evCmdFailure(&scmd);
	}
	////TRACE("\nUPS>%s", response.c_str());
}

//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
double  PowercomUPS::ReadChannel(unsigned char channel)  const
{
	ReadChannels(NULL);
	return GetChannelData(channel);
}


// ��������� ������ �� ������ ����� ���������� ����������
// ��� ������ �� ���������� ������ (������ �� ������ ������ �� �����������)
double  PowercomUPS::GetChannelData(unsigned char channel)  const
{
	return lastData[channel];
}

//��������  ���������� �� ���������� ��������� ������
void PowercomUPS::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	ASSERT(FALSE);
}

//����������  ��������  ����������� �����
bool PowercomUPS::SetInputRange(unsigned char ch, unsigned char range)
{
	ASSERT(FALSE);
	return false;
}

//�������� ��� ���������  ���������  ���������
void PowercomUPS::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	ASSERT(FALSE);
}