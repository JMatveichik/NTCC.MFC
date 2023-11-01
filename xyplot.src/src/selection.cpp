#include "stdafx.h"
#include "selection.h"
#include "lock.h"
#include "linedata.h"
#include "legend.h"
#include "main.h"

static const char BEGINMARK[] = "[selection]";
static const char ENDMARK[] = "[/selection]";

using namespace std;
using namespace xyplot;

Selection::Selection( XYPlot*  pPlot ) : plot ( *pPlot )
{
	::InitializeCriticalSection(&cs);
	markerLine.SetWidth(1);
	markerLine.SetType(PLS_CUSTOM);
	markerLine.SetTemplate("3 3");
	markerLine.SetColor(RGB(64, 180, 64));	

	dwSelectionType  = SM_FLASH;	

	ellipseRadius = 3; 

	activeProfile = 0;
	activeGroup = 0;
	selectedPointInd = 0;

	activeAxis = NULL;

	evRunFlash	= ::CreateEvent( NULL, TRUE, FALSE, NULL );
	evQuitFlash = ::CreateEvent( NULL, FALSE, FALSE, NULL );

	SetFlashSpeed(FLS_FAST);

	DWORD dwID = 0L;
	flashThread = CreateThread( NULL, 0, fnFlashProc, this, 0L, &dwID );
	flashThread = NULL;
}

Selection::~Selection()
{	
	MainCriticalSectionHandler lockDestructor( &cs );
	
	if( flashThread )
	{
		SetEvent( evQuitFlash );
		RunFlashRoutine();
		
		// ensure if the thread has exited
		WaitForSingleObject( flashThread, INFINITE );

		// free handle
		CloseHandle( flashThread );
	}

	::CloseHandle(evRunFlash);
	::CloseHandle(evQuitFlash);
	::DeleteCriticalSection(&cs);
}

void Selection::OnDraw(HDC hdc, const RECT rc)
{
	MainCriticalSectionHandler csh( &cs );

	if (dwSelectionType == SM_FLASH)
		return;

	if (!activeProfile)
		return;

	Profile* p = const_cast<XYPlot&>(plot).FindProfile(activeProfile);
	if (p->Empty())
		return;

	ptSel = p->GetRenderPoint(selectedPointInd, physSelPointX, physSelPointY );

	HPEN old = (HPEN)SelectObject(hdc, (HPEN)markerLine);

	if ( dwSelectionType == SM_VTRACE || dwSelectionType == SM_TRACE)
	{
		MoveToEx(hdc, ptSel.x, ptSel.y, NULL);
		LineTo(hdc, ptSel.x , rc.top);

		MoveToEx(hdc, ptSel.x, ptSel.y, NULL);
		LineTo(hdc, ptSel.x, rc.bottom);
	}

	if ( dwSelectionType == SM_HTRACE || dwSelectionType == SM_TRACE)
	{
		MoveToEx(hdc, ptSel.x, ptSel.y, NULL);
		LineTo(hdc, rc.left, ptSel.y);

		MoveToEx(hdc, ptSel.x, ptSel.y, NULL);
		LineTo(hdc, rc.right, ptSel.y);
	}

	MoveToEx(hdc, ptSel.x, ptSel.y, NULL);
	Ellipse(hdc, ptSel.x - ellipseRadius, ptSel.y + ellipseRadius, ptSel.x + ellipseRadius, ptSel.y - ellipseRadius );

	RECT rcFr;
	SetRect(&rcFr, ptSel.x - 10, ptSel.y - 10, ptSel.x + 10, ptSel.y + 10);
	DrawFocusRect(hdc , &rcFr);
	
	SelectObject(hdc, old);
}

