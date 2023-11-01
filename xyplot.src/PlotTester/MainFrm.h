// MainFrm.h : interface of the CMainFrame class
//


#pragma once

 //__PROFUISAPPWIZ_KEY_RCB2


class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
protected:
	WINDOWPLACEMENT m_dataFrameWP;
 //__PROFUISAPPWIZ_KEY_PERS_POS_MAINFRM
public:

// Operations
public:

// Overrides
public:
	virtual BOOL DestroyWindow();
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};
