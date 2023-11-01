#pragma once
/*#include "afxcmn.h"*/
#include "AutoHideDialog.h"


class CDiscreteDSTreeCtrl : public CExtWFF < CTreeCtrl >
{
	// Construction
public:
	
	CDiscreteDSTreeCtrl();

	// Attributes
public:

	// Operations
public:

	CSize ShowDiscreteDSTree(std::string stdName);
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfStudioTreeCtrl)
protected:
	HTREEITEM  AddDataSource(HTREEITEM hRoot, std::string dsName);
	int GetItemIndent( HTREEITEM  hItem ) const;
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CDiscreteDSTreeCtrl();
	// Generated message map functions
protected:	

	CSize m_szMax;
	CImageList	m_ImageList;	
	CBitmap		m_bitmap;
};


// CDigDSStatePopupDlg dialog
class CDigDSStatePopupDlg : public CAutoHideDialog
{
	DECLARE_DYNAMIC(CDigDSStatePopupDlg)

public:
	CDigDSStatePopupDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDigDSStatePopupDlg();

// Dialog Data
	enum { IDD = IDD_DIGDS_STATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	void ShowDataSourceState(CPoint &pt,  std::string dsName);

	virtual BOOL OnInitDialog();
	CDiscreteDSTreeCtrl m_wndTreeDS;
};
