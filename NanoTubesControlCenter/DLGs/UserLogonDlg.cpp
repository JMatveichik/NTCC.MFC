// DLGs/UserLogonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\NanoTubesControlCenter.h"
#include "UserLogonDlg.h"
#include "..\ah_users.h"


#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

#define ID_USER_FIRST 6000
#define ID_USER_LAST  6050

// CUserLogonDlg dialog

IMPLEMENT_DYNAMIC(CUserLogonDlg, CDialog)

CUserLogonDlg::CUserLogonDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CUserLogonDlg::IDD, pParent)
{

}

CUserLogonDlg::~CUserLogonDlg()
{
}

void CUserLogonDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
 	DDX_Control(pDX, IDC_BTN_USERS, btnUsers);
 	DDX_Control(pDX, IDC_ED_PASSWORD, edPsw);
	
}


BEGIN_MESSAGE_MAP(CUserLogonDlg, CExtResizableDialog)
	
	ON_COMMAND_RANGE(ID_USER_FIRST, ID_USER_LAST, OnAppUserChange)
	ON_REGISTERED_MESSAGE(CExtPopupMenuWnd::g_nMsgPrepareMenu,OnExtMenuPrepare)
	ON_REGISTERED_MESSAGE(CExtPopupMenuWnd::g_nMsgPopupDrawItem,OnDrawPopupMenuItem)
	ON_REGISTERED_MESSAGE(CExtPopupMenuWnd::g_nMsgPopupDrawLeftArea,OnDrawPopupLeftArea)
	ON_WM_MEASUREITEM()
	ON_EN_SETFOCUS(IDC_ED_PASSWORD, OnEnSetFocusEditPass)
	ON_EN_KILLFOCUS(IDC_ED_PASSWORD, OnEnKillFocusEditPass)
	ON_EN_CHANGE(IDC_ED_PASSWORD, OnEnChangeEditPass)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_NUM0, IDC_BTN_NUM9, OnNumButtonClicked)
	ON_BN_CLICKED(IDC_BTN_NUM_CLEAR, OnNumClear)
	ON_BN_CLICKED(IDC_BTN_NUM_DEL, OnNumDel)
	ON_BN_CLICKED(IDC_BTN_NUM_ENTER, OnNumEnter)

END_MESSAGE_MAP()

void CUserLogonDlg::OnAppUserChange(UINT id)
{

}

void CUserLogonDlg::OnNumButtonClicked( UINT nID/*, NMHDR * pNotifyStruct, LRESULT * result*/  )
{
	int nButton = nID - IDC_BTN_NUM0;
	char numToChar[] = {'0','1','2','3','4','5','6','7','8','9'};
	
	CString str;	
	edPsw.GetWindowText(str);
	str += numToChar[nButton];
	edPsw.SetWindowText(str);
}
void CUserLogonDlg::OnNumClear()
{
	edPsw.SetWindowText("");
}

void CUserLogonDlg::OnNumDel()
{
	CString str;	
	edPsw.GetWindowText(str);
	if (str.IsEmpty() )
		return;

	int pos = str.GetLength() - 1;

	str.Delete(pos);
	edPsw.SetWindowText(str);
}

void CUserLogonDlg::OnOK()
{
	CString strUserName;
	btnUsers.GetWindowText(strUserName);

	const NTCC_APP_USER* lpUser = AppUsersHelper::Instance().GetUser( (LPCTSTR)strUserName );

	CString str;
	edPsw.GetWindowText(str);
	std::string psw = (LPCTSTR)str;

	lpUser = AppUsersHelper::Instance().UserLogin(lpUser->login, psw);

	if ( lpUser == NULL )
	{
		MessageBeep(MB_ICONERROR);
		OnNumClear();
	}
	else
		CExtResizableDialog::OnOK();
}

void CUserLogonDlg::OnNumEnter()
{	
	OnOK();
}

void CUserLogonDlg::OnEnChangeEditPass()
{
	CString str;	
	edPsw.GetWindowText(str);	
	GetDlgItem(IDC_BTN_NUM_ENTER)->EnableWindow( !str.IsEmpty() );
}

