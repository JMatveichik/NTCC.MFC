#include "StdAfx.h"
#include "NTCCDataClient.h"

#include "..\\ah_errors.h"
#include "..\\ah_xml.h"
#include "..\\ah_network.h"
#include "DataSrc.h"

#include "..\\NanoTubesControlCenter.h" 

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif


NTCCDataClient::NTCCDataClient(void)
	: IdentifiedPhysDevice("ART-Pte software", "DATA SRV"), 
	hPipe(NULL),
	AIChannelsCount(0),	
	AOChannelsCount(0),
	DIChannelsCount(0), 
	DOChannelsCount(0)
{
}


NTCCDataClient::~NTCCDataClient(void)
{
	CloseHandle(hPipe);
}

NTCCDataClient* NTCCDataClient::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	std::auto_ptr<NTCCDataClient> obj ( new NTCCDataClient() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();	
}

bool NTCCDataClient::Send(std::string cmd, std::string& res) const
{
	DWORD  cbRead;
	char   szBuf[4096];
		
	NTCCDataClient* p = const_cast<NTCCDataClient*>(this);

 	if ( !CallNamedPipe(strPipePath, (LPVOID)cmd.c_str(), cmd.size() + 1, szBuf, 4096, &cbRead, NMPWAIT_NOWAIT ) )
 	{
 		Globals::CheckError(); 			
		p->FailureCallback((void*)this);
		
		if (m_nFailures > 10)
			p->m_bStatusOk = false;

 		return false;
 	}
	
	p->m_nFailures = 0;
	p->m_bStatusOk = true;

	res = szBuf;
	return true;
}


bool NTCCDataClient::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();

	std::string addr;
	if ( !appXML.GetNodeAttributeByName(pNode, "ADDRESS", addr) )
	{
		str << "Address must be set for this device type" << std::ends;		
		err.SetLastError(DCE_DEVICE_ADDRESS_NOT_SET, str.str() );
		return false;
	}

	std::string user;
	if ( !appXML.GetNodeAttributeByName(pNode, "USER", user) )
	{
		str << "Server user name not set" << std::ends;		
		err.SetLastError(DOCE_PARAMETER_NOT_DEFINED, str.str() );
		return false;
	}	

	std::string psw;
	if ( !appXML.GetNodeAttributeByName(pNode, "PSW", psw) )
	{
		str << "Server user password not set" << std::ends;		
		err.SetLastError(DOCE_PARAMETER_NOT_DEFINED, str.str() );
		return false;
	}
	

	if ( !AppNetworkHelper::Instance().Connect(addr.c_str(), user.c_str(), psw.c_str()) )
		return false;

	CString netpath;
	netpath.Format("\\\\%s", addr.c_str());	

	/*NETRESOURCE nr;
	nr.lpLocalName = NULL;
	nr.dwDisplayType = RESOURCEDISPLAYTYPE_GENERIC;
	nr.dwScope = RESOURCE_GLOBALNET;
	nr.dwType = RESOURCETYPE_ANY;
	nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;
	nr.lpRemoteName = (LPSTR)netpath.GetBuffer(netpath.GetLength());
	nr.lpComment = NULL;
	nr.lpProvider = NULL;

	
	if ( WNetAddConnection2(&nr, psw, user, CONNECT_TEMPORARY) != NO_ERROR )
	{
		CheckError(true);
		return false;
	}
	*/

	netpath.ReleaseBuffer();
	strPipePath.Format("\\\\%s\\pipe\\%s", addr.c_str(), m_strName.c_str());
	

	hPipe = CreateFile(strPipePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		Globals::CheckError(true);
		str << "Cannot connect to data server " << addr.c_str() << std::ends;		
		err.SetLastError(DCE_INVALID_DEVICE_ADDRESS, str.str() );
		return false;		
	}

	//�������� ���������� ���������� ������ �� ������� 
	std::string res;
	if ( !Send("get:count:all", res) )
	{
		str << "Cannot connect to data server " << addr.c_str() << std::ends;		
		err.SetLastError(DCE_INVALID_DEVICE_ADDRESS, str.str() );
	}

	int size = atoi(res.c_str());
	//�������������� ������
	srvdata.assign(size, 0.0);
	
	//////////////////////////////////////////////////////////////////////////
	
	//�������� ���������� ���������� ������� ���������� ������ 
	Send("get:count:ai", res);
	AIChannelsCount = atoi(res.c_str());

	Send("get:idxs:ai", res);
	ExtractData<int>(res, ";", IdxsAI);

	//�������� ���������� ���������� �������� ���������� ������ 
	Send("get:count:ao", res);
	AOChannelsCount = atoi(res.c_str());

	Send("get:idxs:ao", res);
	ExtractData<int>(res, ";", IdxsAO);

	//�������� ���������� ���������� ������� ���������� ������ 
	Send("get:count:di", res);
	DIChannelsCount = atoi(res.c_str());

	Send("get:idxs:di", res);
	ExtractData<int>(res, ";", IdxsDI);

	//�������� ���������� ���������� �������� ���������� ������ 
	Send("get:count:do", res);
	DOChannelsCount = atoi(res.c_str());

	Send("get:idxs:do", res);
	ExtractData<int>(res, ";", IdxsDO);

	return true;
}

