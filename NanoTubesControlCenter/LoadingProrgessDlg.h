#pragma once
#include "resource.h"
#include "winmessages.h"

// CLoadingProrgessDlg dialog

class CLoadingProrgessDlg : public CExtNCW < CExtResizableDialog >
{
	//DECLARE_DYNAMIC(CLoadingProrgessDlg)

public:
	CLoadingProrgessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadingProrgessDlg();

// Dialog Data
	enum tagLoadDlgID{ IDD = IDD_LOADINGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);

	long AddLoadingSection(DOCUMENT_SECTION_INFO& sec);
	
	afx_msg LRESULT OnLoadingInit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddLoadingSection(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddInfoSection(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStepSection(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadComplete(WPARAM wParam, LPARAM lParam);

protected:

	CExtGridWnd m_wndGrid;
	CExtLabel m_lblCurStep;
	CExtProgressWnd m_wndCommonProgress;

	int curSectionRow;
public:

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedChkIgnoreError();
	
};
