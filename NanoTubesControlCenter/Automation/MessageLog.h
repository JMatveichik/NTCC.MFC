// MessageLog.h: interface for the CMessageLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGELOG_H__078768BE_1B61_43FF_97F2_FCD636596676__INCLUDED_)
#define AFX_MESSAGELOG_H__078768BE_1B61_43FF_97F2_FCD636596676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>

using namespace std;

class CMessageLog  
{
public:
	bool Create(const char* pszHomePath, bool bClear = false);
	
	CMessageLog();
	virtual ~CMessageLog();

	void LogMessage(const char* pszMessage);

	bool IsReadyToUse() const;

protected:

	HANDLE m_hThread;
	CRITICAL_SECTION m_CriticalSection;
	SYSTEMTIME m_tmCreation;
	string m_strHomePath;
	string m_strLogFilePath;

	ofstream m_streamLog;

	bool m_bReadyToUse;
};

#endif // !defined(AFX_MESSAGELOG_H__078768BE_1B61_43FF_97F2_FCD636596676__INCLUDED_)
