#include "stdafx.h"

#include "DeviceFactory.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"

#include "..\FSDocItem.h"



#include "ADAM4000Series.h"
#include "ADAM6000Series.h"

#include "WatchDogs.h"
#include "Timers.h"
#include "DataSrc.h"
#include "Converters.h"

#include "Omega.h"
#include "BonerGasAnaliser.h"
#include "OwenTPM201.h"
#include "OwenSim2.h"
#include "DevicesEmulators.h"
#include "PowercomUPS.h"

#include "InfracarGasAnalyser.h"
#include "NTCCDataClient.h"
#include "IsmatecPump.h"
#include "EngineControlBlock.h"
#include "FlexKraftPS.h"
#include "KulonMini.h"
#include "EASunInvertor.h"
#include "BatteryMeassureUnit.h"


#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

XMLObjectFactory::AssocDevMap		XMLObjectFactory::m_devCreators; 
XMLObjectFactory::AssocDSMap		XMLObjectFactory::m_dsCreators; 
XMLObjectFactory::AssocWDMap		XMLObjectFactory::m_wdCreators; 
XMLObjectFactory::AssocFSDIMap		XMLObjectFactory::m_fsdiCreators; 
XMLObjectFactory::AssocConverterMap XMLObjectFactory::m_convCreators;
XMLObjectFactory::AssocTimerMap		XMLObjectFactory::m_timerCreators;
XMLObjectFactory::AssocFilterMap	XMLObjectFactory::m_filterCreators;

XMLObjectFactory::~XMLObjectFactory(void)
{

}

