#pragma once

#include "singletone.h"
#include "ah_msgprovider.h"

#ifndef _APP_LOADCFG_HELPER_H_INCLUDED_
#define _APP_LOADCFG_HELPER_H_INCLUDED_

//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]
//////////////////////////////////////////////////////////////////////////

class CLoadingProrgessDlg;

class AppLoadingHelper : public Singletone<AppLoadingHelper>
{
	friend class Singletone<AppLoadingHelper>;

public: 

	virtual ~AppLoadingHelper();

protected:

	AppLoadingHelper();
	AppLoadingHelper(const AppLoadingHelper& );
	const AppLoadingHelper& operator= (const AppLoadingHelper&);
public:	

	BOOL Initialize(const char* pszFilePath);
	BOOL Initialize(IXMLDOMDocument* pXMLDoc);

	void Release();

	void MakeHeader();
	long AddSection(std::string title, std::string name, int nSectionSize);
	void CloseCurrentSection();

	long StepSection(int nSecId, bool bOk = true);

	void AddContentMessage(std::string text, COutMessage::e_MessageType mt);

	BOOL MakeReport(std::string templ, std::string outPath, BOOL bRes);

	UINT LoadingDlgResult(BOOL bRes = false); 
	

protected:

	CLoadingProrgessDlg* wndLoad;

	std::vector <std::string> vTitleLines;
	std::vector <std::string> vContentLines;

};


#endif _APP_LOADCFG_HELPER_H_INCLUDED_
