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
/* ������� ����� �������� �����               */
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

	//��������� �������
	virtual void Draw(Gdiplus::Graphics& gr);

	// 0 = ��� ���������, 1 = ��������� 
	virtual int HitTest(CPoint& point, CFSView* pView, bool bSelected);

	
	//��������� �������
	void GetPosition(CRect& rcPos) const;

	//����� �� ���������� ������ 
	bool IsVisible() const;

public: 

	//��������� ���������� �������������� �������
	std::string GetID() const; 

	//��������� ������ ����������� ��������� ��� �������
	virtual std::string OnNeedHintText() const;
	
	//�������� �������� ������ ��������� � ��������
	virtual DataSrc* GetAssignedDataSource() const;

	//����� �� ������������ ����� �� ��������� ���������
	//��������� ������ ��������� � ��������
	virtual bool UserHasPermission() const;

public:	

	//��������� ������� ������ ����
	virtual bool OnLButtonClick(UINT nFlags);
	virtual bool OnMButtonClick(UINT nFlags);
	virtual bool OnRButtonClick(UINT nFlags);

	//��������� ������� �������� ������ ����
	virtual bool OnLButtonDoubleClick(UINT nFlags);
	virtual bool OnMButtonDoubleClick(UINT nFlags);
	virtual bool OnRButtonDoubleClick(UINT nFlags);
	
protected:

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);
	
	//���������� �������� ������ ��� �������� ��������� �������	
	DiscreteDataSrc*  m_pVisDS;

	//��������� �������� ������ �������� ��������� ��� �����������
	bool			  m_bvisState;

	//������������ ���� �������
	FSLayer* m_pParentLayer;
	
	// ������� �������
	Gdiplus::RectF	m_pos;

	// ��� ������� �������
	Gdiplus::Pen	m_pen;

	// ��� ������� �������
	Gdiplus::SolidBrush	m_brush; 
	
	//���� �������� ������ ����	
	bool m_useHitHest;

	//����� ����������� ��������� 
	std::string m_strHint;	

	//��������� ������������� �������
	std::string m_strID;
};


//////////////////////////////////////////////////////////////////////////
// ��������� ���������� �������

class CDrawTextObj : public CDrawObj
{
// Constructors
public:

	CDrawTextObj ();
	CDrawTextObj (const CRect& position);

	virtual ~CDrawTextObj();

public:

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
	static CDrawTextObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);


public:

	//������ ������������ ������������ ������
	void SetVerticalAlign(Gdiplus::StringAlignment align);
	
	//�������� ������� ������������ ������������ ������
	Gdiplus::StringAlignment GetVerticalAlign () const;

	//������ �������������� ������������ ������
	void SetHorizontalAlign(Gdiplus::StringAlignment align);
	
	//�������� ������� �������������� ������������ ������
	Gdiplus::StringAlignment GetHorizontalAlign () const;

	void GetLogFont(LOGFONT& lf);
	void SetLogFont(LOGFONT& lf);

	//������ ��� ������ 
	void SetFontFace(std::string fntFaceName); 

	//�������� ��� ������
	std::string GetFontFace() const; 

	//������ ������ ������
	void SetFontSize(Gdiplus::REAL  newSize);

	//�������� ������ ������
	Gdiplus::REAL GetFontSize() const;

	//������ ���� � ������������ ������
	void SetFontColor(COLORREF clr, double transparency);

	//�������� ���� ������
	COLORREF GetFontColor() const;

	//�������� ������� ������������ � ���������
	double GetFontTransparency() const; 

