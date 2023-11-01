// DigDSStatePopupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DigDSStatePopupDlg.h"
#include "NanoTubesControlCenter.h"
#include "Automation/DataSrc.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
//////////////////////////////////////////////////////////////////////////

CDiscreteDSTreeCtrl::CDiscreteDSTreeCtrl() :
	m_szMax(0, 0)
{
	VERIFY( m_ImageList.Create( 24, 24, ILC_COLOR32|ILC_MASK, 4, 4 ) );
	m_bitmap.LoadBitmap(IDB_DIGDS_TREE);

	m_ImageList.Add(&m_bitmap, RGB(255, 0, 255));

}

CSize CDiscreteDSTreeCtrl::ShowDiscreteDSTree(std::string dsName)
{
	ASSERT( m_ImageList.GetSafeHandle() != NULL );

	SetImageList( &m_ImageList, TVSIL_NORMAL );


	DeleteAllItems();
	AddDataSource(TVI_ROOT, dsName);

	return m_szMax;
}

int CDiscreteDSTreeCtrl::GetItemIndent( HTREEITEM  hItem ) const
{
	IMAGEINFO ii;
	m_ImageList.GetImageInfo(0, &ii);

	int iconWidth = ii.rcImage.right - ii.rcImage.left;

	int iIndent = GetIndent();

	HTREEITEM hParent = GetParentItem(hItem);
	int levels = 1;
	while (  hParent != NULL  )
	{
		levels++;
		hParent = GetParentItem(hParent);
	}
	
	return  iIndent * levels + iconWidth;
	
}

HTREEITEM  CDiscreteDSTreeCtrl::AddDataSource(HTREEITEM hRoot, std::string dsName)
{
	DiscreteDataSrc* pDDS =  dynamic_cast<DiscreteDataSrc*>( DataSrc::GetDataSource(dsName) );
	AnalogDataSrc*   pADS = dynamic_cast<AnalogDataSrc*>( DataSrc::GetDataSource(dsName) );

	CString strItem; 
	int nImageInd;

	if ( pDDS == NULL && pADS == NULL ) // 
	{
		strItem.Format("<%s> Unknown", dsName.c_str());
		nImageInd = 2;
	}
	
	else if ( pDDS != NULL )
	{
		nImageInd = pDDS->IsEnabled();
		strItem.Format("%s : %s", pDDS->Name().c_str(), pDDS->Description().c_str() );
	}

	else if ( pADS != NULL )
	{
		nImageInd = 3;
		strItem.Format("%s : %s", pADS->Name().c_str(), pADS->Description().c_str() );
	}	


	HTREEITEM hItem = CTreeCtrl::InsertItem(strItem, nImageInd, nImageInd, hRoot);
	
	CSize sz;	
	CDC* pDC = GetDC();
	GetTextExtentPoint(pDC->m_hDC, strItem, strItem.GetLength(), &sz);	
	ReleaseDC(pDC);
	

	int iIndent =  GetItemIndent(hItem);
	m_szMax.cx = max(sz.cx + iIndent, m_szMax.cx);
	m_szMax.cy = sz.cy * ( GetCount() + 2 );


	IComposite* pComposed = dynamic_cast<IComposite*>( DataSrc::GetDataSource(dsName) );
	
	if ( pComposed != NULL )
	{
		for (int i = 0; i < pComposed->Size(); i++)
		{
			const DataSrc* pSubDS = pComposed->GetSubDataSrc(i);
			if (pSubDS != NULL)
				AddDataSource(hItem, pSubDS->Name());
			else
				AddDataSource(hItem, "UNKNOWN");

		}
	}

	Expand(hItem, TVE_EXPAND);

	
	return hItem;
}

CDiscreteDSTreeCtrl::~CDiscreteDSTreeCtrl()
{

}

//////////////////////////////////////////////////////////////////////////





// CDigDSStatePopupDlg dialog

IMPLEMENT_DYNAMIC(CDigDSStatePopupDlg, CAutoHideDialog)

CDigDSStatePopupDlg::CDigDSStatePopupDlg(CWnd* pParent /*=NULL*/)
	: CAutoHideDialog(CDigDSStatePopupDlg::IDD, pParent)
{

}

CDigDSStatePopupDlg::~CDigDSStatePopupDlg()
{
}

void CDigDSStatePopupDlg::DoDataExchange(CDataExchange* pDX)
{
	CAutoHideDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndTreeDS);
}


BEGIN_MESSAGE_MAP(CDigDSStatePopupDlg, CAutoHideDialog)
END_MESSAGE_MAP()


// CDigDSStatePopupDlg message handlers

BOOL CDigDSStatePopupDlg::OnInitDialog()
{
	CAutoHideDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	AddAnchor(IDC_TREE1, __RDA_KEEP, __RDA_BOTH);

	ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDigDSStatePopupDlg::ShowDataSourceState(CPoint &pt,  std::string dsName)
{	
	CSize sz = m_wndTreeDS.ShowDiscreteDSTree(dsName);
	MoveWindow(pt.x, pt.y, int(sz.cx*1.2), int(sz.cy*1.2), TRUE);
	ShowWindow(SW_SHOW);	
}