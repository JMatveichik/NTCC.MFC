#include "stdafx.h"
#include "BaseFlowController.h"
#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

BaseFlowController::BaseFlowController() : 
IdentifiedPhysDevice("Base Flow Controller", "UNKNOWN")
{	
}

BaseFlowController::BaseFlowController(std::string manufact, std::string model) :
IdentifiedPhysDevice(manufact, model)
{
}

BaseFlowController::~BaseFlowController()
{
}


bool BaseFlowController::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	std::string sAtr;
	if (! appXML.GetNodeAttributeByName(pNode, "GAS", sAtr) )
	{
		str << "Controlled gas not select" <<  std::ends;
		err.SetLastError(DCE_CONTROLED_GAS_NOT_SET, str.str() );
		return false;
	}

	return true; 
}

bool BaseFlowController::Create(std::string name, unsigned long updateInterval, std::string controledGas, double maxFlow)
{
	if ( !IdentifiedPhysDevice::Create(name, updateInterval) )
		return false;

	if ( maxFlow <= 0 )
		return false;

	//максимальный расход
	m_maxFlow = maxFlow;
	
	//
	m_strGas = controledGas;

	return m_bStatusOk = true;
}


TestFlowController::TestFlowController()
{
	m_strManufacturer = "LNP Software";
	m_strModel = "Test Flow Controller 01";
	m_dCurFlow = 0.0;
}

TestFlowController::~TestFlowController()
{

}