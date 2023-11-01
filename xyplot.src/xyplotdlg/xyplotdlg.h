// xyplotdlg.h : main header file for the xyplotdlg DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2007_R1 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerXP 
//#define SELECTED_PAINT_MANAGER CExtPaintManager 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerXP 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2003 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2003NoThemes 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerStudio2005 
//#define SELECTED_PAINT_MANAGER CExtPaintManagerNativeXP
//#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2007_R2_LunaBlue 
#define SELECTED_PAINT_MANAGER CExtPaintManagerOffice2007_R2_Obsidian


// CxyplotdlgApp
// See xyplotdlg.cpp for the implementation of this class
//

class CXYPlotDlgApp : public CWinApp
{
public:
	CXYPlotDlgApp();

// Overrides
public:
	virtual BOOL InitInstance();


	DECLARE_MESSAGE_MAP()

protected:

	void SetupUiAdvancedOptions();
};
