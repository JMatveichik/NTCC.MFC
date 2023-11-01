#include "StdAfx.h"
#include "FSLayer.h"
#include "FSDoc.h"
#include "FSDocItem.h"

#include "Automation\errors.h"
#include "Automation\DeviceFactory.h"
#include "Automation\DataSrc.h"
#include "NanoTubesControlCenter.h"

#include "ah_xml.h"
#include "ah_errors.h"
#include "ah_loadcfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

FSLayer::FSLayer(CFSDoc* parent) : 
m_pParentDoc(parent),
pVisDS(NULL),
visState(TRUE),
isVisible(TRUE), 
isActive(FALSE),
basePos(0.0f, 0.0f),
strName("UNKNOWN")
{
}

FSLayer::~FSLayer(void)
{
	POSITION pos = items.GetHeadPosition();

	while(pos != NULL)
	{
		CDrawObj* pDrawObj = items.GetAt(pos);
		delete pDrawObj;

		items.GetNext(pos);
	}
	items.RemoveAll();
}

CFSDoc* const FSLayer::GetParent() const
{
	return m_pParentDoc;
}

CView* FSLayer::GetParentView() const
{
	POSITION pos = m_pParentDoc->GetFirstViewPosition();
	CView* pView = m_pParentDoc->GetNextView(pos);
	return pView;
}

int FSLayer::GetItemCount() const 
{ 
	return items.GetCount(); 
}

std::string FSLayer::Name() const
{
	return strName;
}

std::string FSLayer::GetDefault(const char* pszName) const
{
	map<std::string, std::string>::const_iterator fnd = defaults.find(pszName);
	if ( fnd != defaults.end() )
		return (*fnd).second.c_str();	
	else
		return m_pParentDoc->GetDefault(pszName);
	
}

const Gdiplus::PointF& FSLayer::GetBase() const
{
	return basePos;
}

CDrawObj* FSLayer::HitTest(CPoint& pt) const
{
	if ( !IsVisible() )
		return NULL;

	POSITION pos = items.GetTailPosition();
	int i = 0;
	while(pos != NULL)
	{
		CDrawObj* pDrawObj = items.GetAt(pos);
		
		CRect rc;
		pDrawObj->GetPosition(rc);

		if ( pDrawObj->HitTest(pt, NULL, false) == 1 )
			return  pDrawObj;

		items.GetPrev(pos);
	}

	return NULL;
}

//показать слой 
void FSLayer::Show()
{
	isVisible = TRUE;
}

//спрятать слой
void FSLayer::Hide()
{
	isVisible = FALSE;
}

BOOL FSLayer::IsVisible() const
{
	if ( pVisDS != NULL ) 
		return ((BOOL)pVisDS->IsEnabled() == visState);		
	
	return isVisible;
}

