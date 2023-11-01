#pragma once
#include "..\resource.h"

// CPlotPropertiesDialog dialog

class CPlotPropertiesDialog : public CExtNCW < CExtResizableDialog >
{
	DECLARE_DYNAMIC(CPlotPropertiesDialog)
	

public:
	CPlotPropertiesDialog(CWnd* pParent, long lPlotID);   // standard constructor
	virtual ~CPlotPropertiesDialog();

// Dialog Data
	enum { IDD = IDD_DLG_PLOT_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CAxisPropertyDlg* pDlgAxisPropXT;
	CAxisPropertyDlg* pDlgAxisPropXB;
	CAxisPropertyDlg* pDlgAxisPropYL;
	CAxisPropertyDlg* pDlgAxisPropYR;	

	CGeneralPropDlg*  pDlgGenProp;
	CProfilesPropDlg*  pDlgProfProp;
	CLevelsPropDlg*  pDlgLevelsProp;


	//CListCtrl  m_ctrlPagesList;

	CIconListBox m_wndList;

	CImageList m_pagesIcons;
	CFont      fontPagesList;
	CFont      fontPagesTitle;
	CWnd*	   m_pActivePage;

	XYPlot*	   plot;

	CExtLabel	   m_pageTitle;	
	
	CExtButton	btnLoad;
	CExtButton  btnSave;

	std::vector<COLORREF> pagesColor;


	DECLARE_MESSAGE_MAP()
public:
	

	virtual BOOL OnInitDialog();
//	afx_msg void OnLvnItemchangingPagesList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnSelchangeList();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnSave();

	afx_msg void OnLoadFromFile();
	afx_msg void OnSaveToFile();
	afx_msg void OnLoadDefault();
	afx_msg void OnSaveAsDefault();
	afx_msg void OnBnClickedOk();
};
