#include "stdafx.h"
#include "DataSrc.h"
#include "Timers.h"
#include "DeviceFactory.h"
#include "DataCollector.h"

#include "..\winmessages.h"
#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/********************************************************/
/*				Класс DataSrc							*/
/*		базовый класс для реализации объектов типа		*/
/*		источник данных изменяющихся во времени			*/
/********************************************************/

//инициализация хранилища всех источников данных c доступом по имени
std::map<std::string, DataSrc*>		DataSrc::m_mapDS; 

//инициализация хранилища всех источников данных c доступом по индексу
std::vector<DataSrc*> DataSrc::m_vecDS;

//инициализация журнала 
CMessageLog DataSrc::logDS;

//Возможность установления начальных и конечных состояний
bool DataSrc::bUseInitState(false);


DataSrc::DataSrc() : m_outColor(0x000000), m_prior(100)	, m_bRestored(false), m_bCollected(false)
{
	::InitializeCriticalSection(&m_dataCS);
}

DataSrc::~DataSrc()
{
	::DeleteCriticalSection(&m_dataCS);
}


//Получение источника данных по строковому идентификатору
DataSrc* DataSrc::GetDataSource(std::string name)
{
	std::map<std::string, DataSrc*>::const_iterator fnd =  m_mapDS.find( name );
	DataSrc* pDS = NULL;

	if ( fnd != m_mapDS.end() )
		pDS = (*fnd).second;
	
	return pDS;
}

//удаление всех источников данных
void DataSrc::ClearAll()
{
	for(std::map<std::string, DataSrc*>::iterator it = m_mapDS.begin(); it != m_mapDS.end(); ++it )
		delete (*it).second;

	m_mapDS.clear();
}

//создание источника данных из XML узла 
bool DataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (pNode == NULL)
		return FALSE;

	std::string sAtr;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();

	//Имя источника данных для отображения  
	if ( (!appXML.GetNodeAttributeByName( pNode, "NAME", sAtr)) || sAtr.length() == 0 )
	{
		err.SetLastError(DSCE_NAME_NOT_SET, "Name not set for data source");
		return false;
	}
	std::string name = sAtr;

	//тип источника данных
	if ( (!appXML.GetNodeAttributeByName( pNode, "TYPE", sAtr)) || sAtr.length() == 0 )
	{
		err.SetLastError(DSCE_TYPE_NOT_SET, "Type not set for data source");
		return false;
	}
	std::string type = sAtr;

	//Описание источника данных для отображения  
	CString desc;

	USES_CONVERSION;
	CComBSTR txt;
	pNode->get_text(&txt);
	desc = W2A(txt);
	desc = desc.Trim();
	desc.Replace('\n',' ');
	desc.Replace('\t',' ');

	//Цвет  источника данных для отображения  на графиках
	COLORREF color = 0x000000;
	if ( appXML.GetNodeAttributeByName( pNode, "COLOR", sAtr) )
		color = strtol(sAtr.c_str(), 0, 0);

	//Приоритет доступа
	unsigned int pr = 10;
	if ( appXML.GetNodeAttributeByName( pNode, "PRIORITY", sAtr) )
		pr = atoi(sAtr.c_str());

	//флаг восстановления состояния из реестра при создании
	bool rest = false;
	if ( appXML.GetNodeAttributeByName( pNode, "RESTORED", sAtr) )
		rest = atoi(sAtr.c_str()) > 0;

	bool collect = false;
	if ( appXML.GetNodeAttributeByName( pNode, "COLLECTED", sAtr) )
		rest = atoi(sAtr.c_str()) > 0;

	m_bCollected	 = collect;
	m_prior			 = pr;
	m_strName		 = name;	
	m_strDescription = desc;
	m_outColor		 = color;
	m_bRestored		 = rest;

	//TRACE("\n%s", m_strDescription.c_str());

	return true;
}

//сохранение в XML
bool DataSrc::SaveToXMLNode(IXMLDOMNode* pNode)
{
	/*
	if (pNode == NULL)
		return false;

	std::string sAtr;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();
	
	//appXML.SaveDataSourceToXMLNode()
	*/
	return false;

}

//регистрация источника данных
bool DataSrc::AddDataSource(DataSrc* pDS)
{
	std::strstream str;
	std::string id = pDS->Name();

	if ( id.empty() )
	{
		AppErrorsHelper::Instance().SetLastError(DSCE_EMPTY_PARAMETR, "Пустой идентификатор");
		return false;
	}

	std::map<std::string, DataSrc*>::const_iterator fnd;
	fnd =  m_mapDS.find( id );

	if ( fnd != m_mapDS.end() )
	{
		str << "Data source with name (" << id.c_str() <<") already exist" << std::ends;
		AppErrorsHelper::Instance().SetLastError(DSCE_DUPLICATE_NAME, str.str() );
		return false;
	}

	bool res =  m_mapDS.insert( std::make_pair(id, pDS) ).second;

	if ( res )
		m_vecDS.push_back(pDS);

	return res; 
}

//
bool DataSrc::IsCollected() const
{
	return m_bCollected;
}

//включить ведение журнала
bool DataSrc::EnableLog(const char *pszHomePath/* = "LOG//DS//"*/, bool bClear/* = false*/)
{
	//Создание файла отчета для источников данных
	if ( !logDS.IsReadyToUse() )
		return logDS.Create(pszHomePath, bClear);

	return false;
}