BOOL FSLayer::CreateFromXML(IXMLDOMNode* pNode)
{
	std::string val;
	
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( appXML.GetNodeAttributeByName(pNode, "X", val) )
		basePos.X = Gdiplus::REAL(atof(val.c_str()));

	if ( appXML.GetNodeAttributeByName(pNode, "Y", val) )
		basePos.Y = Gdiplus::REAL(atof(val.c_str()));

	if ( appXML.GetNodeAttributeByName(pNode, "NAME", val) )
		strName = val.c_str();

	CString strMsg;
	strMsg = "Cоздание слоя - ";
	strMsg += strName.c_str();

	std::ofstream log;
	log.open("layers.log", std::ios::app);

	log << (LPCTSTR)strMsg << std::endl;

	AppLoadingHelper& LH = AppLoadingHelper::Instance();
	LH.AddContentMessage((LPCTSTR)strMsg, COutMessage::MSG_OK);

	pVisDS = NULL;
	if ( appXML.GetNodeAttributeByName(pNode, "VISIBLE", val) )
	{
		int curPos = 0;
		CString sVal = val.c_str();
		std::string name = sVal.Tokenize(_T(" :\t\n"), curPos);

		if (name != _T(""))
		{
			//Префикс
			std::string pref = m_pParentDoc->DSPrefix();
			name = pref.c_str() + name;
			
			pVisDS = dynamic_cast<const DiscreteDataSrc*>(DataSrc::GetDataSource(name));
			
			if ( pVisDS == NULL ) 
				AppErrorsHelper::Instance().SetLastError(DSCE_INVALID_DS_ID, "Не найден источник данных для отображения слоя");
			else
			{
				std::string visSt = sVal.Tokenize(_T(" :\t\n"), curPos);
				visState = atoi(visSt.c_str()) > 0;
			}
		}
	}

	defaults.clear();

	//загрузка параметров отображения по умолчанию для слоя
	appXML.GetDrawObjectDefaults(pNode, defaults, false);

	CComPtr<IXMLDOMNodeList> pItemsList;
	HRESULT hr = pNode->selectNodes(CComBSTR("ITEM"), &pItemsList);

	if ( FAILED(hr) )
		return FALSE;

	long itemCount = 0;
	pItemsList->get_length(&itemCount);
	pItemsList->reset();

	if (itemCount == 0 )
		return FALSE;
		
	minPos = basePos;

	log << "Load items" << std::endl;

	for(int i = 0; i < itemCount; i++)
	{

		CComPtr<IXMLDOMNode>	 pItemNode;
		pItemsList->get_item(i, &pItemNode);		

		UINT error;
		CDrawObj* pDrawObj = XMLObjectFactory::CreateDrawObject(pItemNode, *this, error);

		bool bOk = false;
		
		if ( pDrawObj != NULL )
		{
			log << "\tItem " << i << " " << pDrawObj->GetID().c_str() << endl;

			m_pParentDoc->AddItem(pDrawObj);
			items.AddTail(pDrawObj);
			CRect rcItem;
			pDrawObj->GetPosition(rcItem);

			Gdiplus::PointF ptCurBR(Gdiplus::REAL(rcItem.BottomRight().x), Gdiplus::REAL(rcItem.BottomRight().y) );
			maxPos.X = max(maxPos.X, ptCurBR.X);
			maxPos.Y = max(maxPos.Y, ptCurBR.Y);

			Gdiplus::PointF ptCurTL(Gdiplus::REAL(rcItem.TopLeft().x), Gdiplus::REAL(rcItem.TopLeft().y) );
			minPos.X = min(minPos.X, ptCurTL.X);
			minPos.Y = min(minPos.Y, ptCurTL.Y);

			bOk = true;
		}
		else
		{
			std::string str = AppErrorsHelper::Instance().GetLastError(error);
			strMsg.Format("Error 0x%04X - [Layer: %s; Item: %d]  Ошибка создания элемента схемы %s",  error, strName.c_str(), i, str.c_str());		
			LH.AddContentMessage((LPCTSTR)strMsg, COutMessage::MSG_ERROR);
		}


		LH.StepSection(-1, bOk);
		
	}

	return TRUE;
}

BOOL FSLayer::IsActive() const
{
	return isActive;
}

void FSLayer::SetActive(BOOL bActive /*= true */)
{
	isActive = bActive;
}

//отрисовка слоя
void FSLayer::Draw(Gdiplus::Graphics& gr)
{
	if ( !IsVisible() )
		return;

	/*
	if ( isActive )
	{
		Gdiplus::Pen pen(Gdiplus::Color::AliceBlue, 1.0f);

		Gdiplus::Color clrLines(128, 128, 128, 0);
		Gdiplus::Color clrBk(80, 80, 80, 128);
		
		Gdiplus::HatchBrush brushR(Gdiplus::HatchStyleCross, clrLines, clrBk);
		
		Gdiplus::RectF layerRect(minPos, Gdiplus::SizeF( maxPos.X - minPos.X, maxPos.Y - minPos.Y));
		
		gr.FillRectangle(&brushR, layerRect);
		gr.DrawRectangle(&pen, layerRect);
		
		Gdiplus::Color clr1(0xFF, 0xC3, 0xF3, 0xF7);	//градиент 1 #c3f3f7				
		Gdiplus::Color clr2(0xFF, 0xF2, 0xFF, 0xFF);	//градиент 2 #f2ffff
		Gdiplus::LinearGradientBrush brushP( Gdiplus::RectF(0, 0, 8, 8), clr1, clr2, Gdiplus::LinearGradientModeVertical); 

		gr.FillEllipse(&brushP, (INT)basePos.X, (INT)basePos.Y, 8, 8);
		gr.DrawEllipse(&pen, (INT)basePos.X, (INT)basePos.Y, 8, 8);
	}
	*/
	
	POSITION pos = items.GetHeadPosition();
	while(pos != NULL)
	{
		CDrawObj* pDrawObj = items.GetAt(pos);

		if ( pDrawObj->IsVisible() )
			pDrawObj->Draw(gr);

		items.GetNext(pos);
	}
	
}


