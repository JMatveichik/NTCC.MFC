// FSDocItem.cpp : implementation file
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "FSDocItem.h"
#include "FSDoc.h"
#include "FSView.h"
#include "DLGs/UserLogonDlg.h"

#include "ah_errors.h"
#include "ah_xml.h"
#include "ah_users.h"
#include "ah_scripts.h"
#include "ah_fonts.h"
#include "ah_project.h"

#include "Automation/DataSrc.h"
#include "Automation/Timers.h"
#include "Automation/DataCollector.h"
#include "DLGs/ExtControls.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

//Логическая операция "исключающее или"
#define XOR(a, b) (!a || !b) && (a || b)



ImageStrip::ImageStrip()
{

}
ImageStrip::~ImageStrip()
{

}

bool ImageStrip::Create(Gdiplus::Bitmap* image, Gdiplus::REAL width/* = 0.0f*/)
{
	//очищаем 
	RemoveAll();

	UINT cy = image->GetHeight();
	UINT cx = (width == 0.0f) ? cy : (UINT)width;

	if (cx == 0 || cy == 0)
		return false;

	UINT nCount = image->GetWidth() / cx;
	
	for (UINT i = 0; i < nCount; i++)
	{
		Gdiplus::Bitmap* imgPart = image->Clone(cx*i, 0, cx, cy, PixelFormat32bppARGB);
		Add(imgPart);
	}

	return true;
}

bool ImageStrip::Create(const char* pszFileName, Gdiplus::REAL width /*=18*/)
{
	USES_CONVERSION;
	return Create(Gdiplus::Bitmap::FromFile(A2W(pszFileName)), width);
}



void CopyRectF(Gdiplus::RectF& rcDest, const CRect& rcSrc)
{

	rcDest.X	  = (Gdiplus::REAL)rcSrc.TopLeft().x;
	rcDest.Y	  = (Gdiplus::REAL)rcSrc.TopLeft().y;
	rcDest.Width  = (Gdiplus::REAL)rcSrc.Width();
	rcDest.Height = (Gdiplus::REAL)rcSrc.Height();


}

void CopyRectF( CRect& rcDest, const Gdiplus::RectF& rcSrc)
{

	rcDest.left	  = (LONG)rcSrc.X;
	rcDest.top	  = (LONG)rcSrc.Y;
	rcDest.right  = (LONG)rcSrc.X + (LONG)rcSrc.Width;
	rcDest.bottom = (LONG)rcSrc.Y + (LONG)rcSrc.Height;
}


//////////////////////////////////////////////////////////////////////////
#pragma region CDrawObj

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CDrawObj, CCmdTarget, 0)


//  [10/6/2015 Johnny A. Matveichik]
CDrawObj::CDrawObj() :
	m_pVisDS(nullptr),
	m_pParentLayer(NULL),
	m_pos(0, 0, 0, 0),
	m_pen( Gdiplus::Color(0,255,255,255), 0.0f ),
	m_brush(Gdiplus::Color(0,255,255,255)),
	m_useHitHest(true)
{
	
}

CDrawObj::~CDrawObj()
{	
}

CDrawObj::CDrawObj(const CRect& pos) :
	m_pVisDS(nullptr),
	m_pParentLayer(NULL),
	m_pen( Gdiplus::Color(0,255,255,255), 0.0f ),
	m_brush(Gdiplus::Color(0,255,255,255)),
	m_useHitHest(true)
{

	//позиция
	CopyRectF( m_pos, pos); 	

}

void CDrawObj::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

//создание элемента  из XML узла
bool CDrawObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	m_pParentLayer = &parent;

// 	[1/10/2012 Johnny A. Matveichik]
	CComPtr<IXMLDOMNamedNodeMap> pPosAtr;
	pNode->get_attributes(&pPosAtr);

	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	
	
	if (!appXML.GetAttributeByName(pPosAtr, "ID", m_strID) )
	{
		boost::format fmt("%s%05d");
		fmt % parent.Name().c_str();  fmt % parent.GetItemCount();
		m_strID = fmt.str();
	}

	std::string val;
	m_pos.X = 0.0;
	if ( appXML.GetAttributeByName(pPosAtr, "X", val) )
		m_pos.X = (Gdiplus::REAL)atof(val.c_str());	
	
	m_pos.Y = 0.0;
	if ( appXML.GetAttributeByName(pPosAtr, "Y", val) )
		m_pos.Y = (Gdiplus::REAL)atof(val.c_str());

	m_pos.Width  = 0.0;
	if ( appXML.GetAttributeByName(pPosAtr, "W", val) )
		m_pos.Width  = (Gdiplus::REAL)atof(val.c_str());
	
	m_pos.Height = 0.0;
	if ( appXML.GetAttributeByName(pPosAtr, "H", val) )
		m_pos.Height = (Gdiplus::REAL)atof(val.c_str());
		

	m_pos.Offset(m_pParentLayer->GetBase());
	
	m_pVisDS = NULL;
	m_bvisState = true;
	if ( appXML.GetNodeAttributeByName(pNode, "VISIBLE", val) )
	{
		std::vector< std::string > parts;
		boost::split(parts, val,  std::bind2nd(std::equal_to<char>(), ':'));

		if ( parts.size() == 2 )
		{
			//Префикс
			std::string pref = m_pParentLayer->GetParent()->DSPrefix();
			std::string name =  pref + parts[0];

			m_pVisDS = dynamic_cast<DiscreteDataSrc*>(DataSrc::GetDataSource(name));

			if ( m_pVisDS == NULL ) 
			{
				AppErrorsHelper::Instance().SetLastError(DSCE_INVALID_DS_ID, "Не найден источник данных для отображения элемента");
			}
			else
			{
				m_bvisState = atoi( parts[1].c_str() ) > 0;
			}
		}
	}

	m_useHitHest = true;
	if ( appXML.GetAttributeByName(pPosAtr, "HIT", val) )
		m_useHitHest = atoi( val.c_str() ) > 0;

	CComPtr<IXMLDOMNode> pFillNode;
	pNode->selectSingleNode(CComBSTR("FILL"), &pFillNode);
	
	Gdiplus::Color clrFill = Gdiplus::Color::White;
	if ( pFillNode != NULL )
	{
		Gdiplus::Color clr;
		if ( appXML.GetGdiPlusColor(pFillNode, clr, 100) )
			clrFill =  clr;			
		else {
			clrFill = appXML.GetGdiPlusColor(
				m_pParentLayer->GetDefault("FILL.COLOR").c_str(), 
				m_pParentLayer->GetDefault("FILL.OPACITY").c_str()
				);
		}
		
	}
	else{	
		clrFill = appXML.GetGdiPlusColor(
			m_pParentLayer->GetDefault("FILL.COLOR").c_str(), 
			m_pParentLayer->GetDefault("FILL.OPACITY").c_str()
			);
	}
	
	m_brush.SetColor(clrFill);

	CComPtr<IXMLDOMNode> pStrokeNode;
	pNode->selectSingleNode(CComBSTR("STROKE"), &pStrokeNode);

	Gdiplus::Color clrStroke = Gdiplus::Color::Black;
	Gdiplus::REAL  widthStroke = 0.0f;

	if ( pStrokeNode != NULL )
	{	
		Gdiplus::Color clr;
		if ( appXML.GetGdiPlusColor(pStrokeNode, clr, 100) )
			clrStroke = clr;
		else
			clrStroke = appXML.GetGdiPlusColor(
			m_pParentLayer->GetDefault("FILL.COLOR").c_str(), 
			m_pParentLayer->GetDefault("FILL.OPACITY").c_str() 
			);

		std::string stroke;
		if ( !appXML.GetNodeAttributeByName(pStrokeNode, "WIDTH", stroke) )
			stroke = m_pParentLayer->GetDefault("STROKE.WIDTH");
		
		widthStroke = (Gdiplus::REAL)atof(stroke.c_str());
		
	}
	else
	{
		clrStroke = appXML.GetGdiPlusColor(m_pParentLayer->GetDefault("STROKE.COLOR").c_str(), 
											m_pParentLayer->GetDefault("STROKE.OPACITY").c_str());
		widthStroke = (Gdiplus::REAL)atof( m_pParentLayer->GetDefault("STROKE.WIDTH").c_str() );
	}

	m_pen.SetColor(clrStroke);
	m_pen.SetWidth(widthStroke);
	m_pen.SetAlignment(Gdiplus::PenAlignmentInset);
	
	//Подсказка 
	CComBSTR hint;
	pNode->get_text(&hint);

	if ( hint.Length() != 0 )
	{
		USES_CONVERSION;
		std::string h; 
		boost::format fmt("<b><p>Notes:</p></b><i>%s</i>");
		fmt % W2A(hint);
		m_strHint = fmt.str();
	}

//  [1/10/2012 Johnny A. Matveichik]

	return true;
}


//Получение строкового идентификатора объекта
std::string CDrawObj::GetID() const 
{ 
	return m_strID; 
} 

//Получение текста всплывающей подсказки для объекта
std::string CDrawObj::OnNeedHintText() const 
{ 
	return m_strHint; 
};

bool CDrawObj::IsVisible() const
{
	if ( m_pVisDS != NULL )
		return ( bool(m_pVisDS->IsEnabled())  == m_bvisState);
	
	return true;
}

void CDrawObj::GetPosition(CRect& rcPos) const
{
	CopyRectF(rcPos, m_pos);
}

void CDrawObj::Draw(Gdiplus::Graphics& gr)
{
	Gdiplus::RectF tmp = m_pParentLayer->GetParent()->GetFirstViewRect();
	if ( m_pos.Width == 0.0 )
		m_pos.Width = tmp.Width;

	if (m_pos.Height == 0.0)
		m_pos.Height = tmp.Height;

	//закрашиваем 
	gr.FillRectangle(&m_brush, m_pos);

	//обводим
	gr.DrawRectangle(&m_pen, m_pos);
	
}

// point is in logical coordinates
int CDrawObj::HitTest(CPoint& pt, CFSView* pView, bool bSelected)
{
	bSelected;
	ASSERT_VALID(this);
	
	if (!m_useHitHest)
		return 0;

	if (!IsVisible())
		return 0;

	if (m_pos.Contains((Gdiplus::REAL)pt.x, (Gdiplus::REAL)pt.y) )
		return 1;

	return 0;
}

//получить источник данных связанный с объектом
DataSrc* CDrawObj::GetAssignedDataSource() const
{
	return NULL;
}

//имеет ли пользователь права на изменение состояния
//источника данных связанный с объектом
bool CDrawObj::UserHasPermission() const
{
	const DataSrc* pDS = GetAssignedDataSource();
	
	if ( pDS == NULL )
		return true;
	
	LPNTCC_APP_USER lpCurUser = AppUsersHelper::Instance().GetCurrentUser();
	if (lpCurUser == NULL)
		return false;

	if ( (unsigned int)lpCurUser->ug <= pDS->Permission() )
		return true;

	return false;
}


bool CDrawObj::OnLButtonClick(UINT nFlags)
{
	return false;
}

bool CDrawObj::OnMButtonClick(UINT nFlags)
{
	return false;
}
bool CDrawObj::OnRButtonClick(UINT nFlags)
{
	return false;
}

bool CDrawObj::OnLButtonDoubleClick(UINT nFlags)
{
	return false;
}
bool CDrawObj::OnMButtonDoubleClick(UINT nFlags)
{
	return false;
}
bool CDrawObj::OnRButtonDoubleClick(UINT nFlags)
{
	return false;
}


#ifdef _DEBUG
void CDrawObj::AssertValid()
{
	ASSERT( m_pos.GetLeft() <= m_pos.GetRight() );
	ASSERT( m_pos.GetBottom() <= m_pos.GetTop() );
}
#endif
#pragma endregion


//////////////////////////////////////////////////////////////////////////
// Constructors
#pragma region  CDrawTextObj


CDrawTextObj::CDrawTextObj () :
	fontClr(0),
	pFnt ( new Gdiplus::Font (L"Vernada", 10.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint) ),	//Шрифт по умолчанию
	angle(0),
	outTxt("Текст"),
	bShadowEffect(false),
	CDrawObj(CRect(0,0,0,0))
{

}


CDrawTextObj::CDrawTextObj (const CRect& position) : 
	fontClr(0),
	pFnt ( new Gdiplus::Font (L"Vernada", 10.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint) ),	//Шрифт по умолчанию
	angle(0),
	outTxt("Текст"),
	bShadowEffect(false),
	CDrawObj(position)
//	CDrawObj(position),
//	pFnt ( new Gdiplus::Font (L"Vernada", 10.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint) ),	//Шрифт по умолчанию
//	outTxt("Текст")
{	
	SetVerticalAlign(Gdiplus::StringAlignmentCenter);
	SetHorizontalAlign(Gdiplus::StringAlignmentNear);
}

CDrawTextObj::~CDrawTextObj()
{

}

