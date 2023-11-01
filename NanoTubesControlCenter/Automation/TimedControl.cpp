// TimedControl.cpp: implementation of the TimedControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimedControl.h"
//#include "common.h"
// #include <string>
// #include <map>
// #include <strstream>
// #include <iomanip>
#ifdef _DEBUG
#define new DEBUG_NEW	
#endif


inline unsigned long Milliseconds(const SYSTEMTIME& st)
{
	return 24*3600*1000*st.wDay + 3600*1000*st.wHour + 60*1000*st.wMinute + 1000*st.wSecond + st.wMilliseconds;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TimedControl::TimedControl() :
m_hStopEvent(NULL), m_hThread(NULL), m_nInterval(BASEDELAY)   // User is responsible to reset these members in child constructor if necessary
{	
	//Инициализация критической секции
	::InitializeCriticalSection(&m_cs);
	
}

/**  Деструктор */
TimedControl::~TimedControl()
{
	Stop();

	if (m_hStopEvent)
		::CloseHandle(m_hStopEvent);

	if (m_hThread)
		::CloseHandle(m_hThread);

	::DeleteCriticalSection(&m_cs);

}

/** Запуск контроля над процессом **/
bool TimedControl::Start(unsigned long nInterval/* = 0*/)
{
	if (m_hThread) 
	{
		DWORD dwExitCode;

		::GetExitCodeThread(m_hThread, &dwExitCode);

		if (dwExitCode == STILL_ACTIVE)
			return false;

		::CloseHandle(m_hThread);
	}

	if (m_hStopEvent)
		::CloseHandle(m_hStopEvent);

	m_hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hStopEvent)
		return false;

	if (nInterval > BASEDELAY )
		m_nInterval = nInterval;

	m_hThread = ::CreateThread(NULL, 0, ThreadFunction, this, 0, NULL);
	return m_hThread != NULL;
}

//Функция получения интервала ожидания
unsigned long TimedControl::GetInterval() const
{
	unsigned long nInter; 
		
	Lock();
	nInter = m_nInterval;	
	Unlock();

	return nInter;
}

//Установка нового интервала ожидания 
unsigned long TimedControl::SetInterval(unsigned long interval)
{
	unsigned long nOldInter; 

	Lock();
	
	nOldInter = m_nInterval;
	
	if ( interval >= MINDELAY )
		m_nInterval = interval;

	Unlock();

	return nOldInter;
}


/* Остановка контроля над процессом */
bool TimedControl::Stop()
{
	if (!m_hStopEvent || !m_hThread)
		return false;

	::SetEvent(m_hStopEvent);	
	bool bResult = ::WaitForSingleObject(m_hThread, 120000) == WAIT_OBJECT_0;

	return bResult;
}

void TimedControl::Lock() const 
{
	::EnterCriticalSection(&m_cs);
}

void TimedControl::Unlock() const
{
	::LeaveCriticalSection(&m_cs);
}

/* Главная процедура процесса */
DWORD WINAPI TimedControl::ThreadFunction(LPVOID lpParameter)
{
	TimedControl* pSelf = (TimedControl*)lpParameter;	

	SYSTEMTIME CurTime;	
	::GetSystemTime(&CurTime);

	ULONG lCurTime = Milliseconds(CurTime);		
	ULONG lOldTime = lCurTime;

	while (true) 
	{
		if (::WaitForSingleObject(pSelf->m_hStopEvent, 0) == WAIT_OBJECT_0) 
		{
			pSelf->OnExit();
			break;
		}

		::GetSystemTime(&CurTime);

		lCurTime = Milliseconds(CurTime);			

		ULONG lDelta = lCurTime - lOldTime;
		
		pSelf->Lock();
		unsigned long interval = pSelf->m_nInterval;
		pSelf->Unlock();

		if ( lDelta >= interval ) 
		{
			lOldTime = lCurTime;
			pSelf->ControlProc();
		}
		else 
		{
			::Sleep(MINDELAY);
		}
	}
	
	::CloseHandle(pSelf->m_hThread);
	pSelf->m_hThread = NULL;

	::CloseHandle(pSelf->m_hStopEvent);
	pSelf->m_hStopEvent = NULL;

	return 0;
}

/** Функция, выполняющаяся после остановки потока */
void TimedControl::OnExit()
{	
}





	