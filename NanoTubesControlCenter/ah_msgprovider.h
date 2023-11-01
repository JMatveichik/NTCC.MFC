#pragma once

#include <vector>
#include "singletone.h"
#include "Automation/MessageLog.h"

#ifndef _APP_MESSAGE_PROVIDER_HELPER_H_INCLUDED_
#define _APP_MESSAGE_PROVIDER_HELPER_H_INCLUDED_

class COutMessage : public CObject
{
public:
	
	typedef enum e_MessageType 
	{ 
		MSG_OK, 
		MSG_WARNING, 
		MSG_ERROR, 		
		MSG_OBA_SCRIPT,
		MSG_TEXT

	} MESSAGE_TYPE;

	COutMessage( LPCTSTR sText, LPCTSTR sTitle, MESSAGE_TYPE eMT = MSG_OK, BOOL breakLine = TRUE, BOOL useTimePrefix = FALSE) : 
		m_sText(sText), m_sTitle(sTitle), m_type(eMT), m_breakLine(breakLine), m_btimePrefix(useTimePrefix)
		{
		}

		COutMessage(const COutMessage &msg) : m_sText(msg.m_sText), m_sTitle(msg.m_sTitle), m_type(msg.m_type), m_breakLine(msg.m_breakLine), m_btimePrefix(msg.m_btimePrefix)
		{
		}

		LPCTSTR	MessageText() const 
		{ 
			return m_sText; 
		}; 

		LPCTSTR	MessageTitle() const
		{
			return m_sTitle;
		}

		MESSAGE_TYPE MessageType() const
		{
			return m_type;
		}

		BOOL BreakLine () const
		{
			return m_breakLine;
		}

		BOOL UseTimePreFix () const
		{
			return m_btimePrefix;
		}

private:

	CString				m_sText;
	CString				m_sTitle;
	BOOL				m_breakLine; 
	BOOL				m_btimePrefix; 
	MESSAGE_TYPE		m_type;

};


//////////////////////////////////////////////////////////////////////////
//  [5/17/2012 Johnny A. Matveichik]
class AppMessagesProviderHelper : public Singletone<AppMessagesProviderHelper>
{
	friend class Singletone<AppMessagesProviderHelper>;
public:
	virtual ~ AppMessagesProviderHelper();

protected:

	AppMessagesProviderHelper();
	AppMessagesProviderHelper(const AppMessagesProviderHelper& );
	const AppMessagesProviderHelper& operator= (const AppMessagesProviderHelper&);

public:

	bool RegisterOutputMessageDestination(CWnd* pWnd);
	void ShowMessage(std::string msg, COutMessage::MESSAGE_TYPE msgType = COutMessage::MSG_OK, BOOL brl = TRUE, BOOL tp = TRUE) const;
	void ShowMessage(std::istream& msg, COutMessage::MESSAGE_TYPE msgType = COutMessage::MSG_OK) const;


	void ShowPopupMessage(LPCTSTR msg, LPCTSTR title, COutMessage::MESSAGE_TYPE type = COutMessage::MSG_OK) const;
	void ShowPopupMessage(COutMessage* pMsg) const;

protected:

	void LogMessage(COutMessage* pMsg);

	std::vector<CWnd*> m_pMsgDest;	
	CMessageLog m_msgLog;
};



#endif _APP_MESSAGE_PROVIDER_HELPER_H_INCLUDED_
