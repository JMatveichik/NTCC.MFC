// OutputDSControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OutputDSControlDlg.h"
#include "..\NanoTubesControlCenter.h"
#include "..\Automation\Converters.h"
#include "..\winmessages.h"
#include <boost/math/special_functions/round.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// COutputDSControlDlg dialog

#define ID_APPLY_ALL_BTN    1998
#define ID_PRESET_BTN   1999

#define ID_PRESET_FIRST 2000
#define ID_PRESET_LAST  2020

std::map<std::string, COutputDSControlDlg*> COutputDSControlDlg::m_usedDSMap;

IMPLEMENT_DYNAMIC(COutputDSControlDlg, CAutoCalcSizeDialog)

BEGIN_MESSAGE_MAP(COutputDSControlDlg, CExtResizableDialog)
	ON_WM_HSCROLL()	
	ON_MESSAGE( RM_ANALOG_VALUE_CHANGE, OnAnalogValueChange )	
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnTtnNeedText)
END_MESSAGE_MAP()


COutputDSControlDlg::COutputDSControlDlg(CONTROLBAR_INFO cbi, CWnd* pParent /*=NULL*/)
: CAutoCalcSizeDialog(COutputDSControlDlg::IDD, pParent), cbInfo(cbi) 
{
	numPadToChar.insert(make_pair( VK_NUMPAD0, '0'));
	numPadToChar.insert(make_pair( VK_NUMPAD1, '1'));
	numPadToChar.insert(make_pair( VK_NUMPAD2, '2'));
	numPadToChar.insert(make_pair( VK_NUMPAD3, '3'));
	numPadToChar.insert(make_pair( VK_NUMPAD4, '4'));
	numPadToChar.insert(make_pair( VK_NUMPAD5, '5'));
	numPadToChar.insert(make_pair( VK_NUMPAD6, '6'));
	numPadToChar.insert(make_pair( VK_NUMPAD7, '7'));
	numPadToChar.insert(make_pair( VK_NUMPAD8, '8'));
	numPadToChar.insert(make_pair( VK_NUMPAD9, '9'));
	numPadToChar.insert(make_pair( VK_DECIMAL, '.'));


	AppUsersHelper::Instance().AddOnUserChanged( boost::bind( &COutputDSControlDlg::OnChangeUser, this, _1 ) );
}

COutputDSControlDlg::~COutputDSControlDlg()
{
	for (std::vector<AODSBLOCK>::iterator it = m_vdsBlocks.begin(); it < m_vdsBlocks.end(); ++it)
	{
		(*it).pLblName.second->DestroyWindow();		delete (*it).pLblName.second;
		(*it).pLblBack.second->DestroyWindow();		delete (*it).pLblBack.second;
		(*it).pLblMinVal.second->DestroyWindow();	delete (*it).pLblMinVal.second;
		(*it).pLblCurVal.second->DestroyWindow();	delete (*it).pLblCurVal.second;
		(*it).pLblMaxVal.second->DestroyWindow();	delete (*it).pLblMaxVal.second;
		(*it).pSlider.second->DestroyWindow();		delete (*it).pSlider.second;
		(*it).pApplyBtn.second->DestroyWindow();	delete (*it).pApplyBtn.second;	
		
	}
	btnPresets.DestroyWindow();
	DestroyWindow();
}

int COutputDSControlDlg::GetBlockIndex(const AnalogOutputDataSrc* pDS) const
{
	int ind = 0;
	for (std::vector<AODSBLOCK>::const_iterator it = m_vdsBlocks.begin(); it < m_vdsBlocks.end(); ++it, ind++)
	{
		if ( (*it).pAODS == pDS )
			return ind;
	}
	return -1;
}

int COutputDSControlDlg::GetBlockIndex(UINT ID) const
{
	int ind = 0;
	for (std::vector<AODSBLOCK>::const_iterator it = m_vdsBlocks.begin(); it < m_vdsBlocks.end(); ++it, ind++)
	{
		if ( (*it).pLblName.first == ID ||
			 (*it).pLblMinVal.first == ID || 
			 (*it).pLblCurVal.first == ID ||
			 (*it).pLblMaxVal.first == ID ||
			 (*it).pSlider.first == ID ||
			 (*it).pApplyBtn.first == ID )
			 return ind;
	}
	return -1;
}

