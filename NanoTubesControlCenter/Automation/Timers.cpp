#include "stdafx.h"
#include "Timers.h"

#include "..\ah_xml.h"
#include "..\ah_errors.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

std::map<std::string, BaseTimer*> BaseTimer::m_mapTimers;

BaseTimer::BaseTimer(void) : 
m_started( false ),
m_lDur(0),
m_pDurationDS(NULL),
m_startState(true),
m_stopState(false)
{
	
}

BaseTimer::~BaseTimer(void)
{

}

void BaseTimer::AddTimerHandler(UINT handlerType, TIMERHANDLER handler)
{
	switch(handlerType)
	{
	case STARTHANDLER:
		evOnTimerStart.connect(handler);
		break;

	case STOPHANDLER:
		evOnTimerStop.connect(handler);
		break;
	
	case OVERFLOWHANDLER:
		evOnTimerOverFlow.connect(handler);
		break;

	default:
		break;
	}
}

bool BaseTimer::AddTimer(BaseTimer* pTimer)
{
	std::strstream str;
	std::string id = pTimer->Name();

	if ( id.empty() )
	{
		AppErrorsHelper::Instance().SetLastError(DSCE_EMPTY_PARAMETR, "Пустой идентификатор");
		return false;
	}

	std::map<std::string, BaseTimer*>::const_iterator fnd;
	fnd =  m_mapTimers.find( id );

	if ( fnd != m_mapTimers.end() )
	{
		str << "Timer with name (" << id.c_str() <<") already exist" << std::ends;
		AppErrorsHelper::Instance().SetLastError(DSCE_DUPLICATE_NAME, str.str() );
		return false;
	}

	return m_mapTimers.insert( std::make_pair(id, pTimer) ).second; 
}

BaseTimer* BaseTimer::GetTimer(std::string name)
{
	std::map<std::string, BaseTimer*>::const_iterator fnd;
	fnd =  m_mapTimers.find( name );

	if ( fnd == m_mapTimers.end() )
		return NULL;
	else
		return (*fnd).second;
}

void BaseTimer::ClearAll()
{
	for(std::map<std::string, BaseTimer*>::iterator it = m_mapTimers.begin(); it != m_mapTimers.end(); ++it )
		delete (*it).second;

	m_mapTimers.clear();
	
}

std::string BaseTimer::Name() const
{
	return m_strName;
}

long BaseTimer::Duration() const
{
	if ( m_pDurationDS != NULL )
		m_lDur = (long)(m_pDurationDS->GetValue()*60);

	return m_lDur;
}

long BaseTimer::StartTime() const
{
	return (long)m_stStart.GetTime();
}

bool BaseTimer::InitDSListen(bool bStart,  std::string& val)
{
	std::vector< std::string > parts;
	boost::split(parts, val,  std::bind2nd(std::equal_to<char>(), ':'));

	if ( parts.size() != 2 )
		return false;

	DiscreteDataSrc* pDS = NULL;
	bool bState = false;

	std::string name = parts[0];
	if (name.length() > 0 )
	{	
		pDS = dynamic_cast<DiscreteDataSrc*>(DataSrc::GetDataSource(name));
			
		if ( pDS == NULL ) 
			return false;
		
		bState = atoi(parts[1].c_str()) > 0;		
	}
	
	if ( bStart )
	{
		pDS->AddOnChangeState( boost::bind( &BaseTimer::OnStartDSChangeState, this, _1) );
		m_startState = bState;
	}
	else
	{
		pDS->AddOnChangeState( boost::bind( &BaseTimer::OnStopDSChangeState, this, _1) );
		m_stopState = bState;
	}

	return true;
}

//создание таймера из XML узла
bool BaseTimer::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;
	if (!appXML.GetNodeAttributeByName(pNode, "NAME", sAtr) )
	{
		str << "Name must be set for timer" << std::ends;		
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );
		return false;
	}
	m_strName = sAtr.c_str();

	if (!appXML.GetNodeAttributeByName(pNode, "STARTDS", sAtr) )
	{
		str << "Discrete data source id for start timer must be set" << std::ends;		
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );
		return false;
	}
	if ( !InitDSListen(true, sAtr) )
	{
		str << "Invalid data source id for start timer" << std::ends;		
		err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
		return false;
	}

	if (!appXML.GetNodeAttributeByName(pNode, "STOPDS", sAtr) )
	{
		str << "Discrete data source id for stop timer must be set" << std::ends;		
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );
		return false;
	}
	if ( !InitDSListen(false, sAtr) )
	{
		str << "Invalid data source id for stop timer" << std::ends;		
		err.SetLastError(DSCE_INVALID_DS_ID, str.str() );
		return false;
	}
	
	if ( appXML.GetNodeAttributeByName(pNode, "DURATION", sAtr) )
	{
		m_pDurationDS = dynamic_cast<const AnalogDataSrc*>(DataSrc::GetDataSource(sAtr.c_str()));
		if ( m_pDurationDS != NULL )
			m_lDur = (long)(m_pDurationDS->GetValue()*60);
		else
			m_lDur = atol(sAtr.c_str());
	}

	
	m_strOutFmt = "%M:%S";
	if (appXML.GetNodeAttributeByName(pNode, "FMT", sAtr) )
		m_strOutFmt = sAtr.c_str();

	return true;
}		

