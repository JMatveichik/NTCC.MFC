#include "stdafx.h"
#include "ah_msgprovider.h"
#include "ah_project.h"
#include "ah_users.h"

#include "winmessages.h"

AppMessagesProviderHelper::~AppMessagesProviderHelper()
{

}

AppMessagesProviderHelper::AppMessagesProviderHelper()
{

}

//////////////////////////////////////////////////////////
//регистрирует окно которому будут отправляться информационные сообщения
bool AppMessagesProviderHelper::RegisterOutputMessageDestination(CWnd* pWnd)
{
	if ( pWnd == NULL )
		return false;

	if ( !IsWindow(pWnd->m_hWnd) )
		return false;

	m_pMsgDest.push_back(pWnd);
	return true;
}


void AppMessagesProviderHelper::ShowPopupMessage(COutMessage* pMsg) const
{
	std::auto_ptr<COutMessage> pMsgOut ( new COutMessage( *pMsg) );
	const_cast<AppMessagesProviderHelper*>(this)->LogMessage( pMsgOut.get() );

	//	for (std::vector<CWnd*>::iterator it = m_pMsgDest.begin(); it != m_pMsgDest.end(); ++it)
	//	{	
	//		CWnd* pWnd = (*it);
	//		ASSERT( *it != NULL && IsWindow((*it)->m_hWnd) );
	//		::PostMessage((*it)->m_hWnd, RM_SHOW_USER_MESSAGE, 1, (LPARAM)pCopyMsg);
	//	}

	CWnd* pFrm = AfxGetApp()->m_pMainWnd;
	if ( pFrm != NULL )
		pFrm->PostMessage(RM_SHOW_USER_MESSAGE, 1, (LPARAM)pMsgOut.release() );


}

void AppMessagesProviderHelper::LogMessage(COutMessage* pMsg)
{
	CString msg;

	switch(pMsg->MessageType() )
	{
	case COutMessage::MSG_OK:
		msg = "OK   >";
		break;
	case COutMessage::MSG_WARNING:
		msg = "WARN >";
		break;
	case COutMessage::MSG_ERROR:
		msg = "ERR  >";
		break;
	case COutMessage::MSG_OBA_SCRIPT:
		msg = "SCR  >";
		break;
	}

	LPNTCC_APP_USER pUser = AppUsersHelper::Instance().GetCurrentUser();
	if ( pUser != NULL )
	{
		msg += pUser->screenName.c_str();
		msg += " > ";
	}

	if ( strlen(pMsg->MessageTitle()) != 0)
	{
		msg += pMsg->MessageTitle();
		msg += " > ";
	}	

	msg += pMsg->MessageText();


	if ( !m_msgLog.IsReadyToUse() ) 
	{
		std::string path = NTCCProject::Instance().GetPath(NTCCProject::PATH_LOG, "SCRIPTS\\", false );
		m_msgLog.Create( path.c_str() );
	}

	m_msgLog.LogMessage((LPCTSTR)msg);
}

void AppMessagesProviderHelper::ShowPopupMessage(LPCTSTR text, LPCTSTR title, COutMessage::MESSAGE_TYPE type/* = COutMessage::MSG_OK*/ ) const
{
	COutMessage msg(text, title, type);
	ShowPopupMessage(&msg);
}

void AppMessagesProviderHelper::ShowMessage(std::string msg, COutMessage::MESSAGE_TYPE msgType/* = COutMessage::MSG_OK*/, BOOL brl/* = TRUE*/, BOOL tp/* = TRUE*/) const
{

	std::auto_ptr<COutMessage> pMsgOut ( new COutMessage( msg.c_str(), "", msgType, brl, tp ) );
	const_cast<AppMessagesProviderHelper*>(this)->LogMessage(pMsgOut.get());

	//	for (std::vector<CWnd*>::iterator it = m_pMsgDest.begin(); it != m_pMsgDest.end(); ++it)
	///	{	
	//		ASSERT( *it != NULL && IsWindow((*it)->m_hWnd) );
	//		(*it)->PostMessage(RM_SHOW_USER_MESSAGE, 0, (LPARAM)pMsgOut.release());
	//	}

	CWnd* pFrm = AfxGetApp()->m_pMainWnd;
	if (pFrm != NULL)
		pFrm->PostMessage( RM_SHOW_USER_MESSAGE, 0, (LPARAM)pMsgOut.release() );

}

void AppMessagesProviderHelper::ShowMessage(std::istream& msg, COutMessage::MESSAGE_TYPE msgType/* = COutMessage::MSG_OK*/) const
{	

	do 
	{
		std::string line;		
		getline(msg, line);

		if ( line.empty() )
			continue;

		std::auto_ptr<COutMessage> pMsgOut ( new COutMessage( line.c_str(), "", msgType , TRUE ) );
		const_cast<AppMessagesProviderHelper*>(this)->LogMessage(pMsgOut.get());

		//		for (std::vector<CWnd*>::iterator it = m_pMsgDest.begin(); it != m_pMsgDest.end(); ++it)
		//		{	
		//			ASSERT( *it != NULL && IsWindow((*it)->m_hWnd) );		
		//			(*it)->PostMessage(RM_SHOW_USER_MESSAGE, 0, (LPARAM)pMsgOut.release());
		//		}
		CWnd* pFrm = AfxGetApp()->m_pMainWnd;
		pFrm->PostMessage( RM_SHOW_USER_MESSAGE, 0, (LPARAM)pMsgOut.release() );

	} while (msg);

}