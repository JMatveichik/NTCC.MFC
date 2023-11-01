// MessageLog.cpp: implementation of the CMessageLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageLog.h"
#include <Dbghelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageLog::CMessageLog() : m_bReadyToUse(false)

{
	::InitializeCriticalSection(&m_CriticalSection);
}

CMessageLog::~CMessageLog()
{
	::DeleteCriticalSection(&m_CriticalSection);
}

bool CMessageLog::Create(const char *pszHomePath, bool bClear/* = false*/)
{
	m_streamLog.close();

	m_strHomePath = pszHomePath;
	m_strLogFilePath = pszHomePath;
	
	if (m_strLogFilePath.c_str()[m_strLogFilePath.length()-1] != '\\')
		m_strLogFilePath += "\\";

	SYSTEMTIME curTime;
	::GetLocalTime(&curTime);

	m_tmCreation = curTime;

	char szBuffer[MAX_PATH];
	sprintf_s(szBuffer, "%04d\\%02d\\", curTime.wYear, curTime.wMonth );
	
	m_strLogFilePath += szBuffer;
	if (!MakeSureDirectoryPathExists(m_strLogFilePath.c_str() ) )
		return false;

	sprintf_s(szBuffer, "%02d%02d%04d.log", curTime.wDay, curTime.wMonth, curTime.wYear);
	m_strLogFilePath += szBuffer;

	m_streamLog.open(m_strLogFilePath.c_str(), bClear ? ios::out : ios::out|ios::app);
	m_streamLog << "Creation complete" << std::endl;
	if (m_streamLog.fail())
		return false;

	return m_bReadyToUse = true;
}

bool CMessageLog::IsReadyToUse() const
{
	return m_bReadyToUse;
}

void CMessageLog::LogMessage(const char* pszMessage)
{
	::EnterCriticalSection(&m_CriticalSection);

	SYSTEMTIME tmCurr;
	::GetLocalTime(&tmCurr);
	if ( tmCurr.wDay != m_tmCreation.wDay)
		Create(m_strHomePath.c_str(), false);

	char szBuffer[100];
	sprintf_s(szBuffer, 100, "%02d:%02d:%02d.%03d=>", tmCurr.wHour, tmCurr.wMinute, 
		tmCurr.wSecond, tmCurr.wMilliseconds);

	
	m_streamLog << szBuffer;
	m_streamLog << pszMessage;
	int nLen = (int)strlen(pszMessage);

	if (pszMessage[nLen-1] != 10 && pszMessage[nLen-1] != 13) {
		m_streamLog << std::endl;
	}
	//m_streamLog.close();

	::LeaveCriticalSection(&m_CriticalSection);
}