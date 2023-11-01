
#include "stdafx.h"
#include "PhysDevice.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

//ofstream IdentifiedPhysDevice::m_logDevs;

//////////////////////////////////////////////////////////////////////////
std::map<std::string, IdentifiedPhysDevice*> IdentifiedPhysDevice::m_allDevices;

IdentifiedPhysDevice::IdentifiedPhysDevice() : 
m_nFailures(0), 
m_bStatusOk(false), 
m_strName(""), 
m_strManufacturer("UNKNOWN"),	//производитель 
m_strModel("UNKNOWN")		//модель
{
}

bool IdentifiedPhysDevice::LogEnable(const char* pszPath)
{
	if ( m_logDevs.is_open() )
		return false;

	m_logDevs.open(pszPath, ios::ate);

	if ( m_logDevs.bad() )
		return false;
	
	return true;
}

IdentifiedPhysDevice::IdentifiedPhysDevice(std::string manufacturer, std::string model):
m_nFailures(0), 
m_bStatusOk(false), 
m_strName(""), 
m_strManufacturer(manufacturer),	//производитель 
m_strModel(model)					//модель
{
}

IdentifiedPhysDevice::~IdentifiedPhysDevice()
{
}

bool IdentifiedPhysDevice::Create(std::string name, unsigned long interval)
{
	
	std::strstream str;		 
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	if ( GetDevice(name) )
	{
		str << "Device with id ["<< name.c_str() <<"] already exist" << std::ends;
		err.SetLastError(DCE_DUPLICATE_NAME, str.str() ); 
		return false;		
	}

	if ( interval < MINDELAY )
	{
		str << "Invalid contorl procedure interval ["<< interval <<"]. Interval must be greater then ["<< MINDELAY <<"]" << std::ends;
		err.SetLastError(DCE_INVALID_INTERVAL, str.str() ); 
		return false;
	}
	
	m_strName = name;
	m_nInterval = interval;

	return m_bStatusOk = true;
}

bool IdentifiedPhysDevice::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	std::strstream str;		 
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	std::string type;
	if ( !appXML.GetNodeAttributeByName(pNode, "TYPE", type) || type.length() == 0 )
	{
		str << "Device type not set (tag <TYPE>)" << std::ends;
		err.SetLastError(DCE_DEVICE_TYPE_NOT_SET, str.str() ); 
		return false;
	}

	std::string name;
	if ( !appXML.GetNodeAttributeByName(pNode, "NAME", name) || name.length() == 0 )
	{
		str << "Device name not set (tag <NAME>)" << std::ends;
		err.SetLastError(DCE_DEVICE_NAME_NOT_SET, str.str() ); 
		return false;		
	}

	if ( GetDevice( name) )
	{
		str << "Device with id ["<< name.c_str() <<"] already exist" << std::ends;
		err.SetLastError(DCE_DUPLICATE_NAME, str.str() ); 
		return false;		
	}

	m_strName = name.c_str();

	std::string time;
	int interval = 1000;
	if ( appXML.GetNodeAttributeByName(pNode, "TIME", time) )
		interval = atoi( time.c_str() );

	if ( interval < MINDELAY )
	{
		str << "Invalid contorl procedure interval ["<< interval <<"]. Interval must be greater then ["<< MINDELAY <<"]" << std::ends;
		err.SetLastError(DCE_INVALID_INTERVAL, str.str() ); 
		return false;
	}

	m_nInterval = interval;

	//Описание 
	CComBSTR desc;
	pNode->get_text(&desc);

	USES_CONVERSION;
	m_strDesc = (LPCTSTR)W2A(desc);
	//TRACE("%s", m_strDesc.c_str());

	return m_bStatusOk = true;
}

void IdentifiedPhysDevice::EnumDevices(vector <const IdentifiedPhysDevice*>& devs )
{
	devs.clear();

	for(std::map<std::string, IdentifiedPhysDevice*>::iterator it = m_allDevices.begin(); it != m_allDevices.end(); it++)
	{	
		devs.push_back((*it).second);
	}
}


bool IdentifiedPhysDevice::AddDevice(IdentifiedPhysDevice* dev)
{
	if (dev == NULL)
		return false;

	if ( m_allDevices.insert(std::make_pair(dev->Name(), dev)).second ) 
		return true;
	
	std::strstream str;		 
	str << "Duplicate device name (" << dev->Name().c_str() << ")" << std::ends;
	AppErrorsHelper::Instance().SetLastError(DCE_DUPLICATE_NAME, str.str() );
	return false;
}

void IdentifiedPhysDevice::StopAll()
{
	for(std::map<string, IdentifiedPhysDevice*>::iterator it = m_allDevices.begin(); it != m_allDevices.end(); ++it)
		(*it).second->Stop();
}

void IdentifiedPhysDevice::ClearAll()
{
	for(std::map<string, IdentifiedPhysDevice*>::iterator it = m_allDevices.begin(); it != m_allDevices.end(); ++it)
	{
		(*it).second->Stop();
		delete (*it).second;
	}
	m_allDevices.clear();
}

bool IdentifiedPhysDevice::IsAlive() const 
{ 
	Lock();
	bool bStat = m_bStatusOk;
	Unlock();

	return bStat; 
}

bool IdentifiedPhysDevice::FailureCallback(void* p)
{
	IdentifiedPhysDevice* dev = (IdentifiedPhysDevice*)p;
	
	dev->Lock();	
	
	dev->m_nFailures++;
	//dev->m_bStatusOk = false; 

	bool bStat = dev->OnFailure();

	dev->Unlock();

	return true;
}

bool IdentifiedPhysDevice::OnFailure() 
{
	return false;//m_bStatusOk = false;
}

bool IdentifiedPhysDevice::PostConfigProcessing(IXMLDOMNode* pNode)
{
	if ( pNode == NULL )
		return false;
	
	return true;
}

std::string IdentifiedPhysDevice::Manufacturer() const 
{ 
	return m_strManufacturer; 
}

std::string IdentifiedPhysDevice::Model() const 
{
	return m_strModel; 
}

std::string IdentifiedPhysDevice::Name()	const 
{
	return m_strName; 
}

std::string IdentifiedPhysDevice::Description()	const 
{
	return m_strDesc; 
}

IdentifiedPhysDevice* IdentifiedPhysDevice::GetDevice(std::string name)
{
	std::map<std::string, IdentifiedPhysDevice*>::const_iterator fnd;
	fnd =  m_allDevices.find( name );

	if ( fnd == m_allDevices.end() )
		return NULL;
	else
		return (*fnd).second;
}

