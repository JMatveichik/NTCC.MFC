#pragma once
#include "resource.h"


#include "..\xyplot\XYPlotWrapper.h"
#include "FSDoc.h"


class DataSrc;


// CPlotDlg dialog
class CPlotDlg : public CExtResizableDialog
{
	class CPlotToolControlBar : public CExtToolControlBar
	{
		virtual CExtBarContentExpandButton * OnCreateBarRightBtn(){ 
			return NULL; 
		}
	};


	typedef std::pair<double, double> AXISRANGE;
	typedef std::map<std::string, xyplot::HPROFILE> PROFILESMAP;
	typedef std::map<std::string, xyplot::HLEVEL> LEVELSMAP;
	typedef std::map<std::string, xyplot::HPLOTREGION> REGIONSMAP;
	typedef std::map<xyplot::HPLOTREGION, REGIONINFO> REGIONSINFOMAP;


	DECLARE_DYNAMIC(CPlotDlg)

public:
	CPlotDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlotDlg();

// Dialog Data
	enum { IDD = IDD_PLOT };
	
	enum {
		TIMER_HIDE	= 1,
		TIMER_UPDATE = 2
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CPlotToolControlBar	m_wndPlotToolBar;

public:
	virtual BOOL OnInitDialog();


public:

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);	

public :

	void Clear();
	BOOL AddDataSrc(std::string dsName, bool bClearOld,  AXISRANGE rng, int lt = xyplot::PLS_SOLID, int lw = 2, std::string lp = "2 3", bool toLeftAxis = true);
	BOOL AddDataSrc(GRAPHIC_GROUP& group);

	BOOL AddCheckPoint(std::string name, double val);
	BOOL AddRegion(std::string name, const REGIONINFO& ri);
	void UpdateRegions();

	void SetTitle(std::string title);
	void UpdatePlot();
	void AutoHideSet(bool bAutoHide);

protected:
	
	BOOL initPlot();
	BOOL initPlotDefautlts();
	void axisRangeFromDS(const DataSrc* pDS, bool leftAxis);
	void updateAxisSliders();
	void setSlider(UINT id, UINT axis);
	void buildPlotString(std::string text, std::string fontname, int fontsize, COLORREF color, std::string& out);
	void initProfilesData();

	static	DWORD WINAPI UpdateThread(LPVOID lpParameter);

protected:

	
	AXISRANGE axisLeftRange;
	AXISRANGE axisRightRange;
	AXISRANGE axisBottomRange;
	AXISRANGE axisTopRange;

	xyplot::HPLOT	hPlot;

	PROFILESMAP profiles;
	LEVELSMAP levels;
	REGIONSMAP rmap;
	REGIONSINFOMAP rminfo;

	double dAxisShift;

	CExtSliderWnd xMinSlider;
	CExtSliderWnd xMaxSlider;

	CExtSliderWnd yMinSlider;
	CExtSliderWnd yMaxSlider;

	CExtSliderWnd yMinSlider2;
	CExtSliderWnd yMaxSlider2;

	CWnd wndPlot;
	
	// Поток обновления графика
	HANDLE	m_hUpdateThread;

	//
	HANDLE m_hStopUpdate;

	BOOL showAxisSliders; 
	RECT rcPlotRect;

public:

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnShowLegend();
	afx_msg void OnShowRegions();
	afx_msg void OnShowCheckPoints();
	afx_msg void OnShowAxisSliders();

	afx_msg void OnShowLegendUI(CCmdUI *pCmdUI);
	afx_msg void OnShowRegionsUI(CCmdUI *pCmdUI);
	afx_msg void OnShowCheckPointsUI(CCmdUI *pCmdUI);
	afx_msg void OnShowAxisSlidersUI(CCmdUI *pCmdUI);	
	
};