//создание элемента  из XML узла
bool CDrawTextObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	//  [1/17/2012 Johnny A. Matveichik]

	if ( !CDrawObj::CreateFromXMLNode(pNode, parent) )
		return false;

	std::string sVal;
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "SRC", sVal) )
	{
		AppErrorsHelper::Instance().SetLastError(DOCE_PARAMETER_NOT_DEFINED, "Не задан текст элемента (атрибут SRC).");
		return false;
	}
	
	outTxt = sVal.c_str();

	CComPtr<IXMLDOMNode> pAlignNode;  
	HRESULT hr = pNode->selectSingleNode(CComBSTR("ALIGN"), &pAlignNode);

	//Выравнивание текста в прямоугольнике по умолчанию
	fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
	fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	fmt.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox|
						Gdiplus::StringFormatFlagsMeasureTrailingSpaces|
						Gdiplus::StringFormatFlagsDisplayFormatControl|
						Gdiplus::StringFormatFlagsLineLimit);

	std::string strVAlign = m_pParentLayer->GetDefault("ALIGN.VALIGN");
	std::string strHAlign = m_pParentLayer->GetDefault("ALIGN.HALIGN");

	if (SUCCEEDED(hr) && pAlignNode != NULL)
	{
		if ( appXML.GetNodeAttributeByName(pAlignNode, "HALIGN", sVal) )
			strHAlign = sVal;
		else 
			strHAlign = m_pParentLayer->GetDefault("ALIGN.HALIGN");		

		if ( appXML.GetNodeAttributeByName(pAlignNode, "VALIGN", sVal) )
			strVAlign = sVal;
		else 
			strVAlign = m_pParentLayer->GetDefault("ALIGN.VALIGN");
	}
	
	boost::algorithm::to_lower(strHAlign);
	if ( strHAlign == "left")
		fmt.SetAlignment(Gdiplus::StringAlignmentNear);
	else if (strHAlign == "right")
		fmt.SetAlignment(Gdiplus::StringAlignmentFar);
	else if (strHAlign == "center")
		fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

	boost::algorithm::to_lower(strVAlign);
	if ( strVAlign == "top")
		fmt.SetLineAlignment(Gdiplus::StringAlignmentNear);
	else if (strVAlign == "bottom")
		fmt.SetLineAlignment(Gdiplus::StringAlignmentFar);
	else if (strVAlign == "center")
		fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	CComPtr<IXMLDOMNode> pFontNode;
	hr = pNode->selectSingleNode(CComBSTR("FONT"), &pFontNode);

	// Параметры шрифта по умолчанию
	// Шрифт имя по умолчанию
	std::string	   fontName = m_pParentLayer->GetDefault("FONT.NAME");
	
	// Шрифт размер  по умолчанию
	Gdiplus::REAL	fontSz = (Gdiplus::REAL)atof(m_pParentLayer->GetDefault("FONT.SIZE").c_str() );
	
	// Шрифт цвет  по умолчанию
	Gdiplus::Color	fontClrDef = appXML.GetGdiPlusColor(
							m_pParentLayer->GetDefault("FONT.COLOR").c_str(), 
							m_pParentLayer->GetDefault("FONT.OPACITY").c_str()); 
	
	// Шрифт стиль  по умолчанию
	std::string fStyle = m_pParentLayer->GetDefault("FONT.STYLE");
	INT		style = Gdiplus::FontStyleRegular;

	Gdiplus::REAL ang = (Gdiplus::REAL)atof(m_pParentLayer->GetDefault("FONT.ANGLE").c_str());

	if ( SUCCEEDED(hr) && pFontNode != NULL )
	{
		if ( appXML.GetNodeAttributeByName(pFontNode, "NAME", sVal) )
			fontName = sVal;

		if ( appXML.GetNodeAttributeByName(pFontNode, "SIZE", sVal) )
			fontSz = (Gdiplus::REAL)atof(sVal.c_str());			// Шрифт размер		
		
		if ( appXML.GetNodeAttributeByName(pFontNode, "STYLE", sVal) )
			fStyle = sVal;
		
		if ( appXML.GetNodeAttributeByName(pFontNode, "ANGLE", sVal) )
			ang = (Gdiplus::REAL)atof(sVal.c_str());		

		Gdiplus::Color clr;
		if ( appXML.GetGdiPlusColor(pFontNode, clr, 100.0) )
			fontClrDef = clr;
	}

	angle = ang;

	boost::algorithm::to_lower(fStyle);
	bShadowEffect = (boost::algorithm::contains(fStyle, "shadow"));

	if ( boost::algorithm::contains(fStyle, "bold"))
		style |=  Gdiplus::FontStyleBold;

	if ( boost::algorithm::contains(fStyle, "italic"))
		style |=  Gdiplus::FontStyleItalic;

	if ( boost::algorithm::contains(fStyle, "underline"))
		style |=  Gdiplus::FontStyleUnderline;

	if ( boost::algorithm::contains(fStyle, "strikeout"))
		style |=  Gdiplus::FontStyleStrikeout;

	USES_CONVERSION;

	//Gdiplus::Status satus;
	Gdiplus::FontFamily* pFF = new Gdiplus::FontFamily(A2W(fontName.c_str()));
	
	if ( pFF->GetLastStatus() != Gdiplus::Ok)
	{
	
		const Gdiplus::PrivateFontCollection& afc = AppFontsHelper::Instance().AppFontsCollection();
		pFF = new Gdiplus::FontFamily(A2W(fontName.c_str()), &afc);
	}

	pFnt = new Gdiplus::Font ( pFF,  fontSz,  Gdiplus::FontStyle( style ),  Gdiplus::UnitPoint);
	
	fontClr = fontClrDef;	
	
	rcText = m_pos;

	return true;
}
	
//создание элемента  из XML узла
CDrawTextObj* CDrawTextObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawTextObj> obj ( new CDrawTextObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}
	
	return obj.release();
}

void CDrawTextObj::GetLogFont(LOGFONT& lf)
{
	CWnd* pWnd = CWnd::FromHandle( GetDesktopWindow() );
	Gdiplus::Graphics gr( pWnd->GetDC()->m_hDC );

	pFnt->GetLogFontA(&gr, &lf );
}

void CDrawTextObj::SetLogFont(LOGFONT& lf)
{
	SetFontFace( lf.lfFaceName );
	
	CWnd* pWnd = CWnd::FromHandle( GetDesktopWindow() );	
	Gdiplus::REAL fsize = (Gdiplus::REAL)MulDiv(-lf.lfHeight, 72, GetDeviceCaps(pWnd->GetDC()->m_hDC, LOGPIXELSY) );

	SetFontSize( fsize  );

	std::string style;

	if (lf.lfItalic)
		style += "Italic;";
	if ( lf.lfWeight >= FW_BOLD)
		style += "Bold;";
	if ( lf.lfUnderline )
		style += "Underline;";
	if ( lf.lfStrikeOut )
		style += "Strikeout;";

	SetFontStyle(style);

}

//Вертикальное выравнивание текста
void CDrawTextObj::SetVerticalAlign(Gdiplus::StringAlignment align)
{
	fmt.SetLineAlignment( align );
}
Gdiplus::StringAlignment CDrawTextObj::GetVerticalAlign () const
{
	return fmt.GetLineAlignment();
}

//Горизонтальное выравнивание текста
void CDrawTextObj::SetHorizontalAlign(Gdiplus::StringAlignment align)
{
	fmt.SetAlignment( align );	
}

Gdiplus::StringAlignment CDrawTextObj::GetHorizontalAlign () const
{
	return fmt.GetAlignment();
}

//Задать размер шрифта
void CDrawTextObj::SetFontSize(Gdiplus::REAL newSize)
{
	INT	fs =  pFnt->GetStyle();		// Стиль шрифта
	Gdiplus::FontFamily fam;
	pFnt->GetFamily(&fam);

	WCHAR name[LF_FACESIZE];
	fam.GetFamilyName(name, LANG_NEUTRAL);

	delete pFnt;
	pFnt = new Gdiplus::Font ( name, newSize, Gdiplus::FontStyle(fs), Gdiplus::UnitPoint );
		
}

//Получить размер шрифта
Gdiplus::REAL CDrawTextObj::GetFontSize() const
{
	return pFnt->GetSize();
}

//Задать цвет и прозрачность шрифта
void CDrawTextObj::SetFontColor(COLORREF color, double transparency)
{
	ASSERT(transparency >= 0 && transparency <= 100.0);
	BYTE trans = BYTE( 255 * (100 - transparency)/100 );

	Gdiplus::Color clr(trans , GetRValue(color), GetGValue(color), GetBValue(color) );
	fontClr = clr;

	
}

//Получить цвет шрифта
COLORREF CDrawTextObj::GetFontColor() const
{
	return fontClr.ToCOLORREF();
}

//получить уровень прозрачности в процентах
double CDrawTextObj::GetFontTransparency() const
{
	return ( 255.0 - fontClr.GetAlpha()  ) * 100.0 / 255.0;	
}

void CDrawTextObj::SetFontFace(std::string fntFaceName)
{
  	Gdiplus::REAL			fontSz =  pFnt->GetSize();		// Шрифт размер 	
 	INT						style  =  pFnt->GetStyle();		// Шрифт стиль
	
	USES_CONVERSION;
	WCHAR* pFntName = A2W(fntFaceName.c_str());

	delete pFnt;
	pFnt = new Gdiplus::Font ( pFntName, fontSz, style, Gdiplus::UnitPoint );	
	
}

std::string CDrawTextObj::GetFontFace() const
{
	Gdiplus::FontFamily fam;
	pFnt->GetFamily(&fam);
	
	WCHAR name[LF_FACESIZE];
	fam.GetFamilyName(name, LANG_NEUTRAL);
	
	USES_CONVERSION;
	std::string sname = W2A(name);
	return sname; 
}

//Задать стили styles = "bold;italic;underline;strikeout"
//задаются строкой в любой последовательности разделители точка с запятой 
void CDrawTextObj::SetFontStyle(std::string styles)
{
	INT fs = Gdiplus::FontStyleRegular;
	
	boost::algorithm::to_lower(styles);

	if ( boost::algorithm::contains(styles, "bold"))
		fs |=  Gdiplus::FontStyleBold;

	if ( boost::algorithm::contains(styles, "italic"))
		fs |=  Gdiplus::FontStyleItalic;

	if ( boost::algorithm::contains(styles, "underline"))
		fs |=  Gdiplus::FontStyleUnderline;

	if ( boost::algorithm::contains(styles, "strikeout"))
		fs |=  Gdiplus::FontStyleStrikeout;

	SetFontStyle( Gdiplus::FontStyle( fs ) );
}


//Задать стили 
//fs = комбинации стилей  (Gdiplus::FontStyleBoldItalic |Gdiplus::FontStyleUnderline| Gdiplus::FontStyleStrikeout)
//Gdiplus::FontStyleRegular     
//Gdiplus::FontStyleBold        
//Gdiplus::FontStyleItalic      
//Gdiplus::FontStyleBoldItalic  
//Gdiplus::FontStyleUnderline   
//Gdiplus::FontStyleStrikeout
void CDrawTextObj::SetFontStyle(Gdiplus::FontStyle fs)
{
	Gdiplus::REAL		fontSz =  pFnt->GetSize();		// Шрифт размер 	
 	Gdiplus::FontFamily fam;
	pFnt->GetFamily(&fam);
	
	WCHAR name[LF_FACESIZE];
	fam.GetFamilyName(name, LANG_NEUTRAL);
	
	delete pFnt;
	pFnt = new Gdiplus::Font ( name, fontSz, fs, Gdiplus::UnitPoint );
	
}

//Получить текущие стили
Gdiplus::FontStyle CDrawTextObj::GetFontStyle() const
{
	return  Gdiplus::FontStyle(pFnt->GetStyle());
}

//Шрифт полужирный?
bool CDrawTextObj::IsBold() const
{
	return ( (pFnt->GetStyle() & Gdiplus::FontStyleBold) != 0);
}


//Установить снять полужирный стиль
void CDrawTextObj::SetBold(bool bSet/* = true*/)
{
	bool isChange =  XOR( bSet, IsBold() );
	if ( !isChange ) //	
		return;	

	INT curStyle = pFnt->GetStyle();
	
	if ( bSet )
		curStyle |=  Gdiplus::FontStyleBold;
	else
		curStyle &= ~Gdiplus::FontStyleBold;

	SetFontStyle( Gdiplus::FontStyle (curStyle) );	
}

//Шрифт наклонный?
bool CDrawTextObj::IsItalic() const
{
	return ( (pFnt->GetStyle() & Gdiplus::FontStyleItalic) != 0);
}

//Установить снять наклонный стиль
void CDrawTextObj::SetItalic(bool bSet/* = true*/)
{
	bool isChange =  XOR( bSet, IsBold() );
	
	if ( !isChange ) //	
		return;	

	INT curStyle = pFnt->GetStyle();
	
	if ( bSet )
		curStyle |=  Gdiplus::FontStyleItalic;
	else
		curStyle &= ~Gdiplus::FontStyleItalic;

	SetFontStyle( Gdiplus::FontStyle( curStyle) );
}

//Шрифт подчеркнутый?
bool CDrawTextObj::IsUnderline() const
{
	return ( (pFnt->GetStyle() & Gdiplus::FontStyleUnderline) != 0);
}
//Установить снять подчеркнутый стиль
void CDrawTextObj::SetUnderline(bool bSet/* = true*/)
{
	bool isChange =  XOR( bSet, IsBold() );
	if ( !isChange ) //	
		return;	

	INT curStyle = pFnt->GetStyle();
	
	if ( bSet )
		curStyle |=  Gdiplus::FontStyleUnderline;
	else
		curStyle &= ~Gdiplus::FontStyleUnderline;

	SetFontStyle( Gdiplus::FontStyle( curStyle) );	
}

//Шрифт зачеркнутый?
bool CDrawTextObj::IsStrikeout() const
{
	return ( (pFnt->GetStyle() & Gdiplus::FontStyleStrikeout ) != 0);
}

//Установить снять зачеркнутый стиль
void CDrawTextObj::SetStrikeout(bool bSet/* = true*/)
{
		bool isChange =  XOR( bSet, IsBold() );
		if ( !isChange ) //	
			return;	
	
		INT curStyle = pFnt->GetStyle();
		
		if ( bSet )
			curStyle |=  Gdiplus::FontStyleStrikeout;
		else
			curStyle &= ~Gdiplus::FontStyleStrikeout;
	
		SetFontStyle( Gdiplus::FontStyle( curStyle) );		
}

//Шрифт с тенью?
bool CDrawTextObj::IsShadowed() const
{
	return bShadowEffect;
}
	//Установить снять тень
void CDrawTextObj::SetShadow(bool bSet/* = true*/)
{
	bShadowEffect = bSet;
}