void Selection::ResolvePlotHitTest(const LPPLOTHITTESTRESULT lpht)
{
	assert(lpht != NULL);

	MainCriticalSectionHandler csh( &cs );

	StopFlashRoutine();

	activeProfile = 0;
	activeGroup = 0;

	ptSel = lpht->ptHit;

	switch(lpht->itemID)
	{
 	case PI_TITLE:
	case PI_BACKGROUND:
	case PI_NOTHING:
		break;

 	case PI_AXIS_LEFT:
 	case PI_AXIS_TOP:
 	case PI_AXIS_RIGHT:
 	case PI_AXIS_BOTTOM:
		{
			if (dwSelectionType == SM_HTRACE || dwSelectionType == SM_VTRACE)
			{
				Axis* pAxis = (Axis*)lpht->itemID;

				bool isXAxis = pAxis->GetID() == PI_AXIS_TOP || pAxis->GetID() == PI_AXIS_BOTTOM;

				if (isXAxis && dwSelectionType == SM_HTRACE)
					return;

				if (!isXAxis && dwSelectionType == SM_VTRACE)
					return;

				AXISLIMITS al;
				pAxis->GetLimits(&al);

				RECT rc;
				pAxis->GetClientRect(rc);

				int coord;
				if (isXAxis)
				{
					coord = ptSel.x = lpht->ptHit.x;
					ptSel.y = rc.bottom;
					
				}
				else
				{
					ptSel.x = rc.left;
					coord = ptSel.y = lpht->ptHit.y;					
				}

				double val;
				CoordConverter::ConvertToPhysic(*pAxis, coord, val);

				pAxis->GetNearestTick(val, ptSel);
			}			
		}		
 		break;

 	case PI_LEGEND:
		{
			activeProfile = plot.FindProfile(lpht->itemID) == NULL ? 0 : lpht->itemID;
			activeGroup = lpht->keyControl ? 0 : long(profileGroupMap.FindGroup(long(activeProfile)));

			if ( !activeProfile )
				return;

			if (dwSelectionType == SM_FLASH)
				RunFlashRoutine();

			HWND hWnd = plot.GetParent();
			::SendMessage(hWnd, lNotifyMessage, (WPARAM)activeProfile, (LPARAM)-1);
		}
 	    break;

	default:
		{
			activeProfile = plot.FindProfile(lpht->itemID) == NULL ? 0 : lpht->itemID;
			activeGroup = lpht->keyControl ? 0 : long(profileGroupMap.FindGroup(long(activeProfile)));

			if (dwSelectionType == SM_FLASH)
			{
				RunFlashRoutine();
			}
			else
			{
				ptSel =  lpht->ptHit;				
				selectedPointInd = lpht->pointIndex;
			}
			
			HWND hWnd = plot.GetParent();
			::SendMessage(hWnd, lNotifyMessage, (WPARAM)activeProfile, (LPARAM)selectedPointInd );
		}		
	    break;
	}
}

long Selection::GetSelection() const 
{
	::EnterCriticalSection( &cs );
	long sel = activeProfile ? long(activeProfile) : activeGroup;
	::LeaveCriticalSection( &cs );
	return sel;
}

void Selection::RunFlashRoutine()
{
	MainCriticalSectionHandler lockRunFlashRoutine( &cs );
	SetEvent( evRunFlash );
}

void Selection::StopFlashRoutine()
{
	MainCriticalSectionHandler lockStopFlashRoutine( &cs );

	ResetEvent( evRunFlash );

	if (activeGroup)
	{
		std::vector<long> v;
		profileGroupMap.GetProfiles(activeGroup, v);
		for (size_t i=0; i<v.size(); i++)
			plot.FindProfile(v[i])->Show(TRUE);
	}
	else if (activeProfile)
	{
		plot.FindProfile(activeProfile)->Show(TRUE);
	}
}

