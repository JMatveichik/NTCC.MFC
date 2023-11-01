#include "stdafx.h"
#include <Dbghelp.h>

#include "DataCollector.h"

#include "..\ah_project.h"
#include "..\ah_errors.h"
#include "..\ah_xml.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DataCollector::DataCollector() 
{
	
};

DataCollector::~DataCollector(void)
{
	
	Stop();

//	for (NAMED_MULTI_DEQUE::const_iterator it = m_mainDataBuffer.begin(); it != m_mainDataBuffer.end(); ++it)
//		delete &(it->second);
	
	m_mainDataBuffer.clear();
}

DataCollector& DataCollector::Instance()
{
	static DataCollector obj;
	return obj;
}

//
void DataCollector::LockData() const
{
	Lock();
}


void DataCollector::UnlockData() const
{
	Unlock();
}

void DataCollector::ControlProc()
{
	Lock();

	AppendData("TIME", (double)time(NULL) );

	for (std::vector<const DataSrc*>::const_iterator it = m_ds.begin(); it != m_ds.end(); ++it)
	{
		double val = UNKNONW_VALUE;
		const AnalogDataSrc* pADS = dynamic_cast<const AnalogDataSrc*>( (*it) );
		if ( pADS != NULL )
			val = pADS->GetValue();
		
		const DiscreteDataSrc* pDDS = dynamic_cast<const DiscreteDataSrc*>( (*it) );
		if ( pDDS != NULL )
			val = pDDS->IsEnabled() ? 1.0 : 0.0;

		AppendData((*it)->Name(), val);
	}
	Unlock();

}

void DataCollector::AppendData(std::string name, double val)
{
	NAMED_MULTI_DEQUE::iterator itFind = m_mainDataBuffer.find( name );

	if ( itFind != m_mainDataBuffer.end() ) 
	{
		if ( itFind->second.size() >= m_nCapacity )
			itFind->second.pop_front();

		itFind->second.push_back(val);
	}
}

//получить буфер данных времени
double DataCollector::GetTimeInterval() const
{	
	return m_nCapacity * GetInterval()/ 1000;	 
}

bool DataCollector::Initialize(double liveTime, double updateInterval)
{
	DATA_SOURCES_ENUM_PARAMS dsep;
	dsep.mask = DSEM_TYPE;
	dsep.dstype = DS_ANALOG|DS_DISCRETE|DS_COLLECTED;

	DataSrc::EnumDataSources(m_ds, &dsep);

	if ( m_ds.empty() )
		return false;

	SUBBUFFER* col = new SUBBUFFER();
	m_mainDataBuffer.insert(std::make_pair("TIME", *col));
	delete col;
	
	for (vector<const DataSrc*>::const_iterator it = m_ds.begin(); it != m_ds.end(); it ++)
	{
		col = new SUBBUFFER();
		m_mainDataBuffer.insert( std::make_pair( (*it)->Name(), *col ) );
		delete col;
	}
	
	m_nCapacity = (unsigned int)( ceil( liveTime / updateInterval) );

	
	return true;
}

//получить последнне значение записанное в буфер
bool DataCollector::GetLast(std::string name, double& val) const
{
	Lock();
	NAMED_MULTI_DEQUE::const_iterator itFind = m_mainDataBuffer.find(name);

	if ( itFind != m_mainDataBuffer.end() ) 
	{
		SUBBUFFER subbuf = (*itFind).second;
		if ( subbuf.empty() )
		{
			Unlock();
			return false;
		}

		val = subbuf.back();

		Unlock();
		return true;
	}

	Unlock();
	return false;
}

//получить буфер данных для ИД
bool DataCollector::GetDataSourceBuffer(std::string name, std::vector<double>& data) const
{
	Lock();
	NAMED_MULTI_DEQUE::const_iterator itFind = m_mainDataBuffer.find(name);

	if ( itFind != m_mainDataBuffer.end() ) 
	{
		SUBBUFFER subbuf = (*itFind).second;
		
		data.resize(subbuf.size());
		copy( subbuf.begin(), subbuf.end(), data.begin());		
		Unlock();
		return true;
	}

	Unlock();
	return false;
}
//получить буфер данных для ИД за последние N сек
bool DataCollector::GetDataSourceBuffer(std::string name, std::vector<double>& data, int sec) const
{
	Lock();
	NAMED_MULTI_DEQUE::const_iterator itFind = m_mainDataBuffer.find(name);

	if ( itFind != m_mainDataBuffer.end() ) 
	{
		SUBBUFFER subbuf = (*itFind).second;

		//интервал времени
		size_t buffsize = size_t ( sec / ( m_nInterval / 1000.0 ) );
		data.resize( min( subbuf.size(), buffsize) );

		SUBBUFFER::iterator itFrom = ( subbuf.size() < buffsize ) ? subbuf.begin() : subbuf.end() - buffsize;

		copy( itFrom, subbuf.end(), data.begin() );		

		Unlock();
		return true;
	}

	Unlock();
	return false;
}