int COutputDSControlDlg::GetBlockIndex(HWND hWnd) const
{
	int ind = 0;
	for (std::vector<AODSBLOCK>::const_iterator it = m_vdsBlocks.begin(); it < m_vdsBlocks.end(); ++it, ind++)
	{
		if ( (*it).pLblName.second->m_hWnd == hWnd ||
			(*it).pLblMinVal.second->m_hWnd == hWnd || 
			(*it).pLblCurVal.second->m_hWnd == hWnd ||
			(*it).pLblMaxVal.second->m_hWnd == hWnd ||
			(*it).pSlider.second->m_hWnd == hWnd ||
			(*it).pApplyBtn.second->m_hWnd == hWnd )
			return ind;
	}
	return -1;
}

int COutputDSControlDlg::GetBlockIndex(const std::string id) const
{
	int ind = 0;
	for (std::vector<AODSBLOCK>::const_iterator it = m_vdsBlocks.begin(); it < m_vdsBlocks.end(); ++it, ind++)
	{
		if ( (*it).pAODS->Name() == id)
			return ind;
	}
	return -1;
}

void COutputDSControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CAutoCalcSizeDialog::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_SLD_OUT, sld);
	if ( btnPresets.m_hWnd != NULL )
		DDX_Control(pDX, ID_PRESET_BTN, btnPresets);

	for (std::vector<AODSBLOCK>::iterator it = m_vdsBlocks.begin(); it < m_vdsBlocks.end(); it++)
	{
		UINT ID		= (*it).pLblName.first;
		CWnd* wnd	= (*it).pLblName.second;

		DDX_Control(pDX, ID, *wnd);


		ID	= (*it).pSlider.first;
		wnd	= (*it).pSlider.second;

		DDX_Control(pDX, ID, *wnd);
	}	
}



UINT GetFreeCtrlID (HWND hDlg)
{
    for (UINT nID = 4000;nID < 0xFFFF; nID++)
    {
        if ( GetDlgItem (hDlg, nID) == NULL )
            return nID;
    }
    return 0; // error
}

void COutputDSControlDlg::OnAnalogOutputValueChanged(const AnalogOutputDataSrc* pChangedDS)
{

	::SendMessage(m_hWnd, RM_ANALOG_VALUE_CHANGE, 0, (LPARAM)pChangedDS);
}

LRESULT COutputDSControlDlg::OnAnalogValueChange( WPARAM wParam, LPARAM lParam)
{
	AnalogOutputDataSrc* pChangedDS = (AnalogOutputDataSrc*) lParam;

	int ind = GetBlockIndex( pChangedDS );
	if ( ind < 0 )
		return 0L;

	AODSBLOCK dsBlock  = m_vdsBlocks.at(ind);

	CExtSliderWndColoredSelection* pSld = dsBlock.pSlider.second;
	CExtLabel* pLbl =  dsBlock.pLblCurVal.second;

	double val = pChangedDS->GetValue( );
	int pos = (int)dsBlock.pConvert->Convert( val );

	pSld->SetPos(pos);
	pSld->SetSelection(0, pos );
	pSld->UpdateSliderWnd();

	CString str;
	CString fmt = pChangedDS->OutFormat().c_str();
	if ( !pChangedDS->Units().empty() )
	{
		fmt += " , %s";
		str.Format(fmt, val, pChangedDS->Units().c_str() );
	}
	else
		str.Format(fmt, val);
	
	pLbl->SetWindowText(str);

	//TRACE("\n\t > COutputDSControlDlg::OnAnalogValueChange called...");
	return 0L;
}