//записать сообщение в журнал
bool DataSrc::LogMessage(const char *pszMessage)
{
	if ( logDS.IsReadyToUse() )
	{
		logDS.LogMessage(pszMessage);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Атрибуты
#pragma region Атрибуты
 
// Имя источника данных (для сохранения в файл)
std::string DataSrc::Name()	const	
{	
	return m_strName;
}

// Описание источника данных (для информативности)
std::string DataSrc::Description() const 
{	
	return m_strDescription; 
} 

// Цвет отображения на графиках 
COLORREF DataSrc::Color() const 
{   
	return m_outColor; 
}

//Приоритет доступа к источнику данных
unsigned int DataSrc::Permission() const 
{
	return m_prior;
}
#pragma endregion


//////////////////////////////////////////////////////////////////////////
//СТАТИЧЕСКИЕ ФУНКЦИИ ДЛЯ РАБОТЫ СО ВСЕМИ ЗАРЕГЕСТРИРОВАННЫМИ ИСТОЧНИКАМИ ДАННЫХ

#pragma  region СТАТИЧЕСКИЕ ФУНКЦИИ

//Общее количество зарегестрированных источников данных
int DataSrc::Count()
{ 
	return (int)m_mapDS.size(); 
} 

//поиск необходимых источников данных по параметрам
int DataSrc::EnumDataSources(vector<const DataSrc*>& ds, LPDATA_SOURCES_ENUM_PARAMS pdsep)
{
	ds.clear();

	//если не заданы параметры поиска добавляем все 
	if ( pdsep == NULL )
	{
		ds.assign(m_vecDS.begin(), m_vecDS.end());
		return (int)ds.size();
	}

	for(std::map<std::string, DataSrc*>::iterator it = m_mapDS.begin(); it != m_mapDS.end(); it++)
	{

		bool bAdd = false;

		if ( (pdsep->mask & DSEM_TYPE) == DSEM_TYPE )
		{
			bool bUseDevName = ((pdsep->mask & DSEM_DEV) == DSEM_DEV);
			bool bUseChannelNo = ((pdsep->mask & DSEM_WIRE) == DSEM_WIRE);

			if( pdsep->dstype == DS_ALL )
				bAdd = true;
			else if ( ((pdsep->dstype & DS_ANALOG) == DS_ANALOG) && (dynamic_cast<AnalogDataSrc*>( (*it).second) != NULL) )
				bAdd = true;
			else if ( ((pdsep->dstype & DS_DISCRETE) == DS_DISCRETE) && (dynamic_cast<DiscreteDataSrc*>( (*it).second) != NULL) )
				bAdd = true;
			else if ( (pdsep->dstype & DS_AI) == DS_AI ) 
			{
				bAdd = true;
				AnalogInputDataSrc* pDS = dynamic_cast<AnalogInputDataSrc*>( (*it).second );
				if ( pDS != NULL )
				{
					IAnalogInput* pDev = dynamic_cast<IAnalogInput*>(IdentifiedPhysDevice::GetDevice(pdsep->device));
					if ( bUseDevName && ( pDS->GetParentDevice() != pDev ) )
						bAdd = false;
					else if ( bUseChannelNo && (pDS->GetChannelNo() != pdsep->wire)  )
						bAdd = false;
				}
				else
					bAdd = false;
			}
			else if ( (pdsep->dstype & DS_AO) == DS_AO ) 
			{		
				bAdd = true;
				AnalogOutputDataSrc* pDS = dynamic_cast<AnalogOutputDataSrc*>( (*it).second );
				
				if ( pDS != NULL )
				{
					IAnalogOutput* pDev = dynamic_cast<IAnalogOutput*>(IdentifiedPhysDevice::GetDevice(pdsep->device));
					if ( bUseDevName && ( pDS->GetParentDevice() != pDev ) )
						bAdd = false;
					else if ( bUseChannelNo && (pDS->GetChannelNo() != pdsep->wire)  )
						bAdd = false;						
				}
				else
					bAdd = false;
			}		

			else if ( (pdsep->dstype & DS_DI) == DS_DI ) 
			{

				bAdd = true;
				DiscreteInputDataSrc* pDS = dynamic_cast<DiscreteInputDataSrc*>( (*it).second );
				
				if ( pDS != NULL )
				{
					IDigitalInput* pDev = dynamic_cast<IDigitalInput*>(IdentifiedPhysDevice::GetDevice(pdsep->device));
					if ( bUseDevName && (pDS->GetParentDevice() != pDev) )
						bAdd = false;
					else if ( bUseChannelNo && (pDS->GetChannelNo() != pdsep->wire)  )
						bAdd = false;						
				}
				else
					bAdd = false;
			}
			else if ( (pdsep->dstype & DS_DO) == DS_DO ) 
			{
				bAdd = true;
				DiscreteOutputDataSrc* pDS = dynamic_cast<DiscreteOutputDataSrc*>( (*it).second );
				
				if ( pDS != NULL )
				{
					IDigitalOutput* pDev = dynamic_cast<IDigitalOutput*>(IdentifiedPhysDevice::GetDevice(pdsep->device));
					if ( bUseDevName && ( pDS->GetParentDevice() != pDev ) )
						bAdd = false;
					else if ( bUseChannelNo && ( pDS->GetChannelNo() != pdsep->wire)  )
						bAdd = false;						
				}
				else 
					bAdd = false;
			}
			else if ( (pdsep->dstype & DS_TIMERS) == DS_TIMERS && dynamic_cast<BaseTimer*>( (*it).second ) != NULL )
				bAdd = true;
			
			if ( (pdsep->dstype & DS_COLLECTED) == DS_COLLECTED )
				bAdd = (*it).second->IsCollected(); 			

		}

		if ( bAdd )
			ds.push_back((*it).second);
	}
	return (int)ds.size();
}


//Поиск источников данных связаных с устройством с несколькими аналоговыми входами
//pDev - указатель на устройство
//channel - номер канала устройства с которым связан истчник данных (если -1 то поиск по всем каналам )
int DataSrc::FindAnalogInputDataSrc(IAnalogInput* pDev, unsigned char channel, std::vector<AnalogInputDataSrc*> &ds )
{
	ds.clear();

	for (std::map<std::string, DataSrc*>::const_iterator it = m_mapDS.begin(); it != m_mapDS.end(); it++ )
	{
		AnalogInputDataSrc* pDS = dynamic_cast<AnalogInputDataSrc*>( (*it).second );
		
		if ( pDS == NULL )
			continue;
		
		if ( pDS->GetParentDevice() == pDev )
		{
			if ( channel == -1 )
				ds.push_back(pDS);
			else if ( pDS->GetChannelNo() == channel )
				ds.push_back(pDS);				
		}
	}	

	return (int)ds.size();
}

//Поиск источников данных связаных с устройством с несколькими аналоговыми выходами
//pDev - указатель на устройство
//channel - номер канала устройства с которым связан истчник данных (если -1 то поиск по всем каналам )
int DataSrc::FindAnalogOutputDataSrc(IAnalogOutput* pDev, unsigned char channel, std::vector<AnalogOutputDataSrc*> &ds )
{
	ds.clear();

	for (std::map<std::string, DataSrc*>::const_iterator it = m_mapDS.begin(); it != m_mapDS.end(); it++ )
	{
		AnalogOutputDataSrc* pDS = dynamic_cast<AnalogOutputDataSrc*>( (*it).second );

		if ( pDS == NULL )
			continue;

		if ( pDS->GetParentDevice() == pDev )
		{
			if ( channel == -1 )
				ds.push_back(pDS);
			else if ( pDS->GetChannelNo() == channel )
				ds.push_back(pDS);				
		}
	}	

	return (int)ds.size();
}

//Поиск источников данных связаных с устройством с несколькими цифровыми входами
//pDev - указатель на устройство
//channel - номер канала устройства с которым связан истчник данных (если -1 то поиск по всем каналам )
int DataSrc::FindDigitalInputDataSrc(IDigitalInput* pDev, unsigned char channel, std::vector<DiscreteInputDataSrc*> &ds )
{
	ds.clear();

	for (std::map<std::string, DataSrc*>::const_iterator it = m_mapDS.begin(); it != m_mapDS.end(); it++ )
	{
		DiscreteInputDataSrc* pDS = dynamic_cast<DiscreteInputDataSrc*>( (*it).second );

		if ( pDS == NULL )
			continue;

		if ( pDS->GetParentDevice() == pDev )
		{
			if ( channel == -1 )
				ds.push_back(pDS);
			else if ( pDS->GetChannelNo() == channel )
				ds.push_back(pDS);				
		}
	}
	return (int)ds.size();
}

//Поиск источников данных связаных с устройством с несколькими цифровыми выходами
//pDev - указатель на устройство
//channel - номер канала устройства с которым связан истчник данных (если -1 то поиск по всем каналам )
int DataSrc::FindDigitalOutputDataSrc(IDigitalOutput* pDev, unsigned char channel, std::vector<DiscreteOutputDataSrc*> &ds )
{
	ds.clear();

	for (std::map<std::string, DataSrc*>::const_iterator it = m_mapDS.begin(); it != m_mapDS.end(); it++ )
	{
		DiscreteOutputDataSrc* pDS = dynamic_cast<DiscreteOutputDataSrc*>( (*it).second );

		if ( pDS == NULL )
			continue;

		if ( pDS->GetParentDevice() == pDev )
		{
			if ( channel == -1 )
				ds.push_back(pDS);
			else if ( pDS->GetChannelNo() == channel )
				ds.push_back(pDS);				
		}
	}	

	return (int)ds.size();
}
#pragma endregion

/********************************************************/
/*				Класс AnalogDataSrc						*/
/*		базовый класс для реализации объектов типа		*/
/*		источник данных аналоговой величины     		*/
/********************************************************/
AnalogDataSrc::AnalogDataSrc() : 
	m_lastValue(UNKNONW_VALUE), 
	m_minValue(0.0), 
	m_maxValue(0.0)	
{ 

}

AnalogDataSrc::~AnalogDataSrc() 
{
}

// Единицы измерениия  (для информативности)
std::string AnalogDataSrc::Units()		  const	
{	
	return m_strUnits; 
}		

// Формат вывода данных
std::string AnalogDataSrc::OutFormat()	  const	
{	
	return m_strOutFormat; 
}

//Возвращает минимально возможное значение аналоговой величины в единицах пользователя
double AnalogDataSrc::GetMin() const 
{ 
	return m_minValue; 
}

//Возвращает максимально возможное значение аналоговой величины в единицах пользователя
double AnalogDataSrc::GetMax() const 
{ 
	return m_maxValue; 
}

//Получить диапазон значений управляемой величины
void AnalogDataSrc::GetRange(double& minV, double& maxV) const
{
	minV = m_minValue; 
	maxV = m_maxValue;
}

//создание источника данных из XML узла
bool AnalogDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if( !DataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::string sAtr;
	
	//сообщение об ошибке
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	CComPtr<IXMLDOMNodeList> pConverters;
	HRESULT hr = pNode->selectNodes(CComBSTR("CONVERT"), &pConverters);
	
	if ( SUCCEEDED(hr) && (pConverters != NULL))
	{
		long lConvCount = 0;

		pConverters->get_length(&lConvCount);
		pConverters->reset();

		for (long num = 0; num < lConvCount; num++)
		{
			CComPtr<IXMLDOMNode> pConvNode;
			pConverters->get_item(num, &pConvNode);

			std::string id;
			if ( appXML.GetNodeAttributeByName( pConvNode, "CONVID", sAtr) )
				id = sAtr;

			const BaseConverter* pConv = BaseConverter::GetConverter(id);

			if ( pConv == NULL )
			{
				str << "Вторичный преоборазователь (" << id.c_str() <<") не  найден" << std::ends;
				err.SetLastError(DSCE_INVALID_CONVERTER_IN_FORWARD_CHAIN, str.str() );
				m_vConverters.clear();
				return false;
			}
			m_vConverters.push_back(pConv);
		}
	}
	
	//Единицы измерения  для отображения  
	std::string units;
	if (appXML.GetNodeAttributeByName( pNode, "UNITS", sAtr) )
		units = sAtr;

	//формат вывода
	std::string outFmt = "%8.2f";
	if (appXML.GetNodeAttributeByName( pNode, "FMT", sAtr))
		outFmt = sAtr;

	//Минимально возможное значение аналоговой величины
	double minv = 0.0;
	if ( appXML.GetNodeAttributeByName( pNode, "MINV", sAtr) )
		minv = atof(sAtr.c_str());

	//Максимально возможное значение аналоговой величины
	double maxv = 100.0;
	if ( appXML.GetNodeAttributeByName( pNode, "MAXV", sAtr) )
		maxv = atof(sAtr.c_str());

	bool collect = true;
	if ( appXML.GetNodeAttributeByName( pNode, "COLLECTED", sAtr) )
		collect = atoi(sAtr.c_str()) > 0;


	/*if ( minv > maxv )
	{
		str << "Invalid analog value range [" << minv << ";" << maxv << "]"<< std::ends;
		err.SetLastError(DSCE_INVALID_ADS_RANGE, str.str() );
		return false;
	}*/

	m_bCollected = collect;
	m_minValue = minv;
	m_maxValue = maxv; 
	m_strUnits = units;
	m_strOutFormat = outFmt;

	return true;
}


/************************************************************************/
/*              Класс AnalogTimeAverageDataSrc                          */
/************************************************************************/

AnalogTimeAverageDataSrc::AnalogTimeAverageDataSrc()
{

}
AnalogTimeAverageDataSrc::~AnalogTimeAverageDataSrc()
{

}


	//создание источника данных из XML узла
AnalogTimeAverageDataSrc* AnalogTimeAverageDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<AnalogTimeAverageDataSrc> obj ( new AnalogTimeAverageDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

//создание источника данных из XML узла
bool AnalogTimeAverageDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !AnalogDataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::string sAtr;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	std::strstream str;
	
	//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
	if ( !appXML.GetNodeAttributeByName(pNode, "DSID", sAtr) )
	{
		err.SetLastError(DSCE_EMPTY_PARAMETR, "Data source name not set." );
		return false;

	}

	DataSrc* pDS = GetDataSource(sAtr);

	if ( pDS == NULL)
	{
		str << "Data source ["<< sAtr << "] not found." << ends;
		err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
		return false;
	}

	if ( dynamic_cast<AnalogDataSrc*>(pDS) == NULL)
	{
		str << "Provided data source ["<< sAtr << "] is not analog one." << ends;
		err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
		return false;
	}

	//имя источника данных для усреднения
	dsid = sAtr;


	if ( !appXML.GetNodeAttributeByName(pNode, "TWND", sAtr) )
	{
		err.SetLastError(DSCE_EMPTY_PARAMETR, "Time window not set." );
		return false;
	}	
	
	//время усреднения
	timeWndSize = atoi(sAtr.c_str());

	if ( timeWndSize <= 0 )
	{
		str << "Invalid time window size ["<< sAtr << "] ." << ends;
		err.SetLastError(DSCE_INVALID_PARAMETR, str.str() );
		return false;
	}

	return true;
}

//Получить значение источника в единицах пользователя
double AnalogTimeAverageDataSrc::GetValue() const
{
	vector<double> data;
	DataCollector::Instance().GetDataSourceBuffer(dsid, data, timeWndSize);
	double val = accumulate(data.begin(), data.end(), 0.0);
	m_lastValue = val / data.size();
	return m_lastValue;
}

//Получить значение источника в единицах устройства
double AnalogTimeAverageDataSrc::GetSignal() const
{
	return m_lastValue;
}
	

/************************************************************************/
/*              Класс AnalogInputDataSrc                                */
/************************************************************************/

AnalogInputDataSrc::AnalogInputDataSrc() :
pAnalogInputDevice(NULL), 
channel(-1), 
pFilter(NULL)
{

}

AnalogInputDataSrc::~AnalogInputDataSrc() 
{
	if (pFilter != NULL)
		delete pFilter;
}

//Получить номер канала c которым связан источник 
unsigned char AnalogInputDataSrc::GetChannelNo() const 
{ 
	return channel; 
}

//Получить устройство с которым связан источник  данных
IAnalogInput* AnalogInputDataSrc::GetParentDevice() const
{
	return pAnalogInputDevice;
}

//создание источника данных из XML узла
AnalogInputDataSrc* AnalogInputDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<AnalogInputDataSrc> obj ( new AnalogInputDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool AnalogInputDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !AnalogDataSrc::CreateFromXMLNode( pNode ) )
		return false;
	
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;

	if (!appXML.GetNodeAttributeByName(pNode, "DEV", sAtr) )
	{
		str << "Device must be set for this type data source" << std::ends;		
		err.SetLastError(DSCE_DEVICE_NOT_SET, str.str() );
		return false;
	}
	
	IAnalogInput* pDev = dynamic_cast<IAnalogInput*>(IdentifiedPhysDevice::GetDevice(sAtr));

	if ( pDev == NULL )
	{
		str << "Analog input device ["<< sAtr <<"] not found" << std::ends;
		err.SetLastError(DSCE_INVALID_DEVICE, str.str() );
		return false;
	}

	if ( !appXML.GetNodeAttributeByName(pNode, "WIRE", sAtr) )
	{		
		str << "Channel number must be set for this type data source"  << std::ends;
		err.SetLastError(DSCE_CHANNEL_NOT_SET, str.str() );
		return false;
	}
	
	int wire = atoi(sAtr.c_str());

	if ( (wire < 0 )|| (wire >= pDev->GetChannelsCountAI()) )
	{
		str << "Invalid channel number [" << wire << "]"<< std::ends;		
		err.SetLastError(DSCE_INVALID_CHANNEL, str.str() );
		return false;
	}

	pAnalogInputDevice = pDev;
	channel = wire;	

	MEASSURE_RANGE_INFO mri;
	pAnalogInputDevice->GetInputRangeInfo(channel, &mri);


	if ( appXML.GetNodeAttributeByName(pNode, "MINV", sAtr) )
		m_minValue = atof(sAtr.c_str());
	else
		m_minValue = mri.mins;

	if ( appXML.GetNodeAttributeByName(pNode, "MAXV", sAtr) )
		m_maxValue = atof(sAtr.c_str());
	else
		m_maxValue = mri.maxs;

	CComPtr<IXMLDOMNode> pFilterNode;
	HRESULT hr = pNode->selectSingleNode(CComBSTR("FILTER"), &pFilterNode);

	if ( SUCCEEDED(hr) && (pFilterNode != NULL) )		
	{
		UINT error;
		pFilter = XMLObjectFactory::CreateFilterObject(pFilterNode, error);

		if (pFilter == NULL)
			return false;
		else
			const_cast<BaseFilter*>(pFilter)->InitFilter(this);		
	}	

	std::vector<const BaseConverter *>::const_iterator it = m_vConverters.begin();

	for (it; it != m_vConverters.end(); it++)
	{
		m_minValue = (*it)->Convert(m_minValue);
		m_maxValue = (*it)->Convert(m_maxValue);
	}

	return true;
}


//получить значение источника с преобразованием и фильтрацией
double AnalogInputDataSrc::GetValue() const
{
	//получаем значение 
	double val = m_lastValue = GetClearValue();	
		
	//если подключен фильтр коррректируем значение
	if ( pFilter != NULL )
		val = pFilter->Correct(val);
	
	return val;
}


//получить значение источника с преобразованием, но без использования фильтрации
double AnalogInputDataSrc::GetClearValue() const
{
	double val = m_lastValue = pAnalogInputDevice->GetChannelData(channel);
	std::vector<const BaseConverter *>::const_iterator it = m_vConverters.begin();

	for (it; it != m_vConverters.end(); it++)
		val = (*it)->Convert(val);

	return val;
}

//Получить значение с аналогового входа в единицах устройства
double AnalogInputDataSrc::GetSignal() const
{
	return pAnalogInputDevice->GetChannelData(channel);
}


/************************************************************************/
/*              Класс AnalogOutputDataSrc                               */
/************************************************************************/

AnalogOutputDataSrc::AnalogOutputDataSrc() :
pAnalogOutputDevice(NULL), 
channel(-1), dSafeState(0.0), useSafe(false)
{
}

AnalogOutputDataSrc::~AnalogOutputDataSrc()
{
	if (useSafe)
		SetValue(dSafeState);
}


//Получить номер канала c которым связан источник 
unsigned char AnalogOutputDataSrc::GetChannelNo() const 
{ 
	return channel; 
}

//Получить устройство с которым связан источник  данных
IAnalogOutput* AnalogOutputDataSrc::GetParentDevice() const
{
	return pAnalogOutputDevice;
}

//  [5/15/2010 Johnny A. Matveichik]
//Добавление обработчика 
void AnalogOutputDataSrc::AddOnOutputValueChange(boost::function< void (const AnalogOutputDataSrc*) > handler)
{
	evOnValueChange.connect(handler);
}

bool AnalogOutputDataSrc::AddUpdateWnd(HWND hWnd)
{
	updateWnds.push_back(hWnd);
	return true;
}

//создание источника данных из XML узла
AnalogOutputDataSrc* AnalogOutputDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<AnalogOutputDataSrc> obj ( new AnalogOutputDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}


//создание источника данных из XML узла
bool AnalogOutputDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !AnalogDataSrc::CreateFromXMLNode( pNode ) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;

	CComPtr<IXMLDOMNodeList> pConverters;
	HRESULT hr = pNode->selectNodes( CComBSTR("RCONVERT"), &pConverters );

	if ( SUCCEEDED(hr) && (pConverters != NULL))
	{
		long lConvCount = 0;

		pConverters->get_length(&lConvCount);
		pConverters->reset();

		for (long num = 0; num < lConvCount; num++)
		{
			CComPtr<IXMLDOMNode> pConvNode;
			pConverters->get_item(num, &pConvNode);

			std::string id;
			if ( appXML.GetNodeAttributeByName( pConvNode, "CONVID", sAtr) )
				id = sAtr;

			const BaseConverter* pConv = BaseConverter::GetConverter(id);

			if ( pConv == NULL )
			{
				str << "Вторичный преоборазователь (" << id.c_str() <<") не  найден" << std::ends;
				err.SetLastError(DSCE_INVALID_CONVERTER_IN_REVERSE_CHAIN, str.str() );
				m_vRevConverters.clear();
				return false;
			}
			m_vRevConverters.push_back(pConv);
		}
	}

	if (!appXML.GetNodeAttributeByName(pNode, "DEV", sAtr) )
	{
		str << "Device must be set for this type data source" << std::ends;		
		err.SetLastError(DSCE_DEVICE_NOT_SET, str.str() );
		return false;
	}

	IAnalogOutput* pDev = dynamic_cast<IAnalogOutput*>(IdentifiedPhysDevice::GetDevice(sAtr));

	if ( pDev == NULL )
	{
		str << "Analog output device ["<< sAtr <<"] not found" << std::ends;
		err.SetLastError(DSCE_INVALID_DEVICE, str.str() );
		return false;
	}

	if ( !appXML.GetNodeAttributeByName(pNode, "WIRE", sAtr) )
	{		
		str << "Channel number must be set for this type data source"  << std::ends;
		err.SetLastError(DSCE_CHANNEL_NOT_SET, str.str() );
		return false;
	}

	int wire = atoi(sAtr.c_str());

	if ( (wire < 0 )|| (wire >= pDev->GetChannelsCountAO()) )
	{
		str << "Invalid channel number [" << (int)wire << " ] must be between 0 and " << pDev->GetChannelsCountAO() << std::ends;		
		err.SetLastError(DSCE_INVALID_CHANNEL, str.str() );
		return false;
	}

	pAnalogOutputDevice = pDev;
	channel = wire;
	
	//pAnalogOutputDevice->GetMinMax(channel, m_minValue, m_maxValue);	

	MEASSURE_RANGE_INFO mri;
	pAnalogOutputDevice->GetOutputRangeInfo(channel, &mri);	

	if ( appXML.GetNodeAttributeByName(pNode, "MINV", sAtr) )
		m_minValue = atof(sAtr.c_str());
	else
		m_minValue = mri.mins;

	if ( appXML.GetNodeAttributeByName(pNode, "MAXV", sAtr) )
		m_maxValue = atof(sAtr.c_str());
	else
		m_maxValue = mri.maxs;

	dSafeState = m_minValue;	
	if ( appXML.GetNodeAttributeByName(pNode, "SAFE", sAtr) )
	{
		useSafe = true;
		dSafeState = atof(sAtr.c_str());	
	}

	bool bUseDef = false;
	double dInit = 0.0;
	if ( appXML.GetNodeAttributeByName(pNode, "DEFAULT", sAtr) )
	{	
		bUseDef = true;		
		dInit = atof(sAtr.c_str());
		SetValue(dInit);
	}

	restoreState();

	return true;
}