// CUserLogonDlg message handlers

BOOL CUserLogonDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	std::vector<NTCC_APP_USER> users =  AppUsersHelper::Instance().GetAppicationUsers();
	SubclassChildControls();

	if ( users.empty() )
	{
		btnUsers.SetWindowText("Ќет зарегистрированных пользователей");
		edPsw.EnableWindow(FALSE);
		btnUsers.EnableWindow(FALSE);
	}
	else
	{
		CMenu menuUsers;	
		menuUsers.CreateMenu();

		UINT miID  = ID_USER_FIRST;

		MENUITEMINFO mi;
		mi.cbSize = sizeof(MENUITEMINFO);
		mi.fMask = MIIM_SUBMENU;

		HMENU hPopup = CreatePopupMenu();
		mi.hSubMenu = hPopup;
		menuUsers.InsertMenuItem( 0, &mi, TRUE );

		HINSTANCE hInstResource = AfxFindResourceHandle( MAKEINTRESOURCE( IDI_GROUP_USERS ), RT_GROUP_ICON);
		ASSERT( hInstResource != NULL );

		for (std::vector<NTCC_APP_USER> ::const_iterator it = users.begin(); it != users.end(); ++it)
		{
			mi.wID = miID++;

			mi.fMask = MIIM_ID| MIIM_STRING | MIIM_DATA; 
			mi.fType = MFT_STRING; 
			mi.dwTypeData = (LPSTR)((*it).screenName.c_str());
			
			::InsertMenuItem(hPopup, menuUsers.GetMenuItemCount(),  FALSE, &mi );

			UINT iconID = IDI_GROUP_USERS; 
			switch ( (*it).ug )
			{
			case ADMINISTRATORS:
				iconID = IDI_GROUP_ADMINS; 
				break;

			case TECHNOLOGISTS:
				iconID = IDI_GROUP_TECHNOLOGISTS;
				break;

			case OPERATORS:
				iconID = IDI_GROUP_OPERATORS;
				break;

			case ONLOOKER:
				iconID = IDI_GROUP_USERS;
				break;

			default:
				iconID = IDI_GROUP_USERS;
				break;
			}

			HICON hNewIcon = NULL;
			if ( hInstResource != NULL ) 
			{
				hNewIcon = (HICON)::LoadImage( hInstResource, MAKEINTRESOURCE( iconID ), IMAGE_ICON, 32,  32,  0 );
				ASSERT( hNewIcon != NULL );
			}

			VERIFY(g_CmdManager->UpdateFromMenu( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, hPopup  ) );	

			if( hNewIcon != NULL )
				g_CmdManager->CmdSetIcon( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, mi.wID,  hNewIcon, true );

		}


		btnUsers.m_menu.Attach( menuUsers.Detach() );	
		btnUsers.SetSeparatedDropDown( FALSE );
		//btnUsers.m_nMenuLeftAreaWidth = 30;
		btnUsers.m_dwMenuOpt |= TPMX_OWNERDRAW_FIXED;
		btnUsers.m_icon.m_dwFlags |= __EXT_ICON_PERSISTENT_BITMAP_HOVER;

		HICON hIcon = (HICON) ::LoadImage( ::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_GROUP_USERS), IMAGE_ICON, 32, 32, 0 );
		btnUsers.m_icon.m_bmpNormal.AssignFromHICON( hIcon );
		::DeleteObject( hIcon );
	}
	
	CRect rc, rcNumTop;
	GetWindowRect(&rc);
	ScreenToClient(rc);
	bottomDlg = rc.bottom;

	GetDlgItem(IDC_BTN_NUM7)->GetWindowRect(&rcNumTop);
	ScreenToClient(rcNumTop);
	topNumKeys = rcNumTop.top;

	HideNumericButtons(true);

	edPsw.SetPasswordChar('o');

	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
	
	SetFocus();

	SetForegroundWindow();

	return TRUE;
}