//получить буфер данных времени
bool DataCollector::GetTimeBuffer(std::vector<double>& data) const
{
	return GetDataSourceBuffer("TIME", data);
}


//////////////////////////////////////////////////////////////////////////

std::map<std::string, DataLogger*> DataLogger::m_loggers;

DataLogger::DataLogger()
{

}

DataLogger::~DataLogger()
{
	
}



DataLogger* DataLogger::GetDataLogger(std::string name)
{
	std::map<std::string, DataLogger*>::const_iterator fnd;
	fnd =  m_loggers.find( name );

	if ( fnd == m_loggers.end() )
		return NULL;
	else
		return (*fnd).second;
}

void DataLogger::ClearAll()
{
	for(std::map<std::string, DataLogger*>::iterator it = m_loggers.begin(); it != m_loggers.end(); ++it )
	{
		(*it).second->Stop();
		delete (*it).second;
	}

	m_loggers.clear();
}

bool DataLogger::AddDataLogger(DataLogger* pDL)
{
	if (pDL == NULL)
		return false;

	if ( pDL->Name().empty() )
		return false;

	return m_loggers.insert(std::make_pair( pDL->Name(), pDL )).second;
}

DataLogger* DataLogger::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<DataLogger> obj(new DataLogger());
	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool DataLogger::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	//ID   
	std::string sAtr;
	if ( !appXML.GetNodeAttributeByName( pNode, "ID", sAtr) || sAtr.length()  == 0)
	{
		err.SetLastError(DSCE_NAME_NOT_SET, "Name not set for data logger tag <ID>");
		return false;
	}
	std::string name = sAtr;


	//WRITEINTERVAL - интервал записи данных
	unsigned int nWriteInterval = 30000;
	if ( appXML.GetNodeAttributeByName( pNode, "WRITEINTERVAL", sAtr) )
		nWriteInterval = atoi(sAtr.c_str());

	if ( nWriteInterval <= MINDELAY )
	{
		str << "Invalid write interval [ ] " << nWriteInterval << "for data logger" << ends;
		err.SetLastError(DSCE_NAME_NOT_SET, "Name not set for data logger tag <ID>");
		return false;
	}
	m_nInterval = nWriteInterval;

	//DIR - директория записи данных
	std::string sLogDir = NTCCProject::Instance().GetPath(NTCCProject::PATH_DATA);

// 	if ( appXML.GetNodeAttributeByName( pNode, "DIR", sAtr) )
// 		sLogDir = (LPCTSTR)sAtr;

	//FILEEXT - расширение файла записи данных
	std::string sFileExt = "csv";
	if ( appXML.GetNodeAttributeByName( pNode, "FILEEXT", sAtr) )
		sFileExt = sAtr;


	//SEPARATOR - знак разделения данных в файле 
	char cSep = '\t';
	if ( appXML.GetNodeAttributeByName( pNode, "SEPARATOR", sAtr) )
		cSep = sAtr[0];

	//TIMEUNITS - расширение файла записи данных
	std::string sTimeUnits = "MINUTES";
	if ( appXML.GetNodeAttributeByName( pNode, "TIMEUNITS", sAtr) )
		sTimeUnits = sAtr;

	
	//APPEND - дописывать данные в конец файла
	bool bClearOldData = true;
	if ( appXML.GetNodeAttributeByName( pNode, "APPEND", sAtr) )
		bClearOldData = atoi(sAtr.c_str()) <= 0;


	pStartStopDS = NULL;
	if ( appXML.GetNodeAttributeByName( pNode, "STARTDS", sAtr) )
		pStartStopDS = dynamic_cast<const DiscreteDataSrc*>(DataSrc::GetDataSource(sAtr)) ;
	
	//std::vector< std::string > dsNames;
	 std::vector< std::pair<std::string, bool> > dsNames;

	CComPtr<IXMLDOMNodeList>	 pItemsList;
	pNode->selectNodes(CComBSTR("ITEM"), &pItemsList);

	long subItems = 0;
	pItemsList->get_length(&subItems); 
	pItemsList->reset();

	for (int n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList->get_item(n, &pSubItem);

		std::string sAtr;
		//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
		if ( !appXML.GetNodeAttributeByName(pSubItem, "DSID", sAtr) )
			continue;
		
		std::string id =  sAtr;

		//ОТМЕТКИ ОБ ИЗМЕНЕНИЯХ
		bool bMarkChanges = false;
		if ( appXML.GetNodeAttributeByName(pSubItem, "LISTEN", sAtr) )
			bMarkChanges = atoi(sAtr.c_str()) > 0;

		dsNames.push_back(std::make_pair(id, bMarkChanges) );		
	}

	return Create(name, dsNames, sLogDir, sFileExt, bClearOldData, cSep, sTimeUnits) ;
}