DWORD WINAPI Selection::fnFlashProc( LPVOID lpParam )
{	
 	Selection *pSelf = reinterpret_cast<Selection*>( lpParam ); 

	for(;;)
	{
		if( WaitForSingleObject( pSelf->evQuitFlash, 50L ) == WAIT_OBJECT_0 )
			return 0L;

		BOOL visible = FALSE;
		while( WaitForSingleObject( pSelf->evRunFlash, 0L ) == WAIT_OBJECT_0 )
		{
			if( WaitForSingleObject( pSelf->evQuitFlash, 0L ) == WAIT_OBJECT_0 )
				return 0L;

			pSelf->SetFlashState( visible );

			Sleep( visible ? pSelf->durationShow : pSelf->durationHide );

			visible = !visible;
		}
	}
}

unsigned flash_duration[][2] = {
	{0, 0},
	{1200, 600},
	{600, 400},
	{400, 300},
	{300, 150},
	{150, 75},
	{10, 10},
};

FLASH_SPEED Selection::GetFlashSpeed() const
{
	::EnterCriticalSection( &cs );
	FLASH_SPEED fs = m_fs;
	::LeaveCriticalSection( &cs );
	return fs;
}

void Selection::SetFlashSpeed( FLASH_SPEED fs )
{
	MainCriticalSectionHandler lockStopFlashRoutine( &cs );

	if (fs < 0 || fs > sizeof(flash_duration)/sizeof(unsigned)/2)
		throw std::exception("SetFlashSpeed - Invalid argument");

	durationShow = flash_duration[fs][0];
	durationHide = flash_duration[fs][1];
	m_fs = fs;
}

void Selection::SetFlashState( BOOL visible )
{
 	MainCriticalSectionHandler csh( &cs );

	plot.SetRedraw(FALSE);

	if (activeGroup)
	{
		vector<long> v;
		profileGroupMap.GetProfiles(activeGroup, v);
		for (vector<long>::iterator it=v.begin(); it!=v.end(); ++it)
		{
			Profile* p = plot.FindProfile(PROFILE_KEY(*it));
			if (p)
				p->Show(visible);
		}
	}
	else if ( activeProfile )
		plot.FindProfile(activeProfile)->Show(visible);

	csh.Release();

	plot.SetRedraw(TRUE);
}

BOOL Selection::Select(long ID, long index)
{
	StopFlashRoutine();

	Profile* p = plot.FindProfile( PROFILE_KEY(ID) );
	if (p)
	{
		::EnterCriticalSection( &cs );

		unsigned start, end;
		p->GetVisibleRange(start, end);
		if (index < long(start) || index > long(end))
		{
			::LeaveCriticalSection( &cs );
			return FALSE;
		}

		// Select profile
		activeProfile = PROFILE_KEY(ID);
		selectedPointInd = index;

		// Belongs to group?
		activeGroup = profileGroupMap.FindGroup(ID);

		::LeaveCriticalSection( &cs );
	}
	else if (profileGroupMap.IsGroup(ID))
	{
		// Select group
		::EnterCriticalSection( &cs );

		vector<long> v;
		profileGroupMap.GetProfiles(ID, v);
		long max_size = 0;
		long max_size_profile = 0;
		for (vector<long>::iterator it=v.begin(); it!=v.end(); ++it)
		{
			Profile* p = plot.FindProfile(*it);
			if (!p)
				continue;
			if (max_size < long(p->DataSize()))
			{
				max_size = p->DataSize();
				max_size_profile = *it;
			}
		}
		if (index >= max_size)
		{
			::LeaveCriticalSection( &cs );
			return FALSE;
		}

		activeGroup = ID;
		activeProfile = max_size_profile;
		selectedPointInd = index;

		::LeaveCriticalSection( &cs );
	}
	else {
		activeGroup = activeProfile = selectedPointInd = 0;
		return TRUE;
	}

	if (dwSelectionType == SM_FLASH)
		RunFlashRoutine();
	else
		plot.Update();

	return TRUE;
}