bool AnalogOutputDataSrc::saveState() const
{
	if (!m_bRestored)
		return true;

	CWinApp* pApp = AfxGetApp();

	CString strSection = _T("Data sources");
	CString strItem    = m_strName.c_str();
	CString strValue;
	strValue.Format("%g", GetValue());

	if ( !pApp->WriteProfileString(strSection, strItem, strValue) )
		return false;

	return true;
}


void AnalogOutputDataSrc::restoreState()
{
	if (!m_bRestored)
		return;

	CWinApp* pApp = AfxGetApp();

	CString strSection = _T("Data sources");
	CString strItem    = m_strName.c_str();
	CString strValue;
	strValue = pApp->GetProfileString(strSection, strItem, "0.0");
	
	double val = atof(strValue);
	SetValue(val);
}

double AnalogOutputDataSrc::SetValue(double val) const
{
	SYSTEMTIME st1;
	SYSTEMTIME st2;	
	std::strstream str;

	::GetLocalTime(&st1);

	str << std::left << std::setw(16) <<	m_strName.c_str() << std::setw(64) << m_strDescription.c_str();
	str << std::fixed << std::setprecision(2);

	double oldVal = pAnalogOutputDevice->GetChannelStateAO (channel);

	str << "\tOld : " << oldVal << "(";

	for (std::vector<const BaseConverter *>::const_iterator it = m_vConverters.begin(); it != m_vConverters.end(); it++)
		oldVal = (*it)->Convert(oldVal);

	str << oldVal << ")\t" << "New : " << val << "(";

	for (std::vector<const BaseConverter *>::const_iterator it = m_vRevConverters.begin(); it != m_vRevConverters.end(); it++)
		val = (*it)->Convert(val);	

	str << val << ")\t";

	if ( val < m_minValue) 
	{
		str << " below the lower limit. Change to " << m_minValue;
		val = m_minValue;
	}

	if ( val > m_maxValue) 
	{
		str << " more the upper limit. Change to " << m_maxValue;
		val = m_maxValue;
	}


	pAnalogOutputDevice->WriteToChannel(channel, val);
	
	saveState();

	//TRACE("\n\t > evOnValueChange called...");
	if (oldVal != val )
	{
		//используе сигналы boost
		//evOnValueChange(const_cast<AnalogOutputDataSrc*>(this));

		//используем посылку сообщений
		for (std::vector<HWND>::const_iterator it = updateWnds.begin(); it != updateWnds.end(); ++it)
			::SendMessage((*it), RM_ANALOG_VALUE_CHANGE, 0, (LPARAM)this);
	}
	

	::GetLocalTime(&st2);
	str << Milliseconds(st1, st2) << " ms" << std::ends;

	logDS.LogMessage( str.str() );

	return oldVal;
}