//создание таймера из XML узла
BaseTimer* BaseTimer::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<BaseTimer> obj ( new BaseTimer() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool BaseTimer::IsStarted() const
{
	return m_started;
}

bool BaseTimer::Start()
{
	if ( m_started )
		return false;

	m_stStart = CTime::GetCurrentTime();	
	m_started = true;

	//обрабатываем событие запуска таймера
	evOnTimerStart(this);

	return true;
}

bool BaseTimer::Stop()
{
	m_started = false;
	m_stStart = CTime::GetCurrentTime();

	//обрабатываем событие запуска таймера
	evOnTimerStop(this);

	return true;
}

void BaseTimer::OnStartDSChangeState(const  DiscreteDataSrc* pDS )
{
	if ( m_pDurationDS != NULL )
		m_lDur = (long)(m_pDurationDS->GetValue()*60);

	if ( pDS->IsEnabled() == m_startState )
		Start();
}

void BaseTimer::OnStopDSChangeState(const DiscreteDataSrc* pDS )
{
	if ( pDS->IsEnabled() == m_stopState )
		Stop();
}


const char* BaseTimer::ToString()
{
	if ( !m_started )
		m_stStart = CTime::GetCurrentTime();

	CTime stCur = CTime::GetCurrentTime();
	CTimeSpan elapsedTime = stCur - m_stStart;

	CString et = elapsedTime.Format( m_strOutFmt.c_str() );
	m_strElapsedTime = et.GetBuffer();
	et.ReleaseBuffer();
	
	return m_strElapsedTime.c_str();

}

bool BaseTimer::IsOwerflowed() const
{
	if ( !m_started || m_lDur == 0)
		return false;

	if ( m_pDurationDS != NULL )
		(const_cast<BaseTimer*>(this))->m_lDur = (long)(m_pDurationDS->GetValue()*60);

	CTime stCur = CTime::GetCurrentTime();
	CTimeSpan elapsedTime = stCur - m_stStart;

	CTimeSpan dur(m_lDur);
	if ( elapsedTime <= dur )
		return false;
	else 
	{
		evOnTimerOverFlow(this);
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

CountDownTimer::CountDownTimer()
{

}

CountDownTimer::~CountDownTimer()
{

}

//создание таймера из XML узла
bool CountDownTimer::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !BaseTimer::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	std::string sAtr;

	if ( !appXML.GetNodeAttributeByName(pNode, "DURATION", sAtr) )
	{
		str << "Duration not set for timer" << std::ends;		
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );
		return false;
	}

	m_pDurationDS = dynamic_cast<const AnalogDataSrc*>(DataSrc::GetDataSource(sAtr.c_str()));
	if ( m_pDurationDS != NULL )
		m_lDur = (long)(m_pDurationDS->GetValue()*60);
	else
		m_lDur = atol(sAtr.c_str());

	return true;
}



//создание таймера из XML узла
CountDownTimer* CountDownTimer::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<CountDownTimer> obj ( new CountDownTimer() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}


const char* CountDownTimer::ToString() 
{
	if ( !m_started )
		m_stStart = CTime::GetCurrentTime();

	CTime stCur = CTime::GetCurrentTime();
	
	CTimeSpan elapsedTime = stCur - m_stStart;
	CTimeSpan dur(m_lDur);

	CTimeSpan left(0);
	CString et;
	
	if ( elapsedTime <= dur )
	{
		left = dur - elapsedTime;
		et = left.Format( m_strOutFmt.c_str() );		
	}
	else {

		left = elapsedTime - dur;
		std::string fmt("-");
		fmt += m_strOutFmt.c_str();
		et = left.Format( fmt.c_str() );
	}
	
	m_strElapsedTime = et.GetBuffer();
	et.ReleaseBuffer();

	return m_strElapsedTime.c_str();
}