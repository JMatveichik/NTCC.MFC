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
///@brief Класс для работы с базой данных
///@details Осуществляет подключение к базе при создании 
///и закрывает соединение при уничтожении

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

	//Создание и присоединение объекта графика к определенному окну 
	long Create( HWND hwndParent );

	//Уничтожение объекта графика 
	void Destroy();

	//Получение описателя окна в котором отображается график
	HWND GetParent() { return hwndParent; }

	LineData& GetBorderLine() { return border.GetLineData(); };
	void SetBorderLine(LineData& line) { return border.SetLineData(line); };

	//TODO: заменить RECT& на const RECT& 
	RECT& TotalRect()  { return totalRect;  }
	RECT& ClientEdge() { return clientEdge; }

    long SetAxisRange(long axis, double xmin, double xmax );
	long GetAxisRange(long axis, double &dmin, double &dmax ) const;	

	void GetClientBkColors(COLORREF& clr1, COLORREF& clr2) const;
	void GetPlotBkColors(COLORREF& clr1, COLORREF& clr2) const;

	void SetClientBkColors(COLORREF clr1, COLORREF clr2);
	void SetPlotBkColors(COLORREF clr1, COLORREF clr2);
/////////////////////////////////////////////////
//	СОХРАНЕНИЕ / ЗАГРУЗКА СВОЙСТВ ГРАФИКА
/////////////////////////////////////////////////

	//Сохранение свойств графика в файл
	void SaveProperties( const char *lpszFileName );

	//Сохранение свойств графика в файл
	void LoadProperties( const char *lpszFileName );

	//Записать значения по умолчанию в реестр
	void RegSetDefaults();

	//Записать текущие значения свойств графика в реестр
	BOOL RegSaveSettings( const char *keyString, const char* pszValue );

	//Загрузить свойства графика из реестра
	BOOL RegLoadSettings( const char *keyString, const char* pszValue );

	void SetRedraw( BOOL redraw ) {	if (this->redraw == redraw) Update(); }

/////////////////////////////////////////////////
// Функции для работы с профилями
/////////////////////////////////////////////////

	//Добавить кривую 
	long AddProfile( const char* name, COLORREF color, int width, int linetype, const char* line_template, BOOL bVisible, BOOL bMarked, BOOL bottom_axis, BOOL left_axis, long* lProfileID );


	//Установить новые данные для кривой
	long SetData( PROFILE_KEY key, const double *px, const double *py, int count );

	//Дописать данные к кривой
	long AppendData( PROFILE_KEY key, const double *px, const double *py, int count );

	//Дописать данные к кривой
	long AppendData( PROFILE_KEY key, double fx, double fy );

	//удалить кривую
	long DeleteProfile( PROFILE_KEY key );

	//Найти кривую
	Profile* FindProfile( PROFILE_KEY key );
	PROFILE_KEY FindProfile( const Profile* profile ) const;

	//Получить список кривых
	const ProfileMap& GetProfileMap() const { return profileMap; }

/////////////////////////////////////////////////
// Функции для работы с линиями уровня
/////////////////////////////////////////////////

	//Добавить линию уровня
	long AddLevelLine(std::string name,	COLORREF color,  long* lLevelID,   int width = 1, LONG axis = xyplot::PI_AXIS_LEFT, double value = 0.0);

	//Удалить линию уровня
	long DeleteLevelLine( long key );

	//найти линию уровня
	LevelLine* FindLevelLine( long key );

	//Получить список линей уровня
	const LevelsMap& GetLevelsMap() const { return levelsMap; }

	//Установить значение уровня
	long SetLevelValue(long key, double value);

////////////////////////////////////////////////////

/////////////////////////////////////////////////
// Функции для работы с регионами 
/////////////////////////////////////////////////

	//Добавить регион
	long AddRegion(long xAxisID, long yAxisID, long* lRegionID, double x1, double x2, double y1, double y2, const char* pszName);

	//Удалить регион
	long DeleteRegion(long lRegionID);

	//Найти регион
	PlotRegion* FindRegion(long lRegionID);

	//Установить пределы региона 
	long SetRegionLimits(long lRegionID, double x1, double x2, double y1, double y2);

	//Установить надпись региона
	long SetRegionTitle(long lRegionID, const char* pszName);

	//Получить список регионов
	const RegionsMap& GetRegionsMap() const { return regionsMap; };


	/*Обновить часть графического окна определенного прямоугольником pUpdateRect. 
     Если pUpdateRect == NULL  обновить все окно*/
	void Update( RECT *pUpdateRect = NULL );

	void PushClipboard( HDC hdcClip ) { OnDraw( hdcClip ); }	
	void DrawToBitmap(HBITMAP hBmp, bool bDrawWindowBG, bool bDrawClientBG);

    Axis* GetAxis(long axisPos);

	HMODULE hmod;

protected:

	Axis* xAxisBottom;	//Нижняя  ось Х
	Axis* xAxisTop;		//Верхняя ось Х
	Axis* yAxisLeft;	//Левая   ось Y
	Axis* yAxisRight;	//Верхняя ось Y;

	BOOL recalcAxisSize;	//Флаг пересчета размеров осей 

	Border border;			// Объект границы графика
	Legend legend;			// Объект легенда графика
	Title  title;			// Объект заголовок графика
	
	Selection selection;	//Объект выделение графиков

	BackGround bkTotal;	 // Объект для отрисовки фона всего графика
	BackGround bkClient; // Объект для отрисовки фона клиентской части графика

	ProfileMap profileMap;
	LevelsMap  levelsMap;
	RegionsMap regionsMap;
    
	//Функция обработки цикла сообщений окна
	static LRESULT __stdcall fnXYPlotHandlerProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

	//Внедрение в цикл обработки сообщений
	bool HookChain();

	//Освобождение цикла обработки 
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