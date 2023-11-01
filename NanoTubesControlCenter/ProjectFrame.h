#pragma once


#include "ah_users.h"

// CProjectFrame frame


class CProjectFrame : public CMDIChildWnd, public CExtDynamicBarSite
{
	DECLARE_DYNCREATE(CProjectFrame)
protected:

	CProjectFrame();           // protected constructor used by dynamic creation
	virtual ~CProjectFrame();
	
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
	
	virtual void ActivateFrame(int nCmdShow);

public:
	
	void OnChangeUser(const NTCC_APP_USER* pUser);



public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