void CUserLogonDlg::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	////TRACE("\n\t MEASURE item <%d> ", nIDCtl);
	
}
void CUserLogonDlg::HideNumericButtons(BOOL bHide /*=  true*/)
{
	CRect rc, rcNumTop;
	GetWindowRect(&rc);
	ScreenToClient(rc);

	rc.bottom = bHide ? topNumKeys : bottomDlg;
	ClientToScreen(rc);

	MoveWindow(&rc);
}

void CUserLogonDlg::OnEnSetFocusEditPass()
{
	HideNumericButtons(false);
}
void CUserLogonDlg::OnEnKillFocusEditPass()
{
	//HideNumericButtons(true);
}



BOOL CUserLogonDlg::PreTranslateMessage( MSG * pMsg )
{
	if ( pMsg->message == WM_COMMAND )
	{
		UINT nID = pMsg->wParam; 
		if ( (nID >= ID_USER_FIRST) && (nID <= ID_USER_LAST))
		{
			int ind = nID - ID_USER_FIRST;
			
			std::vector<NTCC_APP_USER> users =  AppUsersHelper::Instance().GetAppicationUsers();
			NTCC_APP_USER u = users.at(ind);
			
			btnUsers.SetWindowText(u.screenName.c_str());

			btnUsers.m_icon = g_CmdManager->CmdGetIcon( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, nID );
			edPsw.SetFocus();

			if ( u.ug == ONLOOKER )
			{
				AppUsersHelper::Instance().UserLogin(u.login, u.psw);
				edPsw.SetWindowText(u.psw.c_str());
				OnOK();
			}
		}	

	}

	return CExtResizableDialog::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////

LRESULT CUserLogonDlg::OnDrawPopupLeftArea(WPARAM wParam, LPARAM lParam)
{
	wParam;
	CExtPopupMenuWnd::DRAWLEFTAREADATA * pDrawLeftAreaData =
		reinterpret_cast < CExtPopupMenuWnd::DRAWLEFTAREADATA * > ( lParam );
	ASSERT( pDrawLeftAreaData != NULL );

	// get draw DC
	CDC & dc = *( (CDC *) *pDrawLeftAreaData );

	CRect rcItem = LPCRECT(*pDrawLeftAreaData);
	CExtPaintManager::stat_PaintGradientRect(
		dc,
		&rcItem,
		dc.GetNearestColor( RGB( 0, 0, 0 ) ),
		dc.GetNearestColor( RGB( 100, 100, 255 ) ),
		true
		);

	LOGFONT lf;
	::memset(&lf,0,sizeof(LOGFONT));
	g_PaintManager->m_FontNormal.GetLogFont( &lf );
	lf.lfHeight = -18;
	lf.lfWidth = 0;
	lf.lfWeight = 900;
	lf.lfEscapement = 900;
	__EXT_MFC_STRCPY( lf.lfFaceName, LF_FACESIZE, _T("Times New Roman") );
	CFont font;
	VERIFY(
		font.CreateFontIndirect(&lf)
		);
	CFont * pOldFont = dc.SelectObject( &font );
	INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
	COLORREF clrOldText = dc.SetTextColor( RGB( 0, 0, 0 ) );

	static CString sTitle( _T("Prof-UIS") );
	CPoint ptText( rcItem.left+4, rcItem.bottom-5 );
	dc.DrawState(
		ptText, rcItem.Size(), (LPCTSTR)sTitle,
		DSS_NORMAL,  TRUE, 0, (CBrush*)NULL
		);
	dc.SetTextColor( RGB( 255, 255, 255 ) );
	ptText.Offset( -1, -2 );
	dc.DrawState(
		ptText, rcItem.Size(), (LPCTSTR)sTitle,
		DSS_NORMAL,  TRUE, 0, (CBrush*)NULL
		);

	const int nIconMetric = 24;
	HICON hIcon = (HICON)
		::LoadImage(
		::AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_GROUP_ADMINS),
		IMAGE_ICON,
		nIconMetric,
		nIconMetric,
		0
		);
	if( hIcon != NULL )
	{
		int nOffset = (rcItem.Width() - nIconMetric) / 2;
		VERIFY(
			::DrawIconEx(
			dc.GetSafeHdc(),
			rcItem.left + nOffset,
			rcItem.top + nOffset,
			hIcon,
			nIconMetric,
			nIconMetric,
			0,
			(HBRUSH)NULL,
			DI_NORMAL
			)
			);
		VERIFY( DestroyIcon( hIcon ) );
	}

	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrOldText );
	dc.SelectObject( pOldFont );

	return !0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CUserLogonDlg::OnDrawPopupMenuItem(WPARAM wParam, LPARAM lParam)
{
	return 0;
	wParam;
	CExtPopupMenuWnd::DRAWITEMDATA * pDrawItemData =
		reinterpret_cast < CExtPopupMenuWnd::DRAWITEMDATA * > ( lParam );
	ASSERT( pDrawItemData != NULL );
	
	UINT nCmdID = pDrawItemData->GetCmdID();
//	if( nCmdID != ID_USER_FIRST + 1 )
//		return 0; // default painting

	LOGFONT lf;
	CFont menuFont;

	btnUsers.GetFont()->GetLogFont(&lf);
	menuFont.CreateFontIndirect(&lf);

	LOGFONT lfNormal;
	::memset(&lfNormal, 0, sizeof(LOGFONT) );
	g_PaintManager->m_FontNormal.GetLogFont( &lfNormal);

	g_PaintManager->m_FontNormal.DeleteObject();
	g_PaintManager->m_FontNormal.CreateFontIndirect(&lf);

	// paint some staff in a little bit rect
	CRect rcItem = pDrawItemData->m_pRectItemWithIndents; //LPCRECT(*pDrawItemData);
	rcItem.left = pDrawItemData->m_pItemData->GetIconAreaWidth();

	pDrawItemData->m_pPopup;
	//rcItem.DeflateRect(0,0,50,0);
	// paint default menu item background
	
	pDrawItemData->PaintDefaultBk();
	pDrawItemData->PaintDefault(
		false, // no paint icon
		true, // no paint text
		false, // no paint check/radio mark
		false, // no paint as enabled
		false // no paint as unselected
		);

	//pDrawItemData->Paint

	LPCTSTR sItemText = __EXT_MFC_SAFE_LPCTSTR( *pDrawItemData );
	INT nItemTextLen = INT( _tcslen( sItemText ) );

	// get draw DC
	CDC & dc = *( (CDC *) *pDrawItemData );


	/*
	CExtWndShadow _shadow;
	_shadow.Paint(
		NULL,
		dc, rcItem, CRect(0,0,0,0), CRect(0,0,0,0),
		3,
		CExtWndShadow::DEF_BRIGHTNESS_MIN,
		CExtWndShadow::DEF_BRIGHTNESS_MAX,
		false
		);
	CExtPaintManager::stat_PaintGradientRect(
		dc,
		&rcItem,
		dc.GetNearestColor( RGB( 255, 0, 0 ) ),
		dc.GetNearestColor( RGB( 0, 0, 255 ) ),
		false
		);
	*/

	

	INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
	COLORREF clrOldText = dc.SetTextColor( RGB( 0, 0, 0 ) );
	CFont * pOldFont = dc.SelectObject( &menuFont );

	rcItem.OffsetRect( 2, 1 );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, sItemText, nItemTextLen, &rcItem,  DT_SINGLELINE | DT_LEFT | DT_VCENTER /*| DT_END_ELLIPSIS*/, 0 );

	/*clrOldText = dc.SetTextColor( RGB( 255, 255, 255 ) );
	rcItem.OffsetRect( -2, -1 );
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		sItemText, nItemTextLen, &rcItem, 
		DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS, 0
		);
		*/

	g_PaintManager->m_FontNormal.DeleteObject();
	g_PaintManager->m_FontNormal.CreateFontIndirect(&lfNormal);

	dc.SelectObject( pOldFont );
	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrOldText );

	return !0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CUserLogonDlg::OnExtMenuPrepare(WPARAM wParam, LPARAM lParam)
{
	lParam;
	CExtPopupMenuWnd::MsgPrepareMenuData_t * pData =
		reinterpret_cast
		< CExtPopupMenuWnd::MsgPrepareMenuData_t * >
		( wParam );
	ASSERT( pData != NULL );
	CExtPopupMenuWnd * pPopup = pData->m_pPopup;
	ASSERT( pPopup != NULL );

	
	/*INT nReplacePos =
		pPopup->ItemFindPosForCmdID( ID_COLOR_CTRL_8X5 );
	if( nReplacePos >= 0 )
	{
		CExtPopupColorMenuWnd * pColorPopup = new CExtPopupColorMenuWnd;
		pColorPopup->m_lParamCookie = LPARAM( ID_COLOR_CTRL_8X5 );
		pColorPopup->m_hWndNotifyColorChanged = GetSafeHwnd();
		pColorPopup->m_clrDefault = COLORREF(-1);
		VERIFY(
			pPopup->ItemInsertSpecPopup(
			pColorPopup,
			nReplacePos + 1,
			pPopup->ItemGetText(nReplacePos),
			pPopup->ItemGetIcon(nReplacePos)
			)
			);
		pPopup->ItemSetDisplayed( nReplacePos + 1, true );
		VERIFY( pPopup->ItemRemove(nReplacePos) );
	} // if( nReplacePos >= 0 )
	nReplacePos =
		pPopup->ItemFindPosForCmdID( ID_COLOR_CTRL_8X2 );
	if( nReplacePos >= 0 )
	{
		CExtPopupColorMenuWnd * pColorPopup = new CExtPopupColorMenuWnd;
		pColorPopup->m_lParamCookie = LPARAM( ID_COLOR_CTRL_8X2 );
		pColorPopup->m_hWndNotifyColorChanged = GetSafeHwnd();
		pColorPopup->m_clrDefault = COLORREF(-1);
		pColorPopup->SetColors8x2();
		VERIFY(
			pPopup->ItemInsertSpecPopup(
			pColorPopup,
			nReplacePos + 1,
			pPopup->ItemGetText(nReplacePos),
			pPopup->ItemGetIcon(nReplacePos)
			)
			);
		pPopup->ItemSetDisplayed( nReplacePos + 1, true );
		VERIFY( pPopup->ItemRemove(nReplacePos) );
	} // if( nReplacePos >= 0 )
	nReplacePos =
		pPopup->ItemFindPosForCmdID( ID_COLOR_CTRL_GRAYSCALE );
	if( nReplacePos >= 0 )
	{
		CExtPopupColorMenuWnd * pColorPopup = new CExtPopupColorMenuWnd;
		pColorPopup->m_lParamCookie = LPARAM( ID_COLOR_CTRL_GRAYSCALE );
		pColorPopup->m_hWndNotifyColorChanged = GetSafeHwnd();
		pColorPopup->m_clrDefault = COLORREF(-1);
		pColorPopup->RemoveAllColors();
		int nGrayColors = 16;
		pColorPopup->SetColorsInRow( nGrayColors );
		pColorPopup->m_sizeColorItemCell.cy *= 2;
		for( int nColorIndex = 0; nColorIndex < nGrayColors; nColorIndex++ )
		{
			double f = double( nColorIndex ) / double( nGrayColors - 1 );
			COLORREF clr = CExtBitmap::stat_HLStoRGB( 0.0, f, 0.0 );
			int n = GetRValue(clr);
			CExtSafeString str;
			str.Format(
				_T("#%02X%02X%02X"),
				n,
				n,
				n
				);
			pColorPopup->AddColor(
				new CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY(
				0,
				clr,
				str
				)
				);
		}
		VERIFY(
			pPopup->ItemInsertSpecPopup(
			pColorPopup,
			nReplacePos + 1,
			pPopup->ItemGetText(nReplacePos),
			pPopup->ItemGetIcon(nReplacePos)
			)
			);
		pPopup->ItemSetDisplayed( nReplacePos + 1, true );
		VERIFY( pPopup->ItemRemove(nReplacePos) );
	} // if( nReplacePos >= 0 )
	nReplacePos =
		pPopup->ItemFindPosForCmdID( ID_COLOR_CTRL_HLS );
	if( nReplacePos >= 0 )
	{
		CExtPopupColorMenuWnd * pColorPopup = new CExtPopupColorMenuWnd;
		pColorPopup->m_lParamCookie = LPARAM( ID_COLOR_CTRL_HLS );
		pColorPopup->m_hWndNotifyColorChanged = GetSafeHwnd();
		pColorPopup->m_clrDefault = COLORREF(-1);
		pColorPopup->RemoveAllColors();
		int nStepsHue = 12;
		int nStepsSat = 8;
		pColorPopup->m_sizeColorItemCell.cx = 14;
		pColorPopup->m_sizeColorItemCell.cy = 12;
		pColorPopup->m_sizeColorItemSpaces.cx = 1;
		pColorPopup->m_sizeColorItemSpaces.cy = 1;
		pColorPopup->SetColorsInRow( nStepsHue );
		for( int nSat = 0; nSat < nStepsSat; nSat++ )
		{
			double fSat = 1.0 - double( nSat ) / double( nStepsSat );
			for( int nHue = 0; nHue < nStepsHue; nHue++ )
			{
				double fHue = double( nHue ) / double( nStepsHue );
				COLORREF clr = CExtBitmap::stat_HLStoRGB( fHue, 0.5, fSat );
				CExtSafeString str;
				str.Format(
					_T("#%02X%02X%02X"),
					GetRValue(clr),
					GetGValue(clr),
					GetBValue(clr)
					);
				pColorPopup->AddColor(
					new CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY(
					0,
					clr,
					str
					)
					);
			}
		}
		VERIFY(
			pPopup->ItemInsertSpecPopup(
			pColorPopup,
			nReplacePos + 1,
			pPopup->ItemGetText(nReplacePos),
			pPopup->ItemGetIcon(nReplacePos)
			)
			);
		pPopup->ItemSetDisplayed( nReplacePos + 1, true );
		VERIFY( pPopup->ItemRemove(nReplacePos) );
	} // if( nReplacePos >= 0 )

#if (!defined __EXT_MFC_NO_DATE_PICKER)
	nReplacePos =
		pPopup->ItemFindPosForCmdID(ID_DATE_PICKER_CTRL);

	if( nReplacePos >= 0 )
	{
		CExtPopupControlMenuWnd::g_bControlMenuWithShadows = true;
		CExtPopupDatePickerMenuWnd * pDatePickerPopup = new CExtPopupDatePickerMenuWnd;
		VERIFY(
			pPopup->ItemInsertSpecPopup(
			pDatePickerPopup,
			nReplacePos + 1,
			pPopup->ItemGetText(nReplacePos),
			pPopup->ItemGetIcon(nReplacePos)
			)
			);
		pPopup->ItemSetDisplayed( nReplacePos + 1, true );
		VERIFY( pPopup->ItemRemove(nReplacePos) );
	} // if( nReplacePos >= 0 )
#endif // #if (!defined __EXT_MFC_NO_DATE_PICKER)	

	nReplacePos = pPopup->ItemFindPosForCmdID( ID_PALETTE_MENU );
	if( nReplacePos >= 0 )
	{
		pPopup->ItemRemove( nReplacePos );
		CExtPopupMenuWnd * pPalettePopup = new CExtPopupMenuWnd;
		pPopup->ItemInsertSpecPopup(
			pPalettePopup,
			nReplacePos,
			g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( m_hWnd ),
			ID_PALETTE_MENU
			)->m_sMenuText
			);
		pPalettePopup->TrackFlagsSet(
			pPalettePopup->TrackFlagsGet()
			| TPMX_PALETTE
			);

		pPalettePopup->ItemInsertCommand(
			1,
			-1,
			_T("Palette 6 x 2:")
			);
		INT nLastPos = pPalettePopup->ItemGetCount() - 1;
		pPalettePopup->ItemEnabledSet( nLastPos, false );
		pPalettePopup->ItemEnabledAppearanceSet( nLastPos );
		pPalettePopup->ItemBoldSet( nLastPos );
		pPalettePopup->ItemPaletteRowWrapSet( nLastPos );

		CExtBitmap _bmp;
		VERIFY( _bmp.LoadBMP_Resource( MAKEINTRESOURCE(IDB_PAINT_PALETTE) ) );
		INT nRow, nColumn;
		static const INT nRowCountPaint = 2, nColumnCountPaint = 6;
		static const COLORREF clrTransparentPaint = RGB(255,0,255);
		static const CSize _sizeBitmapPartPaint(16,16);
		for( nRow = 0; nRow < nRowCountPaint; nRow ++ )
		{
			for( nColumn = 0; nColumn < nColumnCountPaint; nColumn ++ )
			{
				CPoint ptBitmapOffset(
					nRow * nColumnCountPaint * _sizeBitmapPartPaint.cx
					+ nColumn * _sizeBitmapPartPaint.cx,
					0
					);
				CRect rcBitmapPart(
					ptBitmapOffset,
					_sizeBitmapPartPaint
					);
				CExtCmdIcon _icon;
				_icon.m_bmpNormal.FromBitmap(
					_bmp,
					&rcBitmapPart
					);
				ASSERT( ! _icon.IsEmpty() );
				_icon.m_bmpNormal.AlphaColor( clrTransparentPaint, RGB(0,0,0), 0 );
				pPalettePopup->ItemInsertCommand(
					3 + nRow*nColumnCountPaint + nColumn,
					-1,
					NULL,
					NULL,
					_icon
					);
				if( nColumn == (nColumnCountPaint-1) )
					pPalettePopup->ItemPaletteRowWrapSet(
					pPalettePopup->ItemGetCount() - 1
					);
			} // for( nColumn = 0; nColumn < nColumnCountPaint; nColumn ++ )
		} // for( nRow = 0; nRow < nRowCountPaint; nRow ++ )

		pPalettePopup->ItemInsert();
		pPalettePopup->ItemInsertCommand(
			2,
			-1,
			_T("Palette 6 x 7:")
			);
		nLastPos = pPalettePopup->ItemGetCount() - 1;
		pPalettePopup->ItemEnabledSet( nLastPos, false );
		pPalettePopup->ItemEnabledAppearanceSet( nLastPos );
		pPalettePopup->ItemBoldSet( nLastPos );
		pPalettePopup->ItemPaletteRowWrapSet( nLastPos );

		VERIFY( _bmp.LoadBMP_Resource( MAKEINTRESOURCE(IDB_EXT_CUSTOM_TB_IMAGES) ) );
		static const INT nRowCountCustom = 7, nColumnCountCustom = 6;
		static const COLORREF clrTransparentCustom = RGB(255,0,255);
		static const CSize _sizeBitmapPartCustom(16,16);
		for( nRow = 0; nRow < nRowCountCustom; nRow ++ )
		{
			for( nColumn = 0; nColumn < nColumnCountCustom; nColumn ++ )
			{
				CPoint ptBitmapOffset(
					nRow * nColumnCountCustom * _sizeBitmapPartCustom.cx
					+ nColumn * _sizeBitmapPartCustom.cx,
					0
					);
				CRect rcBitmapPart(
					ptBitmapOffset,
					_sizeBitmapPartCustom
					);
				CExtCmdIcon _icon;
				_icon.m_bmpNormal.FromBitmap(
					_bmp,
					&rcBitmapPart
					);
				ASSERT( ! _icon.IsEmpty() );
				_icon.m_bmpNormal.AlphaColor( clrTransparentCustom, RGB(0,0,0), 0 );
				pPalettePopup->ItemInsertCommand(
					3 + nRow*nColumnCountCustom + nColumn,
					-1,
					NULL,
					NULL,
					_icon
					);
				if( nColumn == (nColumnCountCustom-1) )
					pPalettePopup->ItemPaletteRowWrapSet(
					pPalettePopup->ItemGetCount() - 1
					);
			} // for( nColumn = 0; nColumn < nColumnCountCustom; nColumn ++ )
		} // for( nRow = 0; nRow < nRowCountCustom; nRow ++ )
	} // if( nReplacePos >= 0 )
	*/
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////