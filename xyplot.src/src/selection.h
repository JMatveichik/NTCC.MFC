#ifndef __SELECTION_H_INCLUDED__
#define __SELECTION_H_INCLUDED__
#include "group.h"
#include "profile.h"
#include "xyplotconst.h"
#include "SavableItem.h"

class LineData;

typedef struct tagPLOTHITTESTRESULT
{
	POINT ptHit;			// the point itself
	long itemID;			// ID of plot item or profile key
	unsigned pointIndex;	// profile nearst point if applicable
	BOOL keyControl;
} PLOTHITTESTRESULT, *LPPLOTHITTESTRESULT;

class Selection : public SavableItem
{
public:
	Selection( XYPlot*  pPlot );
	virtual ~Selection();
	
	BOOL Select(long ID, long index);

	virtual BOOL Write(HANDLE hFile) const;
	virtual BOOL Read(HANDLE hFile);

	long GetSelection() const;

	DWORD GetMode() const { return dwSelectionType; }; 
	
	DWORD SetMode(DWORD dwType) 
	{
		StopFlashRoutine();
		DWORD oldType = dwSelectionType;
		dwSelectionType = dwType;	
		return oldType; 
	}; 

	void SetNotifyMessageID(long lNotifyMsg) 
	{
		lNotifyMessage = lNotifyMsg;
	}

	void SetFlashSpeed( xyplot::FLASH_SPEED fs );
	xyplot::FLASH_SPEED GetFlashSpeed() const;

	void ResolvePlotHitTest(const LPPLOTHITTESTRESULT  lpht);
	void ResolveKeyPress( long code );
	
	void OnDraw(HDC hdc, const RECT rc);

 	long CreateGroup() { return profileGroupMap.CreateGroup(); };
 	long DeleteGroup(long group) { return profileGroupMap.DeleteGroup(group); };
 	long FindGroup(long profile) { return profileGroupMap.FindGroup(profile); };
 	long AddProfile(long group, long profile) { return profileGroupMap.AddProfile(group, profile); };

protected:
	XYPlot&  plot;

 	ProfileGroupMap profileGroupMap;

	//BOOL bGroupSelected;
	PROFILE_KEY activeProfile;
	long activeGroup;
	long lNotifyMessage;
	unsigned selectedPointInd;
	Axis*	 activeAxis;
	xyplot::FLASH_SPEED m_fs;

	LineData markerLine;
	DWORD	 dwSelectionType;	 
	POINT	 ptSel; 
	double	 physSelPointX;
	double	 physSelPointY;
	unsigned ellipseRadius;	

	

	HANDLE flashThread;
	HANDLE evRunFlash;
	HANDLE evQuitFlash;

	mutable CRITICAL_SECTION cs;

	unsigned durationHide;
	unsigned durationShow;

	static unsigned long __stdcall fnFlashProc( LPVOID lpParam );
	void SetFlashState( BOOL visible );
	void RunFlashRoutine();
	void StopFlashRoutine();
};

// class Selection : public SavableItem
// {
// public:
// 	static enum { MaxShowTime = 1000, MaxHideTime = 1000 };
// 	static enum { Outside, This, New }; // int state;
// 	static enum { None = SM_NONE, Flash = SM_FLASH, VerticalTrace = SM_VTRACE, HorizontalTrace = SM_HTRACE };
// 	static enum { Line, Cross }; // int traceHitState
// 
// 	inline LineData& GetLineData() { return *line; }
// 
// 	int GetMode();
// 	void SetMode( int mode );
// 
// 	void SetFlashTimes( unsigned show, unsigned hide );
// 	void GetFlashTimes( unsigned &show, unsigned &hide ) const { show = showDuration; hide = hideDuration; }
// 
// 	void SelectProfileKey( long key );
// 	long GetSelectedKey();
// 
// 	void SelectProfileGroupKey( long key );
// 	long GetSelectedGroupKey();
// 
// 	//inline int& State() { return state; }
// 	//int& TraceHitState() { return traceHitState; }
// 	//BOOL& IsGroupSelected() { return groupSelected; }
// 
// 	//BOOL& IsPointFound() { return pointFound; }
// 
// 	//double& XScale() { return xScale; }
// 	//double& YScale() { return yScale; }
// 	//int& RealIndex() { return realIndex; }
// 
// 	//POINT& PointLastHit() { return pointLastHit; }
// 	//D3dPoint& D3dPointLastHit() { return d3dPointLastHit; }
// 	//int& EllipseRadius() { return ellipseRadius; }
// 	
// 	//BOOL WaitOne( BOOL run, unsigned long time );
// 
// 	//void ResetTraceSelection();
// 
// 	void HandleKeydown( long code );
// 
// 	//void ShiftLeft();
// 	//void ShiftRight();
// 	//void ShiftBegin();
// 	//void ShiftEnd();
// 
// 	virtual BOOL Write(HANDLE hFile) const;
// 	virtual BOOL Read(HANDLE hFile);
// 
// 
// 	void ResolveHit( long key );
// 
// private:
// 	static unsigned long __stdcall fnFlashProc( LPVOID lpParam );
// 	unsigned long FlashProc();
// 	void SetFlashState( BOOL visible );
// 
// 	LineData* line; // selection linedata when in trace mode
// 
// 	HANDLE flashThread;
// 	HANDLE runFlashThread;
// 	HANDLE stopFlashThread;
// 	HANDLE quitFlashThread;
// 
// 	HPROFILE
// 
// 	CRITICAL_SECTION cs;
// 
// 	unsigned showDuration; // show time duration
// 	unsigned hideDuration; // hide time duration
// 
// 	long key;		// profile key if selected
// 	long groupKey;	// profile group key if group selected
// 	long traceMode; // selection mode (Flash|VerticalTrace|HorizontalTrace)
// 
// 	long HitState;	// indicates whether to show cross (profile has been hit) or single line
// 
// 	unsigned ellipseRadius; // radius of cross in trace mode
// 
// 	/*
// 
// 	POINT pointLastHit;
// 	D3dPoint d3dPointLastHit;
// 
// 	double xScale;
// 	double yScale;
// 
// 	int realIndex;
// 	BOOL isFlashing; // indicates whether flash routine is in progress
// 	int state; // indicates selection state after last hit-test
// 	BOOL pointFound; 
// 	BOOL groupSelected; // indicates whether profile group is selected
// 
// 	int HitState; // indicates whether to show cross (profile has been hit) or single line
// 	int showLength; // show time duration
// 	int hideLength; // hide time duration
// */
// };

#endif //__SELECTION_H_INCLUDED__