//отрисовка объекта
void CDrawTextObj::Draw(Gdiplus::Graphics& gr)
{
	CDrawObj::Draw(gr);

	Gdiplus::SolidBrush br( OnNeedColor() );
	
	std::string txt = OnNeedText();	
	
	std::replace(txt.begin(), txt.end(), '|', '\n');
	//подготовка области отрисовки текста
	OnCalcTextRect();

	USES_CONVERSION;
	WCHAR* pOut = A2W(txt.c_str());

	/*
	Gdiplus::PointF pt;
	m_pos.GetLocation(&pt);

	gr.MeasureString(pOut, -1, pFnt, pt, &rcText);
	*/

	gr.SetTextRenderingHint( Gdiplus::TextRenderingHintClearTypeGridFit);
	

	if ( bShadowEffect )
	{
		Gdiplus::SolidBrush br1( Gdiplus::Color(0xff, 0x80, 0x80, 0x80) );
		Gdiplus::RectF rc1 = rcText;
	
		rc1.Offset(1, 1);	
		gr.DrawString(pOut, -1, pFnt, rc1, &fmt, &br1);
	}

	if (angle == 0)
		gr.DrawString(pOut, -1, pFnt, rcText, &fmt, &br);
	else
		DrawString(gr, txt, rcText, angle, br);
}

// Метод для рисования под углом.
void CDrawTextObj::DrawString(Gdiplus::Graphics& g, std::string txt, Gdiplus::RectF rc,  Gdiplus::REAL ang, Gdiplus::Brush& brush)
{
	USES_CONVERSION;
	WCHAR* pOut = A2W(txt.c_str());

	Gdiplus::REAL x = rc.GetLeft();
	Gdiplus::REAL y = rc.GetBottom();

	// Устанавливаем начало координат в точку (x, y).
	g.TranslateTransform(x, y);
	// Поворачиваем систему кооординат.
	g.RotateTransform(ang);
	// Выводим надпись.
	g.DrawString(pOut, -1, pFnt, Gdiplus::RectF(0, 0, rc.Width, rc.Height), &fmt, &brush);
	// Поворачиваем систему кооординат обратно.
	g.RotateTransform(-ang);
	// Устанавливаем начало координат в первоначальную точку.
	g.TranslateTransform(-x, -y);
}

std::string CDrawTextObj::OnNeedText() const
{
	std::string txt = outTxt;
	StringDataSrc *pSDS = dynamic_cast<StringDataSrc *>(DataSrc::GetDataSource(outTxt));
	if (pSDS != NULL)
		txt = pSDS->GetString(); 

	return txt;
}

void CDrawTextObj::OnCalcTextRect()
{
	//если надо изменить положение текста  в наследниках 
	rcText = m_pos;
}

Gdiplus::Color CDrawTextObj::OnNeedColor() const
{
	return fontClr;
}

#ifdef _DEBUG
void CDrawTextObj::AssertValid()
{
	ASSERT(pFnt != NULL);
	ASSERT( m_pos.GetLeft() <= m_pos.GetRight() );
	ASSERT( m_pos.GetBottom() <= m_pos.GetTop() );
}
#endif

#pragma endregion

#pragma region CDrawDateTimeObj


CMapStringToString CDrawDateTimeObj::timeFormats;

void CDrawDateTimeObj::initTimeFormats()
{
	timeFormats["Время"]="%H:%M";
	timeFormats["Время (полный формат)"]="%H:%M:%S";
	timeFormats["Дата  (короткий формат)"]="%d.%m.%y";
	timeFormats["Дата  (полный формат)"]="%d %M %Y";
	timeFormats["Дата и время"]="%d %M %Y, %H:%M";
	timeFormats["Дата и время (полный формат)"]="%d %B %Y, %H:%M";
	timeFormats["Дата (день месяца)"]="%d";
	timeFormats["Дата (месяц)"]="%B";
	timeFormats["Дата (год)"]="%Y";
	timeFormats["Дата (день недели)"]="%A";
}

CDrawDateTimeObj::CDrawDateTimeObj() :
	CDrawTextObj ( CRect(0,0,0,0) ) ,
	timeFormat("%d %B %Y, %A %H:%M")
{
	initTimeFormats();
}

CDrawDateTimeObj::CDrawDateTimeObj( const CRect& position) :
	CDrawTextObj (position ), 
	timeFormat("%d %B %Y, %A %H:%M")
{
	initTimeFormats();
}

CDrawDateTimeObj::~CDrawDateTimeObj()
{

}

CMapStringToString& CDrawDateTimeObj::EnumTimeFormats()
{
	return timeFormats;
}

void CDrawDateTimeObj::SetTimeFormat(std::string fmt)
{
	timeFormat = fmt;
}

std::string CDrawDateTimeObj::GetTimeFormat() const
{
	return timeFormat;
}

//создание элемента  из XML узла
bool CDrawDateTimeObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawTextObj::CreateFromXMLNode(pNode, parent) )
		return false;

	std::string sVal;
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( appXML.GetNodeAttributeByName(pNode, "SRC", sVal) )
		timeFormat = sVal;

	return true;
}

//создание элемента  из XML узла
CDrawDateTimeObj* CDrawDateTimeObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawDateTimeObj> obj ( new CDrawDateTimeObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);		
		return NULL;
	}
	
	return obj.release();
}

std::string CDrawDateTimeObj::OnNeedHintText() const 
{ 
	std::string txt = "<b>Current time:</b>";
	txt += CTime::GetCurrentTime().Format("<i><p>%d %B %Y,</p><p>%A %H:%M</p></i>");
	return txt;
}

std::string CDrawDateTimeObj::OnNeedText() const
{
	return (LPCTSTR)CTime::GetCurrentTime().Format(timeFormat.c_str());
}


#pragma endregion

#pragma region CDrawDateTimeObj

CDrawTimerObj::CDrawTimerObj() :
CDrawTextObj ( CRect(0,0,0,0) ) ,
	pTimer(NULL), 
	fontClrOwerflow(128, 0, 0)
{

}

CDrawTimerObj::CDrawTimerObj( const CRect& position) :
CDrawTextObj (position )
{

}

CDrawTimerObj::~CDrawTimerObj()
{

}

bool CDrawTimerObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawTextObj::CreateFromXMLNode(pNode, parent) )
		return false;

	return true;
}

CDrawTimerObj* CDrawTimerObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawTimerObj> obj ( new CDrawTimerObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) ) 
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	std::string sAtr;
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
		
	if (!appXML.GetNodeAttributeByName(pNode, "SRC", sAtr) )
	{
		str << "Timer id not set for item" << std::ends;		
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str() );
		return false;
	}

	obj->pTimer = BaseTimer::GetTimer( sAtr );
	if ( obj->pTimer == NULL )
		return NULL;

	return obj.release();
}

std::string CDrawTimerObj::OnNeedHintText() const 
{ 
	if ( pTimer->IsStarted() )
	{
		std::string out =  m_strHint;
		boost::replace_first(out, "%TIME%", pTimer->ToString());		
		return  out;
	}
	else
		return "";
}

Gdiplus::Color CDrawTimerObj::OnNeedColor() const
{
	return pTimer->IsOwerflowed() ? fontClrOwerflow : fontClr;
}

std::string CDrawTimerObj::OnNeedText() const
{
	if ( pTimer->IsStarted() )
		return pTimer->ToString();
	else
		return "";
}

#pragma endregion

#pragma region CDrawCurrentUserObj

CDrawCurrentUserObj::CDrawCurrentUserObj() :
CDrawTextObj ( CRect(0,0,0,0) )
{

}

CDrawCurrentUserObj::CDrawCurrentUserObj( const CRect& position) :
CDrawTextObj (position )
{

}

CDrawCurrentUserObj::~CDrawCurrentUserObj()
{

}

//создание элемента  из XML узла
bool CDrawCurrentUserObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawTextObj::CreateFromXMLNode(pNode, parent) )
		return false;

	return true;
}

CDrawCurrentUserObj* CDrawCurrentUserObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawCurrentUserObj> obj ( new CDrawCurrentUserObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) ) 
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}

	return obj.release();
}


void CDrawCurrentUserObj::OnCalcTextRect()
{
	//если надо изменить положение текста  в наследниках 
	rcText;// = m_pos;
}


bool CDrawCurrentUserObj::OnLButtonClick(UINT nFlags)
{
	return true;
}

bool CDrawCurrentUserObj::OnLButtonDoubleClick(UINT nFlags)
{
	AppUsersHelper::Instance().UserLogin();
	return true;
}


std::string CDrawCurrentUserObj::OnNeedText() const
{
	LPNTCC_APP_USER lpUser = AppUsersHelper::Instance().GetCurrentUser();
	if (lpUser != NULL)
		return lpUser->screenName.c_str();
	else
		return "Вход не выполнен";
}
//отрисовка объекта
void CDrawCurrentUserObj::Draw(Gdiplus::Graphics& gr)
{
	Gdiplus::Image* userIcon = AppUsersHelper::Instance().GetCurrentUserImage(48);
	
	Gdiplus::REAL cx = (Gdiplus::REAL)userIcon->GetWidth();
	Gdiplus::REAL cy = (Gdiplus::REAL)userIcon->GetHeight();

	Gdiplus::RectF rcIcon(m_pos.X, 0, cx, cy ); 
	
	switch (fmt.GetLineAlignment() )
	{
		case Gdiplus::StringAlignmentNear:
			rcIcon.Y = m_pos.Y;
			break;
		
		case Gdiplus::StringAlignmentFar:
			rcIcon.Y = m_pos.Y + m_pos.Height  - cy;
			break;

		case Gdiplus::StringAlignmentCenter:
			rcIcon.Y = m_pos.Y + m_pos.Height / 2  - cy / 2;
			break;
	}
	
	gr.DrawImage(userIcon, rcIcon);

	rcText = m_pos;
	rcText.X += cx;

	CDrawTextObj::Draw(gr);
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
#pragma region CDrawProgressAnalogDSObj

CDrawProgressAnalogDSObj::CDrawProgressAnalogDSObj()
{
}

CDrawProgressAnalogDSObj::CDrawProgressAnalogDSObj(const CRect& position)
{

}

CDrawProgressAnalogDSObj::~CDrawProgressAnalogDSObj()
{
}

CDrawProgressAnalogDSObj* CDrawProgressAnalogDSObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{	
	auto_ptr<CDrawProgressAnalogDSObj> obj ( new CDrawProgressAnalogDSObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) ) 
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}
	
	return obj.release();
}

//создание элемента  из XML узла
bool CDrawProgressAnalogDSObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if (!CDrawAnalogDSObj::CreateFromXMLNode(pNode, parent))
		return false;

	std::string sAtr;	
	const AppXMLHelper& appXML = AppXMLHelper::Instance();	

	//получаем диапазон аналоговой величины
	m_pADS->GetRange(valRange.first, valRange.second);

	//переопределяем диапазон аналоговой величины если задан явно
	if (appXML.GetNodeAttributeByName(pNode, "MINV", sAtr) )
		valRange.first = atof(sAtr.c_str());

	//переопределяем диапазон аналоговой величины если задан явно
	if (appXML.GetNodeAttributeByName(pNode, "MAXV", sAtr) )
		valRange.second = atof(sAtr.c_str());


	//Направление 
	//T2B = 0 - сверху вниз
	//B2T = 1 - снизу вверх
	//L2R = 2 - слева направо
	//R2L = 3 - справа налево

	//по умолчанию
	progressDir = DEFAULT;

	//если переопределено
	if (appXML.GetNodeAttributeByName(pNode, "DIRECTION", sAtr) )
	{
		boost::algorithm::to_upper(sAtr);
		if (sAtr == "T2B")
			progressDir = CDrawProgressAnalogDSObj::TOP2BOTTOM;
		else if (sAtr == "B2T")
			progressDir = CDrawProgressAnalogDSObj::BOTTOM2TOP;
		else if (sAtr == "L2R")
			progressDir = CDrawProgressAnalogDSObj::LEFT2RIGHT;
		else if (sAtr == "R2L")
			progressDir = CDrawProgressAnalogDSObj::RIGHT2LEFT;
		else 
			progressDir = CDrawProgressAnalogDSObj::DEFAULT;
	}

	//если переопределено
	showText = TRUE;

	if (appXML.GetNodeAttributeByName(pNode, "SHOWTEXT", sAtr) )
	{
		showText = (atoi(sAtr.c_str()) > 0);
	}

	CComPtr<IXMLDOMNode> pBarNode;
	HRESULT hr = pNode->selectSingleNode(CComBSTR("BAR"), &pBarNode);

	appXML.GetGdiPlusColor(pBarNode, barColor, 100.0);	

	//отображать текст
	barMargins.push_back(1.0f);

	if (appXML.GetNodeAttributeByName(pBarNode, "MARGINS", sAtr) )
	{
		ExtractData<Gdiplus::REAL>(sAtr, " ,;", barMargins);		
	}

	CComPtr<IXMLDOMNodeList> pBarColorizeList;
	hr = pBarNode->selectNodes(CComBSTR("COLORIZE"), &pBarColorizeList);
	if ( SUCCEEDED(hr) && pBarColorizeList != NULL)
	{
		long range_count;
		pBarColorizeList->get_length(&range_count);
		pBarColorizeList->reset();

		for ( long с = 0; с < range_count; с++  )
		{
			CComPtr<IXMLDOMNode>	 pColorItemNode;
			pBarColorizeList->get_item(с, &pColorItemNode);
			
			std::string sVal;

			double from = 0;
			if ( !appXML.GetNodeAttributeByName(pColorItemNode, "FROM", sVal) )
				continue;

			from = atof(sVal.c_str());
			double to = 0;
			if ( !appXML.GetNodeAttributeByName(pColorItemNode, "TO", sVal) )
				continue;

			to = atof(sVal.c_str());
			Gdiplus::Color clr;
			appXML.GetGdiPlusColor(pColorItemNode, clr, 100.0);

			colorize.insert( std::make_pair( std::make_pair(from, to), clr));
		}
	}

	return true;
}

