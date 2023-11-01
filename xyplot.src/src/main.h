#ifndef __MAIN_H_INCLUDED__
#define __MAIN_H_INCLUDED__

#include "xyplot.h"
#include "border.h"
#include "digits.h"
#include "label.h"
#include "legend.h"
#include "profile.h"
#include "ticks.h"
#include "title.h"
#include "group.h"
#include "lock.h"
#include "selection.h"
#include "LevelLine.h"
#include "BackGroundFill.h"
#include "Regions.h"
#include "axis.h"

///\~russian
///@brief ����� ��� ������ � ����� ������
///@details ������������ ����������� � ���� ��� �������� 
///� ��������� ���������� ��� �����������

class XYPlot
{
public:

	XYPlot(HMODULE hmod);
	~XYPlot();

public :

	Title* GetTitle() { return &title; }
	Border* GetBorder() { return &border; }
	Legend* GetLegend() { return &legend; }
	Selection* GetSelection() { return &selection; }
	BackGround* GetPlotBG() { return &bkTotal; }
	BackGround* GetClientBG() { return &bkClient; }

	//�������� � ������������� ������� ������� � ������������� ���� 
	long Create( HWND hwndParent );

	//����������� ������� ������� 
	void Destroy();

	//��������� ��������� ���� � ������� ������������ ������
	HWND GetParent() { return hwndParent; }

	LineData& GetBorderLine() { return border.GetLineData(); };
	void SetBorderLine(LineData& line) { return border.SetLineData(line); };

	//TODO: �������� RECT& �� const RECT& 
	RECT& TotalRect()  { return totalRect;  }
	RECT& ClientEdge() { return clientEdge; }

    long SetAxisRange(long axis, double xmin, double xmax );
	long GetAxisRange(long axis, double &dmin, double &dmax ) const;	

	void GetClientBkColors(COLORREF& clr1, COLORREF& clr2) const;
	void GetPlotBkColors(COLORREF& clr1, COLORREF& clr2) const;

	void SetClientBkColors(COLORREF clr1, COLORREF clr2);
	void SetPlotBkColors(COLORREF clr1, COLORREF clr2);
/////////////////////////////////////////////////
//	���������� / �������� ������� �������
/////////////////////////////////////////////////

	//���������� ������� ������� � ����
	void SaveProperties( const char *lpszFileName );

	//���������� ������� ������� � ����
	void LoadProperties( const char *lpszFileName );

	//�������� �������� �� ��������� � ������
	void RegSetDefaults();

	//�������� ������� �������� ������� ������� � ������
	BOOL RegSaveSettings( const char *keyString, const char* pszValue );

	//��������� �������� ������� �� �������
	BOOL RegLoadSettings( const char *keyString, const char* pszValue );

	void SetRedraw( BOOL redraw ) {	if (this->redraw == redraw) Update(); }

/////////////////////////////////////////////////
// ������� ��� ������ � ���������
/////////////////////////////////////////////////

	//�������� ������ 
	long AddProfile( const char* name, COLORREF color, int width, int linetype, const char* line_template, BOOL bVisible, BOOL bMarked, BOOL bottom_axis, BOOL left_axis, long* lProfileID );


	//���������� ����� ������ ��� ������
	long SetData( PROFILE_KEY key, const double *px, const double *py, int count );

	//�������� ������ � ������
	long AppendData( PROFILE_KEY key, const double *px, const double *py, int count );

	//�������� ������ � ������
	long AppendData( PROFILE_KEY key, double fx, double fy );

	//������� ������
	long DeleteProfile( PROFILE_KEY key );

	//����� ������
	Profile* FindProfile( PROFILE_KEY key );
	PROFILE_KEY FindProfile( const Profile* profile ) const;

	//�������� ������ ������
	const ProfileMap& GetProfileMap() const { return profileMap; }

/////////////////////////////////////////////////
// ������� ��� ������ � ������� ������
/////////////////////////////////////////////////

	//�������� ����� ������
	long AddLevelLine(std::string name,	COLORREF color,  long* lLevelID,   int width = 1, LONG axis = xyplot::PI_AXIS_LEFT, double value = 0.0);

