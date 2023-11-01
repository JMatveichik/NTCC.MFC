#pragma once

#include <gdiplus.h>
#include "..\xyplot\XYPlotWrapper.h"

class CFSView;
class FSLayer;
class DataSrc;
class AnalogDataSrc;
class DiscreteDataSrc;
class BaseTimer;


class CAutoSelComboBox;

class ImageStrip : public CArray< Gdiplus::Bitmap* >
{
public:	

	ImageStrip();	
	~ImageStrip();

	bool Create(Gdiplus::Bitmap* Image, Gdiplus::REAL width = 0.0f);
	bool Create(const char* pszFileName, Gdiplus::REAL width = 0.0f);
};

/**********************************************/
/* Базовый класс элемента схемы               */
/**********************************************/
class CDrawObj : public CObject
{
protected:
	
	DECLARE_SERIAL(CDrawObj);
	CDrawObj( );

// Constructors
public:

	CDrawObj(const CRect& position);
	
	virtual ~CDrawObj();
	virtual void Serialize(CArchive& ar);

#ifdef _DEBUG
	void AssertValid();
#endif

	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);

	// 0 = нет попадания, 1 = попадание 
	virtual int HitTest(CPoint& point, CFSView* pView, bool bSelected);

	
	//положение объекта
	void GetPosition(CRect& rcPos) const;

	//Нужно ли отображать объект 
	bool IsVisible() const;

public: 

	//Получение строкового идентификатора объекта
	std::string GetID() const; 

	//Получение текста всплывающей подсказки для объекта
	virtual std::string OnNeedHintText() const;
	
	//получить источник данных связанный с объектом
	virtual DataSrc* GetAssignedDataSource() const;

	//имеет ли пользователь права на изменение состояния
	//источника данных связанный с объектом
	virtual bool UserHasPermission() const;

public:	

	//обработка событий щелчка мыши
	virtual bool OnLButtonClick(UINT nFlags);
	virtual bool OnMButtonClick(UINT nFlags);
	virtual bool OnRButtonClick(UINT nFlags);

	//обработка событий двойного щелчка мыши
	virtual bool OnLButtonDoubleClick(UINT nFlags);
	virtual bool OnMButtonDoubleClick(UINT nFlags);
	virtual bool OnRButtonDoubleClick(UINT nFlags);
	
protected:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);
	
	//Дискретный источник данных для контроля видимости объекта	
	DiscreteDataSrc*  m_pVisDS;

	//Состояние источник данных контроля видимости для отображения
	bool			  m_bvisState;

	//Родительский слой объекта
	FSLayer* m_pParentLayer;
	
	// позиция объекта
	Gdiplus::RectF	m_pos;

	// для обводки области
	Gdiplus::Pen	m_pen;

	// для заливки области
	Gdiplus::SolidBrush	m_brush; 
	
	//Флаг проверки щелчка мыши	
	bool m_useHitHest;

	//Текст всплывающей подсказки 
	std::string m_strHint;	

	//Строковый идентификатор объекта
	std::string m_strID;
};


//////////////////////////////////////////////////////////////////////////
// Отрисовка текстового объекта

class CDrawTextObj : public CDrawObj
{
// Constructors
public:

	CDrawTextObj ();
	CDrawTextObj (const CRect& position);

	virtual ~CDrawTextObj();

public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawTextObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);


public:

	//Задать вертикальное выравнивание текста
	void SetVerticalAlign(Gdiplus::StringAlignment align);
	
	//Получить текущее вертикальное выравнивание текста
	Gdiplus::StringAlignment GetVerticalAlign () const;

	//Задать горизонтальное выравнивание текста
	void SetHorizontalAlign(Gdiplus::StringAlignment align);
	
	//Получить текущее горизонтальное выравнивание текста
	Gdiplus::StringAlignment GetHorizontalAlign () const;

	void GetLogFont(LOGFONT& lf);
	void SetLogFont(LOGFONT& lf);

	//Задать имя шрифта 
	void SetFontFace(std::string fntFaceName); 

	//Получить имя шрифта
	std::string GetFontFace() const; 

	//Задать размер шрифта
	void SetFontSize(Gdiplus::REAL  newSize);

	//Получить размер шрифта
	Gdiplus::REAL GetFontSize() const;

	//Задать цвет и прозрачность шрифта
	void SetFontColor(COLORREF clr, double transparency);

	//Получить цвет шрифта
	COLORREF GetFontColor() const;

	//получить уровень прозрачности в процентах
	double GetFontTransparency() const; 