Gdiplus::RectF CDrawProgressAnalogDSObj::CalcBarRect() const
{
	Gdiplus::RectF barRect = m_pos;
	if (barMargins.size() == 1)
		barRect.Inflate( Gdiplus::PointF(barMargins[0], barMargins[0]) );
	else if (barMargins.size() == 2)
		barRect.Inflate( barMargins[0], barMargins[1]);

	bool vertical = (progressDir == TOP2BOTTOM || progressDir == BOTTOM2TOP);
	double val = m_pADS->GetValue();

	double progress = (val - valRange.first) / (valRange.second - valRange.first);

	Gdiplus::REAL curBarRng = Gdiplus::REAL( vertical ? ( barRect.Height*progress ) :  ( barRect.Width*progress ) ); 
	
	switch (progressDir)
	{
		case CDrawProgressAnalogDSObj::TOP2BOTTOM:
			barRect.Height  = curBarRng;
			break;

		case CDrawProgressAnalogDSObj::BOTTOM2TOP:
			barRect.Y = barRect.Y + barRect.Height - curBarRng;
			barRect.Height  = curBarRng;
			break;


		case CDrawProgressAnalogDSObj::RIGHT2LEFT:
			barRect.X = barRect.X - curBarRng;
			barRect.Width  = curBarRng;
			break;

		case CDrawProgressAnalogDSObj::LEFT2RIGHT:			
		default:
			barRect.Width  = curBarRng;
			break;
	}
	return barRect;
}

Gdiplus::Color CDrawProgressAnalogDSObj::getBrushColor() const
{
	Gdiplus::Color color;
	
	if (colorize.empty())
		color = barColor;
	else 
	{
		double val = m_pADS->GetValue();
		std::map<VALUE_RANGE, Gdiplus::Color>::const_iterator it = std::find_if (colorize.begin(), colorize.end(), double_in_range(val));

		if (it != colorize.end() )
			color = (*it).second;
		else
			color = barColor;
	}
	
	return color;
}

void CDrawProgressAnalogDSObj::DrawBar( Gdiplus::Graphics& gr )
{
	Gdiplus::RectF barRect = CalcBarRect();
	Gdiplus::SolidBrush br(getBrushColor());
	
	//закрашиваем 
	gr.FillRectangle(&br, barRect);
}

//отрисовка объекта
void CDrawProgressAnalogDSObj::Draw(Gdiplus::Graphics& gr)
{
	CDrawObj::Draw(gr);

	DrawBar(gr);

	if (!showText)
		return;

	Gdiplus::SolidBrush br( OnNeedColor() );

	std::string txt = OnNeedText();	

	//подготовка области отрисовки текста
	OnCalcTextRect();

	USES_CONVERSION;
	WCHAR* pOut = A2W(txt.c_str());

	if ( bShadowEffect )
	{
		Gdiplus::SolidBrush br1( Gdiplus::Color(0xff, 0x80, 0x80, 0x80) );
		Gdiplus::RectF rc1 = rcText;

		rc1.Offset(1, 1);	
		gr.DrawString(pOut, -1, pFnt, rc1, &fmt, &br1);
	}

	if (angle == 0)
		gr.DrawString(pOut, -1, pFnt, rcText, &fmt, &br);
	else
		DrawString(gr, txt, rcText, angle, br);

}



#pragma endregion


//////////////////////////////////////////////////////////////////////////
//  [2/1/2016 Johnny A. Matveichik]
//////////////////////////////////////////////////////////////////////////
#pragma region CDrawAnalogDSObj

CDrawAnalogDSObj::CDrawAnalogDSObj() : 
CDrawTextObj (CRect(0,0,0,0) ), m_pADS(NULL)
{

}

CDrawAnalogDSObj::CDrawAnalogDSObj(const CRect& position) : 
CDrawTextObj (position ), m_pADS(NULL)
{

}

CDrawAnalogDSObj::~CDrawAnalogDSObj()
{

}

CDrawAnalogDSObj* CDrawAnalogDSObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	
	auto_ptr<CDrawAnalogDSObj> obj ( new CDrawAnalogDSObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) ) 
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}
	
	return obj.release();
}

//создание элемента  из XML узла
bool CDrawAnalogDSObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if (!CDrawTextObj::CreateFromXMLNode(pNode, parent))
		return false;

	std::string sVal;
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "SRC", sVal) )
	{
		boost::format msg("Не задан аналоговый источник данных (атрибут SRC) для объекта %s.");
		msg % m_strID.c_str();

		AppErrorsHelper::Instance().SetLastError(DOCE_PARAMETER_NOT_DEFINED, msg.str());
		return false;
	}

	sVal = m_pParentLayer->GetParent()->DSPrefix() + sVal;

	if ( !SetDataSource(sVal ))
	{
		boost::format msg("Не найден аналоговый источник данных (атрибут SRC= %s ) для объекта %s.");
		msg %  sVal.c_str();
		msg % m_strID.c_str();
		AppErrorsHelper::Instance().SetLastError(DOCE_INVALID_DATA_SRC, msg.str() );

		return false;
	}

	bUnits = true;
	if ( appXML.GetNodeAttributeByName(pNode, "UNITS", sVal) )
		bUnits = atoi(sVal.c_str()) > 0;

	prefix = "";
	if (appXML.GetNodeAttributeByName(pNode, "PREFIX", sVal))
		prefix = sVal.c_str();

	return true;
}

std::string CDrawAnalogDSObj::OnNeedHintText() const 
{ 
	boost::format fmt("<b><p>Inner variable:</p></b><p>%s</p><p><i>%s</i></p>");
	fmt % m_pADS->Name().c_str() % m_pADS->Description().c_str();

	std::string txt = fmt.str();

	const IComposite* pComp = dynamic_cast<const IComposite*>(m_pADS);
	if ( pComp != NULL )
	{
		txt += "<b><p>Composed:</p></b>";
		for (int i = 0 ; i < pComp->Size(); i++)
		{
			const AnalogDataSrc* pads = dynamic_cast<const AnalogDataSrc*>( pComp->GetSubDataSrc(i) );
			
			if (pads != NULL)
			{
				boost::format fmtsub("<p>%s - <i>%s</i></p>");
				fmtsub % pads->Name().c_str() %  pads->Description().c_str();

				txt +=  fmtsub.str();
			}
		}
	}

	txt += CDrawObj::OnNeedHintText();		
	return txt; 
};

bool CDrawAnalogDSObj::SetDataSource(std::string id)
{
	m_pADS = dynamic_cast<AnalogDataSrc*>(DataSrc::GetDataSource(id));
	if ( m_pADS == NULL ) 
		return false;

	return true; 
}


std::string CDrawAnalogDSObj::GetDataSource() const
{
	if ( m_pADS != NULL ) 
		return m_pADS->Name();

	return "";
}

void CDrawAnalogDSObj::ShowUnits(bool bShow/* = true*/)
{
	bUnits = bShow;
}

bool CDrawAnalogDSObj::IsUnitsShow() const
{
	return  bUnits;
}

void CDrawAnalogDSObj::SetPrefix(std::string pref)
{
	prefix = pref;
}

std::string CDrawAnalogDSObj::GetPrefix() const
{
	return prefix;
}


std::string CDrawAnalogDSObj::OnNeedText() const
{
	std::string out;

	if ( m_pADS != NULL )
	{
		boost::format fmt(m_pADS->OutFormat());
		fmt % m_pADS->GetValue();		
		out = prefix + fmt.str();

		if ( bUnits )
		{
			std::string units = m_pADS->Units().c_str();
			out += " ";
			out += units.c_str();
		}
	}
	else
		out = _T("- - -");

	return out;
	
}

//отрисовка объекта
void CDrawAnalogDSObj::Draw(Gdiplus::Graphics& gr)
{
	CDrawTextObj::Draw(gr);
	/*
	if ( bUnits )
	{
		std::string units = m_pADS->Units().c_str();
		
		if (units.GetLength() == 0 )
			return; 

		Gdiplus::SolidBrush br(fontClr);
		Gdiplus::REAL fSize = pFnt->GetHeight(&gr) - 2.0f;
		Gdiplus::FontFamily  family;
		pFnt->GetFamily(&family);
		Gdiplus::Font* fontUnits = new Gdiplus::Font(&family, fSize, pFnt->GetStyle(), pFnt->GetUnit());

		USES_CONVERSION;
		WCHAR* pOut = A2W(units);
		Gdiplus::RectF rc1 = m_pos;
		Gdiplus::PointF pt(rc1.GetBottom(), rc1.GetLeft());

		gr.MeasureString(pOut, units.GetLength(), fontUnits, pt, &rc1);
		
		
		if ( bShadowEffect )
		{
			Gdiplus::SolidBrush br1( Gdiplus::Color(0xff, 0x80, 0x80, 0x80) );
			Gdiplus::RectF rc1 = m_pos;

			rc1.Offset(1, 1);	
			gr.DrawString(pOut, -1, pFnt, rc1, &fmt, &br1);
		}

		gr.DrawString(pOut, -1, pFnt, m_pos, &fmt, &br);
	}
	*/
}

//получить источник данных связанный с объектом
DataSrc* CDrawAnalogDSObj::GetAssignedDataSource() const
{
	return m_pADS;
}


#pragma endregion
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#pragma region CDrawDiscreteDSAsTextObj

CDrawDiscreteDSAsTextObj::CDrawDiscreteDSAsTextObj() : 
CDrawTextObj (CRect(0,0,0,0) ), m_pDDS(NULL)
{

}

CDrawDiscreteDSAsTextObj::CDrawDiscreteDSAsTextObj(const CRect& position) : 
CDrawTextObj (position ), m_pDDS(NULL)
{

}

CDrawDiscreteDSAsTextObj::~CDrawDiscreteDSAsTextObj()
{

}

bool CDrawDiscreteDSAsTextObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if (!CDrawTextObj::CreateFromXMLNode(pNode, parent))
		return false;

	std::string sVal;	
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "SRC", sVal) )
	{
		boost::format msg("Не задан дискретный источник данных (атрибут SRC) для объекта %s.");
		msg % m_strID.c_str();
		AppErrorsHelper::Instance().SetLastError(DOCE_PARAMETER_NOT_DEFINED, msg.str() );
		return false;
	}

	sVal = parent.GetParent()->DSPrefix() + sVal;

	if ( !SetDataSource( sVal ) )
	{
		boost::format msg("Не найден дискретный источник данных (атрибут SRC = %s ) для объекта %s.");
		msg % sVal.c_str() % m_strID.c_str();
		AppErrorsHelper::Instance().SetLastError(DOCE_INVALID_DATA_SRC, msg.str());
		return false;
	}

	m_pParentLayer = &parent;

	return true;
}

CDrawDiscreteDSAsTextObj* CDrawDiscreteDSAsTextObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawDiscreteDSAsTextObj> obj ( new CDrawDiscreteDSAsTextObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) ) 
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}
	
	return obj.release();
}

bool CDrawDiscreteDSAsTextObj::SetDataSource(std::string id)
{
	m_pDDS = dynamic_cast<DiscreteDataSrc*>(DataSrc::GetDataSource(id));
	if ( m_pDDS == NULL ) 
		return false;

	return true; 
}

std::string CDrawDiscreteDSAsTextObj::OnNeedText() const
{
	std::string out;

	if ( m_pDDS != NULL )
		out = m_pDDS->ToString().c_str();
	else
		out = _T("- - -");

	return out;
}

std::string CDrawDiscreteDSAsTextObj::OnNeedHintText() const 
{ 
	std::string txt;
	boost::format fmt("<b><p>Inner variable:</p></b><p>%s</p><p><i>%s</i></p><b><p>Current state:</p></b><p><img src=\"%s.bmp\"><i>%s</i></p>");
	fmt % m_pDDS->Name().c_str();
	fmt % m_pDDS->Description().c_str();
	fmt % (m_pDDS->IsEnabled() ? "on": "off");
	fmt % m_pDDS->ToString().c_str();
	
	const IComposite* pComp = dynamic_cast<const IComposite*>(m_pDDS);
	if ( pComp != NULL )
	{
		txt += "<b><p>Composed:</p></b>";
		for (int i = 0 ; i < pComp->Size(); i++)
		{
			const DiscreteDataSrc* pdds = dynamic_cast<const DiscreteDataSrc*>( pComp->GetSubDataSrc(i) );

			if (pdds != NULL)
			{
				boost::format fmtsub("<p><img src=\"%s.bmp\"> %s - <i>%s</i></p>");
				fmtsub % (pdds->IsEnabled() ? "on": "off") % pdds->Name().c_str() % pdds->Description().c_str();
				txt +=  fmtsub.str() ;
			}
		}
	}

	txt += CDrawObj::OnNeedHintText();		
	return txt; 
}

//получить источник данных связанный с объектом
DataSrc* CDrawDiscreteDSAsTextObj::GetAssignedDataSource() const
{
	return m_pDDS;
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////


#pragma region CDrawImageObj

//////////////////////////////////////////////////////////////////////////
CDrawImageObj::CDrawImageObj() :
	CDrawObj(CRect(0,0,0,0))
{
}

CDrawImageObj::CDrawImageObj(const CRect& position) :
CDrawObj(position)
{
}

CDrawImageObj::~CDrawImageObj()
{
}

//создание элемента  из XML узла
bool CDrawImageObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawObj::CreateFromXMLNode(pNode, parent) )
		return false;

	std::string sVal;
	std::string sMsg;
	
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "SRC", sVal) )
	{
		boost::format fmt("Не задан путь к изображению (атрибут SRC) для объекта  %s.");
		fmt % m_strID.c_str();
		AppErrorsHelper::Instance().SetLastError(DOCE_PARAMETER_NOT_DEFINED, fmt.str() );
		return false;
	}
	
	AppImagesHelper& appImages = AppImagesHelper::Instance();
	srcImg = appImages.GetImage(sVal);

	if ( srcImg == NULL )
	{
		boost::format fmt("Ошибка задания пути к изображению (атрибут SRC = %s ) объекта %s.");
		fmt % sVal.c_str() % m_strID.c_str();

		AppErrorsHelper::Instance().SetLastError(DOCE_INVALID_PATH, fmt.str() );
		return false;
	}

	return true;
}