//////////////////////////////////////////////////////////////////////////
// ������ �� �������
	
	//������ ����� styles = "bold;italic;underline;strikeout;shadow"
	//�������� ������� � ����� ������������������ ����������� ����� � ������� 
	void SetFontStyle(std::string styles); 

	//������ ����� 
	//fs = ���������� ������  (Gdiplus::FontStyleBoldItalic |Gdiplus::FontStyleUnderline| Gdiplus::FontStyleStrikeout)
	//Gdiplus::FontStyleRegular     
	//Gdiplus::FontStyleBold        
	//Gdiplus::FontStyleItalic      
	//Gdiplus::FontStyleBoldItalic  
	//Gdiplus::FontStyleUnderline   
	//Gdiplus::FontStyleStrikeout
	void SetFontStyle(Gdiplus::FontStyle fs); 

	//�������� ������� �����
	Gdiplus::FontStyle GetFontStyle() const;
	
	bool CDrawTextObj::IsBold() const;

	//���������� ����� ���������� �����
	void CDrawTextObj::SetBold(bool bSet = TRUE);

	//����� ���������?
	bool CDrawTextObj::IsItalic() const;

	//���������� ����� ��������� �����
	void CDrawTextObj::SetItalic(bool bSet = TRUE);

	//����� ������������?
	bool CDrawTextObj::IsUnderline() const;
	
	//���������� ����� ������������ �����
	void CDrawTextObj::SetUnderline(bool bSet = TRUE);

	//����� �����������?
	bool CDrawTextObj::IsStrikeout() const;
	

	//���������� ����� ����������� �����
	void CDrawTextObj::SetStrikeout(bool bSet = TRUE);

	//����� � �����?
	bool CDrawTextObj::IsShadowed() const;

	//���������� ����� ����
	void CDrawTextObj::SetShadow(bool bSet = TRUE);
	

	//��������� �������
	virtual void Draw(Gdiplus::Graphics& gr);

	void DrawString(Gdiplus::Graphics& g, std::string txt, Gdiplus::RectF rc, Gdiplus::REAL ang, Gdiplus::Brush& brush);

#ifdef _DEBUG
	void AssertValid();
#endif

protected:
	
	virtual std::string OnNeedText() const;
	virtual void OnCalcTextRect();
	virtual Gdiplus::Color OnNeedColor() const; 

	Gdiplus::StringFormat	fmt;		// ������������ ������
	Gdiplus::Color			fontClr;	// ����� ����
	Gdiplus::Font*			pFnt;		// �����		
	Gdiplus::RectF			rcText;
	Gdiplus::REAL angle;	//���� ��������
	std::string		outTxt;	//
	bool bShadowEffect;	
};


//////////////////////////////////////////////////////////////////////////
//����� ��� ����������� �������� �������
class CDrawDateTimeObj : public CDrawTextObj
{

public:

	CDrawDateTimeObj();
	CDrawDateTimeObj( const CRect& position);

	virtual ~CDrawDateTimeObj();


public:

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
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
//����� ��� ����������� �������
class CDrawTimerObj : public CDrawTextObj
{
public:
	CDrawTimerObj();
	CDrawTimerObj( const CRect& position);

	virtual ~CDrawTimerObj();

public:

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
	static CDrawTimerObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent,  UINT &error);


protected:

	virtual std::string OnNeedHintText() const;
	virtual std::string OnNeedText() const;
	virtual Gdiplus::Color OnNeedColor() const;

protected :

	// ������
	BaseTimer* pTimer;

	// ����� ���� ��� ������������ �������	
	Gdiplus::Color	fontClrOwerflow; 
};


class CDrawCurrentUserObj : public CDrawTextObj
{
public:

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
	static CDrawCurrentUserObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	CDrawCurrentUserObj();
	CDrawCurrentUserObj( const CRect& position);

	virtual ~CDrawCurrentUserObj();

	virtual bool OnLButtonClick(UINT nFlags);	
	virtual bool OnLButtonDoubleClick(UINT nFlags);
	

public:

	//��������� �������
	void Draw(Gdiplus::Graphics& gr);

	//virtual std::string OnNeedHintText() const;

protected:

	virtual void OnCalcTextRect();
	virtual std::string OnNeedText() const;
};

//////////////////////////////////////////////////////////////////////////
//����� ��� ����������� �������� ��������� ���������� ��������
class CDrawAnalogDSObj : public CDrawTextObj
{

public:

	CDrawAnalogDSObj();
	CDrawAnalogDSObj( const CRect& position);

	virtual ~CDrawAnalogDSObj();

public:
	
	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
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


	//��������� �������
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

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
	static CDrawProgressAnalogDSObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	
public:

	//��������� �������
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
//����� ��� ����������� �������� ��������� ���������� ��������
class CDrawDiscreteDSAsTextObj : public CDrawTextObj
{

public:
	
	CDrawDiscreteDSAsTextObj();
	CDrawDiscreteDSAsTextObj( const CRect& position);

	virtual ~CDrawDiscreteDSAsTextObj();

public:

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
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
// ����� ��� ��������� �����������
class CDrawImageObj : public CDrawObj
{

public:

	CDrawImageObj();
	CDrawImageObj(const CRect& position);

	virtual ~CDrawImageObj();

public:

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);
	