//////////////////////////////////////////////////////////////////////////
// РАБОТА СО СТИЛЯМИ
	
	//Задать стили styles = "bold;italic;underline;strikeout;shadow"
	//задаются строкой в любой последовательности разделители точка с запятой 
	void SetFontStyle(std::string styles); 

	//Задать стили 
	//fs = комбинации стилей  (Gdiplus::FontStyleBoldItalic |Gdiplus::FontStyleUnderline| Gdiplus::FontStyleStrikeout)
	//Gdiplus::FontStyleRegular     
	//Gdiplus::FontStyleBold        
	//Gdiplus::FontStyleItalic      
	//Gdiplus::FontStyleBoldItalic  
	//Gdiplus::FontStyleUnderline   
	//Gdiplus::FontStyleStrikeout
	void SetFontStyle(Gdiplus::FontStyle fs); 

	//Получить текущие стили
	Gdiplus::FontStyle GetFontStyle() const;
	
	bool CDrawTextObj::IsBold() const;

	//Установить снять полужирный стиль
	void CDrawTextObj::SetBold(bool bSet = TRUE);

	//Шрифт наклонный?
	bool CDrawTextObj::IsItalic() const;

	//Установить снять наклонный стиль
	void CDrawTextObj::SetItalic(bool bSet = TRUE);

	//Шрифт подчеркнутый?
	bool CDrawTextObj::IsUnderline() const;
	
	//Установить снять подчеркнутый стиль
	void CDrawTextObj::SetUnderline(bool bSet = TRUE);

	//Шрифт зачеркнутый?
	bool CDrawTextObj::IsStrikeout() const;
	

	//Установить снять зачеркнутый стиль
	void CDrawTextObj::SetStrikeout(bool bSet = TRUE);

	//Шрифт с тенью?
	bool CDrawTextObj::IsShadowed() const;

	//Установить снять тень
	void CDrawTextObj::SetShadow(bool bSet = TRUE);
	

	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);

	void DrawString(Gdiplus::Graphics& g, std::string txt, Gdiplus::RectF rc, Gdiplus::REAL ang, Gdiplus::Brush& brush);

#ifdef _DEBUG
	void AssertValid();
#endif

protected:
	
	virtual std::string OnNeedText() const;
	virtual void OnCalcTextRect();
	virtual Gdiplus::Color OnNeedColor() const; 

	Gdiplus::StringFormat	fmt;		// Выравнивание текста
	Gdiplus::Color			fontClr;	// Шрифт цвет
	Gdiplus::Font*			pFnt;		// Шрифт		
	Gdiplus::RectF			rcText;
	Gdiplus::REAL angle;	//угол поворота
	std::string		outTxt;	//
	bool bShadowEffect;	
};


//////////////////////////////////////////////////////////////////////////
//класс для отображения текущего времени
class CDrawDateTimeObj : public CDrawTextObj
{

public:

	CDrawDateTimeObj();
	CDrawDateTimeObj( const CRect& position);

	virtual ~CDrawDateTimeObj();


public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawDateTimeObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	virtual std::string OnNeedHintText() const;


	void SetTimeFormat(std::string fmt);
	std::string GetTimeFormat() const;

	static CMapStringToString& EnumTimeFormats();

	
protected:

	virtual std::string OnNeedText() const;
	void initTimeFormats();

	std::string timeFormat;
	static CMapStringToString  timeFormats;
};

//////////////////////////////////////////////////////////////////////////
//класс для отображения таймера
class CDrawTimerObj : public CDrawTextObj
{
public:
	CDrawTimerObj();
	CDrawTimerObj( const CRect& position);

	virtual ~CDrawTimerObj();

public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawTimerObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent,  UINT &error);


protected:

	virtual std::string OnNeedHintText() const;
	virtual std::string OnNeedText() const;
	virtual Gdiplus::Color OnNeedColor() const;

protected :

	// Таймер
	BaseTimer* pTimer;

	// Шрифт цвет при переполнении таймера	
	Gdiplus::Color	fontClrOwerflow; 
};


class CDrawCurrentUserObj : public CDrawTextObj
{
public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawCurrentUserObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	CDrawCurrentUserObj();
	CDrawCurrentUserObj( const CRect& position);

	virtual ~CDrawCurrentUserObj();

	virtual bool OnLButtonClick(UINT nFlags);	
	virtual bool OnLButtonDoubleClick(UINT nFlags);
	

public:

	//отрисовка объекта
	void Draw(Gdiplus::Graphics& gr);

	//virtual std::string OnNeedHintText() const;

protected:

	virtual void OnCalcTextRect();
	virtual std::string OnNeedText() const;
};

//////////////////////////////////////////////////////////////////////////
//Класс для отображения текущего состояния аналоговой величины
class CDrawAnalogDSObj : public CDrawTextObj
{

public:

	CDrawAnalogDSObj();
	CDrawAnalogDSObj( const CRect& position);

	virtual ~CDrawAnalogDSObj();

public:
	
	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawAnalogDSObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

public:

	bool SetDataSource(std::string id);
	std::string GetDataSource() const;

	void ShowUnits(bool bShow = true);
	bool IsUnitsShow() const;

	void SetPrefix(std::string pref);
	std::string GetPrefix() const;


public:

	virtual std::string OnNeedHintText() const;

	virtual DataSrc* GetAssignedDataSource() const;


	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);

protected:
	
	virtual std::string OnNeedText() const;

	AnalogDataSrc* m_pADS;

	std::string prefix;
	bool bUnits;
};


class CDrawProgressAnalogDSObj : public CDrawAnalogDSObj
{

public:

	CDrawProgressAnalogDSObj();
	CDrawProgressAnalogDSObj( const CRect& position);

	virtual ~CDrawProgressAnalogDSObj();

public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawProgressAnalogDSObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	
public:

	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);


protected:	

private:

	Gdiplus::Color getBrushColor() const;

	Gdiplus::RectF CalcBarRect() const;
	void DrawBar( Gdiplus::Graphics& gr );

	enum tagProgressDirection { 
		TOP2BOTTOM, 
		BOTTOM2TOP, 
		LEFT2RIGHT, 
		RIGHT2LEFT, 
		DEFAULT = LEFT2RIGHT 
	} progressDir;

	Gdiplus::Color barColor;
	std::vector<Gdiplus::REAL> barMargins;

	BOOL showText;
	typedef std::pair<double, double> VALUE_RANGE;
	
	VALUE_RANGE valRange;

	std::map<VALUE_RANGE, Gdiplus::Color> colorize;
};


//////////////////////////////////////////////////////////////////////////
//Класс для отображения текущего состояния аналоговой величины
class CDrawDiscreteDSAsTextObj : public CDrawTextObj
{

public:
	
	CDrawDiscreteDSAsTextObj();
	CDrawDiscreteDSAsTextObj( const CRect& position);

	virtual ~CDrawDiscreteDSAsTextObj();

public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawDiscreteDSAsTextObj* CreateFromXML(IXMLDOMNode* pNode,FSLayer& parent, UINT &error);

	bool SetDataSource(std::string id);

public:

	virtual std::string OnNeedHintText() const;
	virtual DataSrc* GetAssignedDataSource() const;

protected:

	virtual std::string OnNeedText() const;

	DiscreteDataSrc* m_pDDS;

};

//////////////////////////////////////////////////////////////////////////
// Класс для отрисовки изображений
class CDrawImageObj : public CDrawObj
{

public:

	CDrawImageObj();
	CDrawImageObj(const CRect& position);

	virtual ~CDrawImageObj();

public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);
	
	//создание элемента  из XML узла
	static CDrawImageObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);

protected:
	
	virtual Gdiplus::Image* OnNeedImage() const { return srcImg; };
	
	Gdiplus::Bitmap* srcImg;

private:

};

//////////////////////////////////////////////////////////////////////////
//Класс для отображения текущего состояния дискретной величины
class CDrawDiscreteDSObj : public CDrawImageObj
{

public:

	CDrawDiscreteDSObj();
	CDrawDiscreteDSObj(const CRect& position);

	virtual ~CDrawDiscreteDSObj();


public:
	
	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//создание элемента  из XML узла
	static CDrawDiscreteDSObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	bool SetDataSource(std::string id);

public:
	
	virtual std::string OnNeedHintText() const;

	virtual DataSrc* GetAssignedDataSource() const;

	virtual bool OnLButtonDoubleClick(UINT nFlags);

protected:
	
	virtual Gdiplus::Image* OnNeedImage() const;

	DiscreteDataSrc* m_pDS;

	ImageStrip imgStrip;
};

//////////////////////////////////////////////////////////////////////////
///
class CDrawSystemBtnObj : public CDrawImageObj
{

public:
	
	CDrawSystemBtnObj();
	CDrawSystemBtnObj(const CRect& position);

	virtual ~CDrawSystemBtnObj();

public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//создание элемента  из XML узла
	static CDrawSystemBtnObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);
	
public:

	virtual bool OnLButtonDoubleClick(UINT nFlags);

private:

	bool ParseAction(std::string act);
	bool GetItemIdListFromPath (LPCSTR lpszPath, LPITEMIDLIST *lpItemIdList);

	void doAction();

	UINT actionType;
	std::string actionparams;


};

//////////////////////////////////////////////////////////////////////////
//Класс для отображения текущего состояния дискретной величины
class CDrawFilmObj : public CDrawDiscreteDSObj
{

public:

	CDrawFilmObj();
	CDrawFilmObj(const CRect& position);

	virtual ~CDrawFilmObj();

public:
	
	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawFilmObj * CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

		
public:
	
	virtual bool OnLButtonDoubleClick(UINT nFlags) { return TRUE; };

protected:
	