//������� �������� ���������� ���������� ����� �������� �������� �������
void NTCCDataClient::ControlProc()
{
	ReadChannels();
}

//////////////////////////////////////////////////////////////////////////
//IAnalogInput
//��������� ������ ����� ���������� ������
int NTCCDataClient::GetChannelsCountAI() const
{
	return AIChannelsCount;
}

//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
void	NTCCDataClient::ReadChannels(double* data/*  = NULL*/) const
{
	std::string res;
	if ( !Send("get:data", res) )
		return; 

	Lock();
	std::vector<double> newdata;
	ExtractData<double>(res, ";", newdata);
	
	
	if (newdata.size() == srvdata.size() )
	{
// 		static bool first = true;
// 				if ( first ) {
// 					srvdata.assign(newdata.begin(), newdata.end());
// 					first = false;
// 				}
// 		
// 				for (int i = 0; i < (int)IdxsAO.size(); i++)
// 				{
// 					int idx = IdxsAO[i];
// 					if ( newdata[idx] != srvdata[idx] )
// 					{
// 						vector<const DataSrc*> dsAO;
// 						DATA_SOURCES_ENUM_PARAMS ep;
// 		
// 						ep._type = DS_AO;
// 						ep._enummask = DSEM_TYPE|DSEM_DEV|DSEM_WIRE;
// 						ep._wire = i;
// 						ep._devName = m_strName;
// 		
// 						DataSrc::EnumDataSources(dsAO, &ep);
// 		
// 						for( vector<const DataSrc*>::iterator it = dsAO.begin(); it != dsAO.end(); ++it )
// 						{
// 							AnalogOutputDataSrc* pAODS = const_cast<AnalogOutputDataSrc*>(dynamic_cast<const AnalogOutputDataSrc*>( (*it) ));					
// 							pAODS->SetValue(newdata[idx]);
// 						}
// 					}
// 				}

		srvdata.assign(newdata.begin(), newdata.end());
	}

	Unlock();

	
}

//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
double  NTCCDataClient::ReadChannel(unsigned char channel)  const
{
	ReadChannels();
	double val = 0.0;

	Lock();
	val = srvdata[ IdxsAI[channel] ];
	Unlock();

	return val;
}

// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
// (������ �� ������ ������ �� �����������)
double  NTCCDataClient::GetChannelData(unsigned char channel)  const
{
	double val = 0.0;
	Lock();
	val = srvdata[ IdxsAI[channel] ];
	Unlock();
	return val;
}

void NTCCDataClient::GetAnalogRange(int channel, LPMEASSURE_RANGE_INFO lpmci, bool isInputRng) const
{
	std::strstream out;
	out << "get:range:" << (isInputRng ? "ai:" : "ao:") << channel << std::ends;

	std::string res;
	if ( Send(out.str(), res) )
	{
		std::vector<double> rng;
		ExtractData<double>(res, ";", rng);

		if (rng.size() != 2)
			return;

		lpmci->mins = rng[0];
		lpmci->maxs = rng[1];
	}
}