//создание элемента  из XML узла
CDrawImageObj* CDrawImageObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	
	auto_ptr<CDrawImageObj> obj ( new CDrawImageObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) ) 
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}
	
	return obj.release();
}

//отрисовка объекта
void CDrawImageObj::Draw(Gdiplus::Graphics& gr)
{
	CDrawObj::Draw(gr);
	
	Gdiplus::ColorMatrix ClrMatrix =         
	{ 
            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	//ClrMatrix.m[3][3] = 20.0 / 100.0;

	Gdiplus::ImageAttributes ImgAttr;

	ImgAttr.SetColorMatrix(&ClrMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

	Gdiplus::Image* pImg = OnNeedImage();
	gr.DrawImage(pImg, 
		Gdiplus::RectF(m_pos.X, m_pos.Y, (Gdiplus::REAL)srcImg->GetWidth(), (Gdiplus::REAL)srcImg->GetHeight() ), 0, 0,  (Gdiplus::REAL)srcImg->GetWidth(), (Gdiplus::REAL)srcImg->GetHeight(), Gdiplus::UnitPixel, &ImgAttr);

	//gr.DrawImage(srcImg, Gdiplus::Point(m_pos.X, m_pos.Y));
}

#pragma endregion 

#pragma region CDrawDiscreteDSObj 


CDrawDiscreteDSObj::CDrawDiscreteDSObj() :
CDrawImageObj(CRect(0,0,0,0) ) , m_pDS(NULL)
{

}

CDrawDiscreteDSObj::CDrawDiscreteDSObj(const CRect& position) :
CDrawImageObj( position ) , m_pDS(NULL)
{

}
CDrawDiscreteDSObj::~CDrawDiscreteDSObj()
{
}

//создание элемента  из XML узла
bool CDrawDiscreteDSObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawImageObj::CreateFromXMLNode(pNode, parent) )
		return false;

	Gdiplus::REAL frameWidth = m_pos.Width;
	Gdiplus::Bitmap* bmp = srcImg;

	if ( !imgStrip.Create(bmp, m_pos.Width) || imgStrip.GetSize() == 0)
	{
		AppErrorsHelper::Instance().SetLastError(DOCE_INVALID_IMAGE_STRIP, "Ошибка создания полосы изображений для объекта");
		return false;
	}
	
	std::string sVal;
	
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "DSID", sVal) )
	{
		boost::format msg("Не задан дискретный источник данных (атрибут DSID) для объекта %s.");
		msg % m_strID.c_str();

		AppErrorsHelper::Instance().SetLastError(DOCE_PARAMETER_NOT_DEFINED, msg.str());
		return false;
	}

	sVal = m_pParentLayer->GetParent()->DSPrefix() + sVal;	

	if ( !SetDataSource(sVal ) )
	{
		boost::format msg("Не найден дискретный источник данных (атрибут DSID = %s) для объекта %s.");
		msg % sVal.c_str() % m_strID.c_str();

		AppErrorsHelper::Instance().SetLastError(DOCE_INVALID_DATA_SRC, msg.str());
		return false;
	}

	return true;
}

CDrawDiscreteDSObj* CDrawDiscreteDSObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawDiscreteDSObj> obj ( new CDrawDiscreteDSObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) ) 
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}
	
	return obj.release();
}

std::string CDrawDiscreteDSObj::OnNeedHintText() const 
{ 
	std::string txt;
	boost::format msg("<b><p>Inner variable:</p></b><p>%s</p><p><i>%s</i></p><b><p>Current state:</p></b><p><img src=\"%s.bmp\"><i>%s</i></p>");
	
	msg % m_pDS->Name().c_str();
	msg % m_pDS->Description().c_str();
	msg % (m_pDS->IsEnabled() ? "on": "off");
	msg % m_pDS->ToString().c_str();

	txt = msg.str();

	const IComposite* pComp = dynamic_cast<const IComposite*>(m_pDS);
	if ( pComp != NULL )
	{
		txt += "<b><p>Composed:</p></b>";
		for (int i = 0 ; i < pComp->Size(); i++)
		{
			const DiscreteDataSrc* pdds = dynamic_cast<const DiscreteDataSrc*>( pComp->GetSubDataSrc(i) );
			if (pdds != NULL) 
			{
				boost::format msgsub("<p><img src=\"%s.bmp\"> %s - <i>%s</i></p>");
				msgsub % (pdds->IsEnabled() ? "on": "off") %  pdds->Name().c_str() % pdds->Description().c_str();
				txt +=  msgsub.str();
			}
		}
	}

	txt += CDrawObj::OnNeedHintText();		
	return txt; 
}

bool CDrawDiscreteDSObj::SetDataSource(std::string id)
{
	m_pDS = dynamic_cast<DiscreteDataSrc*>(DataSrc::GetDataSource(id));
	if ( m_pDS == NULL ) 
		return false;

	return true;
}

Gdiplus::Image* CDrawDiscreteDSObj::OnNeedImage() const
{
	UINT enabl = 0;
	UINT base  = 0;
	
	if ( dynamic_cast<DiscreteOutputDataSrc*>(m_pDS) )
	{
		enabl  = m_pDS->IsEnabled()  ? 1 : 0;
		base   = UserHasPermission() ? 0 : 2;
	}
	if ( dynamic_cast<ControledDiscreteOutputDataSrc*>(m_pDS) )
	{
		enabl  = m_pDS->IsEnabled()  ? 1 : 0;
		base   = UserHasPermission() && (dynamic_cast<ControledDiscreteOutputDataSrc*>(m_pDS))->CanChangeStateTo( !m_pDS->IsEnabled() ) ? 0 : 2;
	}
	else 
	{
		enabl  = m_pDS->IsEnabled()  ? 1 : 0;
		base   = 0;
	}

	

	UINT ind = base + enabl;

	return imgStrip[ind];
}

//получить источник данных связанный с объектом
DataSrc* CDrawDiscreteDSObj::GetAssignedDataSource() const
{
	return m_pDS;
}

bool CDrawDiscreteDSObj::OnLButtonDoubleClick(UINT nFlags)
{
	DiscreteOutputDataSrc* pOutDS = dynamic_cast<DiscreteOutputDataSrc*>(m_pDS);
	
	if ( !UserHasPermission() )
		return false;

	if ( pOutDS != NULL )
	{
		pOutDS->ToggleState();
		return true;
	}

	return false;
}
	
#pragma endregion
//////////////////////////////////////////////////////////////////////////
CDrawSystemBtnObj::CDrawSystemBtnObj() :
CDrawImageObj(CRect(0,0,0,0) )
{

}

CDrawSystemBtnObj::CDrawSystemBtnObj(const CRect& position) :
CDrawImageObj( position )
{

}
CDrawSystemBtnObj::~CDrawSystemBtnObj()
{
}

bool CDrawSystemBtnObj::ParseAction(std::string act)
{
	boost::algorithm::to_upper(act);

	if (act == "EXIT")
	{
		actionType = 0;
		return true;
	}

	if (act == "FOLDER")
	{
		actionType = 1;
		return true;
	}

	if (act == "EXECUTE") 
	{
		actionType = 2;
		return true;
	}

	return false;
}

//создание элемента  из XML узла
bool CDrawSystemBtnObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawImageObj::CreateFromXMLNode(pNode, parent) )
		return false;

	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	
	std::string sAtr;

	if (!appXML.GetNodeAttributeByName(pNode, "ACTION", sAtr) || !ParseAction(sAtr))
		return false;

	if ( appXML.GetNodeAttributeByName(pNode, "LOGGERS", sAtr) )
		actionparams = sAtr;

	if ( appXML.GetNodeAttributeByName(pNode, "PARAMS", sAtr) )
		actionparams = sAtr;


	return true;
}

CDrawSystemBtnObj* CDrawSystemBtnObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawSystemBtnObj> obj ( new CDrawSystemBtnObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) ) 
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}

	return obj.release();
}


bool CDrawSystemBtnObj::GetItemIdListFromPath (LPCSTR lpszPath, LPITEMIDLIST *lpItemIdList)
{
	LPSHELLFOLDER pShellFolder = NULL;
	HRESULT         hr;
	ULONG         chUsed;

	// Get desktop IShellFolder interface
	if (SHGetDesktopFolder (&pShellFolder) != NOERROR)
		return false;     // failed

	OLECHAR       olePath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszPath, -1,
		olePath, MAX_PATH);

	// convert the path to an ITEMIDLIST
	hr = pShellFolder->ParseDisplayName (
		NULL,           // owner window
		NULL,           // reserved (must be NULL)
		olePath,       // folder name
		&chUsed,    // number of chars parsed
		lpItemIdList,   // ITEMIDLIST
		NULL            // attributes (can be NULL)
		);

	if (FAILED(hr))
	{
		pShellFolder->Release();
		*lpItemIdList = NULL;
		return false;
	}

	pShellFolder->Release();
	return true;
} // GetItemIdListFromPath

void CDrawSystemBtnObj::doAction()
{
	switch (actionType)
	{
		//exit application action
		case 0:
		{
			CString message;
			message.Format("Are you sure you want to exit the application?");
			int res = MessageBox(::AfxGetMainWnd()->m_hWnd, (LPCTSTR)message, "Info", MB_ICONQUESTION|MB_YESNO);

			if ( res == IDYES )
				::AfxGetMainWnd()->PostMessage(WM_CLOSE);					
		}
		break;
		
		///open logger folder 
		case 1:
			{
				std::vector<std::string> parts;
				std::vector< std::string > paths;
				std::string  logdir;

				boost::split(parts, actionparams,  std::bind2nd(std::equal_to<char>(), ';'));

				for (auto it = parts.begin(); it != parts.end(); ++it)
				{
					const DataLogger* pdl = DataLogger::GetDataLogger( (*it) );

					if ( pdl == NULL )
						continue;

					paths.push_back( pdl->GetOutFilePath() );
					logdir = (*paths.rbegin());
				}

				if ( paths.empty())
					return ;

				//Directory to open
				boost::filesystem::path p(paths[0]);
				std::string folderPath =  (p.parent_path().string());

				ITEMIDLIST* dir = ILCreateFromPath(folderPath.c_str());
				LPITEMIDLIST* selection = new LPITEMIDLIST[ (int)paths.size() ];

				int curItem = 0;
				for (std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it)
				{
					//Items in directory to select
					GetItemIdListFromPath ( (*it).c_str(), &selection[curItem++]);
				}

				int count = sizeof(selection) / sizeof(ITEMIDLIST);

				//Perform selection
				SHOpenFolderAndSelectItems(dir, (UINT)paths.size(), const_cast<LPCITEMIDLIST *>(selection), 0);

				//Free resources
				ILFree(dir);
				for (int i = 0; i < count; i++ )
					ILFree( const_cast<LPITEMIDLIST>(selection[i]) );

				delete [] selection;
			}
			break;

		///execute other application
		case 2:
			{
				STARTUPINFO cif;
				ZeroMemory(&cif,sizeof(STARTUPINFO));
				PROCESS_INFORMATION pi;

				std::vector< std::string > parts;
				std::string  logdir;

				boost::split(parts, actionparams,  std::bind2nd(std::equal_to<char>(), ';'));
				
				if (parts.empty())
					return;

				std::string exepath = NTCCProject::Instance().GetPath(NTCCProject::PATH_BIN);
				exepath += parts[0];

				std::string cmdline;
				
				for(int i = 1; i < parts.size(); i++) {
					cmdline += " ";
					cmdline += parts[i];
				}

				char szCmd[MAX_PATH];
				strcpy_s(szCmd, MAX_PATH, cmdline.c_str());

				if (!CreateProcess(exepath.c_str(), szCmd, NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi))
				{
					CString message;
					message.Format("Can't execute programm {%s} !", parts[0].c_str());
					MessageBox(::AfxGetMainWnd()->m_hWnd, (LPCTSTR)message, "Error", MB_ICONEXCLAMATION|MB_OK);
				}
				
			}
			break;

		default:
			break;

	}
}

bool CDrawSystemBtnObj::OnLButtonDoubleClick(UINT nFlags)
{	
	doAction();
	return false;
}
//////////////////////////////////////////////////////////////////////////


#pragma region CDrawFilm

CDrawFilmObj::CDrawFilmObj() 
: CDrawDiscreteDSObj(CRect(0,0,0,0)), curFrame(-1)
{

}

CDrawFilmObj::CDrawFilmObj(const CRect& position) :
CDrawDiscreteDSObj(position ), curFrame(-1)
{

}

CDrawFilmObj::~CDrawFilmObj()
{
}

bool CDrawFilmObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	return CDrawDiscreteDSObj::CreateFromXMLNode(pNode, parent);
}

CDrawFilmObj* CDrawFilmObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawFilmObj> obj ( new CDrawFilmObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}
	
	return obj.release();
}

Gdiplus::Image* CDrawFilmObj::OnNeedImage() const
{
	if ( m_pDS == NULL)
		return NULL;

	if (m_pVisDS != NULL && m_pVisDS->IsEnabled() != m_bvisState)
		return NULL;

	if ( m_pDS->IsEnabled() ) {
		if ( ++curFrame >= imgStrip.GetSize() )
			curFrame = 0;
	}
	else {
		curFrame = 0;
	}

	return imgStrip[curFrame];
}

#pragma endregion