double AnalogOutputDataSrc::GetValue() const
{
	double val = pAnalogOutputDevice->GetChannelStateAO (channel);

	for (std::vector<const BaseConverter *>::const_iterator it = m_vConverters.begin(); it != m_vConverters.end(); it++)
		val = (*it)->Convert(val);

	
	if ( m_lastValue != val)
	{
		//используем посылку сообщений
		for (std::vector<HWND>::const_iterator it = updateWnds.begin(); it != updateWnds.end(); ++it)
			::SendMessage((*it), RM_ANALOG_VALUE_CHANGE, 0, (LPARAM)this);

		m_lastValue = val;
	}
	
	return val;
}

//Получить текущее значение управляемой величины в единицах устройства	
double AnalogOutputDataSrc::GetSignal() const
{
	return pAnalogOutputDevice->GetChannelStateAO (channel);
}

void AnalogOutputDataSrc::GetRange(double& minV, double& maxV) const
{
	MEASSURE_RANGE_INFO msri;
	pAnalogOutputDevice->GetOutputRangeInfo(channel, &msri);
	
	minV = msri.mins;  
	maxV = msri.maxs;

	minV = m_minValue;  
	maxV = m_maxValue;

	for (std::vector<const BaseConverter *>::const_iterator it = m_vConverters.begin(); it != m_vConverters.end(); it++)
	{
		minV = (*it)->Convert(minV);
		maxV = (*it)->Convert(maxV);
	}
}
//////////////////////////////////////////////////////////////////////////
//  [6/20/2017 Johnny A. Matveichik]
//  Класс AnalogOutputListDataSrc для задания аналоговой величины по предустановленным значениям
//////////////////////////////////////////////////////////////////////////

AnalogOutputListDataSrc::AnalogOutputListDataSrc() : valuesPresition(0)
{
}

AnalogOutputListDataSrc::~AnalogOutputListDataSrc()
{
}

//создание источника данных из XML узла
AnalogOutputListDataSrc* AnalogOutputListDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<AnalogOutputListDataSrc> obj ( new AnalogOutputListDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

//создание источника данных из XML узла
bool AnalogOutputListDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !AnalogOutputDataSrc::CreateFromXMLNode( pNode ) )
		return false;

	//vector<double> presetValues;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;


	CComPtr<IXMLDOMNodeList> pValues;
	HRESULT hr = pNode->selectNodes( CComBSTR("VALUE"), &pValues );

	if ( SUCCEEDED(hr) && (pValues != NULL))
	{
		long lValVount = 0;

		pValues->get_length(&lValVount);
		pValues->reset();

		for (long num = 0; num < lValVount; num++)
		{
			CComPtr<IXMLDOMNode> pValNode;
			pValues->get_item(num, &pValNode);

			ANALOG_VALUE_INFO avi;
			if ( appXML.GetNodeAttributeByName( pValNode, "VAL", sAtr) )
				avi.Value = atof(sAtr.c_str());
			
			USES_CONVERSION;
			CComBSTR txt;
			pValNode->get_text(&txt);
			avi.Info = W2A(txt);

			presetValues.push_back(avi);
		}
	}
	
	if ( presetValues.empty() )
	{
		if ( appXML.GetNodeAttributeByName(pNode, "VALUES", sAtr) )
		{
			vector<double> data;
			ExtractData<double>(sAtr, " ,;\t", data);
			for (auto it = data.begin(); it != data.end(); ++it)
			{
				ANALOG_VALUE_INFO avi;
				avi.Value = (*it);
				presetValues.push_back(avi);
			}
		}
	}

	if (presetValues.empty())
	{
		str << "Values not set" << std::ends;		
		err.SetLastError(DSCE_INVALID_PARAMETR, str.str() );
		return false;
	}

	if ( presetValues.size() == 1 )
	{
		str << "Values list to short" << std::ends;		
		err.SetLastError(DSCE_INVALID_PARAMETR, str.str() );
		return false;
	}

	std::sort(presetValues.begin(), presetValues.end(), analog_value_comparer);
	return true;
}

bool analog_value_comparer (ANALOG_VALUE_INFO v1, ANALOG_VALUE_INFO v2) 
{ 
	return (v1.Value < v2.Value); 
}	

//Задать новое значение управляемой величины
double AnalogOutputListDataSrc::SetValue(double val) const
{
	std::vector<ANALOG_VALUE_INFO>::iterator it = std::find_if (presetValues.begin(), presetValues.end(), double_cmp(val, valuesPresition));
	
	if ( it  != presetValues.end() )
		return AnalogOutputDataSrc::SetValue(val);

	else
		return 0.0;
}

const std::vector<ANALOG_VALUE_INFO>& AnalogOutputListDataSrc::GetValues() const
{
	return presetValues;
}



/************************************************************************/
/*             Класс CompositeDataSrc                                   */
/*			[3/25/2010 Johnny A. Matveichik]							*/
/************************************************************************/

//деструктор
IComposite::~IComposite()
{

}


CompositeAnalogDataSrc::CompositeAnalogDataSrc()
{

}
CompositeAnalogDataSrc::~CompositeAnalogDataSrc()
{

}

//создание источника данных из XML узла
bool CompositeAnalogDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !AnalogDataSrc::CreateFromXMLNode( pNode ) )
		return false;

	
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	
	CComPtr<IXMLDOMNodeList>	 pItemsList;
	HRESULT hr = pNode->selectNodes(CComBSTR("ITEM"), &pItemsList);

	if ( FAILED(hr) || (pItemsList == NULL) )
	{
		err.SetLastError(DSCE_EMPTY_SUBDS_LIST, "Empty subitems list" );
		return false;
	}

	
	long subItems = 0;

	pItemsList->get_length(&subItems); 
	pItemsList->reset();

	for (long n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList->get_item(n, &pSubItem);

		std::string sAtr;
	
		//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
		if ( !appXML.GetNodeAttributeByName(pSubItem, "DSID", sAtr) )
		{
			err.SetLastError(DSCE_EMPTY_PARAMETR, "Subitem name not set." );
			return false;
			
		}
		
		DataSrc* pDS = GetDataSource(sAtr);
		
		if ( pDS == NULL)
		{
			std::strstream str;

			str << "Data source ["<< sAtr << "] not found." << ends;
			err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
			return false;
		}

		if ( dynamic_cast<AnalogDataSrc*>(pDS) == NULL)
		{
			std::strstream str;
			str << "Provided data source ["<< sAtr << "] is not analog one." << ends;
			err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
			return false;
		}

		double dMin, dMax;
		AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(pDS);
		pADS->GetRange(dMin, dMax);
		
		m_minValue = (dMin < m_minValue ) ? dMin : m_minValue;
		m_maxValue = (dMax > m_maxValue ) ? dMax : m_maxValue;

		vSubDS.push_back( pADS );
	}

	return true; 
}


//количество подисточников данных
int CompositeAnalogDataSrc::Size() const
{
	return (int)vSubDS.size();
}

// получить подисточник
const DataSrc* CompositeAnalogDataSrc::GetSubDataSrc(int ind) const
{
	return vSubDS.at(ind);
}

//Получить текущее значение величины в единицах устройства	
double CompositeAnalogDataSrc::GetSignal() const
{ 
	return GetValue(); 
}

/************************************************************************/
/*          Класс CalcMaxDataSrc                                        */
/*       вычислить сумму от источников данных							*/
/************************************************************************/
//конструктор
CalcSummDataSrc::CalcSummDataSrc()
{

}

//деструктор
CalcSummDataSrc::~CalcSummDataSrc()
{

}