bool XMLObjectFactory::Init()
{
	RegisterDevice("VIRTUAL", reinterpret_cast<LPCREATEDEVFROMXML>(VirtualDevice::CreateFromXML) );	

	RegisterDevice("ADAM-4019P", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4019P::CreateFromXML));
	RegisterDevice("ADAM-4018P", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4018P::CreateFromXML));
	RegisterDevice("ADAM-4011", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4011::CreateFromXML));
	RegisterDevice("ADAM-4015", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4015::CreateFromXML));
	RegisterDevice("ADAM-4521", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4521::CreateFromXML));
	RegisterDevice("ADAM-4024", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4024::CreateFromXML));
	RegisterDevice("ADAM-4050", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4050::CreateFromXML));
	RegisterDevice("ADAM-4052", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4052::CreateFromXML));
	RegisterDevice("ADAM-4053", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4053::CreateFromXML));
	RegisterDevice("ADAM-4068", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4068::CreateFromXML));
	RegisterDevice("ADAM-4069", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4069::CreateFromXML));
	RegisterDevice("ADAM-4080", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM4080::CreateFromXML));
	

	
	RegisterDevice("ADAM-6018", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM6018::CreateFromXML));
	RegisterDevice("ADAM-6017", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM6017::CreateFromXML));
	RegisterDevice("ADAM-6050", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM6050::CreateFromXML));
	RegisterDevice("ADAM-6052", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM6052::CreateFromXML));
	RegisterDevice("ADAM-6066", reinterpret_cast<LPCREATEDEVFROMXML>(ADAM6066::CreateFromXML));
	
	
	RegisterDevice("FMA-2620A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2605A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2606A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2607A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2608A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2609A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2610A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2611A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2612A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2613A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));
	RegisterDevice("FMA-2621A", reinterpret_cast<LPCREATEDEVFROMXML>(OmegaMFC::CreateFromXML));

	RegisterDevice("VGD-2000 RM", reinterpret_cast<LPCREATEDEVFROMXML>(PowercomUPS::CreateFromXML));	

	RegisterDevice("BONER TEST-01", reinterpret_cast<LPCREATEDEVFROMXML>(BonerGasAnaliserTest01::CreateFromXML));
	RegisterDevice("BONER TEST-14", reinterpret_cast<LPCREATEDEVFROMXML>(BonerGasAnaliserTest14::CreateFromXML));

	RegisterDevice("OWEN TPM201", reinterpret_cast<LPCREATEDEVFROMXML>(OwenTPM201::CreateFromXML));
	RegisterDevice("OWEN TPM210", reinterpret_cast<LPCREATEDEVFROMXML>(OwenTPM210::CreateFromXML));
	RegisterDevice("OWEN SIM2",  reinterpret_cast<LPCREATEDEVFROMXML>(OwenSIM2::CreateFromXML));

	RegisterDevice("INFRACAR-M",  reinterpret_cast<LPCREATEDEVFROMXML>(InfracarGasAnalyser::CreateFromXML));
	RegisterDevice("DATA SRV",  reinterpret_cast<LPCREATEDEVFROMXML>(NTCCDataClient::CreateFromXML));

	RegisterDevice("ISM321",  reinterpret_cast<LPCREATEDEVFROMXML>(IsmatecPump::CreateFromXML));
	RegisterDevice("WORM-001",  reinterpret_cast<LPCREATEDEVFROMXML>(EngineControlBlock::CreateFromXML));
	RegisterDevice("FLEXKRAFT-PS",  reinterpret_cast<LPCREATEDEVFROMXML>(FlexKraftPS::CreateFromXML));
	RegisterDevice("KULON-MINI-PS",  reinterpret_cast<LPCREATEDEVFROMXML>(KulonMini::CreateFromXML));

	RegisterDevice("EASUNINVERTOR",  reinterpret_cast<LPCREATEDEVFROMXML>(EASunInvertor::CreateFromXML));
	RegisterDevice("BMU-001",  reinterpret_cast<LPCREATEDEVFROMXML>(BatteryMeassureUnit::CreateFromXML));
	
	

	RegisterDataSource("AIN", reinterpret_cast<LPCREATEDSFROMXML>(AnalogInputDataSrc::CreateFromXML));
	RegisterDataSource("AOUT", reinterpret_cast<LPCREATEDSFROMXML>(AnalogOutputDataSrc::CreateFromXML));
	RegisterDataSource("AOUTLIST", reinterpret_cast<LPCREATEDSFROMXML>(AnalogOutputListDataSrc::CreateFromXML));
	RegisterDataSource("STR",  reinterpret_cast<LPCREATEDSFROMXML>(StringDataSrc::CreateFromXML));
	RegisterDataSource("A2BITS", reinterpret_cast<LPCREATEDSFROMXML>(AnalogToBitsetDataSrc::CreateFromXML));
	
	RegisterDataSource("CALC", reinterpret_cast<LPCREATEDSFROMXML>(CalcDataSrc::CreateFromXML));
	RegisterDataSource("SUMM", reinterpret_cast<LPCREATEDSFROMXML>(CalcSummDataSrc::CreateFromXML));
	RegisterDataSource("AVRG", reinterpret_cast<LPCREATEDSFROMXML>(CalcAverageDataSrc::CreateFromXML));
	RegisterDataSource("MUL", reinterpret_cast<LPCREATEDSFROMXML>(CalcMulDataSrc::CreateFromXML));
	RegisterDataSource("MIN", reinterpret_cast<LPCREATEDSFROMXML>(CalcMinDataSrc::CreateFromXML));
	RegisterDataSource("MAX", reinterpret_cast<LPCREATEDSFROMXML>(CalcMaxDataSrc::CreateFromXML));

	RegisterDataSource("TAVRG", reinterpret_cast<LPCREATEDSFROMXML>(AnalogTimeAverageDataSrc::CreateFromXML));
	

	RegisterDataSource("DIN", reinterpret_cast<LPCREATEDSFROMXML>(DiscreteInputDataSrc::CreateFromXML));
	RegisterDataSource("DOUT", reinterpret_cast<LPCREATEDSFROMXML>(DiscreteOutputDataSrc::CreateFromXML));
	RegisterDataSource("CDOUT", reinterpret_cast<LPCREATEDSFROMXML>(ControledDiscreteOutputDataSrc::CreateFromXML));
	RegisterDataSource("AND", reinterpret_cast<LPCREATEDSFROMXML>(DiscreteANDDataSrc::CreateFromXML));
	RegisterDataSource("OR",  reinterpret_cast<LPCREATEDSFROMXML>(DiscreteORDataSrc::CreateFromXML));

	RegisterDataSource("LEVEL", reinterpret_cast<LPCREATEDSFROMXML>(AnalogLevelAsDiscreteDataSrc::CreateFromXML));
	RegisterDataSource("RANGE", reinterpret_cast<LPCREATEDSFROMXML>(AnalogRangeAsDiscreteDataSrc::CreateFromXML));
	RegisterDataSource("DEVSTATE", reinterpret_cast<LPCREATEDSFROMXML>(DeviceStateDataSrc::CreateFromXML));
	RegisterDataSource("STR", reinterpret_cast<LPCREATEDSFROMXML>(StringDataSrc::CreateFromXML));

	RegisterWatchDog("WDDOUT", reinterpret_cast<LPCREATEWDFROMXML>(WatchDogDO::CreateFromXML)); 
	RegisterWatchDog("WDSCRIPT", reinterpret_cast<LPCREATEWDFROMXML>(WatchDogScriptControl::CreateFromXML)); 
	RegisterWatchDog("WDSHUTDOWN", reinterpret_cast<LPCREATEWDFROMXML>(WatchDogSystemShutdown::CreateFromXML)); 


	RegisterDrawObject("TEXT",  reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawTextObj::CreateFromXML));
	RegisterDrawObject("DATETIME",  reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawDateTimeObj::CreateFromXML));
	
	RegisterDrawObject("IMAGE", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawImageObj::CreateFromXML));
	RegisterDrawObject("ANALOG", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawAnalogDSObj::CreateFromXML));
	RegisterDrawObject("PROGRESS", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawProgressAnalogDSObj::CreateFromXML));
	RegisterDrawObject("DISCRETE", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawDiscreteDSObj::CreateFromXML));
	RegisterDrawObject("STATE", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawDiscreteDSAsTextObj::CreateFromXML));
	RegisterDrawObject("IMAGESTRIP", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawFilmObj::CreateFromXML));
	RegisterDrawObject("ACTION", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawActionObj::CreateFromXML));
	RegisterDrawObject("USER", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawCurrentUserObj::CreateFromXML));
	RegisterDrawObject("TIMER", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawTimerObj::CreateFromXML));
	RegisterDrawObject("CROSSECTION", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawReactorCrossSection::CreateFromXML));
	RegisterDrawObject("ANALOGOUTSELECT", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawAnalogValueSelectObj::CreateFromXML));
	RegisterDrawObject("SELECT", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawSelectObj::CreateFromXML));
	RegisterDrawObject("PATHANIMATION", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawPathAnimation::CreateFromXML));
	RegisterDrawObject("SYSTEM", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawSystemBtnObj::CreateFromXML));
	RegisterDrawObject("PLOT", reinterpret_cast<LPCREATEFSITEMFROMXML>(CDrawPlotObj::CreateFromXML));
	

	RegisterConverter("LINEAR",		reinterpret_cast<LPCREATECONVERTERFROMXML>(LinearConverter::CreateFromXML));
	RegisterConverter("POLY", reinterpret_cast<LPCREATECONVERTERFROMXML>(PolynomialConverter::CreateFromXML));
	RegisterConverter("POLYN", reinterpret_cast<LPCREATECONVERTERFROMXML>(PolynomialNegativeConverter::CreateFromXML));
	RegisterConverter("MULTIRANGE", reinterpret_cast<LPCREATECONVERTERFROMXML>(MultiRangeConverter::CreateFromXML));
	RegisterConverter("TABLE", reinterpret_cast<LPCREATECONVERTERFROMXML>(TableConverter::CreateFromXML));
	RegisterConverter("CALC", reinterpret_cast<LPCREATECONVERTERFROMXML>(FormulaConverter::CreateFromXML));

	RegisterTimer("TIMER",		reinterpret_cast<LPCREATETIMERFROMXML>(BaseTimer::CreateFromXML ));
	RegisterTimer("COUNTDOWN",	reinterpret_cast<LPCREATETIMERFROMXML>(CountDownTimer::CreateFromXML ));

	RegisterFilter("KALMAN",  reinterpret_cast<LPCREATEFILTERFROMXML>(KalmanFilter::CreateFromXML));
	RegisterFilter("AVERAGE", reinterpret_cast<LPCREATEFILTERFROMXML>(AverageFilter::CreateFromXML));

	return true;
}