	//�������� ��������  �� XML ����
	static CDrawImageObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	//��������� �������
	virtual void Draw(Gdiplus::Graphics& gr);

protected:
	
	virtual Gdiplus::Image* OnNeedImage() const { return srcImg; };
	
	Gdiplus::Bitmap* srcImg;

private:

};

//////////////////////////////////////////////////////////////////////////
//����� ��� ����������� �������� ��������� ���������� ��������
class CDrawDiscreteDSObj : public CDrawImageObj
{

public:

	CDrawDiscreteDSObj();
	CDrawDiscreteDSObj(const CRect& position);

	virtual ~CDrawDiscreteDSObj();


public:
	
	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//�������� ��������  �� XML ����
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

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//�������� ��������  �� XML ����
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
//����� ��� ����������� �������� ��������� ���������� ��������
class CDrawFilmObj : public CDrawDiscreteDSObj
{

public:

	CDrawFilmObj();
	CDrawFilmObj(const CRect& position);

	virtual ~CDrawFilmObj();

public:
	
	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
	static CDrawFilmObj * CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

		
public:
	
	virtual bool OnLButtonDoubleClick(UINT nFlags) { return TRUE; };

protected:
	
	virtual Gdiplus::Image* OnNeedImage() const;
	mutable int curFrame;
	
};

//����� ��� ����������� �������� ��������� ���������� ��������
class CDrawActionObj : public CDrawDiscreteDSObj
{
public:

	CDrawActionObj();
	CDrawActionObj(const CRect& position);

	virtual ~CDrawActionObj();
public:
	
	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//�������� ��������  �� XML ����
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

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//�������� ��������  �� XML ����
	static CDrawSelectObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);
		

	//��������� �������
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

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent); 

	//�������� ��������  �� XML ����
	static CDrawAnalogValueSelectObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);


	//��������� �������
	virtual void Draw(Gdiplus::Graphics& gr);

	virtual bool OnSelectionChanged(long data);

protected:

	virtual bool OnFillContent();	

};

//////////////////////////////////////////////////////////////////////////
// CDrawReactorCrossSection - ����� ��� ��������� 
// ������������� ����������� � ������������ ������� ��������
class CDrawReactorCrossSection : public CDrawObj
{

public:
	
	CDrawReactorCrossSection();
	CDrawReactorCrossSection(const CRect& position);

	virtual ~CDrawReactorCrossSection();

public:

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
	static CDrawReactorCrossSection* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

public:

	//��������� �������
	virtual void Draw(Gdiplus::Graphics& gr);

	bool GetOrientation() const;

	void SetOrientation(bool vert);


protected:

private:

	//CMap<double, double, CPair<> > map

	//reactorMap.insert(pos, std::make_pair(DSnmaeAtLeft, DsnameAtRight ) );
	//pos - ������� ����� � ��������� ������� � ������� ����� (0-100)
	//DSnameAtLeft - ��� ��������� ������ ��� ��������� ����������� � ����� �����;
	//DsnameAtRight - ��� ��������� ������ ��� ��������� ����������� � ����� ������;
	//����� ���������� ������ ����� � ������ ����� ���� ����������� - ��� ������ 
	//��� ����������� �������� � ����� ����� � ����������� ���������� ��� ����� �����
	std::map<double, std::pair<AnalogDataSrc*, AnalogDataSrc*> > reactorMap;


	//���������� � ����������� ��������� ����������� ��������
	double minT;
	double maxT;

	//���������� ������������
	bool vertical;
	
	//������������ 
	double opacity;

	////������������� �������
	//temperaturePalette.insert(pos, color);
	//pos - ������� ����������� [0-100%] � �������� [minT - maxT] 
	//color - ���� ����������� �����������
	std::map<double, Gdiplus::Color, std::greater<double> > temperaturePalette;

	Gdiplus::Color GetPaletteColor(double T);
	
};

class CDrawPathAnimation : public CDrawObj
{
public:

	CDrawPathAnimation();
	CDrawPathAnimation(const CRect& position);

	virtual ~CDrawPathAnimation();

	//�������� ��������  �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent);

	//�������� ��������  �� XML ����
	static CDrawPathAnimation* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	//��������� �������
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

	//�������� ��������  �� XML ����
	static CDrawPlotObj* CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error);

	//��������� �������
	virtual void Draw(Gdiplus::Graphics& gr);

protected:

	//�������� ��������  �� XML ����
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