#pragma region CDrawActionObj
//Класс для отображения текущего состояния дискретной величин
CDrawActionObj::CDrawActionObj() :
CDrawDiscreteDSObj(CRect(0,0,0,0))
{

}

CDrawActionObj::CDrawActionObj(const CRect& position) :
CDrawDiscreteDSObj( position)
{
}

CDrawActionObj::~CDrawActionObj()
{
}

//создание элемента  из XML узла
bool CDrawActionObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawDiscreteDSObj::CreateFromXMLNode(pNode,  parent) )
		return false;

	std::string sVal;
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( appXML.GetNodeAttributeByName(pNode, "SCRIPT", sVal) )
	{
		SCRIPTINFO si;
		AppOBAScriptsHelper::Instance().GetScriptInfo(sVal, si);
		
		actionName = si.name;
		bIsScript = true;

		if ( si.name.empty() )
			return false;
	}
	else if ( appXML.GetNodeAttributeByName(pNode, "ACTION", sVal) )
	{
		actionName = sVal;
		bIsScript = false;		
	}

	return true;
}


CDrawActionObj * CDrawActionObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawActionObj> obj ( new CDrawActionObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}
	
	return obj.release();
}

		
bool CDrawActionObj::OnLButtonDoubleClick(UINT nFlags)
{
	LPNTCC_APP_USER pUser = AppUsersHelper::Instance().GetCurrentUser();

	if ( bIsScript )
	{
		if ( pUser->ug <= m_pDS->Permission() )
		{
			if ( !m_pDS->IsEnabled() )
			{
				AppOBAScriptsHelper::Instance().RunScript(actionName);
			}
		}	
		else
			AfxMessageBox(IDS_SCRIPT_RUN_PERMITIONS, MB_ICONWARNING);
	}
	else
	{
		
	}

	return true;
}

#pragma endregion


#pragma region CDrawSelectObj
//////////////////////////////////////////////////////////////////////////
/// КЛАС ВЫБОРА ИЗМЕРЯЕМОГО/ЗАДАВАЕМОГО ДИАПАЗОНА
CDrawSelectObj::CDrawSelectObj() 
{
	m_pWnd = new CAutoSelComboBox(this);
}

CDrawSelectObj::CDrawSelectObj( const CRect& position)
{

}

CDrawSelectObj::~CDrawSelectObj()
{
	m_pWnd->DestroyWindow();
	delete m_pWnd;

}

bool CDrawSelectObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawObj::CreateFromXMLNode(pNode, parent) )
		return false;

	std::string sVal;	
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "SRC", sVal) )
		return false;

	//Префикс
	sVal = m_pParentLayer->GetParent()->DSPrefix() + sVal;

	m_pds = DataSrc::GetDataSource( sVal );
	if ( !m_pds )
		return false;
	
	AnalogInputDataSrc*  paids = dynamic_cast<AnalogInputDataSrc*>(m_pds); 
	AnalogOutputDataSrc* paods = dynamic_cast<AnalogOutputDataSrc*>(m_pds); 
	
	if ( paids == NULL && paods == NULL )
		return false;

	CFrameWnd* pFrm = (CFrameWnd*)AfxGetApp()->GetMainWnd();
	
	CFSDoc* pDoc = m_pParentLayer->GetParent();

	POSITION pos = pDoc->GetFirstViewPosition();
	CView* pParentView =  pDoc->GetNextView(pos);

	if ( pParentView == NULL )
		return false;

	CRect rc((int)m_pos.X, (int)m_pos.Y, (int)(m_pos.X + m_pos.Width), (int)(m_pos.Y + m_pos.Height+200) );

	if (!m_pWnd->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_HASSTRINGS/*|CBS_SORT|CBS_HASSTRINGS|CBS_OWNERDRAWFIXED*/, rc, pParentView, IDC_STATIC) )
		return false;
		
	m_pWnd->SetFont(&g_PaintManager->m_FontNormal, true);
	
	return OnFillContent();
}

bool CDrawSelectObj::OnFillContent()
{
	AnalogInputDataSrc*  paids = dynamic_cast<AnalogInputDataSrc*>(m_pds); 
	AnalogOutputDataSrc* paods = dynamic_cast<AnalogOutputDataSrc*>(m_pds); 
	
	vector<MEASSURE_RANGE_INFO> msriAll;
	MEASSURE_RANGE_INFO msriCh;

	if ( paids != NULL)
	{
		IAnalogInput* pDev = paids->GetParentDevice();
		ASSERT(pDev);

		pDev->EnumInputRanges(paids->GetChannelNo(), msriAll);
		pDev->GetInputRangeInfo(paids->GetChannelNo(), &msriCh);

	}

	if ( paods != NULL )
	{
		IAnalogOutput* pDev = paods->GetParentDevice();
		ASSERT(pDev);		

		pDev->EnumOutputRanges(paods->GetChannelNo(), msriAll);
		pDev->GetOutputRangeInfo(paods->GetChannelNo(), &msriCh);
	}

	int sel = -1;
	for( vector<MEASSURE_RANGE_INFO>::const_iterator it = msriAll.begin(); it != msriAll.end(); ++it )
	{
		std::string txt = (*it).desc;
		txt += " ";
		txt += (*it).units;

		int ind = m_pWnd->AddString( txt.c_str() );
		
		m_pWnd->SetItemData(ind, (*it).rngid );
		if ( (*it).rngid == msriCh.rngid )
			sel = ind;
	}	

	m_pWnd->SendMessage(CB_SETDROPPEDWIDTH, 200,0);

	m_pWnd->SetCurSel(sel);

	return true;
}


CDrawSelectObj* CDrawSelectObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawSelectObj> obj ( new CDrawSelectObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}

	return obj.release();
}

	//отрисовка объекта
void CDrawSelectObj::Draw(Gdiplus::Graphics& gr)
{
	
}

bool CDrawSelectObj::OnSelectionChanged(long data)
{
	AnalogInputDataSrc*  paids = dynamic_cast<AnalogInputDataSrc*>(m_pds); 
	AnalogOutputDataSrc* paods = dynamic_cast<AnalogOutputDataSrc*>(m_pds); 
	
	IdentifiedPhysDevice* pPhysDev = NULL;
	if ( paids != NULL)
	{
		IAnalogInput* pDev = paids->GetParentDevice();
		ASSERT(pDev);

		pDev->SetInputRange(paids->GetChannelNo(), (unsigned char)data);
		pPhysDev = dynamic_cast<IdentifiedPhysDevice*>(pDev);
	
		//TRACE("\n\tКонфигурация входного канала %d устройства %s была изменена", paids->GetChannelNo(), pPhysDev->Name().c_str());
	}

	if ( paods != NULL )
	{
		IAnalogOutput* pDev = paods->GetParentDevice();
		ASSERT(pDev);		
		
		pDev->SetOutputRange(paods->GetChannelNo(), (unsigned char)data);
		pPhysDev = dynamic_cast<IdentifiedPhysDevice*>(pDev);

		//TRACE("\n\tКонфигурация выходного канала %d устройства %s была изменена", paods->GetChannelNo(), pPhysDev->Name().c_str());
	}

	return true;
}
#pragma endregion


#pragma region CDrawSelectObj
//////////////////////////////////////////////////////////////////////////
/// КЛАС ВЫБОРА ИЗМЕРЯЕМОГО/ЗАДАВАЕМОГО ДИАПАЗОНА
CDrawAnalogValueSelectObj::CDrawAnalogValueSelectObj() 
{	
}

CDrawAnalogValueSelectObj::CDrawAnalogValueSelectObj( const CRect& position)
{

}

CDrawAnalogValueSelectObj::~CDrawAnalogValueSelectObj()
{
}

bool CDrawAnalogValueSelectObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawSelectObj::CreateFromXMLNode(pNode, parent) )
		return false;

	return true;
}

bool CDrawAnalogValueSelectObj::OnFillContent()
{
	AnalogOutputListDataSrc* paods = dynamic_cast<AnalogOutputListDataSrc*>(m_pds); 
	if ( paods == NULL )
	{
		AppErrorsHelper::Instance().SetLastError(DSCE_INVALID_DS_TYPE, "Неверный тип источника данных");
		return false;
	}

	std::vector<ANALOG_VALUE_INFO> values  = paods->GetValues();
	
	int sel = -1;	
	for( auto it = values.begin(); it != values.end(); ++it )
	{
		boost::format fmt(paods->OutFormat());
		fmt % (*it).Value;

		std::string txt = boost::str(fmt);
		if ( paods->Units().size() != 0)
		{
			txt += " ";
			txt += paods->Units();
		}

		if ((*it).Info.length() != 0)
		{
			txt += " - ";
			txt += (*it).Info;
		}
		

		int ind = m_pWnd->AddString( txt.c_str() );

		m_pWnd->SetItemData(ind, ind );
		if ( (*it).Value == paods->GetValue() )
			sel = ind;
	}	

	m_pWnd->SendMessage(CB_SETDROPPEDWIDTH, 200,0);
	m_pWnd->SetCurSel(sel);

	return true;
}


CDrawAnalogValueSelectObj* CDrawAnalogValueSelectObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawAnalogValueSelectObj> obj ( new CDrawAnalogValueSelectObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}

	return obj.release();
}

//отрисовка объекта
void CDrawAnalogValueSelectObj::Draw(Gdiplus::Graphics& gr)
{

}

bool CDrawAnalogValueSelectObj::OnSelectionChanged(long data)
{
	
	AnalogOutputListDataSrc* paods = dynamic_cast<AnalogOutputListDataSrc*>(m_pds); 
	if ( paods == NULL )
		return false;

	std::vector<ANALOG_VALUE_INFO> values = paods->GetValues();

	paods->SetValue(values[data].Value);

	return true;
}
#pragma endregion


#pragma region 

CDrawReactorCrossSection::CDrawReactorCrossSection():
CDrawObj(CRect(0,0,0,0) )
{
}


CDrawReactorCrossSection::CDrawReactorCrossSection(const CRect& position) :
CDrawObj(position )
{

}

CDrawReactorCrossSection::~CDrawReactorCrossSection()
{

}

//создание элемента  из XML узла
bool CDrawReactorCrossSection::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawObj::CreateFromXMLNode(pNode, parent) )
		return false;

	std::string sVal;
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "TMIN", sVal) )
		return false;

	minT = atof(sVal.c_str());

	if ( !appXML.GetNodeAttributeByName(pNode, "TMAX", sVal) )
		return false;

	maxT = atof(sVal.c_str());

	vertical = true;
	if ( appXML.GetNodeAttributeByName(pNode, "VERTICAL", sVal) )
		vertical = atoi(sVal.c_str()) > 0 ? true : false;
	
	opacity = 100;
	if ( appXML.GetNodeAttributeByName(pNode, "OPACITY", sVal) )
		opacity = atoi(sVal.c_str());

	CComPtr<IXMLDOMNode> pPalNode;
	HRESULT hr = pNode->selectSingleNode(CComBSTR("PALETTE"), &pPalNode);

	if ( hr == S_OK && pPalNode != NULL)
	{
		CComPtr<IXMLDOMNodeList> pPalItemList;
		HRESULT hr = pPalNode->selectNodes(CComBSTR("PITEM"), &pPalItemList);

		long lPalSize;	

		pPalItemList->get_length(&lPalSize);		
		pPalItemList->reset();

		for(int i = 0; i < lPalSize; i++)
		{
			CComPtr<IXMLDOMNode>	 pPalItemNode;
			pPalItemList->get_item(i, &pPalItemNode);

			double pos = 0;
			if ( !appXML.GetNodeAttributeByName(pPalItemNode, "POS", sVal) )
				continue;

			pos = atof(sVal.c_str());

			if (pos < 0.0 || pos > 100.0)
				continue;

			Gdiplus::Color color; 			
			appXML.GetGdiPlusColor(pPalItemNode, color, opacity);

			temperaturePalette.insert( make_pair(pos,  color) );
		}
	}
	else {
		//палитра по умолчанию

		Gdiplus::Color Colors[] = 
		{
			Gdiplus::Color::Violet,
			Gdiplus::Color::Indigo,
			Gdiplus::Color::Blue,
			Gdiplus::Color::Green,
			Gdiplus::Color::Yellow,			
			Gdiplus::Color::Orange,
			Gdiplus::Color::Red			
		};
		double pos[] = 
		{
			0.0,
			16.66,
			33.33,
			50.0,
			66.66,
			83.33,
			100.0
		};
		
		for(int i = 0; i < 7; i++ )
			temperaturePalette.insert( make_pair(pos[i],  Colors[i]	) );		
		
		
	}

	/*
	<PALETTE>      
      <PITEM POS="0"   COLOR="0x200020"/> 
      <PITEM POS="20"  COLOR="0x0000FF"/>
      <PITEM POS="40"  COLOR="0x008040"/>
      <PITEM POS="60"  COLOR="0x00FFFF"/>
      <PITEM POS="80"  COLOR="0xFF0000"/>
      <PITEM POS="100" COLOR="0x800000"/>
    </PALETTE>
    */

	CComPtr<IXMLDOMNode> pSectionsNode;
	hr = pNode->selectSingleNode(CComBSTR("SECTIONS"), &pSectionsNode);

	if ( hr == S_OK && pSectionsNode != NULL)
	{
		CComPtr<IXMLDOMNodeList> pSecItemList;
		HRESULT hr = pSectionsNode->selectNodes(CComBSTR("CS"), &pSecItemList);

		long lCSSize;

		pSecItemList->get_length(&lCSSize);		
		pSecItemList->reset();

		for(int i = 0; i < lCSSize; i++)
		{
			CComPtr<IXMLDOMNode>	 pCSNode;
			pSecItemList->get_item(i, &pCSNode);

			double pos = 0;
			if ( !appXML.GetNodeAttributeByName(pCSNode, "POS", sVal) )
				continue;

			pos = atof(sVal.c_str());

			if (pos < 0.0 || pos > 100.0)
				continue;

			//ПРЕФИКС
			std::string  dspref = m_pParentLayer->GetParent()->DSPrefix();
			

			std::pair<AnalogDataSrc*, AnalogDataSrc*> csValues;
			
			if ( !appXML.GetNodeAttributeByName(pCSNode, "VAL1", sVal) )
				return false;
			
			sVal = dspref + sVal;
			csValues.first = dynamic_cast<AnalogDataSrc*>( DataSrc::GetDataSource ( sVal ) );

			if ( !appXML.GetNodeAttributeByName(pCSNode, "VAL2", sVal) )
				return false;

			sVal = dspref + sVal;
			csValues.second = dynamic_cast<AnalogDataSrc*>( DataSrc::GetDataSource ( sVal ) );

			if ( csValues.first == NULL || csValues.second == NULL )
				return false;


			if ( !reactorMap.insert (make_pair (pos, csValues )).second )
				return false;
			
		}
	}

	
	/*
    <SECTIONS>
      <CS POS="0"   VAL1="TR1_M1_1" VAL2="TR1_M1_2"/>
      <CS POS="25"  VAL1="TR1_M1_3" VAL2="TR1_M1_3"/>
      <CS POS="50"  VAL1="TR1_M1_4" VAL2="TR1_M1_4"/>
      <CS POS="75"  VAL1="TR1_M1_5" VAL2="TR1_M1_5"/>
      <CS POS="100" VAL1="TR1_M1_6" VAL2="TR1_M1_6"/>
    </SECTIONS>
	*/

	return true;
}

