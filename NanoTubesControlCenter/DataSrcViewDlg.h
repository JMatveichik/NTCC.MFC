#pragma once

#include "resource.h"
#include "ProjectDoc.h"
// CDataSrcViewDlg dialog


class CDataSrcViewDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CDataSrcViewDlg)

public:
	CDataSrcViewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDataSrcViewDlg();

// Dialog Data
	enum { IDD = IDD_DS_VIEW_DLG };

	virtual BOOL Create(UINT nIDTemplate, CWnd* pParent, CProjectDoc* pDoc);  

	void SetViewLayout(UINT nType);

	void UpdateDataList();

protected:
	

	HTREEITEM InsertGroup( LPDS_VIEW_GROUP group );
	HTREEITEM InsertDataSource(HTREEITEM hGroup, std::string  dsID, LPDS_VIEW_GROUP pGroup);

	

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	

	afx_msg void  OnDataSrcViewUserLayout();
	afx_msg void  OnDataSrcViewByTypeLayout();
	afx_msg void  OnDataSrcViewByDeviceLayout();

	afx_msg void  OnUpdateDataSrcViewUserLayout(CCmdUI* pCmdUI);
	afx_msg void  OnUpdateDataSrcViewByTypeLayout(CCmdUI* pCmdUI);
	afx_msg void  OnUpdateDataSrcViewByDeviceLayout(CCmdUI* pCmdUI);

	afx_msg void  OnDSGridDblClick( NMHDR * pNotifyStruct, LRESULT * result );
	DECLARE_MESSAGE_MAP()


public:
	
	virtual BOOL OnInitDialog();

	UINT m_nLayoutType;
	CProjectDoc* m_pDoc;

protected:

	enum tagDataColumns
	{
		COL_IND_NAME, 
		COL_IND_SIGNAL, 
		COL_IND_VALUE, 
		COL_IND_UNITS, 
		COL_IND_DESCRIPTION
	};

	bool SetInnerItem(  HTREEITEM hItem,  int col,  LPCTSTR strText,  int iconInd = -1,  COLORREF clr = -1L,  bool bold = false, bool italic = false);

	int AddGridIcon(HICON hIcon);
	int AddGridIcon(UINT idIcon, int cx = 16);
	int AddGridIcon(std::string name);

	void PrepareCell(CExtGridCell* pCell, DWORD dwStyle = -1L, COLORREF clr = -1L, LONG fntHeight = -1L, LONG fntWeight = -1L, __EXT_MFC_SAFE_LPCTSTR sFaseName = NULL);
	void PrepareCell(HTREEITEM hItem, INT nCol, DWORD dwStyle = -1L, COLORREF clr = -1L, LONG fntHeight = -1L, LONG fntWeight = -1L, __EXT_MFC_SAFE_LPCTSTR sFaseName = NULL);

	void SetItemColor(HTREEITEM hItem, COLORREF clr);


	CExtToolControlBar	m_wndToolBar;
	CExtTreeGridWnd		m_wndDSList;	// "Иcточники данных"

	std::map<LPDS_VIEW_GROUP, HTREEITEM> groupNodes;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnDataSrcViewDynamic();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
