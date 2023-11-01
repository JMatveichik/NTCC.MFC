#pragma once

#include ".\..\xyplot\XYPlotWrapper.h"
#include "FSDoc.h"

typedef std::map<std::string, xyplot::HPROFILE> PROFILESMAP;
typedef std::map<std::string, xyplot::HLEVEL> LEVELSMAP;
typedef std::map<std::string, xyplot::HPLOTREGION> REGIONSMAP;
typedef std::map<xyplot::HPLOTREGION, REGIONINFO> REGIONSINFOMAP;

// CPopupGraphWnd
class CPopupGraphWnd : public CWnd, public CExtPmBridge
{
	DECLARE_DYNAMIC(CPopupGraphWnd)

	DECLARE_CExtPmBridge_MEMBERS( CPopupGraphWnd );

public:
	
	CPopupGraphWnd();
	virtual ~CPopupGraphWnd();

protected:

	DECLARE_MESSAGE_MAP()

	TRACKMOUSEEVENT tme;

	enum tagAxisScaleType
	{
		AXISSCALEAUTO = 0,
		AXISSCALECALCULATE,
		AXISSCALEFIXED
	};

	xyplot::HPLOT	m_hPlot;
	PROFILESMAP		m_profiles;
	LEVELSMAP		m_checkPoints;
	REGIONSMAP		m_regions;
	REGIONSINFOMAP	m_regionsInfo;
	int				m_nBufferSize;
	
	double m_dYAxisMin;
	double m_dYAxisMax;

	tagAxisScaleType GetAxisScaleType(const std::pair<double, double>& r);
	void SetupAxis(UINT axisID, std::string title, const std::pair<double, double>& r);

	tagAxisScaleType leftYAxisScale;
	tagAxisScaleType rightYAxisScale;

	xyplot::HPROFILE m_hLastAddedProfile;

public:

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);	
	
	BOOL AddDataSrc(std::string name, bool bClearOld = true, int lt = xyplot::PLS_SOLID, int lw = 2, std::string lp = "2 3", bool toLeftAxis = true);
	BOOL AddDataSrc(GRAPHIC_GROUP& gg);

	BOOL AddCheckPoint(std::string name, double val);
	BOOL AddRegion(std::string name, const REGIONINFO& ri);


	void Clear();
	void SetTitle(std::string title);

	void AutoHideSet(bool bAutoHide = true);	

protected:

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	void UpdatePlot();
	void UpdateRegions();

	void SelectLastAddedProfile();

	void OptimizeAxisScale(std::pair<double, double>& ar );
	std::pair<double, double> CalcAxisRange(UINT axisID);

public:

	HCURSOR hCursorArrow;
	HCURSOR hCursorMove;
	HCURSOR hCursorNS;
	HCURSOR hCursorWE;
	HCURSOR hCursorNWSE;
	HCURSOR hCursorNESW;
	
	CPoint ptFrom;
	double m_dAxisShift;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	

	bool bLButtonDown;

	//static CString className;
	
protected:

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	virtual void PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM );

public:

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);	
};


