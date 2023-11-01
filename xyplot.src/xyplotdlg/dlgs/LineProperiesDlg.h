#pragma once

#include "..\resource.h"

// CLineProperiesDlg dialog

//TODO: Добавить сохранение и загрузку типов линий в реестр
class CLineProperiesDlg : public CExtNCW < CExtResizableDialog >
{
	DECLARE_DYNAMIC(CLineProperiesDlg)

public:
	CLineProperiesDlg(CWnd* pParent);   // standard constructor
	virtual ~CLineProperiesDlg();

// Dialog Data
	enum { IDD = IDD_DLG_LINE_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void DrawExampleLine();

	DECLARE_MESSAGE_MAP()

protected:
	CExtColorButton	btnLineColor;
	int				lineType;
	CString			lineTemplate;
	int				lineWidth;
	COLORREF		lineColor;	

	CBitmap			exampBmp;
	CDC				memDC;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelendokCmbLineType();
	afx_msg void OnDeltaposSpinLineWidth(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
	afx_msg void OnPaint();
};