CalcSummDataSrc* CalcSummDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<CalcSummDataSrc> obj ( new CalcSummDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

double CalcSummDataSrc::GetValue() const 
{ 
	m_lastValue = 0.0;
	
	for (int i = 0; i < Size(); i++)
	{
		const AnalogDataSrc* pds = dynamic_cast<const AnalogDataSrc*>( GetSubDataSrc(i) );
		m_lastValue += pds->GetValue();
	}
	
	return m_lastValue;
}

/************************************************************************/
/*          Класс CalcMaxDataSrc                                        */
/*       вычислить среднее от источников данных							*/
/************************************************************************/

CalcAverageDataSrc::CalcAverageDataSrc()
{

}

CalcAverageDataSrc::~CalcAverageDataSrc()
{

}

CalcAverageDataSrc* CalcAverageDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<CalcAverageDataSrc> obj ( new CalcAverageDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

double CalcAverageDataSrc::GetValue() const 
{ 
	double summ  = 0.0;
	double count = 0;

	for (int i = 0; i < Size(); i++)
	{
		const AnalogDataSrc* pds = dynamic_cast<const AnalogDataSrc*>(GetSubDataSrc(i));
		summ += pds->GetValue();
		count++;
	}
	
	if ( count == 0)
		return UNKNONW_VALUE;
	
	m_lastValue = summ / count;

	return m_lastValue;
}
/************************************************************************/
/*          Класс CalcMulDataSrc                                        */
/************************************************************************/
CalcMulDataSrc::CalcMulDataSrc()
{

}

CalcMulDataSrc::~CalcMulDataSrc()
{

}

CalcMulDataSrc* CalcMulDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<CalcMulDataSrc> obj ( new CalcMulDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

double CalcMulDataSrc::GetValue() const 
{ 		
	double m_lastValue = 1.0;
	for (int i = 0; i < Size(); i++)
	{
		
		const AnalogDataSrc* pds = dynamic_cast<const AnalogDataSrc*>(GetSubDataSrc(i));
		m_lastValue *= pds->GetValue();			
		
	}
	
	return m_lastValue;
} 

/************************************************************************/
/*          Класс CalcMinDataSrc                                        */
/************************************************************************/
CalcMinDataSrc::CalcMinDataSrc()
{

}

CalcMinDataSrc::~CalcMinDataSrc()
{

}

CalcMinDataSrc* CalcMinDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<CalcMinDataSrc> obj ( new CalcMinDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

double CalcMinDataSrc::GetValue() const 
{ 		
	double min = DBL_MAX;
	for (int i = 0; i < Size(); i++)
	{
		
		const AnalogDataSrc* pds = dynamic_cast<const AnalogDataSrc*>(GetSubDataSrc(i));
		double val = pds->GetValue();
		if ( val < min)
			min = val;
		
	}
	
	if ( min == DBL_MAX )
		return UNKNONW_VALUE;

	return m_lastValue = min;
} 

/************************************************************************/
/*          Класс CalcMaxDataSrc                                        */
/************************************************************************/
CalcMaxDataSrc::CalcMaxDataSrc()
{

}

CalcMaxDataSrc::~CalcMaxDataSrc()
{

}

CalcMaxDataSrc* CalcMaxDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<CalcMaxDataSrc> obj ( new CalcMaxDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}


double CalcMaxDataSrc::GetValue() const 
{ 		
	double max = DBL_MIN;
	for (int i = 0; i < Size(); i++)
	{
		const AnalogDataSrc* pds = dynamic_cast<const AnalogDataSrc*>(GetSubDataSrc(i));
		double val = pds->GetValue();
		if ( val > max)
			max = val;		
	}

	if ( max == DBL_MIN )
		return UNKNONW_VALUE;

	return m_lastValue = max;
}

/************************************************************************/
/*          Класс CalcDataSrc                                        */
/************************************************************************/
CalcDataSrc::CalcDataSrc()
{

}

CalcDataSrc::~CalcDataSrc()
{

}

CalcDataSrc* CalcDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<CalcDataSrc> obj ( new CalcDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}
//создание источника данных из XML узла
bool CalcDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !AnalogDataSrc::CreateFromXMLNode( pNode ) )
		return false;
	
	vars.clear();
	vSubDS.clear();

	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::strstream str;
	
	std::string sAtr;
	if ( !appXML.GetNodeAttributeByName(pNode, "FORMULA", sAtr) )
	{
		err.SetLastError(DSCE_EMPTY_PARAMETR, "Formula not set" );
		return false;
	}
	std::string sFormula = sAtr;

	CComPtr<IXMLDOMNodeList>	 pItemsList;
	HRESULT hr = pNode->selectNodes(CComBSTR("ITEM"), &pItemsList);

	if ( FAILED(hr) || (pItemsList == NULL) )
	{
		err.SetLastError(DSCE_EMPTY_SUBDS_LIST, "Empty subitems list" );
		return false;
	}

	long subItems = 0;

	pItemsList->get_length(&subItems); 
	pItemsList->reset();

	vars.assign(subItems, 0.0 );

	for (long n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList->get_item(n, &pSubItem);

		//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
		if ( !appXML.GetNodeAttributeByName(pSubItem, "DSID", sAtr) )
		{
			err.SetLastError(DSCE_EMPTY_PARAMETR, "Subitem name not set." );
			return false;
			
		}
		
		DataSrc* pDS = GetDataSource(sAtr);
		
		if ( pDS == NULL)
		{
			str << "Data source ["<< sAtr << "] not found." << ends;
			err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
			return false;
		}

		/*AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(pDS);

		if ( pADS == NULL)
		{	str << "Provided data source ["<< sAtr << "] is not analog one." << ends;
			err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
			return false;
		}
		*/

		//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
		if ( !appXML.GetNodeAttributeByName(pSubItem, "VAR", sAtr) )
		{
			err.SetLastError(DSCE_EMPTY_PARAMETR, "Variable name not set" );
			return false;
		}
		std::string vName = sAtr;

		parser.DefineVar(vName, &vars[n]);
		vSubDS.push_back( pDS );
	}

	parser.SetExpr(sFormula);

	mu::varmap_type v = parser.GetUsedVar();

	m_lastValue = parser.Eval();
	return true;
}

double CalcDataSrc::GetValue() const 
{ 		
	for (int i = 0; i < Size(); i++)
	{
		const DataSrc* pDS = GetSubDataSrc(i);
		
		if ( dynamic_cast<const AnalogDataSrc*>(pDS) )
			vars[i] = (dynamic_cast<const AnalogDataSrc*>(pDS))->GetValue();

		else if (dynamic_cast<const DiscreteDataSrc*>(pDS) )
			vars[i] = dynamic_cast<const DiscreteDataSrc*>(pDS)->IsEnabled() ? 1.0 : 0.0;
	}
	
	m_lastValue = parser.Eval();

	return  m_lastValue;
}

/****************************************************************/
/*              Класс DiscreteDataSrc							*/
/****************************************************************/

DiscreteDataSrc::DiscreteDataSrc() : 
enabled(false), strLowState("Disabled"), strHighState("Enabled")
{

}

DiscreteDataSrc::~DiscreteDataSrc()
{

}

//создание источника данных из XML узла
bool DiscreteDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !DataSrc::CreateFromXMLNode(pNode) )
		return false;
		
	std::string sAtr;
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	bInvert = false;
	if ( appXML.GetNodeAttributeByName(pNode, "INVERT", sAtr) )
		bInvert = atoi(sAtr.c_str()) > 0;

	if ( appXML.GetNodeAttributeByName(pNode, "TOSTR", sAtr) )
	{
		string strStates = sAtr;
		string::size_type pos = strStates.find(';');
		
		if ( (pos !=  string::npos) && (strStates.length() > 3))
		{
			std::string lo (strStates, 0, pos ); 
			std::string hi (strStates, ++pos);

			if (!lo.empty() && !hi.empty())
			{
				strLowState  = lo;
				strHighState = hi;
			}
		}
	}

	return true;
}


/*****************************************************************/
/*              Класс DiscreteInputDataSrc                       */
/*****************************************************************/
DiscreteInputDataSrc::DiscreteInputDataSrc() :
pDigitalInputDevice (NULL), channel(-1)
{
	
}

DiscreteInputDataSrc::~DiscreteInputDataSrc()
{
}

//Получить номер канала c которым связан источник 
unsigned char DiscreteInputDataSrc::GetChannelNo() const 
{ 
	return channel; 
}

//Получить устройство с которым связан источник  данных
IDigitalInput* DiscreteInputDataSrc::GetParentDevice() const
{
	return pDigitalInputDevice;
}