bool DataLogger::Create(std::string name, std::vector< std::pair<std::string, bool> > DSIDs, 
						std::string sLogFileDir/* = ""*/, 
						std::string sFileExt/* = "csv"*/, 
						bool bClearOldData/* = true*/, 
						char cSep/* = '\t'*/, 
						std::string sTimeFormat/* = "MINUTES"*/ )

{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	std::map<std::string, DataLogger*>::iterator fnd = m_loggers.find(name);
	
	if ( fnd != m_loggers.end() )
	{
		str << "Data logger with name ["<< name.c_str()<<"] alredy exist" << ends;
		err.SetLastError(DSCE_DUPLICATE_NAME, str.str() );
		return false;		
	}
	m_strID = name;

	//единицы измерения времени
	std::string s = sTimeFormat;

	std::transform(s.begin(), s.end(), s.begin(), toupper);

	int nInd = MINUTES;

	if (s == "SECONDS")
		nInd = SECONDS;
	else if (s == "MINUTES")
		nInd = MINUTES;
	else if (s == "HOURS")
		nInd = HOURS;

	m_dTimeCoef = TIME_COEFFS[nInd];
	m_cSep = cSep;
	m_strFileExt = sFileExt;

	
	for (std::vector<std::pair<std::string, bool> >::const_iterator it = DSIDs.begin(); it != DSIDs.end(); it++)
 	{
		DataSrc* pDS = DataSrc::GetDataSource( (*it).first );
		
 		/*если ИД найден и это не таймер добавляем*/ 
		if ( pDS != NULL/* && dynamic_cast<const SimpleTimer*>(pDS) == NULL*/ )
			m_ds.push_back(pDS);			
		else {
			str << "Data source ["<< (*it).first.c_str()<<"] not found" << ends;
			err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
			return false;
		}
	}
	
	if ( m_ds.empty() )
	{
		DataSrc::EnumDataSources(m_ds, NULL);
	}


	if ( !MakeOutFile(sLogFileDir, bClearOldData) )
		return false;
	
	for (std::vector<std::pair<std::string, bool> >::const_iterator it = DSIDs.begin(); it != DSIDs.end(); it++)
 	{
		DataSrc* pDS = DataSrc::GetDataSource( (*it).first );
 		
		AnalogOutputDataSrc* pADS = dynamic_cast<AnalogOutputDataSrc*>(pDS);
		if ( pADS != NULL && (*it).second ) 
		{
			pADS->AddOnOutputValueChange( boost::bind( &DataLogger::OnAnalogDSChanged,  this, _1) );
			continue;
		}

		DiscreteDataSrc* pDDS = dynamic_cast<DiscreteDataSrc*>(pDS);
		if ( pDDS != NULL && (*it).second) 
		{
			pDDS->AddOnChangeState(boost::bind( &DataLogger::OnDiscreteDSChanged,  this, _1));
			continue;
		}	
	}

	return true;
}