bool XMLObjectFactory::RegisterDevice(std::string id, LPCREATEDEVFROMXML creator)
{
	return m_devCreators.insert(std::make_pair(id, creator)).second;
}

IdentifiedPhysDevice* XMLObjectFactory::CreateDeviceObject(IXMLDOMNode* pNode, UINT& nError)
{
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "TYPE", val) )
	{
		str << "Device type not set (tag <TYPE>)" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}
	
	AssocDevMap::const_iterator fnd = m_devCreators.find(val.c_str()); 

	if ( fnd != m_devCreators.end() )
		return (fnd->second)(pNode, nError);
	else
	{
		str << "Unknown device type [" << val.c_str() <<"]" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str());
		return  NULL;
	}
}

bool XMLObjectFactory::RegisterDataSource(std::string id, LPCREATEDSFROMXML creator)
{
	return m_dsCreators.insert(std::make_pair(id, creator)).second;
}

DataSrc* XMLObjectFactory::CreateDataSourceObject(IXMLDOMNode* pNode, UINT& nError)
{
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "TYPE", val) )
	{
		str << "Data source type not set (tag <TYPE>)" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}

	AssocDSMap::const_iterator fnd = m_dsCreators.find(val.c_str()); 
		
	if ( fnd != m_dsCreators.end() )
		return (fnd->second)(pNode, nError);
	else
	{
		str << "Unknown data source type [" << val.c_str() <<"]" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str());
		return  NULL;
	}

	return  NULL;
}