//создание источника данных из XML узла
DiscreteInputDataSrc* DiscreteInputDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<DiscreteInputDataSrc> obj ( new DiscreteInputDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
		{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

//создание источника данных из XML узла
bool DiscreteInputDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{

	if ( !DiscreteDataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;
	if ( !appXML.GetNodeAttributeByName(pNode, "DEV", sAtr) )
	{
		str << "Device must be set for this type data source" << std::ends;
		err.SetLastError(DSCE_DEVICE_NOT_SET, str.str() );
		return false;
	}
	
	IDigitalInput* pDev = dynamic_cast<IDigitalInput*>(IdentifiedPhysDevice::GetDevice(sAtr));
	
	if ( pDev == NULL )
	{
		str << "Digital input device ["<< sAtr <<"] not found" << std::ends;
		err.SetLastError(DSCE_INVALID_DEVICE, str.str() );
		return false;
	}

	if ( !appXML.GetNodeAttributeByName(pNode, "WIRE", sAtr) )
	{		
		str << "Channel number must be set for this type data source"  << std::ends;
		err.SetLastError(DSCE_CHANNEL_NOT_SET, str.str() );
		return false;
	}
	
	int wire = atoi(sAtr.c_str());

	if ( (wire < 0) || (wire >= pDev->GetChannelsCountDI()) )
	{
		str << "Invalid channel number [" << wire << " ]"<< std::ends;		
		err.SetLastError(DSCE_INVALID_CHANNEL, str.str() );
		return false;
	}

	pDigitalInputDevice = pDev;
	channel				= wire;

	return true;
}

bool DiscreteInputDataSrc::IsEnabled() const
{
	ASSERT(pDigitalInputDevice != NULL);

	bool state = pDigitalInputDevice->GetChannelStateDI(channel, false);

	if ( state != enabled) 
	{
		enabled = state;
		evOnChangeState(this);		
	}
	enabled = state;
	return bInvert ? !enabled : enabled;
}

/*****************************************************************/
/*              Класс DiscreteOutputDataSrc                      */
/*****************************************************************/

DiscreteOutputDataSrc::DiscreteOutputDataSrc() :	
	pDigitalOutputDevice(NULL), 
	channel(-1), 
	bSafeState(false), 
	useSafe(false)
{
	
}

DiscreteOutputDataSrc::~DiscreteOutputDataSrc()
{
	if (useSafe)
		Enable(bSafeState);
}

//возможность изменения состояния
bool DiscreteOutputDataSrc::CanChangeStateTo(bool state) const 
{ 
	return true; 
};

//Получить номер канала c которым связан источник 
unsigned char DiscreteOutputDataSrc::GetChannelNo() const 
{
	return channel; 
}

//Получить устройство с которым связан источник  данных
IDigitalOutput* DiscreteOutputDataSrc::GetParentDevice() const
{
	return pDigitalOutputDevice;
}

bool DiscreteOutputDataSrc::saveState() const
{
	if (!m_bRestored)
		return true;

	CWinApp* pApp = AfxGetApp();

	CString strSection = _T("Data sources");
	CString strItem    = m_strName.c_str();
	
	if ( !pApp->WriteProfileInt( strSection, strItem, IsEnabled() ? 1 : 0 ) )
		return false;

	return true;
}


void DiscreteOutputDataSrc::restoreState()
{
	if (!m_bRestored)
		return;

	CWinApp* pApp = AfxGetApp();

	CString strSection = _T("Data sources");
	CString strItem    = m_strName.c_str();
	UINT nValue;
	nValue = pApp->GetProfileInt(strSection, strItem, 0);

	Enable( nValue > 0 );
}

//создание источника данных из XML узла
DiscreteOutputDataSrc* DiscreteOutputDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<DiscreteOutputDataSrc> obj ( new DiscreteOutputDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

//создание источника данных из XML узла
bool DiscreteOutputDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !DiscreteDataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;
	if (!appXML.GetNodeAttributeByName(pNode, "DEV", sAtr) )
	{
		str << "Device must be set for this type data source" << std::ends;
		err.SetLastError(DSCE_DEVICE_NOT_SET, str.str() );
		return false;
	}

	IDigitalOutput* pDev = dynamic_cast<IDigitalOutput*>(IdentifiedPhysDevice::GetDevice( sAtr));

	if ( pDev == NULL )
	{
		str << "Digital output device ["<< sAtr <<"] not found" << std::ends;
		err.SetLastError(DSCE_INVALID_DEVICE, str.str() );
		return false;
	}

	if ( !appXML.GetNodeAttributeByName(pNode, "WIRE", sAtr) )
	{		
		str << "Channel number must be set for this type data source"  << std::ends;
		err.SetLastError(DSCE_CHANNEL_NOT_SET, str.str() );
		return false;
	}

	int wire = atoi(sAtr.c_str());
	if ( (wire < 0) || (wire >= pDev->GetChannelsCountDO()) )
	{
		str << "Invalid channel number [" << wire << " ]"<< std::ends;		
		err.SetLastError(DSCE_INVALID_CHANNEL, str.str() );
		return false;
	}

	pDigitalOutputDevice = pDev;
	channel				 = wire;
	
	if ( appXML.GetNodeAttributeByName(pNode, "DEFAULT", sAtr) )
	{		
		Enable( atoi( sAtr.c_str())  > 0 );
	}

	bSafeState = false;
	
	if ( appXML.GetNodeAttributeByName(pNode, "SAFE", sAtr) )
	{
		useSafe = true;
		bSafeState = atoi( sAtr.c_str() )  > 0;
	}
	//восстанавливаем значение из реестра
	restoreState();

	return true;
}


//Установка состояния сигнала
bool DiscreteOutputDataSrc::Enable(bool bEnable)
{
	//bool newState = bInvert ? !bEnable:bEnable;
	//TRACE("%s - %d", Name().c_str(), enabled);
	if (enabled == bEnable)
		return IsEnabled();

	SYSTEMTIME st1;
	SYSTEMTIME st2;	
	std::strstream str;

	::GetLocalTime(&st1);

	str << std::left << std::setw(16) <<	m_strName.c_str() << std::setw(64) << m_strDescription.c_str();
	str << std::fixed << std::setprecision(2);

	str << "\tOld : " << enabled; 	

	bool bCanChangeState =  CanChangeStateTo(bEnable);

	str << "\tCan change : " << bCanChangeState; 

	if ( !bCanChangeState )
		return IsEnabled();
			
	pDigitalOutputDevice->SetChannelStateDO(channel, bInvert ? !bEnable:bEnable);

	enabled = bEnable;
	saveState();

	evOnChangeState(this);
			
	str << "\tNew : " << enabled << "\t";
		
	::GetLocalTime(&st2);

	str << Milliseconds(st1, st2) << " ms" << std::ends;
	logDS.LogMessage( str.str() );
	 
	return IsEnabled();
}

bool DiscreteOutputDataSrc::ToggleState()
{
	//bool newState = bInvert ? !IsEnabled();
	return Enable(!enabled);
}

bool DiscreteOutputDataSrc::IsEnabled() const 
{
	ASSERT(pDigitalOutputDevice != NULL);
	
	bool state = pDigitalOutputDevice->GetChannelStateDO(channel, false);
	enabled = bInvert ? !state : state;

	return  enabled;/*DiscreteDataSrc::IsEnabled()*/;
}


/*****************************************************************/
/*              Класс DiscreteOutputDataSrc                      */
/*****************************************************************/

ControledDiscreteOutputDataSrc ::ControledDiscreteOutputDataSrc() : controlCanChange (true)
{
		
}

ControledDiscreteOutputDataSrc ::~ControledDiscreteOutputDataSrc()
{
	controlCanChange = false;
	Enable(bSafeState);
}

//создание источника данных из XML узла
ControledDiscreteOutputDataSrc* ControledDiscreteOutputDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<ControledDiscreteOutputDataSrc> obj ( new ControledDiscreteOutputDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}


//создание источника данных из XML узла
bool ControledDiscreteOutputDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !DiscreteOutputDataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string idToLow;
	std::string idToHigh;

	std::string sAtr;
	if ( appXML.GetNodeAttributeByName(pNode, "TOLOWSTATE", sAtr) )
		idToLow = sAtr;

	if ( appXML.GetNodeAttributeByName(pNode, "TOHIGHSTATE", sAtr) )
		idToHigh = sAtr;

	bool isControledDS = ( !idToLow.empty()) || ( !idToHigh.empty() );

	if ( !isControledDS ) 
	{
		str << "At least one digital data source for control mast be set for this data source type"  << std::ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );
		return false;
	}
	
	DataSrc* pDS0 = NULL;
	DataSrc* pDS1 = NULL;

	
	std::pair<std::string, bool> csLow = extractDSControl(idToLow);
	std::pair<std::string, bool> csHi  = extractDSControl(idToHigh);

	pDS0 = GetDataSource(csLow.first);
	vDSCS.push_back(std::make_pair(reinterpret_cast<const DiscreteDataSrc*>(pDS0), csLow.second) );

	
	pDS1 = GetDataSource(csHi.first);
	vDSCS.push_back(std::make_pair(reinterpret_cast<const DiscreteDataSrc*>(pDS1), csHi.second) );	
	

	if ( pDS0 == NULL && pDS1 == NULL) 
	{
		str << "Both data source for low and hi state control switching not set"  << std::ends;
		err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
		return false;
	}
	if ( pDS0 != NULL && dynamic_cast<DiscreteDataSrc*>(pDS0) == NULL)
	{
		str << "Provided data source ["<< idToLow.c_str() << "] for control switching to low state is not discrete one." << ends;
		err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
		return false;
	}

	if ( pDS1 != NULL && dynamic_cast<DiscreteDataSrc*>(pDS1) == NULL)
	{
		str << "Provided data source ["<< idToHigh.c_str() << "] for control switching to high state is not discrete one." << ends;
		err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
		return false;
	}

	vSubDS.push_back(dynamic_cast<DiscreteDataSrc*>(pDS0) );
	vSubDS.push_back(dynamic_cast<DiscreteDataSrc*>(pDS1) );
	
	return true;
}

std::pair<std::string, bool> ControledDiscreteOutputDataSrc::extractDSControl(std::string str)
{
	bool state = true;
	std::string id;

	if (!str.empty() )
	{
		std::vector< std::string > parts;
		boost::split(parts, str,  std::bind2nd(std::equal_to<char>(), ':'));

		if ( !parts.empty() )
			id = parts[0];

		if (parts.size() == 2)
			state = (atoi(parts[1].c_str()) != 0 );
	}
	return std::make_pair(id, state);
}

bool ControledDiscreteOutputDataSrc::CanChangeStateTo(bool newState) const
{
	if (!controlCanChange)
		return true;

	const DiscreteDataSrc* pDS = NULL;
	bool controlstate = true;
	if ( newState == HIGH_STATE )
	{
		pDS = vDSCS[SUB_DS_IND_TOHIGHSTATE].first;
		controlstate  = vDSCS[SUB_DS_IND_TOHIGHSTATE].second;
	}
	else{
		pDS = vDSCS[SUB_DS_IND_TOLOWSTATE].first;		
		controlstate  = vDSCS[SUB_DS_IND_TOLOWSTATE].second;
	}	
	
	if ( pDS != NULL)		 
		return ( pDS->IsEnabled() == controlstate);
	else
		return true;	
}

//////////////////////////////////////////////////////////////////////////
//интерфейс IComposite

//количество подисточников данных
int ControledDiscreteOutputDataSrc::Size() const
{
	return (int)vSubDS.size();
}

//получить подисточник данных 
const DataSrc* ControledDiscreteOutputDataSrc::GetSubDataSrc(int ind) const
{
	return vSubDS.at(ind);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

CompositeDiscreteDataSrc::CompositeDiscreteDataSrc()
{

}

CompositeDiscreteDataSrc::~CompositeDiscreteDataSrc()
{

}

bool CompositeDiscreteDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !DiscreteDataSrc::CreateFromXMLNode(pNode) )
		return false;

	
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	CComPtr<IXMLDOMNodeList>	 pItemsList;
	HRESULT hr = pNode->selectNodes(CComBSTR("ITEM"), &pItemsList);

	if ( FAILED(hr) || (pItemsList == NULL) )
	{
		err.SetLastError(DSCE_EMPTY_SUBDS_LIST, "Empty subitems list" );
		return false;
	}

	
	
	long subItems = 0;
	pItemsList->get_length(&subItems); 
	pItemsList->reset();

	if (subItems == 0)
	{
		err.SetLastError(DSCE_EMPTY_SUBDS_LIST, "Empty subitems list" );
		return false;
	}
	
	for (long n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList->get_item(n, &pSubItem);

		std::string sAtr;
	
		//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
		if ( !appXML.GetNodeAttributeByName(pSubItem, "DSID", sAtr) )
		{
			err.SetLastError(DSCE_EMPTY_PARAMETR, "Subitem name not set." );
			return false;
			
		}
		
		DataSrc* pDS = GetDataSource(sAtr);
		
		if ( pDS == NULL)
		{
			std::strstream str;

			str << "Data source ["<< sAtr << "] not found." << ends;
			err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
			return false;
		}

		if ( dynamic_cast<DiscreteDataSrc*>(pDS) == NULL)
		{
			std::strstream str;
			str << "Provided data source ["<< sAtr << "] is not discrete one." << ends;
			err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
			return false;
		}

		vSubDS.push_back( dynamic_cast<DiscreteDataSrc*>(pDS) );
	}

	return true; 
}

