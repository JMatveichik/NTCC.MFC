// ModuleFrm.h : interface of the CModuleFrame class
//
#pragma once

#include "DLGs\OutputDSControlDlg.h"
#include "FSDoc.h"

class CNoCloseControlBar : public CExtControlBar
{
	virtual void OnNcAreaButtonsReinitialize()
	{
		INT nCountOfNcButtons = NcButtons_GetCount();
		if( nCountOfNcButtons > 0 )
			return;

		//NcButtons_Add( new CExtBarNcAreaButtonClose(this) );

		#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			NcButtons_Add( new CExtBarNcAreaButtonAutoHide(this) );
		#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			NcButtons_Add( new CExtBarNcAreaButtonMenu(this) );
	}

};

class CModuleFrame : public CMDIChildWnd, public CExtDynamicBarSite
{
	DECLARE_DYNCREATE(CModuleFrame)
public:
	CModuleFrame();

// Attributes
public:

// Operations
public:

// Overrides	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	BOOL AddControlBar(CONTROLBAR_INFO& bi);
	
// Implementation
public:
	virtual ~CModuleFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
			
	//  [12/28/2010 Johnny A. Matveichik]
	std::vector <CNoCloseControlBar*>			m_pOutBars;
	std::vector <CExtResizableDialog*>		m_pOutDlgs;

	

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:

	virtual LRESULT WindowProc(UINT message,  WPARAM wParam, LPARAM lParam);

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);	

public:

	virtual void ActivateFrame(int nCmdShow = -1);	
};