//создание элемента  из XML узла
CDrawReactorCrossSection* CDrawReactorCrossSection::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawReactorCrossSection> obj ( new CDrawReactorCrossSection() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}

	return obj.release();
}

bool CDrawReactorCrossSection::GetOrientation() const
{ 
	return vertical; 
} 

void CDrawReactorCrossSection::SetOrientation(bool vert)
{
	vertical = vert;
}

//отрисовка объекта
void CDrawReactorCrossSection::Draw(Gdiplus::Graphics& gr)
{

	CDrawObj::Draw(gr);

	std::map<double, std::pair<AnalogDataSrc*, AnalogDataSrc*> >::iterator itRPos = reactorMap.begin();

	std::vector<Gdiplus::REAL>  pPos;
	pPos.assign(reactorMap.size(), 0.0f );

	std::vector<Gdiplus::Color> pColors;
	pColors.assign(reactorMap.size(), Gdiplus::Color::White);

	/*//TRACE("\n=> ");*/
	int  i = 0;
	for(itRPos; itRPos != reactorMap.end(); i++, ++itRPos)
	{
		pPos[i] = Gdiplus::REAL((*itRPos).first) / 100.0f;
		
		std::pair<AnalogDataSrc*, AnalogDataSrc*> crossT = itRPos->second;
		/////////////////////////////////////
		//температура на срезе
		double TCur = (crossT.first->GetValue() + crossT.second->GetValue()) / 2.0;
		
		// проверка выхода за границы температурного диапазона
		TCur = ( TCur > maxT ) ? maxT : TCur; 
		TCur = ( TCur < minT ) ? minT : TCur;

		double tPos = 100 * (TCur - minT) / (maxT - minT);
		/*//TRACE("\t {%06.2f - %06.2f}", TCur, tPos);*/
		pColors[i] = GetPaletteColor( tPos );

	}	

	Gdiplus::PointF ptFrom(m_pos.X, m_pos.Y);
	Gdiplus::PointF ptTo   = vertical ? 
			Gdiplus::PointF(m_pos.X, m_pos.GetBottom()) : 
			Gdiplus::PointF(m_pos.GetRight(), m_pos.Y);

	Gdiplus::LinearGradientBrush br(ptFrom, ptTo, Gdiplus::Color::White, Gdiplus::Color::Black );
	br.SetInterpolationColors(pColors.data(), pPos.data(), reactorMap.size());
	gr.FillRectangle(&br, m_pos);

// 	delete [] pPos;
// 	delete [] pColors;
}

Gdiplus::Color CDrawReactorCrossSection::GetPaletteColor(double T)
{
	std::map<double, Gdiplus::Color, std::greater<double> >::iterator itL = temperaturePalette.lower_bound(T);	
	std::map<double, Gdiplus::Color, std::greater<double> >::iterator itH = temperaturePalette.upper_bound(T);	

	if ( itL != temperaturePalette.begin())
		--itL;

	if ( itH == temperaturePalette.end())
		--itH;

	Gdiplus::Color clr1 = Gdiplus::Color::Black;
	Gdiplus::Color clr2 = Gdiplus::Color::White;

	double v1 = 0.0;
	double v2 = 1.0;

	
	clr1 = itL->second;
	v1   = itL->first;
	
	clr2 = itH->second;
	v2   = itH->first;
		
 	double al = (T - v1) / (v2 - v1);
 	BYTE from[3] = {clr1.GetR(), clr1.GetG(), clr1.GetB() };
 	BYTE to[3]   = {clr2.GetR(), clr2.GetG(), clr2.GetB() };
 
 	BYTE iR = (BYTE)(from[0] + al * (to[0] - from[0]));
 	BYTE iG = (BYTE)(from[1] + al * (to[1] - from[1]));
 	BYTE iB = (BYTE)(from[2] + al * (to[2] - from[2]));


	BYTE iA = BYTE(255 - 255 * (100 - opacity) / 100 );
	

	return Gdiplus::Color(iA, iR, iG, iB);//fabs(v1 - T) < fabs(v1 - T) ? clr1 : clr2;
}

#pragma endregion


CDrawPathAnimation::CDrawPathAnimation():
CDrawObj(CRect(0,0,0,0) ), pAniControl(NULL)
{
}


CDrawPathAnimation::CDrawPathAnimation(const CRect& position) :
CDrawObj(position ), pAniControl(NULL)
{

}

CDrawPathAnimation::~CDrawPathAnimation()
{

}

//создание элемента  из XML узла
bool CDrawPathAnimation::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawObj::CreateFromXMLNode(pNode, parent) )
		return false;

	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	std::string sVal;
	if ( !appXML.GetNodeAttributeByName(pNode, "SRC", sVal) )
		return false;
	
	AppImagesHelper& appImages = AppImagesHelper::Instance();
	srcImg = appImages.GetImage(sVal);	
	
	if (srcImg == NULL)
		return false;

	std::string  dspref = m_pParentLayer->GetParent()->DSPrefix();
	
	if ( appXML.GetNodeAttributeByName(pNode, "ANICONTROL", sVal) )
	{
		sVal = dspref + sVal;
		pAniControl = dynamic_cast<DiscreteDataSrc*>(DataSrc::GetDataSource(sVal));
	}

	step = 0.5f;
	if ( appXML.GetNodeAttributeByName(pNode, "STEP", sVal) )
		step = boost::lexical_cast<float>(sVal.c_str());
	
	distance = 4;
	if ( appXML.GetNodeAttributeByName(pNode, "DIST", sVal) )
		distance = boost::lexical_cast<int>(sVal.c_str());
	
	CComPtr<IXMLDOMNode> pPathNode;
	HRESULT hr = pNode->selectSingleNode(CComBSTR("PATH"), &pPathNode );
	
	if (FAILED(hr))
		return false;

	if ( !appXML.GetNodeAttributeByName(pPathNode, "POINTS", sVal) )
		return false;


	pathPoints.RemoveAll();

	//Доступные разделители ",;|\t "
	boost::char_separator<char> sep(" |\t");
	std::string points = sVal.c_str();
	//
	
	boost::tokenizer< boost::char_separator<char> > tokens(points, sep);
	for ( boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		try
		{
			//TRACE("\n\t\t%s", (*it).c_str());
			std::vector<std::string> parts;
			std::string coords((*it));
			//using namespace boost::algorithm;
			boost::split(parts, coords,  std::bind2nd(std::equal_to<char>(), ';'));

			if ( parts.size() != 2 )
				return false;
			
			Gdiplus::PointF pt;
			pt.X  = boost::lexical_cast<float>(parts[0].c_str());
			pt.Y  = boost::lexical_cast<float>(parts[1].c_str());
			
			pathPoints.Add(pt);
			
		}
		catch(boost::bad_lexical_cast &)
		{
			return false;
		}
	}

	float st = step * (float)srcImg->GetWidth();

	animationPoints.RemoveAll();
	for (int i = 0 ; i < pathPoints.GetCount() - 1; i++)
	{
		Gdiplus::PointF ptFrom = pathPoints.GetAt(i);
		Gdiplus::PointF ptTo   = pathPoints.GetAt(i + 1);
		ptFrom.X -= srcImg->GetWidth() / 2;
		ptFrom.Y -= srcImg->GetHeight() / 2;
		ptTo.X -= srcImg->GetWidth() / 2;
		ptTo.Y -= srcImg->GetHeight() / 2;

		float xLen = (ptFrom.X - ptTo.X);
		float yLen = (ptFrom.Y - ptTo.Y);

		float len = sqrt(xLen*xLen + yLen*yLen);
		float den = (len / st);
		den = floor(den);

		den = (den == 0) ? 1 : den; 

		float dx = xLen / den; 
		float dy = yLen / den; 

		Gdiplus::PointF pt = ptFrom;
		for (int j = 0; j < den; j++)
		{	
			animationPoints.Add(pt);
			pt.X -= dx;
			pt.Y -= dy;
		}
	}
	
	curStep = 0;

	return true;
}

//создание элемента  из XML узла
CDrawPathAnimation* CDrawPathAnimation::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawPathAnimation> obj ( new CDrawPathAnimation() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}

	return obj.release();
}