bool XMLObjectFactory::RegisterWatchDog(std::string id, LPCREATEWDFROMXML creator)
{
	return m_wdCreators.insert(std::make_pair(id, creator)).second;
}

BaseWatchDog* XMLObjectFactory::CreateWatchDogObject(IXMLDOMNode* pNode, UINT& nError)
{
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "TYPE", val) )
	{
		str << "Watchdog type not set (tag <TYPE>)" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}

	AssocWDMap::const_iterator fnd = m_wdCreators.find(val.c_str()); 
	
	if ( fnd != m_wdCreators.end() )
		return (fnd->second)(pNode, nError);
	else
	{
		str << "Unknown whatchdog object type [" << val.c_str() <<"]" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str());
		return  NULL;
	}
}


bool XMLObjectFactory::RegisterDrawObject(std::string id, LPCREATEFSITEMFROMXML creator)
{
	return m_fsdiCreators.insert(std::make_pair(id, creator)).second;
}

CDrawObj* XMLObjectFactory::CreateDrawObject(IXMLDOMNode* pNode, FSLayer& parentLayer, UINT& nError)
{
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "TYPE", val) )
	{
		str << "Drawing object type not set (tag <TYPE>)" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}

	AssocFSDIMap::const_iterator fnd = m_fsdiCreators.find(val.c_str()); 

	if ( fnd != m_fsdiCreators.end() )
		return (fnd->second)(pNode, parentLayer, nError);
	else
	{		
		str << "Unknown drawing object type [" << val.c_str() <<"]" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str());
		return  NULL;
	}	
}

bool XMLObjectFactory::RegisterConverter(std::string id, LPCREATECONVERTERFROMXML creator)
{
	return m_convCreators.insert(std::make_pair(id, creator)).second;
}

BaseConverter* XMLObjectFactory::CreateConverterObject(IXMLDOMNode* pNode, UINT& nError)
{
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "TYPE", val) )
	{
		str << "Converter type not set (tag <TYPE>)" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}

	AssocConverterMap::const_iterator fnd = m_convCreators.find(val.c_str()); 

	if ( fnd != m_convCreators.end() )
		return (fnd->second)(pNode, nError);
	else
	{
		str << "Unknown converter type [" << val.c_str() <<"]" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}
}


bool XMLObjectFactory::RegisterTimer(std::string id, LPCREATETIMERFROMXML creator)
{
	return m_timerCreators.insert(std::make_pair(id, creator)).second;
}

BaseTimer* XMLObjectFactory::CreateTimerObject(IXMLDOMNode* pNode, UINT& nError)
{
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	if ( !appXML.GetNodeAttributeByName(pNode, "TYPE", val) )
	{
		str << "Timer type not set (tag <TYPE>)" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}

	AssocTimerMap::const_iterator fnd = m_timerCreators.find(val.c_str()); 

	if ( fnd != m_timerCreators.end() )
		return (fnd->second)(pNode, nError);
	else
	{
		str << "Unknown filter type [" << val.c_str() <<"]" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}
}

bool XMLObjectFactory::RegisterFilter(std::string id, LPCREATEFILTERFROMXML creator)
{
	return m_filterCreators.insert(std::make_pair(id, creator)).second;
}

BaseFilter* XMLObjectFactory::CreateFilterObject(IXMLDOMNode* pNode, UINT& nError)
{
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "TYPE", val) )
	{
		str << "Filter type not set (tag <TYPE>)" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}

	AssocFilterMap::const_iterator fnd = m_filterCreators.find(val.c_str()); 

	if ( fnd != m_filterCreators.end() )
		return (fnd->second)(pNode, nError);
	else
	{
		str << "Unknown filter type [" << val.c_str() <<"]" << ends;
		err.SetLastError(DSCE_UNKNOWN_TYPE, str.str() );
		return NULL;
	}
}