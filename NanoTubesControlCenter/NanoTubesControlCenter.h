// NanoTubesControlCenter.h : main header file for the NanoTubesControlCenter application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2007_R1 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerXP 
//#define SELECTED_PAINT_MANAGER CExtPaintManager 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerXP 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2003 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2003NoThemes 
#define SELECTED_PAINT_MANAGER CExtPaintManagerStudio2005 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerNativeXP
//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2007_R2_LunaBlue 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2007_R2_Obsidian

//
// Prof-UIS command manager profile name
//
#define __PROF_UIS_PROJECT_CMD_PROFILE_NAME _T("CNanoTubesControlCenterApp-command-manager-profile")
#define __PROF_UIS_PROJECT_DLG_PERSIST_REG_KEY _T("CNanoTubesControlCenterApp-resizable-dialog-positions")


// CNanoTubesControlCenterApp:
// See NanoTubesControlCenter.cpp for the implementation of this class
//

class NTCCDataSrv;
class CPlotDlg;

class CNanoTubesControlCenterApp : public CWinApp
{
public:
	CNanoTubesControlCenterApp();


// Overrides
public:
	
	virtual BOOL InitInstance();
	virtual void ParseCommandLine( CCommandLineInfo& rCmdInfo  );

	virtual int Run( );

// Implementation
	//
	// Prof-UIS advanced options
        //
	void SetupUiAdvancedOptions();
	
	static void  AddDocument(CDocument* pDoc);

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	static CString GetAppInfo();
	
	BOOL SelectProject() const;
	
	CPlotDlg* GetPlotDlg();

	std::ofstream log;

protected:
	
	ULONG_PTR guiPlusToken;
	static std::vector<CDocument*> m_vSubDocs;	

	#ifdef _DEBUG
		//_CrtMemState _ms; 
	#endif


	NTCCDataSrv* pDataSrv;
	CPlotDlg* pPlotDlgWnd;

	std::map<std::string, std::string> cmdLineParams;

	bool GetCmdValue(std::string key, std::string& val) const;
public:
	
	afx_msg void  OnAddUser();
	afx_msg void  OnLockApp();

	afx_msg void OnAppExit();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	virtual int ExitInstance();
	afx_msg void OnViewLoadResults();
	afx_msg void OnViewActualConfig();

};

extern CNanoTubesControlCenterApp theApp;