	//������� ����� ������
	long DeleteLevelLine( long key );

	//����� ����� ������
	LevelLine* FindLevelLine( long key );

	//�������� ������ ����� ������
	const LevelsMap& GetLevelsMap() const { return levelsMap; }

	//���������� �������� ������
	long SetLevelValue(long key, double value);

////////////////////////////////////////////////////

/////////////////////////////////////////////////
// ������� ��� ������ � ��������� 
/////////////////////////////////////////////////

	//�������� ������
	long AddRegion(long xAxisID, long yAxisID, long* lRegionID, double x1, double x2, double y1, double y2, const char* pszName);

	//������� ������
	long DeleteRegion(long lRegionID);

	//����� ������
	PlotRegion* FindRegion(long lRegionID);

	//���������� ������� ������� 
	long SetRegionLimits(long lRegionID, double x1, double x2, double y1, double y2);

	//���������� ������� �������
	long SetRegionTitle(long lRegionID, const char* pszName);

	//�������� ������ ��������
	const RegionsMap& GetRegionsMap() const { return regionsMap; };


	/*�������� ����� ������������ ���� ������������� ��������������� pUpdateRect. 
     ���� pUpdateRect == NULL  �������� ��� ����*/
	void Update( RECT *pUpdateRect = NULL );

	void PushClipboard( HDC hdcClip ) { OnDraw( hdcClip ); }	
	void DrawToBitmap(HBITMAP hBmp, bool bDrawWindowBG, bool bDrawClientBG);

    Axis* GetAxis(long axisPos);

	HMODULE hmod;

protected:

	Axis* xAxisBottom;	//������  ��� �
	Axis* xAxisTop;		//������� ��� �
	Axis* yAxisLeft;	//�����   ��� Y
	Axis* yAxisRight;	//������� ��� Y;

	BOOL recalcAxisSize;	//���� ��������� �������� ���� 

	Border border;			// ������ ������� �������
	Legend legend;			// ������ ������� �������
	Title  title;			// ������ ��������� �������
	
	Selection selection;	//������ ��������� ��������

	BackGround bkTotal;	 // ������ ��� ��������� ���� ����� �������
	BackGround bkClient; // ������ ��� ��������� ���� ���������� ����� �������

	ProfileMap profileMap;
	LevelsMap  levelsMap;
	RegionsMap regionsMap;
    
	//������� ��������� ����� ��������� ����
	static LRESULT __stdcall fnXYPlotHandlerProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

	//��������� � ���� ��������� ���������
	bool HookChain();

	//������������ ����� ��������� 
	void UnhookChain();

	static void WriteProperties( XYPlot *XYPlot, const char *filename, std::pair<void*, long>& );
	static void ReadProperties( XYPlot *XYPlot, const char *fileName, std::pair<void*,long> inPair );

	PLOTHITTESTRESULT HitTest( const POINT pptHit,  LONG flags );


	WNDPROC lpfnOriginalWndProc;
	HWND hwndParent;

	RECT totalRect;
	RECT clientEdge;

	CRITICAL_SECTION mainCritical;
	CRITICAL_SECTION tracerCritical;
	CRITICAL_SECTION streamerCritical;

	COLORREF bkColor;

	//
	bool enableQuickScaleMode;
	bool selectionStart;
	POINT fromPt;
	POINT toPt;
	RECT  sel;
	void NormalRect(POINT p1, POINT p2);
	void DrawSelectingRect(HDC hdc);
	//

	double m_dGrowthFactorX;
	double m_dGrowthFactorY;


	int appendMode;
	BOOL redraw;
	double xpixel;
	double ypixel;

	BOOL disabled;
	HBITMAP bitmap;
	
	//////////////////////////////////////////////////////////////////////////

	void DeflateVisible( RECT &visible );
	void PreDraw( HDC hdc, LPRECT lpRect = NULL );

	void TraceMessage( HDC hdc );
	void OnDraw( HDC hdc, LPRECT lpRect = NULL, bool bDrawWindowBG = true, bool bDrawClientBG = true  );

	void VisualException(HDC hdc, std::string strMessage, std::string strTitle);
};
#endif