//отрисовка объекта
void CDrawPathAnimation::Draw(Gdiplus::Graphics& gr)
{
	//CDrawObj::Draw(gr);
// 	if ( (pAniControl == NULL) || (!pAniControl->IsEnabled()))
// 		return;

	Gdiplus::ColorMatrix ClrMatrix =         
	{ 
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	//ClrMatrix.m[3][3] = 20.0 / 100.0;

	Gdiplus::ImageAttributes ImgAttr;

	ImgAttr.SetColorMatrix(&ClrMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

	Gdiplus::Color clr(0xFF, 0x4D, 0x73, 0x99);    //линия		 #4d7399
	Gdiplus::Pen pen(clr, 1.0f);

	if ( m_pParentLayer->GetParent()->IsDebugMode() )
		gr.DrawLines(&pen, pathPoints.GetData(), pathPoints.GetCount());

	if ( curStep >=  distance)
		curStep = 0;
	
	
	for (int i = curStep; i < animationPoints.GetCount(); i = i + distance)
	{	
		Gdiplus::PointF pt = animationPoints.GetAt(i);
		Gdiplus::RectF rc(pt.X, pt.Y, (Gdiplus::REAL)srcImg->GetWidth(),  (Gdiplus::REAL)srcImg->GetHeight());

		gr.DrawImage(srcImg, rc, 0, 0,  (Gdiplus::REAL)srcImg->GetWidth(), (Gdiplus::REAL)srcImg->GetHeight(), Gdiplus::UnitPixel, &ImgAttr);
	}

	curStep++;	
}



using namespace xyplot;

CDrawPlotObj::~CDrawPlotObj()
{
	XYPlotManager& pm = XYPlotManager::Instance();
	pm.DestroyPlot(hPlot);

	::DestroyWindow(hwndPlot);
}

//создание элемента  из XML узла
CDrawPlotObj* CDrawPlotObj::CreateFromXML(IXMLDOMNode* pNode, FSLayer& parent, UINT &error)
{
	auto_ptr<CDrawPlotObj> obj ( new CDrawPlotObj() );

	if( !obj->CreateFromXMLNode(pNode, parent) )
	{
		AppErrorsHelper& err = AppErrorsHelper::Instance();
		err.GetLastError(error);
		return NULL;
	}

	return obj.release();
}


//создание элемента  из XML узла
bool CDrawPlotObj::CreateFromXMLNode(IXMLDOMNode* pNode, FSLayer& parent)
{
	if ( !CDrawObj::CreateFromXMLNode(pNode, parent) )
		return false;

	CView *pParentView = parent.GetParentView();
	
	if (pParentView == NULL) 
		return false;

	hwndPlot = ::CreateWindow(TEXT("STATIC"), "", 
						WS_CHILD, 
						(int)m_pos.X, 
						(int)m_pos.Y, 
						(int)m_pos.Width, 
						(int) m_pos.Height,
						pParentView->m_hWnd, 
						NULL, 
						::AfxGetInstanceHandle(), 
						NULL );

	XYPlotManager& pm = XYPlotManager::Instance();
	hPlot = pm.CreatePlot(hwndPlot);

	if (!hPlot)
		return false;

	pm.SetRedraw(hPlot, FALSE);

	initPlotDefautlts();

	dAxisShift = 300;

	axisIDs.insert(make_pair(PI_AXIS_LEFT, "left"));
	axisIDs.insert(make_pair(PI_AXIS_RIGHT, "right"));
	axisIDs.insert(make_pair(PI_AXIS_BOTTOM, "bottom"));
	axisIDs.insert(make_pair(PI_AXIS_TOP, "top"));

	setupAxis(pNode, PI_AXIS_LEFT);
	setupAxis(pNode, PI_AXIS_BOTTOM);
	//setupAxis(pNode, PI_AXIS_TOP);
	setupAxis(pNode, PI_AXIS_RIGHT);

	setupProfiles(pNode);

	//////////////////////////////////////////////////////////////////////////
	
	setupTitle(pNode);

	//pm.EnableItem(hPlot, PI_LEGEND, 0, TRUE);

	//////////////////////////////////////////////////////////////////////////
	LONG lStyle = GetWindowLong(hwndPlot, GWL_STYLE);
	lStyle |= WS_VISIBLE;
	SetWindowLong(hwndPlot, GWL_STYLE, lStyle);

	pm.SetRedraw(hPlot, TRUE);

	return true;
}

void CDrawPlotObj::setupTitle(IXMLDOMNode* pNode)
{
	CComPtr<IXMLDOMNode> pTitle;
	HRESULT hr = pNode->selectSingleNode(CComBSTR("TITLE"), &pTitle);
	if ( FAILED(hr) || pTitle == NULL )
		return;

	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	std::string val;
	if ( !appXML.GetNodeAttributeByName(pTitle, "TEXT", val) )
		return; 
		

	CComPtr<IXMLDOMNode> pFont;
	hr = pTitle->selectSingleNode(CComBSTR("FONT"), &pFont);
	if ( SUCCEEDED(hr) && pTitle != NULL )
	{
		std::string plotString;
		buildPlotStringFromXml(val, pFont, plotString);

		XYPlotManager& pm = XYPlotManager::Instance();
		pm.SetString(hPlot, PI_TITLE, 0, plotString.c_str() );
	}
}

void CDrawPlotObj::setupAxis(IXMLDOMNode* pNode, long axisID)
{
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	XYPlotManager& pm = XYPlotManager::Instance();

	CString axisBstr;
	axisBstr.Format("AXIS[@POS=\"%s\"]", axisIDs[axisID].c_str());

	CComPtr<IXMLDOMNode> pAxis;
	HRESULT hr = pNode->selectSingleNode(CComBSTR(axisBstr), &pAxis);
	if ( FAILED(hr) || pAxis == NULL )
		return;
	
	std::string val;

	double maxY = 0.0;
	if ( appXML.GetNodeAttributeByName(pAxis, "MAX", val) )
		maxY = atof(val.c_str());

	double minY = 0.0;
	if (appXML.GetNodeAttributeByName(pAxis, "MIN", val) )
		minY = atof(val.c_str());

	if ( axisID != PI_AXIS_BOTTOM )
	{
		if (maxY == 0.0 && minY == 0.0)
			pm.EnableItem(hPlot, axisID, PAI_AUTOSCALE, TRUE);	
		else
			pm.SetAxisRange(hPlot, axisID, minY, maxY);
	}

	//////////////////////////////////////////////////////////////////////////
	///AXIS TITLE
	std::string title;
	if (appXML.GetNodeAttributeByName(pAxis, "TITLE", title ) )
	{
		CComPtr<IXMLDOMNode> pFont;
		hr = pAxis->selectSingleNode( CComBSTR("FONT"), &pFont);

		std::string plotString;
		buildPlotStringFromXml(title, pFont, plotString);

		pm.EnableItem(hPlot, axisID, PAI_TITLE, TRUE );
		pm.SetString(hPlot, axisID, PAI_TITLE, plotString.c_str() );
	}

	//////////////////////////////////////////////////////////////////////////
	///AXIS TICKS
	int ticks = pm.GetLong(hPlot, axisID, PAI_MAJOR_TICKS_COUNT);
	if ( appXML.GetNodeAttributeByName(pAxis, "STEP", val) )
	{
		pm.GetAxisRange(hPlot, axisID, minY, maxY);
		double rng = maxY - minY;

		double step = fabs( atof( val.c_str() ) );
		ticks = int(rng / step) + 1;

		pm.SetLong(hPlot, axisID, PAI_MAJOR_TICKS_COUNT, ticks);
	}

	pm.EnableItem(hPlot, axisID, PAI_MAJOR_DIGITS, TRUE);
	pm.EnableItem(hPlot, axisID, PAI_MINOR_DIGITS, TRUE);
	 
}



void CDrawPlotObj::setupProfiles(IXMLDOMNode* pNode)
{
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	XYPlotManager& pm = XYPlotManager::Instance();

	CComPtr<IXMLDOMNodeList> pProfileList;
	HRESULT hr = pNode->selectNodes(CComBSTR("PROFILES/PROFILE"), &pProfileList);

	long lProfileCount;
	pProfileList->get_length(&lProfileCount);
	pProfileList->reset();
	
	for (int j=0; j < lProfileCount; j++)
	{
		CComPtr<IXMLDOMNode> pProfile;
		pProfileList->get_item(j, &pProfile);
		std::string name;
		if (!appXML.GetNodeAttributeByName(pProfile, "DSID", name) )
			continue;

		const DataSrc* pDS = DataSrc::GetDataSource(name);

		if( pDS == NULL )
			continue;

		std::string val;
		int lw = 2;
		if (appXML.GetNodeAttributeByName(pProfile, "LW", val) )
			lw = atoi(val.c_str());

		int lt = 1;
		if (appXML.GetNodeAttributeByName(pProfile, "LT", val) )
			lt = atoi(val.c_str());		

		std::string lp = "2 3";
		if (appXML.GetNodeAttributeByName(pProfile, "LP", val) )
			lp = val;		

		HPROFILE hProf = pm.CreateProfile(hPlot, name.c_str(), pDS->Color(), lw, lt, lp.c_str(), true, false, PI_AXIS_BOTTOM, PI_AXIS_LEFT);	
		profiles.insert(std::make_pair(name, hProf));
	}


	const DataCollector& dc = DataCollector::Instance(); 

	std::vector<double> xVal;
	if ( dc.GetTimeBuffer( xVal ) )
	{
		pm.SetRedraw(hPlot, FALSE);

		for (PROFILESMAP::const_iterator it = profiles.begin(); it != profiles.end(); ++it)
		{
			std::vector<double> yVal;

			if ( !dc.GetDataSourceBuffer((*it).first, yVal) )
				continue;

			HPROFILE hProfile = (*it).second;
			pm.SetData(hPlot, hProfile, xVal.data(), yVal.data(), xVal.size() );		
		}

		pm.SetRedraw(hPlot, TRUE);
	}
}

bool CDrawPlotObj::initPlotDefautlts()
{
	XYPlotManager& pm = XYPlotManager::Instance();

	pm.EnableItem(hPlot, PI_AXIS_BOTTOM, PAI_TIME_HISTORY, TRUE );

	pm.SetLong(hPlot, PI_AXIS_BOTTOM, PAI_LINE_WIDTH, 1);	
	pm.SetLong(hPlot, PI_AXIS_LEFT, PAI_LINE_WIDTH, 1);	
	pm.SetLong(hPlot, PI_AXIS_TOP, PAI_LINE_WIDTH, 1);	
	pm.SetLong(hPlot, PI_AXIS_RIGHT, PAI_LINE_WIDTH, 1);	

	pm.SetLong(hPlot, PI_AXIS_BOTTOM, PAI_MINOR_TICKS_COUNT, 1);	
	pm.SetLong(hPlot, PI_AXIS_BOTTOM, PAI_MAJOR_TICKS_COUNT, 7);	

	pm.SetLong(hPlot, PI_SELECTION, SP_MODE, SM_TRACE);

	pm.SetLong(hPlot, PI_AXIS_LEFT, PAI_MINOR_TICKS_COUNT, 1);
	pm.SetLong(hPlot, PI_AXIS_LEFT, PAI_MAJOR_TICKS_COUNT, 11);	

	pm.EnableItem(hPlot, PI_AXIS_LEFT, PAI_TITLE, FALSE);
	pm.EnableItem(hPlot, PI_AXIS_LEFT, PAI_MINOR_DIGITS, FALSE);
	

	pm.EnableItem(hPlot, PI_AXIS_LEFT, PAI_AUTOSCALE, FALSE);		
	pm.EnableItem(hPlot, PI_AXIS_RIGHT, PAI_AUTOSCALE, FALSE);		
	pm.EnableItem(hPlot, PI_AXIS_BOTTOM, PAI_AUTOSCALE, FALSE);
	pm.EnableItem(hPlot, PI_AXIS_TOP, PAI_AUTOSCALE, FALSE );

	pm.EnableItem(hPlot, PI_AXIS_RIGHT, PAI_TITLE, FALSE);

	// 	pm.EnableItem(hPlot, PI_LEGEND, 0, true);
	// 	pm.SetLong(hPlot, PI_LEGEND, PLP_ALIGN, PLP_LEFT);
	// 	pm.SetSelectionMode(hPlot, SM_TRACE);

	double xInterval = DataCollector::Instance().GetTimeInterval();
	std::string xTitle;
	
	double x1 = (double)CTime::GetCurrentTime().GetTime();
	x1 = x1 - int(x1) % 300;
	double x2 = x1 + xInterval;

	pm.SetAxisRange(hPlot, PI_AXIS_BOTTOM, x1, x2);

	pm.SetString(hPlot, PI_AXIS_BOTTOM, PAI_TITLE, xTitle.c_str() );

	pm.SetString(hPlot, PI_AXIS_TOP, PAI_TITLE, "" );
	pm.SetString(hPlot, PI_AXIS_RIGHT, PAI_TITLE, "" );

	int ticks = pm.GetLong(hPlot, PI_AXIS_BOTTOM, PAI_MAJOR_TICKS_COUNT) - 1;	

	COLORREF clrStart = g_PaintManager->GetColor(CExtPaintManagerOffice2003::_2003CLR_GRADIENT_DARK);
	COLORREF clrEnd   = g_PaintManager->GetColor(CExtPaintManagerOffice2003::_2003CLR_GRADIENT_LIGHT);

	pm.SetColor(hPlot, PI_BACKGROUND, BGP_COLOR_START, clrStart);
	pm.SetColor(hPlot, PI_BACKGROUND, BGP_COLOR_END,	clrEnd);

	pm.SetColor(hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_END,	clrStart);
	pm.SetColor(hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_START,	clrEnd);


	return TRUE; 
}

void CDrawPlotObj::buildPlotString(std::string text, std::string fontname, int fontsize, COLORREF color, std::string& out)
{
	std::strstream str;

	str << "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 ";
	str << fontname.c_str() << ";}}{\\colortbl ";
	str << "\\red"	 << (int)GetRValue(color);
	str << "\\green" << (int)GetGValue(color);
	str << "\\blue"  << (int)GetBValue(color);
	str << ";}\\cf0\\f0\\fs" << fontsize;
	str << text.c_str();
	str << "\\par}" << ends;

	out = str.str();
}

void CDrawPlotObj::buildPlotStringFromXml(std::string text, IXMLDOMNode*pFontNode,  std::string& out)
{

	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	////////////////////////////////////////////////
	int fntSize = atoi( m_pParentLayer->GetDefault("FONT.SIZE").c_str() ) * 2;
	COLORREF fntClr = strtol( m_pParentLayer->GetDefault("FONT.COLOR").c_str(), 0, 0);
	std::string fntName =  m_pParentLayer->GetDefault("FONT.NAME");
	
	if ( pFontNode != NULL )
	{
		std::string val;
		if ( appXML.GetNodeAttributeByName(pFontNode, "NAME", val ) )
			fntName = val;

		if ( appXML.GetNodeAttributeByName(pFontNode, "SIZE", val ) )
			fntSize = atoi(val.c_str()) * 2;

		if ( appXML.GetNodeAttributeByName(pFontNode, "COLOR", val ) )
			fntClr = strtol( val.c_str(), 0, 0);;
	}

	buildPlotString(text, fntName, fntSize, fntClr, out );
}

//отрисовка объекта
void CDrawPlotObj::Draw(Gdiplus::Graphics& gr)
{
	updatePlot();
}


void CDrawPlotObj::updatePlot()
{
	if (hPlot == NULL)
		return;

	XYPlotManager& plot = XYPlotManager::Instance();
	plot.SetRedraw(hPlot, FALSE);

	const DataCollector& dc = DataCollector::Instance(); 
	std::vector<double> xVal;
	if ( !dc.GetTimeBuffer( xVal ) )
	{
		plot.SetRedraw(hPlot, TRUE);
		return;
	}

	if (xVal.size() >= dc.GetSubBufferCapacity())
	{
		double dMinXA;
		double dMaxXA;
		plot.GetAxisRange(hPlot, PI_AXIS_BOTTOM, dMinXA, dMaxXA);

		if (dMinXA <= xVal.front())
		{
			dMinXA = xVal.front();
			dMaxXA = xVal.back();

			dMinXA = dMinXA - (int)dMinXA % 300;
			dMaxXA = dMinXA + DataCollector::Instance().GetTimeInterval();			

			//TRACE("> Scroll X range");

			plot.SetAxisRange(hPlot, PI_AXIS_BOTTOM, dMinXA, dMaxXA);
			//axisBottomRange.first  = axisTopRange.first  = dMinXA;
			//axisBottomRange.second = axisTopRange.second = dMaxXA;
		}

		if ( xVal.back() >= dMaxXA )
		{
			dMinXA += dAxisShift;
			dMaxXA += dAxisShift; 			
			//TRACE("> Shift X range");
			plot.SetAxisRange(hPlot, PI_AXIS_BOTTOM, dMinXA, dMaxXA);
// 			axisBottomRange.first  = axisTopRange.first  = dMinXA;
// 			axisBottomRange.second = axisTopRange.second = dMaxXA;
		}
	}

	if  ( !xVal.empty() )
	{
// 		const double x = xVal.back(); 		
// 
// 		for (PROFILESMAP::const_iterator it = profiles.begin(); it != profiles.end(); ++it)
// 		{
// 			double y;
// 
// 			if ( !dc.GetLast((*it).first, y) )
// 				continue;
// 
// 			HPROFILE hProfile = (*it).second;
// 			plot.AppendData(hPlot, hProfile, &x, &y, 1 );		
// 		}

		for (PROFILESMAP::const_iterator it = profiles.begin(); it != profiles.end(); ++it)
		{
			std::vector<double> yVal;

			if ( !dc.GetDataSourceBuffer((*it).first, yVal) )
				continue;

			HPROFILE hProfile = (*it).second;
			plot.SetData(hPlot, hProfile, xVal.data(), yVal.data(), xVal.size() );		
		}
	}

	plot.SetRedraw(hPlot, TRUE);
}