//количество подисточников данных
int CompositeDiscreteDataSrc::Size() const
{
	return (int)vSubDS.size(); 
}

//получить подисточник данных 
const DataSrc* CompositeDiscreteDataSrc::GetSubDataSrc(int ind) const
{
	return vSubDS.at(ind);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
DiscreteANDDataSrc::DiscreteANDDataSrc()
{
}

DiscreteANDDataSrc::~DiscreteANDDataSrc()
{
}

DiscreteANDDataSrc* DiscreteANDDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<DiscreteANDDataSrc> obj ( new DiscreteANDDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool DiscreteANDDataSrc::IsEnabled() const
{
	//TODO::Throw exception empsy list
	if (vSubDS.size() == 0)
		return false;

	bool state = vSubDS[0]->IsEnabled();

	for (size_t i = 0; i < vSubDS.size(); i++)
		state = (state)  && (vSubDS[i]->IsEnabled());
	
	if (state != enabled)
	{
		enabled = state;
		evOnChangeState(this);		
	}
	
	enabled = state;
	return bInvert ? !enabled : enabled;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
DiscreteORDataSrc::DiscreteORDataSrc()
{
}

DiscreteORDataSrc::~DiscreteORDataSrc()
{
}

DiscreteORDataSrc* DiscreteORDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<DiscreteORDataSrc> obj ( new DiscreteORDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool DiscreteORDataSrc::IsEnabled() const
{
	bool state = vSubDS[0]->IsEnabled();

	for (size_t i = 0; i < vSubDS.size(); i++)
		state = (state) || (vSubDS[i]->IsEnabled());
	
	if (state != enabled)
	{
		enabled = state;
		evOnChangeState(this);
	}
	
	enabled = state;
	return bInvert ? !enabled : enabled;
}


/************************************************************************/
/*		Класс AnalogLevelAsDiscreteDataSrc								*/
/* Класс - дискретный источник данных устанавливающийся					*/
/* в состояние включено при привышении	присоединенным аналоговым		*/
/* источником данных определенного уровня								*/
/************************************************************************/

AnalogLevelAsDiscreteDataSrc::AnalogLevelAsDiscreteDataSrc() : 
	pControledADS(NULL) , 
	pBaseADS(NULL), 
	lvl(0.0)

{
}	

AnalogLevelAsDiscreteDataSrc::~AnalogLevelAsDiscreteDataSrc() 
{
}	 

AnalogLevelAsDiscreteDataSrc* AnalogLevelAsDiscreteDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<AnalogLevelAsDiscreteDataSrc> obj ( new AnalogLevelAsDiscreteDataSrc() );
	
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool AnalogLevelAsDiscreteDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !DiscreteDataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;

	if ( !appXML.GetNodeAttributeByName(pNode, "DSID", sAtr) )
	{
		str << "Controlled analog data source ID not set" << std::ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );		
		return false;
	}

	DataSrc* pDS = GetDataSource( sAtr );
				
	if ( pDS == NULL)
	{
		str << "Data source ["<< sAtr  << "] not found." << ends;
		err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
		return false;
	}
		
	if ( dynamic_cast<AnalogDataSrc*>(pDS) == NULL)
	{
		str << "Provided data source ["<< sAtr << "] for level control is not analog one." << ends;
		err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
		return false;
	}
	
	pControledADS = dynamic_cast<AnalogDataSrc*>(pDS);

	//УРОВЕНЬ СИГНАЛА	
	if ( !appXML.GetNodeAttributeByName(pNode, "LVL", sAtr) )
	{		
		str << "Level value or level data source not set" << std::ends;
		err.SetLastError(DSCE_LEVEL_NOT_SET, str.str() );		
		return false;
	}

	double lvl;

	pBaseADS = dynamic_cast<AnalogDataSrc*>( GetDataSource( sAtr ) );
	if ( pBaseADS != NULL )  
		lvl = pBaseADS->GetValue();	
	else 
		lvl = atof(sAtr.c_str());

	//получаем диапазон изменения аналоговой величины
	double minv, maxv;
	pControledADS->GetRange(minv, maxv);
	
	if (( lvl < minv) || (lvl > maxv) )
	{
		str << "Level value [" << lvl << "] is not valid for analog data source ["<< pControledADS->Name().c_str() << "] range["<< minv << ";" << maxv << "]" << std::ends;
		err.SetLastError(DSCE_INVALID_ADS_RANGE, str.str() );		
		return false;
	}
	this->lvl = lvl;

	bForGrowth = true;
	if ( appXML.GetNodeAttributeByName(pNode, "GROWTH", sAtr) )
		bForGrowth = atoi(sAtr.c_str()) > 0;
	

	return true;
}

bool AnalogLevelAsDiscreteDataSrc::IsEnabled() const
{
	ASSERT(pControledADS != NULL);

	double val = pControledADS->GetValue();
	double  actualLvl = (pBaseADS == NULL) ? lvl : pBaseADS->GetValue();

	bool state = bForGrowth ?  ( val > actualLvl ) :  ( val < actualLvl );

	if ( state != enabled )
	{
		enabled = state;
		evOnChangeState(this);
	}
	
	enabled = state;
	return bInvert ? !enabled : enabled;
}


//Реализация интерфейс IComposite

//количество подисточников данных (только один)
int AnalogLevelAsDiscreteDataSrc::Size() const
{
	return 1;
}

//получить подисточник данных 
const DataSrc* AnalogLevelAsDiscreteDataSrc::GetSubDataSrc(int ind) const
{
	if (ind != 0)
		return NULL;

	return pControledADS;
}

//Получить контролируемый уровень сигнала
double AnalogLevelAsDiscreteDataSrc::GetLevel() const
{
	return lvl;
};

//Задать  контролируемый уровень сигнала
void AnalogLevelAsDiscreteDataSrc::SetLevel(double level) 
{ 
	lvl = level; 
};

/************************************************************************/
/*		Класс AnalogRangeAsDiscreteDataSrc								*/
/* Класс - дискретный источник данных позволяющий устанавливающийся		*/
/* в состояние включено при выходе	значения присоединенного аналогового*/
/* источника данных за границы дипазона 								*/
/************************************************************************/

AnalogRangeAsDiscreteDataSrc::AnalogRangeAsDiscreteDataSrc() 
{
	vSubBuf.assign(3, NULL);
}

AnalogRangeAsDiscreteDataSrc::~AnalogRangeAsDiscreteDataSrc() 
{

}

//создание источника данных из XML узла
AnalogRangeAsDiscreteDataSrc* AnalogRangeAsDiscreteDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<AnalogRangeAsDiscreteDataSrc> obj ( new AnalogRangeAsDiscreteDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

//создание источника данных из XML узла
bool AnalogRangeAsDiscreteDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !DiscreteDataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;

	if ( !appXML.GetNodeAttributeByName(pNode, "DSID", sAtr) )
	{
		str << "Controlled analog data source ID not set" << std::ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );		
		return false;
	}

	DataSrc* pDS = GetDataSource( sAtr );

	if ( pDS == NULL)
	{
		str << "Data source ["<< sAtr  << "] not found." << ends;
		err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
		return false;
	}

	if ( dynamic_cast<AnalogDataSrc*>(pDS) == NULL)
	{
		str << "Provided data source ["<< sAtr << "] for range control is not analog one." << ends;
		err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
		return false;
	}

	const AnalogDataSrc* pADS = dynamic_cast<const AnalogDataSrc*>(pDS);

	vSubBuf[DSBASE] = pADS;	

	//МИНИМАЛЬНЫЙ УРОВЕНЬ СИГНАЛА 	
	if ( !appXML.GetNodeAttributeByName(pNode, "LOLVL", sAtr) )
	{		
		str << "Low limit range value or variable not set" << std::ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );		
		return false;
	}

	double lvlMin = 0.0;

	const AnalogDataSrc* pLoADS = dynamic_cast<const AnalogDataSrc*>(DataSrc::GetDataSource( sAtr ) );
	if ( pLoADS != NULL )
	{	
		lvlMin = pLoADS->GetValue();
		vSubBuf[DSLOW] = pLoADS;
	}
	else
	{
		lvlMin = atof(sAtr.c_str());
		vSubBuf[DSLOW] = NULL;
	}


	//МАКСИМАЛЬНЫЙ УРОВЕНЬ СИГНАЛА 	
	if ( !appXML.GetNodeAttributeByName(pNode, "HILVL", sAtr) )
	{		
		str << "High limit range value or variable not set" << std::ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );		
		return false;
	}
	double lvlMax = 0.0;

	const AnalogDataSrc* pHiADS = dynamic_cast<const AnalogDataSrc*>(DataSrc::GetDataSource( sAtr ) );
	if ( pHiADS != NULL )
	{	
		lvlMax = pHiADS->GetValue();
		vSubBuf[DSHIGH] = pHiADS;
	}
	else
	{
		lvlMax = atof(sAtr.c_str());
		vSubBuf[DSHIGH] = NULL;
	}

	
	if ( lvlMin >= lvlMax )
	{
		str << "Invalid range [" << lvlMin << ";" << lvlMax << "]" << std::ends;
		err.SetLastError(DSCE_INVALID_ADS_RANGE, str.str() );		
		return false;
	}

	//получаем диапазон изменения аналоговой величины
	double minv, maxv;
	pADS->GetRange(minv, maxv);

	if ( (lvlMin <= minv) || (lvlMin >= maxv) || (lvlMax <= minv) || (lvlMax >= maxv))
	{
		str << "Specified range [" << lvlMin << ";" << lvlMax << "] is not valid for analog data source ["<< pADS->Name().c_str() << "] range["<< minv << ";" << maxv << "]" << std::ends;
		err.SetLastError(DSCE_INVALID_ADS_RANGE, str.str() );		
		return false;
	}

	lvlLow = lvlMin;	
	lvlUp  = lvlMax;

	bInRange = true;
	if ( appXML.GetNodeAttributeByName(pNode, "INSIDE", sAtr) )
		bInRange = atoi(sAtr.c_str()) > 0;

	return true;
}