// COutputDSControlDlg message handlers
BOOL COutputDSControlDlg::AddOutputDSBlock(CONTROLBAR_ITEM_INFO& cbii)
{
	AODSBLOCK dsBlock;

	dsBlock.pAODS = dynamic_cast<AnalogOutputDataSrc*>( DataSrc::GetDataSource(cbii.dsid) );

	if ( dsBlock.pAODS == NULL )
		return FALSE;
	
	if ( cbii.precition <= 0.0 )
		return FALSE;

	dsBlock.autoApply = cbii.autoapply;

	//используем сигналы boost
	//dsBlock.pAODS->AddOnOutputValueChange( boost::bind( &COutputDSControlDlg::OnAnalogOutputValueChanged, this, _1) );

	//используем посылку сообщений
	dsBlock.pAODS->AddUpdateWnd(this->m_hWnd);

	CString str;

	std::size_t ind = m_vdsBlocks.size();

	int m_nLayout = 1;

	int TOP_MARGIN	= 3;
	int LEFT_MARGIN = 5;
	
	int LABELS_WIDTH   = 120;
	int LABELS_WIDTH2  = 220;
	int LABELS_HEIGHT  = 20;	

	int SLIDER_WIDTH = 360;
	int SLIDER_HEIGHT = 40;	

	int PRESET_BTN_WIDTH = (btnPresets.m_hWnd == NULL) ? 0 : 48;

	int TOP_START = PRESET_BTN_WIDTH;

	if (m_nLayout == 0)
	{
		TOP_START  = PRESET_BTN_WIDTH + TOP_MARGIN * (ind + 1) + (LABELS_HEIGHT*2 + SLIDER_HEIGHT) * ind;
		TOP_START += (btnPresets.m_hWnd == NULL) ? 0 : 2*TOP_MARGIN;
	}
	else{
		TOP_START  = PRESET_BTN_WIDTH + TOP_MARGIN * (ind + 1) + (LABELS_HEIGHT + SLIDER_HEIGHT) * ind;
		TOP_START += (btnPresets.m_hWnd == NULL) ? 0 : 2*TOP_MARGIN;
	}

	int LEFT_START = LEFT_MARGIN;
	
	UINT ID = GetFreeCtrlID(m_hWnd);
	
	CRect rc;

	//фон
	dsBlock.pLblBack.first = ID;
	dsBlock.pLblBack.second	=  new CExtLabel();

	int RIGHT_START = 730;
	rc.SetRect(LEFT_START - 1, TOP_START - 1, RIGHT_START, TOP_START + /*LABELS_HEIGHT + SLIDER_HEIGHT +*/ 1);

	if( !dsBlock.pLblBack.second->Create(str, WS_CHILD|WS_VISIBLE|SS_ETCHEDHORZ, rc, this, ID) )
		return FALSE;

	AddAnchor(dsBlock.pLblBack.second->m_hWnd, __RDA_LT, __RDA_RT);

	//имя и описание источника данных
	dsBlock.pLblName.first = ID;
	dsBlock.pLblName.second	=  new CExtLabel();

	str.Format("%s", dsBlock.pAODS->Description().c_str() ); 

	
	TOP_START += 2;
	if ( m_nLayout == 0 )
		rc.SetRect(LEFT_START, TOP_START, LEFT_MARGIN + SLIDER_WIDTH, TOP_START + LABELS_HEIGHT);
	else
	{
		rc.SetRect(LEFT_START, TOP_START, LEFT_START + LABELS_WIDTH2, TOP_START + LABELS_HEIGHT + SLIDER_HEIGHT);
		LEFT_START = LEFT_START + LABELS_WIDTH2;
	}

	if( !dsBlock.pLblName.second->Create(str, WS_CHILD|WS_VISIBLE|SS_LEFT, rc, this, ID) )
		return FALSE;

	
	//dsBlock.pLblName.second->SetBkColor(RGB(99,99,99));
	//dsBlock.pLblName.second->SetTextColor(true, RGB(255,247,153));
	//dsBlock.pLblName.second->SetFontBold();
	//AddAnchor(dsBlock.pLblName.second->m_hWnd, __RDA_LT, __RDA_RT);


	double dMin, dMax;
	dsBlock.pAODS->GetRange(dMin, dMax);


	//минимальное значение 
	ID = GetFreeCtrlID(m_hWnd);

	dsBlock.pLblMinVal.first	= ID; 
	dsBlock.pLblMinVal.second	=  new CExtLabel();
	str.Format("%3.1f",  dMin);	

	if ( m_nLayout == 0 )
		rc.SetRect(LEFT_MARGIN, TOP_START, LEFT_MARGIN + LABELS_WIDTH, TOP_START + LABELS_HEIGHT);
	else
	{
		rc.SetRect(LEFT_START + LEFT_MARGIN, TOP_START, LEFT_START + LABELS_WIDTH, TOP_START + LABELS_HEIGHT);
	}
	
	if( !dsBlock.pLblMinVal.second->Create(str, WS_CHILD|WS_VISIBLE|SS_LEFT|SS_NOTIFY|SS_CENTERIMAGE, rc, this, ID) )
		return FALSE;


	//максимальное значение
	ID = GetFreeCtrlID(m_hWnd);

	dsBlock.pLblMaxVal.first	= ID; 
	dsBlock.pLblMaxVal.second  =  new CExtLabel();
	str.Format("%3.1f",  dMax);
	
	if ( m_nLayout == 0 )
		rc.SetRect(LEFT_MARGIN + SLIDER_WIDTH  - LABELS_WIDTH, TOP_START, LEFT_MARGIN + SLIDER_WIDTH, TOP_START + LABELS_HEIGHT);
	else
	{
		rc.SetRect(LEFT_START + SLIDER_WIDTH - LABELS_WIDTH, TOP_START, LEFT_START + SLIDER_WIDTH - LEFT_MARGIN, TOP_START + LABELS_HEIGHT);
	}

	if( !dsBlock.pLblMaxVal.second->Create(str, WS_CHILD|WS_VISIBLE|SS_RIGHT|SS_NOTIFY|SS_CENTERIMAGE, rc, this, ID) )
		return FALSE;

	AddAnchor(dsBlock.pLblMaxVal.second->m_hWnd, __RDA_RT, __RDA_RT);

	//текущее значение
	double curVal = dsBlock.pAODS->GetValue();
	ID = GetFreeCtrlID(m_hWnd);

	dsBlock.pLblCurVal.first	= ID; 
	dsBlock.pLblCurVal.second   =  new CExtLabel();

	CString fmt = dsBlock.pAODS->OutFormat().c_str();
	if ( !dsBlock.pAODS->Units().empty() )
	{
		fmt += " , %s";
		str.Format(fmt, curVal, dsBlock.pAODS->Units().c_str() );
	}
	else
		str.Format(fmt, curVal);

	if ( m_nLayout == 0 )
		rc.SetRect(LEFT_MARGIN + SLIDER_WIDTH/2 - LABELS_WIDTH/2, TOP_START,LEFT_MARGIN + SLIDER_WIDTH/2 + LABELS_WIDTH/2 , TOP_START + LABELS_HEIGHT);
	else
		rc.SetRect(LEFT_START  + SLIDER_WIDTH/2 - LABELS_WIDTH/2, TOP_START, LEFT_START + SLIDER_WIDTH/2 + LABELS_WIDTH/2 , TOP_START + LABELS_HEIGHT);

	TOP_START += LABELS_HEIGHT;

	if( !dsBlock.pLblCurVal.second->Create(str, WS_CHILD|WS_VISIBLE|SS_CENTER|SS_NOTIFY|SS_CENTERIMAGE, rc , this, ID) )
		return FALSE;
	
	AddAnchor(dsBlock.pLblCurVal.second->m_hWnd, CPoint(50, 0), CPoint(50, 0));

	//слайдер управления
	ID = GetFreeCtrlID(m_hWnd);

	dsBlock.pSlider.first		= ID; 
	dsBlock.pSlider.second		=  new CExtSliderWndColoredSelection();	

	int ticks = boost::math::round( ( dMax - dMin ) / cbii.precition );

	std::string convID = dsBlock.pAODS->Name() + "_sld";
	const LinearConverter* plc = dynamic_cast<const LinearConverter*>(BaseConverter::GetConverter(convID) );

	dsBlock.pConvert = const_cast<LinearConverter*>(plc);

	if ( dsBlock.pConvert == NULL )
	{
		dsBlock.pConvert = new LinearConverter();	
		if ( !dsBlock.pConvert->Create(convID, "", dMin, 0, dMax,  ticks ) )
		{
			delete dsBlock.pConvert;
			return FALSE;
		}
		else
			BaseConverter::AddConverter(dsBlock.pConvert);
	}

	if ( m_nLayout == 0 )
		rc.SetRect(LEFT_MARGIN, TOP_START, LEFT_MARGIN + SLIDER_WIDTH, TOP_START + SLIDER_HEIGHT);
	else
	{
		rc.SetRect(LEFT_START + LEFT_MARGIN, TOP_START, LEFT_START + SLIDER_WIDTH, TOP_START + SLIDER_HEIGHT);
		LEFT_START += SLIDER_WIDTH;
	}

	if ( !dsBlock.pSlider.second->Create(WS_VISIBLE|TBS_AUTOTICKS|WS_CHILD|TBS_TOP|TBS_ENABLESELRANGE|TBS_HORZ|TBS_TOOLTIPS, rc, this, ID  ))
		return FALSE;
	
	
	dsBlock.pSlider.second->SetRange(0, ticks);
	
	int tickFreq = int( 1 );	
	int pageSize = int( 5 ); 
	int lineSize = int( 1 );

	dsBlock.pSlider.second->SetTicFreq( tickFreq );
	dsBlock.pSlider.second->SetPageSize( pageSize );
	dsBlock.pSlider.second->SetLineSize( lineSize );
	
	dsBlock.pSlider.second->SetStyle(CExtSliderWnd::ES_PROFUIS);

	UINT bkID = IDB_AQUA_PROGRESS + cbii.colorid;
	if ( bkID > IDB_VIOLETE_PROGRESS )
		bkID = IDB_AQUA_PROGRESS;

	dsBlock.pSlider.second->Init(bkID, IDB_PROGRESS_BORDER);

	int pos = (int)dsBlock.pConvert->Convert(curVal);
	dsBlock.pSlider.second->SetPos(pos);
	dsBlock.pSlider.second->SetSelection(0, pos);

	AddAnchor(dsBlock.pSlider.second->m_hWnd, __RDA_LT, __RDA_RT);
	
	//кнопка применить
	ID = GetFreeCtrlID(m_hWnd);
	
	if ( m_nLayout == 0 )
		rc.SetRect(2*LEFT_MARGIN +  SLIDER_WIDTH, TOP_START, 2*LEFT_MARGIN +  SLIDER_WIDTH + LABELS_WIDTH, TOP_START + SLIDER_HEIGHT);
	else
		rc.SetRect(LEFT_START + LEFT_MARGIN, TOP_START - LABELS_HEIGHT/2, LEFT_START + int(1.5*LABELS_WIDTH), TOP_START + SLIDER_HEIGHT  - LABELS_HEIGHT/2);

	dsBlock.pApplyBtn.first	= ID; 
	dsBlock.pApplyBtn.second   =  new CExtButton();
	if( !dsBlock.pApplyBtn.second->Create("Apply", WS_CHILD|WS_VISIBLE, rc, this, ID) )
		return FALSE;	

	HINSTANCE hInstResource = AfxFindResourceHandle( MAKEINTRESOURCE( IDI_APPLY ), RT_GROUP_ICON);
	ASSERT( hInstResource != NULL );

	dsBlock.pApplyBtn.second->m_icon.AssignFromHICON( (HICON) ::LoadImage( hInstResource, MAKEINTRESOURCE(IDI_APPLY), IMAGE_ICON, 32, 32, 0 ), false );

	dsBlock.pApplyBtn.second->EnableWindow(FALSE);

	AddAnchor(dsBlock.pApplyBtn.second->m_hWnd, __RDA_RT, __RDA_RT);

	dsBlock.pLblBack.second->SetWindowPos(&wndTop, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	dsBlock.pApplyBtn.second->SetWindowPos(&wndBottom, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	
	

	m_vdsBlocks.push_back(dsBlock);
	return TRUE;
}

void COutputDSControlDlg::OnSwitchAutoMode (const DiscreteDataSrc* pChangedDS)
{
	ASSERT(pChangedDS != NULL);
	bool bState = pChangedDS->IsEnabled();

	for (std::vector<AODSBLOCK>::const_iterator it = m_vdsBlocks.begin(); it < m_vdsBlocks.end(); ++it)
	{
		(*it).pSlider.second->EnableWindow(!bState);
		//(*it).pApplyBtn.second->EnableWindow(!bState);
	}
	::EnableWindow( btnPresets.m_hWnd, !bState );
	::EnableWindow( btnApplyAll.m_hWnd, !bState );
}

void COutputDSControlDlg::OnChangeUser(const NTCC_APP_USER* pUser)
{
	
	for (std::vector<AODSBLOCK>::const_iterator it = m_vdsBlocks.begin(); it < m_vdsBlocks.end(); ++it)
	{
		bool bState = (unsigned int)pUser->ug <= (*it).pAODS->Permission();

		(*it).pSlider.second->EnableWindow(bState);
		//(*it).pApplyBtn.second->EnableWindow(bState);
	}
	::EnableWindow( btnPresets.m_hWnd, pUser->ug < ONLOOKER );
	::EnableWindow( btnApplyAll.m_hWnd, pUser->ug < ONLOOKER );
}

UINT COutputDSControlDlg::miID = ID_PRESET_FIRST;

BOOL COutputDSControlDlg::OnInitDialog()
{
	if ( !cbInfo.cbpresets.empty() )
	{
		HINSTANCE hInstResource = AfxFindResourceHandle( MAKEINTRESOURCE( IDI_APPLY ), RT_GROUP_ICON);
		ASSERT( hInstResource != NULL );

		CMenu menuPresets;	
		menuPresets.CreateMenu();


		CRect rc;
		rc.SetRect(5, 5, 300, 53);

		if( !btnPresets.Create("Select presets", WS_CHILD|WS_VISIBLE, rc, this, ID_PRESET_BTN) ) 
			return FALSE;

		btnPresets.m_icon.AssignFromHICON( (HICON) ::LoadImage( hInstResource, MAKEINTRESOURCE(IDI_PRESETS), IMAGE_ICON, 32, 32, 0 ), false );
		//btnPresets.SetIcon(IDI_PRESETS);
		

		rc.SetRect(590, 5, 720, 53);

		if( !btnApplyAll.Create("Apply all", WS_CHILD|WS_VISIBLE, rc, this, ID_APPLY_ALL_BTN) ) 
			return FALSE;
		btnApplyAll.m_icon.AssignFromHICON( (HICON) ::LoadImage( hInstResource, MAKEINTRESOURCE(IDI_APPLY_ALL), IMAGE_ICON, 32, 32, 0 ), false );;

		AddAnchor(btnApplyAll.m_hWnd, __RDA_RT, __RDA_RT);

		startID = miID;

		MENUITEMINFO mi;
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask = MIIM_SUBMENU;

		HMENU hPopup = CreatePopupMenu();
		mi.hSubMenu = hPopup;
		menuPresets.InsertMenuItem( 0, &mi, TRUE );
	
		
   
		
		for (std::vector<CONTROLBAR_PRESET_INFO>::iterator it = cbInfo.cbpresets.begin(); it != cbInfo.cbpresets.end(); ++it)
		{
			mi.wID = miID++;

			mi.fMask = MIIM_ID| MIIM_STRING | MIIM_DATA;
			mi.fType = MFT_STRING; 
			mi.dwTypeData = (LPSTR)((*it).title.c_str());

			::InsertMenuItem(hPopup, menuPresets.GetMenuItemCount(),  FALSE, &mi );

			VERIFY(g_CmdManager->UpdateFromMenu( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, hPopup  ) );	
		
			HICON hNewIcon = (HICON)::LoadImage( hInstResource, (*it).icon.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE );
			
			if( hNewIcon != NULL )
				g_CmdManager->CmdSetIcon( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, mi.wID,  hNewIcon, true );
		
		}
	
	
		btnPresets.m_menu.Attach( menuPresets.Detach() );	
		//btnPresets.m_dwMenuOpt |= TPMX_OWNERDRAW_FIXED;
	}


	for (std::vector<CONTROLBAR_ITEM_INFO>::iterator it = cbInfo.cbitems.begin(); it != cbInfo.cbitems.end(); ++it)
	{
		if ( !AddOutputDSBlock( (*it) ) )
		{
			//TRACE("\n BLOCK NOT ADDED!!!");
			return FALSE;
		}
	}

	SubclassChildControls();

	/*
	if ( m_vdsBlocks.size() == 0 )
		return FALSE;
	*/

	DiscreteDataSrc* pDS = dynamic_cast<DiscreteDataSrc*>( DataSrc::GetDataSource( cbInfo.disabledDS ) );
	
	if ( pDS != NULL )
	{
		pDS->AddOnChangeState(boost::bind( &COutputDSControlDlg::OnSwitchAutoMode, this, _1));
		OnSwitchAutoMode(pDS);
	}
	
	
	return CAutoCalcSizeDialog::OnInitDialog();  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COutputDSControlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int ind = GetBlockIndex( pScrollBar->GetDlgCtrlID() );
	if ( ind < 0 )
		return;

	AODSBLOCK dsb = m_vdsBlocks.at(ind);

	AnalogOutputDataSrc* pADS = dsb.pAODS;

	int pos = dsb.pSlider.second->GetPos();
	double newVal = dsb.pConvert->RevConvert( pos );
	
	dsb.pApplyBtn.second->EnableWindow(TRUE);

	bool updateNow = dsb.autoApply;

	CString str;
	CString fmt = pADS->OutFormat().c_str();
	if ( !pADS->Units().empty() )
	{
		fmt += " , %s";
		str.Format(fmt, newVal, pADS->Units().c_str() );
	}
	else
		str.Format(fmt, newVal);

	
// 	CToolTipCtrl* pToolTip = dsb.pSlider.second->GetToolTips();
// 	//pToolTip->SetWindowText();
// 	pToolTip->UpdateTipText((LPCTSTR)str, dsb.pSlider.second);

	dsb.pLblCurVal.second->SetWindowText(str);

	if ( nSBCode == SB_ENDSCROLL && updateNow )
		pADS->SetValue(newVal);

	CAutoCalcSizeDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL COutputDSControlDlg::OnTtnNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	//UNREFERENCED_PARAMETER(id);

	NMTTDISPINFO *pTTT = (NMTTDISPINFO *) pNMHDR;
	UINT_PTR nID = pNMHDR->idFrom;
	BOOL bRet = FALSE;

	
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		// idFrom is actually the HWND of the tool
		/*CWnd* pWnd = FromHandle(pNMHDR->hwndFrom);*/
		nID = ::GetDlgCtrlID((HWND)nID);

		int ind = GetBlockIndex( nID );
		if ( ind < 0 )
			return FALSE;

		AODSBLOCK dsb = m_vdsBlocks.at(ind);
		AnalogOutputDataSrc* pADS = dsb.pAODS;

		int pos = dsb.pSlider.second->GetPos();
		double newVal = dsb.pConvert->RevConvert( pos );

		dsb.pApplyBtn.second->EnableWindow(TRUE);

		bool updateNow = dsb.autoApply;

		CString str;
		CString fmt = pADS->OutFormat().c_str();
		if ( !pADS->Units().empty() )
		{
			fmt += " , %s";
			str.Format(fmt, newVal, pADS->Units().c_str() );				
		}
		else 
			str.Format(fmt, newVal);

		strcpy_s(pTTT->szText, sizeof(pTTT->szText) / sizeof(TCHAR),  (LPCTSTR)str);
		pTTT->hinst = AfxGetResourceHandle();
		bRet = TRUE;
		
	}

	*pResult = 0;

	return bRet;
}

BOOL COutputDSControlDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{

	if (nCode == BN_CLICKED)
	{
		if (nID == ID_APPLY_ALL_BTN)
		{
			OnApplyAll();
			return TRUE;
		}

 		int ind = GetBlockIndex( nID );
		if ( ind < 0 )
			return TRUE;

		CExtSliderWndColoredSelection* pSld = m_vdsBlocks.at(ind).pSlider.second;
		AnalogOutputDataSrc* pAODS  = m_vdsBlocks.at(ind).pAODS;
		LinearConverter* pConv  = m_vdsBlocks.at(ind).pConvert;

		bool bApply = ( nID == m_vdsBlocks.at(ind).pApplyBtn.first );

		if ( bApply )
		{
			int pos = pSld->GetPos();
			double newVal = pConv->RevConvert( pos );

			pAODS->SetValue( newVal );

			//pSld->SetSelection(0, pos );
			//pSld->UpdateSliderWnd();
		}

		bool bMinMax = ( ( nID == m_vdsBlocks.at(ind).pLblMinVal.first ) || ( nID == m_vdsBlocks.at(ind).pLblMaxVal.first ) );

		if ( bMinMax )
		{
			bool bmax = ( nID == m_vdsBlocks.at(ind).pLblMaxVal.first );

			int pos = bmax ? pSld->GetRangeMax() : pSld->GetRangeMin();

			pSld->SetPos(pos);
		}
		m_vdsBlocks.at(ind).pApplyBtn.second->EnableWindow(FALSE);
	}

	return CAutoCalcSizeDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL COutputDSControlDlg::PreTranslateMessage( MSG * pMsg )
{
	if ( pMsg->message == WM_COMMAND )
	{
		UINT nID = pMsg->wParam; 
		if ( (nID >= ID_PRESET_FIRST) && (nID <= ID_PRESET_LAST))
		{
			int ind = nID - startID;

			CONTROLBAR_PRESET_INFO cbpi = cbInfo.cbpresets.at(ind);
			
			std::vector<PRESET_ITEM> presetItems = cbpi.presetItems;

			for ( std::vector<PRESET_ITEM>::iterator it = presetItems.begin(); it != presetItems.end(); it ++)
			{
				////TRACE("\n\t%s - %6.2f", (*it).first.c_str(), (*it).second);
				int blockInd = GetBlockIndex( (*it).name );
				
				if ( blockInd < 0 )
					continue;

				AODSBLOCK dsBlock  = m_vdsBlocks.at(blockInd);

				CExtSliderWndColoredSelection* pSld = dsBlock.pSlider.second;
				CExtLabel* pLbl =  dsBlock.pLblCurVal.second;

				double val = (*it).val;
				int pos = (int)floor(dsBlock.pConvert->Convert( val ));

				pSld->SetPos(pos);				
				pSld->UpdateSliderWnd();

				CString str;
				CString fmt = dsBlock.pAODS->OutFormat().c_str();
				if ( !dsBlock.pAODS->Units().empty() )
				{
					fmt += " , %s";
					str.Format(fmt, val, dsBlock.pAODS->Units().c_str() );
				}
				else
					str.Format(fmt, val);
				
				pLbl->SetWindowText(str);

			}
		}		
	}	
	/*
	if ( pMsg->message == WM_KEYUP )
	{ 
	
		if ( pMsg->wParam >=  VK_PRIOR && pMsg->wParam <= VK_DOWN )
			return FALSE;//CAutoCalcSizeDialog::PreTranslateMessage(pMsg);
	
		int ind = GetBlockIndex(pMsg->hwnd);
		
		if (ind < 0 )
			return  FALSE;
			
		AODSBLOCK dsBlock  = m_vdsBlocks.at(ind);

		std::map <UINT, char>::const_iterator fnd = numPadToChar.find(pMsg->wParam);
		
		if (fnd != numPadToChar.end() )
			pressKeyBuffer += (*fnd).second;
		
		if ( pMsg->wParam == VK_BACK  || pMsg->wParam == VK_DELETE)
			pressKeyBuffer.clear();


		double f = atof( pressKeyBuffer.c_str() );

		int pos = (int)dsBlock.pConvert->Convert(f);
		dsBlock.pSlider.second->SetPos(pos);

		CString str;
		str.Format("%3.2f, %s", f, dsBlock.pAODS->Units().c_str() );

		dsBlock.pLblCurVal.second->SetWindowText(str);

		if ( pMsg->wParam == VK_RETURN)
		{			
			pos = dsBlock.pSlider.second->GetPos();
			f = (int)dsBlock.pConvert->ReverseConvert(pos);

			dsBlock.pAODS->SetValue(f);
			pressKeyBuffer.clear();
		}		

		return TRUE;
	}
	*/
	return CAutoCalcSizeDialog::PreTranslateMessage(pMsg);
}


void COutputDSControlDlg::OnApplyAll()
{
	for (vector<AODSBLOCK>::const_iterator it = m_vdsBlocks.begin(); it != m_vdsBlocks.end(); ++it)
	{
		HWND btnWnd = (*it).pApplyBtn.second->m_hWnd;
		UINT id = (*it).pApplyBtn.first;
		SendMessage(WM_COMMAND, MAKEWPARAM( id, BN_CLICKED), (LPARAM)btnWnd);
	}	
}