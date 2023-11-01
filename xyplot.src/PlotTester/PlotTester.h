// PlotTester.h : main header file for the PlotTester application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//
// Prof-UIS command manager profile name
//
#define __PROF_UIS_PROJECT_CMD_PROFILE_NAME _T("CPlotTesterApp-command-manager-profile")

#define __PROF_UIS_PROJECT_DLG_PERSIST_REG_KEY _T("CPlotTesterApp-resizable-dialog-positions")


// CPlotTesterApp:
// See PlotTester.cpp for the implementation of this class
//

class CPlotTesterApp : public CWinApp
{
public:
	CPlotTesterApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	//
	// Prof-UIS advanced options
        //
	void SetupUiAdvancedOptions();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPlotTesterApp theApp;