	virtual Gdiplus::Image* OnNeedImage() const;
	mutable int curFrame;
	
};

//Класс для отображения текущего состояния дискретной величины
class CDrawActionObj : public CDrawDiscreteDSObj
{
public:

	CDrawActionObj();
	CDrawActionObj(const CRect& position);

	virtual ~CDrawActionObj();
public:
	
	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//создание элемента  из XML узла
	static CDrawActionObj * CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	
		
public:
	
	virtual bool OnLButtonDoubleClick(UINT nFlags);

protected:
	
	std::string actionName;
	bool bIsScript;
};

class CDrawSelectObj : public CDrawObj
{

public:

	CDrawSelectObj();
	CDrawSelectObj( const CRect& position);

	virtual ~CDrawSelectObj();
public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//создание элемента  из XML узла
	static CDrawSelectObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);
		

	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);
	
	virtual bool OnSelectionChanged(long data);
		
protected:

	virtual bool OnFillContent();	

protected:
	
	DataSrc* m_pds;
	CAutoSelComboBox* m_pWnd;

};


class CDrawAnalogValueSelectObj : public CDrawSelectObj
{

public:

	CDrawAnalogValueSelectObj();
	CDrawAnalogValueSelectObj( const CRect& position);

	virtual ~CDrawAnalogValueSelectObj();
public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//создание элемента  из XML узла
	static CDrawAnalogValueSelectObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);


	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);

	virtual bool OnSelectionChanged(long data);

protected:

	virtual bool OnFillContent();	

};

//////////////////////////////////////////////////////////////////////////
// CDrawReactorCrossSection - класс для отрисовки 
// расперделения температуры в вертикальном сечении реактора
class CDrawReactorCrossSection : public CDrawObj
{

public:
	
	CDrawReactorCrossSection();
	CDrawReactorCrossSection(const CRect& position);

	virtual ~CDrawReactorCrossSection();

public:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawReactorCrossSection* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

public:

	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);

	bool GetOrientation() const;

	void SetOrientation(bool vert);


protected:

private:

	//CMap<double, double, CPair<> > map

	//reactorMap.insert(pos, std::make_pair(DSnmaeAtLeft, DsnameAtRight ) );
	//pos - позиция среза в процентах начиная с верхней части (0-100)
	//DSnameAtLeft - имя источника данных для измерения температуры в срезе слева;
	//DsnameAtRight - имя источника данных для измерения температуры в срезе справа;
	//имена источников данных слева и справа могут быть одинаковыми - это значит 
	//что температура меряется в одной точке и принимается одинаковой для всего среза
	std::map<double, std::pair<AnalogDataSrc*, AnalogDataSrc*> > reactorMap;


	//минимально и максимально возможная температура реактора
	double minT;
	double maxT;

	//ориентация вертикальная
	bool vertical;
	
	//прозрачность 
	double opacity;

	////температурная палитра
	//temperaturePalette.insert(pos, color);
	//pos - позиция температуры [0-100%] в дипазоне [minT - maxT] 
	//color - цвет отображения температуры
	std::map<double, Gdiplus::Color, std::greater<double> > temperaturePalette;

	Gdiplus::Color GetPaletteColor(double T);
	
};

class CDrawPathAnimation : public CDrawObj
{
public:

	CDrawPathAnimation();
	CDrawPathAnimation(const CRect& position);

	virtual ~CDrawPathAnimation();

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//создание элемента  из XML узла
	static CDrawPathAnimation* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);

	
protected:

	CArray<Gdiplus::PointF> pathPoints;
	CArray<Gdiplus::PointF> animationPoints;
	Gdiplus::Bitmap* srcImg;
	DiscreteDataSrc* pAniControl;

	int distance;
	float step;
	int curStep;
};


class CDrawPlotObj : public CDrawObj
{
public:

	virtual ~CDrawPlotObj();

	//создание элемента  из XML узла
	static CDrawPlotObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	//отрисовка объекта
	virtual void Draw(Gdiplus::Graphics& gr);

protected:

	//создание элемента  из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

private:
	
	bool initPlotDefautlts();
	void setupProfiles(IXMLDOMNode* pNode);
	void setupTitle(IXMLDOMNode* pNode);

	void setupAxis(IXMLDOMNode* pNode, long axisID);
	void buildPlotString(std::string text, std::string fontname, int fontsize, COLORREF color, std::string& out);
	void buildPlotStringFromXml(std::string text, IXMLDOMNode*pFontNode,  std::string& out);
	void updatePlot();

	double dAxisShift;

	xyplot::HPLOT hPlot;
	HWND hwndPlot;

	std::map<std::string, xyplot::HPROFILE> profiles;
	std::map<long, std::string> axisIDs;
};