bool DataLogger::MakeOutFile(std::string dir, bool bClearOldData)
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	char szTmpPath[MAX_PATH];
	
	//если директория не указана создаем в текущей
	if ( dir.empty() )
	{
		::GetCurrentDirectory(MAX_PATH, szTmpPath);
		m_strBaseDir = szTmpPath;
	}
	else	
		m_strBaseDir = dir;

	size_t len = m_strBaseDir.length();

	//добавляем слеш в конце если отсутствует	
	if ( m_strBaseDir.c_str()[len - 1] != '\\' )
		m_strBaseDir += "\\";

	//создаем поддиректории с текущим годом и месяцем
	SYSTEMTIME curTime;
	::GetLocalTime( &curTime );

	m_nLatsDay = curTime.wDay;

	sprintf_s(szTmpPath, "%04d\\%02d\\", curTime.wYear, curTime.wMonth );
	
	m_strLogFilePath = m_strBaseDir;
	m_strLogFilePath += szTmpPath;

	//если невозможно создать директорию
	if ( !MakeSureDirectoryPathExists(m_strLogFilePath.c_str()) )
	{
		str << "Invalid data logger path ["<< m_strLogFilePath.c_str()<<"]" << ends;
		err.SetLastError(DSCE_DUPLICATE_NAME, str.str() );
		return false;		
	}

	char szBuffer[2048];
	sprintf_s(szBuffer, 2048, "%s_%02d%02d%04d.%s", m_strID.c_str(), curTime.wDay, curTime.wMonth, curTime.wYear, m_strFileExt.c_str());

	m_strLogFilePath += szBuffer;

	if ( m_streamData.is_open() )
		m_streamData.close();

	m_streamData.open(m_strLogFilePath.c_str(), bClearOldData ? ios::out : ios::out|ios::app);
	if ( m_streamData.bad() )
	{
		str << "Output file [" << m_strLogFilePath.c_str()<< "] failed" << ends;
		err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
		return false;
	}
	
	if ( m_ds.empty() )
	{
		str << "Empty data source list" << ends;

		m_streamData << str.str() << endl;
 		err.SetLastError(DSCE_INVALID_DS_ID, str.str() );

 		return false;
	}

	
	/*m_streamData.width(16);*/
	m_streamData << ">> New data block started >> " << m_strID.c_str() << " >> " << PrintTime() << " >> DS count : " << m_ds.size() << endl;
	m_streamData.setf( ios::left );
	m_streamData << "TIME" << m_cSep;
	m_streamData << "TIME2" << m_cSep;

	for(std::vector <const DataSrc*>::const_iterator it = m_ds.begin(); it != m_ds.end(); ++it)	
	{
		const DataSrc* pDS = (*it);

		/*m_streamData.width(16);*/
		m_streamData.setf( ios::left );
		//m_streamData << pDS->Name() << m_cSep;
		m_streamData << pDS->Name() << "[" << pDS->Description() << "]" << m_cSep;
	}

	m_streamData << endl;
	return true;
}

void DataLogger::WriteData(std::string comment/* = ""*/)
{
	if ( pStartStopDS != NULL && !pStartStopDS->IsEnabled() )
		return;

	Lock();

	unsigned long lDiffTime = Milliseconds() - m_lStartTime; 
	double dTime = (double)CTime::GetCurrentTime().GetTime();//( lDiffTime * m_dTimeCoef);

	m_streamData.setf(ios::fixed);
	m_streamData.precision(3);
	m_streamData.setf( ios::left );
	m_streamData << dTime << m_cSep;
	m_streamData << PrintTime().c_str() << m_cSep;	

	for(std::vector <const DataSrc*>::const_iterator it = m_ds.begin(); it != m_ds.end(); ++it)	
	{
		const AnalogDataSrc* pADS = dynamic_cast<const AnalogDataSrc*>(*it);

		m_streamData.setf( ios::left );
		
		if ( pADS != NULL )
			m_streamData << pADS->GetValue();		

		const DiscreteDataSrc* pDDS = dynamic_cast<const DiscreteDataSrc*>(*it);

		if ( pDDS != NULL )
			m_streamData << pDDS->IsEnabled();		

		std::vector <const DataSrc*>::const_iterator next = it;
		if( ++next != m_ds.end() )
			m_streamData << m_cSep;
	}
	
	 if ( !comment.empty() )
		m_streamData << m_cSep << "!" << comment.c_str();

	
	m_streamData << endl;
	Unlock();
	
}

bool DataLogger::Start(unsigned long nInterval/* = 0*/)
{
	m_lStartTime = Milliseconds();
	return TimedControl::Start(nInterval);
}

void DataLogger::ControlProc()
{
	SYSTEMTIME curTime;
	::GetLocalTime( &curTime );

	if ( m_nLatsDay != curTime.wDay )
		MakeOutFile(m_strBaseDir, false);

	////TRACE("\n\t%s-\tDataLogger::ControlProc function called..", PrintTime().c_str() );
	WriteData( PrintTime() );
	////TRACE("- after WriteData");
}

void DataLogger::OnAnalogDSChanged(const AnalogOutputDataSrc* pADS)
{
	if ( m_ds.empty() )
		return;

	std::vector <const DataSrc*>::const_iterator fnd = find(m_ds.begin(), m_ds.end(), pADS);

	if (fnd != m_ds.end() )
	{
		std::strstream str;
		str << PrintTime() << " => " << pADS->Description() << " был изменен" << ends;
		WriteData( str.str() );
	}	
}

void DataLogger::OnDiscreteDSChanged(const DiscreteDataSrc* pDDS)
{
	if (m_ds.empty())
		return;

	std::vector <const DataSrc*>::const_iterator fnd = find(m_ds.begin(), m_ds.end(), pDDS );

	if (fnd != m_ds.end() )
	{
		std::strstream str;
		str << PrintTime() << " => " << pDDS->Description() << " был изменен" << ends;
		WriteData( str.str() );
	}
	
}