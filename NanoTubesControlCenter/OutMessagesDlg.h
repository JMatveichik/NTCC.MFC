#pragma once


// COutMessagesDlg dialog

class COutMessagesDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(COutMessagesDlg)

public:
	COutMessagesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COutMessagesDlg();

// Dialog Data
	enum { IDD = IDD_OUT_MSG_DLG };

protected:
	BOOL InitMsgList();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnMessageComming(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:

	CListCtrl m_wndMsgList;
	CImageList m_ImageList;
	CExtToolControlBar m_wndToolBar;
	CBitmap m_bitmap;



public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	
};