void Selection::ResolveKeyPress( long code )
{
	if (dwSelectionType == SM_FLASH)
	{
		if( code == VK_TAB )
		{
			BOOL shift = HIBYTE ( GetKeyState( VK_SHIFT ) ) != 0;

			const ProfileMap& profiles = plot.GetProfileMap();

			ProfileMapConstIterator it = profiles.find(activeProfile);
			
			if ( it == profiles.end() )
				return;

			ProfileMap::const_iterator itNew;
			if ( shift )
			{
				if (it == profiles.begin())
					advance(it, profiles.size() - 1);
				else
					--it;
			}
			else
			{
				++it;
				if (it == profiles.end())
					it = profiles.begin();
			}
			
			StopFlashRoutine();
			activeProfile = (*it).first;
			RunFlashRoutine();

			HWND hWnd = plot.GetParent();
			::SendMessage(hWnd, lNotifyMessage, (WPARAM)activeProfile, (LPARAM)-1);
		}
	}
	else
	{
		if( !activeProfile )
			return;

		Profile* p = plot.FindProfile(activeProfile);
		assert(p);

		unsigned start, end;
		p->GetVisibleRange(start, end);

		switch( code )
		{
		
		case VK_LEFT:
		case VK_DOWN:
			
			if ( selectedPointInd > start )
				selectedPointInd--;
			break;

		case VK_RIGHT:
		case VK_UP:
			
			if ( selectedPointInd < end )
				selectedPointInd++;
			break;

		case VK_PRIOR:	// нажата клавища PageUp
			selectedPointInd = ( ( selectedPointInd + 10 ) <= end) ? selectedPointInd + 10 : end;
			break;
		case VK_NEXT:	// нажата клавиша PageDown
			selectedPointInd = ( ( int(selectedPointInd) - 10 ) >= int(start) ) ? selectedPointInd - 10 : start;
			break;

		case VK_HOME:
			selectedPointInd = start;
		    break;

		case VK_END:
			selectedPointInd = end;
		    break;
		default:
		    break;
		}

		HWND hWnd = plot.GetParent();
		::SendMessage(hWnd, lNotifyMessage, (WPARAM)activeProfile, (LPARAM)selectedPointInd);
	}
}

BOOL Selection::Write(HANDLE hFile) const
{
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;

	if (!markerLine.Write(hFile))
		return FALSE;

	DWORD dwRes;
	if (!WriteFile(hFile, &ellipseRadius, sizeof(ellipseRadius), &dwRes, NULL) || dwRes != sizeof(ellipseRadius))
		return FALSE;

	if (!WriteFile(hFile, &dwSelectionType, sizeof(dwSelectionType), &dwRes, NULL) || dwRes != sizeof(dwSelectionType))
		return FALSE;	

	if (!WriteFile(hFile, &durationShow, sizeof(durationShow), &dwRes, NULL) || dwRes != sizeof(durationShow))
		return FALSE;

	if (!WriteFile(hFile, &durationHide, sizeof(durationHide), &dwRes, NULL) || dwRes != sizeof(durationHide))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;
	
	return TRUE;
}

BOOL Selection::Read(HANDLE hFile)
{
	try {
		if (ReadString(hFile) != BEGINMARK)
			return FALSE;

		if (!markerLine.Read(hFile))
			return FALSE;

		DWORD dwRes;
		if (!ReadFile(hFile, &ellipseRadius, sizeof(ellipseRadius), &dwRes, NULL) || dwRes != sizeof(ellipseRadius))
			return FALSE;

		if (!ReadFile(hFile, &dwSelectionType, sizeof(dwSelectionType), &dwRes, NULL) || dwRes != sizeof(dwSelectionType))
			return FALSE;

		if (!ReadFile(hFile, &durationShow, sizeof(durationShow), &dwRes, NULL) || dwRes != sizeof(durationShow))
			return FALSE;

		if (!ReadFile(hFile, &durationHide, sizeof(durationHide), &dwRes, NULL) || dwRes != sizeof(durationHide))
			return FALSE;

		if (ReadString(hFile) != ENDMARK)
			return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}