std::string NTCCDataClient::SetChannel(int channel, double val, bool isAnalog) const
{
	std::strstream out;
	out << "set:" << (isAnalog ? "ao:" : "do:") << channel << ":" << val << std::ends;

	std::string res;
	if ( !Send(out.str(), res) )
	{
		return "FAILURE SEND";
	}
	return res;
}

//��������  ���������� �� ���������� ��������� ������
void NTCCDataClient::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	GetAnalogRange(channel, lpmci, true);
}

//����������  ��������  ����������� �����
bool NTCCDataClient::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  ���������
void NTCCDataClient::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{
	rngs.clear();
}

//��������� ��� ��������� ���������� � ���� ���� ��� ��������� 
//���������� ������ � ������� ����������� ������

//��������� ������ ����� ���������� ������
int NTCCDataClient::GetChannelsCountDI() const
{
	return DIChannelsCount;
}

//�������� ��������� ���������� ������	
bool NTCCDataClient::UpdateChannelsStateDI() const
{
	ReadChannels(NULL);
	return true;
}

//��������� ��������� ������ �� ���������� ������	
bool  NTCCDataClient::GetChannelStateDI(unsigned char channel, bool update)  const
{
	if (update)
		ReadChannels(NULL);
	
	bool state;
	
	Lock();

	int wire = IdxsDI[channel];
	state = srvdata[ wire ] > 0.0; 

	Unlock();

	return state;
}


//��������� ��� ��������� ���������� � ���� ���� ��� ��������� 
//���������� ������� � ������� ������������ ������

//��������� ������ ����� ���������� ������� 
int NTCCDataClient::GetChannelsCountAO() const
{
	return AOChannelsCount;
}

//�������� ������ � ���������� ����� 
double  NTCCDataClient::WriteToChannel(unsigned char channel, double val)
{
	std::string res = SetChannel(channel, val, true);
	
	//�������� ������
	//ReadChannels(NULL);

	if ( res != "FAILURE SEND" )
		return atof(res.c_str());

	return UNKNONW_VALUE;
}

//�������� ������ �� ��� ������
void NTCCDataClient::WriteToAllChannels(const std::vector<double>& vals)
{

}

//�������� ��������� ���������� � ���������� ����� �������� 
double NTCCDataClient::GetChannelStateAO (unsigned char channel) const
{
	Lock();
	int wire = IdxsAO[channel];
	double val = srvdata[ wire ];
	Unlock();

	return val;
}

//�������� �������� �������� ������ 
void NTCCDataClient::GetMinMax(unsigned char channel, double& minOut, double& maxOut) const
{

}

//��������  ���������� �� ��������� ������
void NTCCDataClient::GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	GetAnalogRange(channel, lpmci, false);
}

//����������  ��������  ����������� ������
bool NTCCDataClient::SetOutputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//�������� ��� ���������  ���������  
void NTCCDataClient::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const
{

}


//////////////////////////////////////////////////////////////////////////
//IDigitalOutput
//��������� ������ ����� �������� ������� 
int NTCCDataClient::GetChannelsCountDO() const
{
	return DOChannelsCount;
}

//�������� ������� ��������� ���������� �������	
bool NTCCDataClient::UpdateChannelsStateDO() const
{
	ReadChannels(NULL);
	return true;
}

//�������� ��������� ��������� ������
bool NTCCDataClient::GetChannelStateDO(unsigned char channel, bool update) const
{
	if (update)
		ReadChannels(NULL);
	
	bool state;
	
	Lock();
	int wire = IdxsDO[channel];
	state = srvdata[ wire ] > 0.0;
	Unlock();

	return state;
}

//���������� ��������� ��������� ������ 
bool  NTCCDataClient::SetChannelStateDO(unsigned char channel, bool enable)
{
	double val = enable ? 1.0 : 0.0;
	std::string res = SetChannel(channel, val, false);

	//�������� ������
	ReadChannels(NULL);


	if ( res != "FAILURE SEND" )
		return atof(res.c_str()) > 0;

	return true;
}