void AnalogRangeAsDiscreteDataSrc::UpdateRange() const
{
	if (vSubBuf[DSHIGH] != NULL)
		lvlUp  = vSubBuf[DSHIGH]->GetValue();
	
	if (vSubBuf[DSLOW] != NULL) 
		lvlLow = vSubBuf[DSLOW]->GetValue();
}

bool AnalogRangeAsDiscreteDataSrc::IsEnabled() const
{	
	double val = vSubBuf[DSBASE]->GetValue();
	UpdateRange();

	bool state = bInRange ? 
		((val <= lvlUp) && (val >= lvlLow)) :
		((val > lvlUp)  || (val < lvlLow));

	if (state != enabled)
	{
		enabled = state;
		evOnChangeState(this);
	}
	
	enabled = state;
	return bInvert ? !enabled : enabled;
}

//Получить контролируемы диапазон сигнала
void AnalogRangeAsDiscreteDataSrc::GetRange(double& dLow, double& dHigh)
{
	dLow  = lvlLow;	
	dHigh = lvlUp;
}

//////////////////////////////////////////////////////////////////////////
// интерфейс IComposite
//количество подисточников данных
//количество подисточников данных (только один)
int AnalogRangeAsDiscreteDataSrc::Size() const
{
	return (int)vSubBuf.size();
}

//получить подисточник данных 
const DataSrc* AnalogRangeAsDiscreteDataSrc::GetSubDataSrc(int ind) const
{
	if ((ind < 0) || (ind >= (int)vSubBuf.size()))
		return NULL;

	return vSubBuf.at(ind);
}


#pragma region DeviceStateDataSrc

DeviceStateDataSrc::DeviceStateDataSrc() : pDevice(NULL)
{

}

DeviceStateDataSrc::~DeviceStateDataSrc()
{

}

//Получить устройство с которым связан источник  данных
//  [9/25/2011 Johnny A. Matveichik]
IdentifiedPhysDevice* DeviceStateDataSrc::GetParentDevice() const
{
	return pDevice;
}

//создание источника данных из XML узла
bool DeviceStateDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !DiscreteDataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;

	if ( !appXML.GetNodeAttributeByName(pNode, "DEV", sAtr) )
	{
		str << "Device must be set for this type data source" << std::ends;
		err.SetLastError(DSCE_DEVICE_NOT_SET, str.str() );
		return false;
	}

	pDevice = IdentifiedPhysDevice::GetDevice( sAtr );
	if ( pDevice == NULL )
	{
		str << "Digital input device ["<< sAtr <<"] not found" << std::ends;
		err.SetLastError(DSCE_INVALID_DEVICE, str.str() );
		return false;
	}
	return true;
}

DeviceStateDataSrc* DeviceStateDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<DeviceStateDataSrc> obj ( new DeviceStateDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

//Проверка состояния сигнала
bool DeviceStateDataSrc::IsEnabled() const
{
	assert(pDevice);
	return ( enabled = pDevice->IsAlive() );
}


#pragma endregion DeviceStateDataSrc

//  [11/24/2014 Johnny A. Matveichik]
AnalogToBitsetDataSrc::AnalogToBitsetDataSrc() : lastWriten(0.0)
{
}

AnalogToBitsetDataSrc::~AnalogToBitsetDataSrc()
{

}

//создание источника данных из XML узла
AnalogToBitsetDataSrc* AnalogToBitsetDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<AnalogToBitsetDataSrc> obj ( new AnalogToBitsetDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();

}

//создание источника данных из XML узла
bool AnalogToBitsetDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !AnalogDataSrc::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	
	std::string sAtr;
	CComPtr<IXMLDOMNodeList>	 pItemsList;
	HRESULT hr = pNode->selectNodes(CComBSTR("ITEM"), &pItemsList);

	if ( FAILED(hr) || (pItemsList == NULL) )
	{
		err.SetLastError(DSCE_EMPTY_SUBDS_LIST, "Empty subitems list" );
		return false;
	}


	long subItems = 0;

	pItemsList->get_length(&subItems); 
	pItemsList->reset();

	for (long n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList->get_item(n, &pSubItem);

		//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
		if ( !appXML.GetNodeAttributeByName(pSubItem, "DSID", sAtr) )
		{
			err.SetLastError(DSCE_EMPTY_PARAMETR, "Subitem name not set." );
			return false;

		}

		DataSrc* pDS = (GetDataSource(sAtr));

		if ( pDS == NULL)
		{
			std::strstream str;

			str << "Data source ["<< sAtr << "] not found." << ends;
			err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
			return false;
		}

		if ( dynamic_cast<DiscreteOutputDataSrc*>(pDS) == NULL)
		{
			std::strstream str;
			str << "Provided data source ["<< sAtr << "] is not discrete output one." << ends;
			err.SetLastError(DSCE_INVALID_DS_TYPE, str.str() );
			return false;
		}

		DiscreteOutputDataSrc* pDODS = dynamic_cast<DiscreteOutputDataSrc*>(pDS);
		pDODS->AddOnChangeState(boost::bind( &AnalogToBitsetDataSrc::OnDiscreteDSChanged,  this, _1));

		bits.push_back( pDODS );
	}
	
	return true;
}

void AnalogToBitsetDataSrc::OnDiscreteDSChanged(const DiscreteDataSrc* pDDS)
{
	bitset<64> curbits(0);
		
	for (size_t i = 0; i < bits.size(); i++)
		curbits.set(i, bits.at(i)->IsEnabled() );

	unsigned long lv = curbits.to_ulong();
	double newv = (double)lv;
	double wval = 0.0;

	try {
		if ( !m_vConverters.empty() )
			wval = m_vConverters[0]->RevConvert(newv);

		lastWriten = wval;
	}
	catch (std::invalid_argument& e) 
	{
		lastWriten = lastWriten;
	}
}

//Получить текущее значение управляемой величины в единицах пользователя
double AnalogToBitsetDataSrc::GetValue() const
{
	return lastWriten;
}

//Получить текущее значение управляемой величины в единицах устройства	
double AnalogToBitsetDataSrc::GetSignal() const
{
	return GetValue();
}

//Задать новое значение управляемой величины
double AnalogToBitsetDataSrc::SetValue(double val) const
{
	unsigned long long newv = (int)val;
	double oldVal = lastWriten;

	if ( !m_vConverters.empty() )
		newv = (unsigned long long)m_vConverters[0]->Convert(val);

	bitset<64> newbits(newv);
	
	for (size_t i = 0; i <bits.size();  i++)
	{
		TRACE("%s -> %d -> %d\n", bits.at(i)->Name().c_str(), bits.at(i)->IsEnabled(), newbits.test(i));
		bits.at(i)->Enable( newbits.test(i) );
	}
	
	lastWriten = val;

	if (oldVal != val )
	{
		//используе сигналы boost
		//evOnValueChange(const_cast<AnalogOutputDataSrc*>(this));

		//используем посылку сообщений
		for (std::vector<HWND>::const_iterator it = updateWnds.begin(); it != updateWnds.end(); ++it)
			::SendMessage((*it), RM_ANALOG_VALUE_CHANGE, 0, (LPARAM)this);
	}

	return oldVal;
}

//Получить диапазон значений управляемой величины
void AnalogToBitsetDataSrc::GetRange(double& minV, double& maxV) const
{
	if ( !m_vConverters.empty() )
		m_vConverters[0]->GetMinMax(minV, maxV);
	else
	{
		minV = 0;
		maxV = pow(2.0, (double)bits.size() );
	}
}

//Получить номер канала c которым связан источник 
unsigned char  AnalogToBitsetDataSrc::GetChannelNo() const 
{ 
	return -1; 
}

//Получить устройство с которым связан источник  данных
IAnalogOutput*  AnalogToBitsetDataSrc::GetParentDevice() const
{
	return NULL;
}


//количество подисточников данных
int  AnalogToBitsetDataSrc::Size() const
{
	return (int)bits.size();
}

//получить подисточник данных 
const DataSrc*  AnalogToBitsetDataSrc::GetSubDataSrc(int ind) const
{
	return bits.at(ind);
}




StringDataSrc::StringDataSrc()
{

}

StringDataSrc::~StringDataSrc()
{

}

//создание источника данных из XML узла
StringDataSrc* StringDataSrc::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<StringDataSrc> obj ( new StringDataSrc() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

//создание источника данных из XML узла
bool StringDataSrc::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !DataSrc::CreateFromXMLNode(pNode) )
		return false;

	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	appXML.GetNodeAttributeByName( pNode, "DEFAULT", str);	

	return true;
}

std::string StringDataSrc::GetString() const
{
	return str;	
}

void StringDataSrc::SetString(std::string text)
{
	str = text;
}	











