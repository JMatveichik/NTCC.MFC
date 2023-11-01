// FSDoc.cpp : implementation of the CFSDoc class
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"

#include "FSDoc.h"
#include "FSDocItem.h"
#include "winmessages.h"

#include "ah_xml.h"
#include "ah_project.h"
#include "ah_loadcfg.h"

#include "Automation/DataSrc.h"
#include "Automation/Timers.h"
#include "Automation/DataCollector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define SEND_MESSAGE_TO_ALL_VIEWS(msg, wp, lp) \
	POSITION pos = GetFirstViewPosition(); \
	while ( pos != NULL ) {	CView* pView = GetNextView(pos); ::SendMessage(pView->m_hWnd, msg, wp, (LPARAM)lp); }
	

// CFSDoc

IMPLEMENT_DYNCREATE(CFSDoc, CDocument)

BEGIN_MESSAGE_MAP(CFSDoc, CDocument)
END_MESSAGE_MAP()


// CFSDoc construction/destruction

CFSDoc::CFSDoc() : rcMsgList(0, 0, 0, 0)
{
	
}

CFSDoc::~CFSDoc()
{
	
}

BOOL CFSDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}


CDrawObj* CFSDoc::HitTest(CPoint& pt) const
{

	std::list<FSLayer*>::const_reverse_iterator it = layers.rbegin() ;

	for( ; it != layers.rend(); ++it)
	{
		////TRACE("\n\tLayer %d - %d items", (*it).first, (*it).second->GetItemCount());
		CDrawObj* pObj = (*it)->HitTest(pt);
		(*it)->SetActive(false);
		if ( pObj != NULL )
		{
			(*it)->SetActive(true);
			return pObj;
		}
	}	
	
	return NULL;
}

// CFSDoc serialization

void CFSDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CFSDoc diagnostics

#ifdef _DEBUG
void CFSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CFSDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if ( !FromXML(lpszPathName) )
		return FALSE;

	return TRUE;
}

BOOL CFSDoc::Rebuild()
{
	CString strFilePath = GetPathName();

	//Очищаем хранилища
	DeleteContents();
	
	//загружаемся из XML
	if ( !FromXML(strFilePath) )
		return FALSE;
	

	UpdateAllViews(NULL, 0, NULL);
	return TRUE;
}

void CFSDoc::AddItem(CDrawObj* pDO)
{
	m_docItems.AddTail(pDO);
}

void CFSDoc::BuildDefaultsAttributes(IXMLDOMNode* pMainNode)
{
	defaults.clear();

	const AppXMLHelper& appXML = appXML.Instance();
	appXML.GetDrawObjectDefaults(pMainNode, defaults, true);
}

std::string CFSDoc::GetDefault(const char* pszName) const
{
	map<std::string, std::string>::const_iterator fnd = defaults.find(pszName);
	if ( fnd != defaults.end() )
		return (*fnd).second.c_str();
	else
		return "";
}

BOOL CFSDoc::FromXML(const char* pszFileName)
{
	CComPtr<IXMLDOMDocument2> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument60));

	if ( FAILED(hr) )
		return FALSE;

	VARIANT_BOOL bSuccess = false;

	hr = pXMLDoc->load(CComVariant(pszFileName), &bSuccess);
	pXMLDoc->setProperty(L"SelectionLanguage", CComVariant(L"XPath"));

	CComPtr<IXMLDOMNode> pMainNode;
	hr = pXMLDoc->selectSingleNode(CComBSTR("FSHEMA"), &pMainNode);

	if ( FAILED(hr) || !bSuccess )
		return FALSE;
	
	
	BuildDefaultsAttributes(pMainNode);

	std::string strVal;

	USES_CONVERSION;

	AppImagesHelper& appImages = AppImagesHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();

	if ( appXML.GetNodeAttributeByName(pMainNode, "TENDENCEIMAGE",	strVal) )
	{
		if (!ti.Create(appImages.GetImage(strVal)))
			return FALSE;
	}

	if (appXML.GetNodeAttributeByName(pMainNode, "DSPREFIX", strVal) )
		dspref = strVal.c_str();
	

	bDebugMode = false;
	if (appXML.GetNodeAttributeByName(pMainNode, "EDITMODE",	strVal) )
		bDebugMode = atoi(strVal.c_str()) != 0;
	else
		bDebugMode = NTCCProject::Instance().IsDebugMode();


	m_lLBMouseHoldInterval = 500; //время удержания левой кнопки мыши для срабатывания
	if (appXML.GetNodeAttributeByName(pMainNode, "LBDHOLDINTERVAL",	strVal) )
		m_lLBMouseHoldInterval = atol(strVal.c_str());

	m_lUpdateInterval = DataCollector::Instance().GetInterval(); //время обновления окна
	if (appXML.GetNodeAttributeByName(pMainNode, "UPDATEINTERVAL",	strVal) )
		m_lUpdateInterval = atol(strVal.c_str());

	std::string iconPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_IMAGES);
	if (appXML.GetNodeAttributeByName(pMainNode, "ICON",	strVal) )
		iconPath += strVal;

	m_hDocIcon = (HICON)LoadImage(NULL, iconPath.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

	if (appXML.GetNodeAttributeByName(pMainNode, "TITLE",	strVal) )
		m_strEmmebedDocTitle = strVal.c_str();	


	if (appXML.GetNodeAttributeByName(pMainNode, "MSGLISTPOS",	strVal) )
	{
		CString tmp = strVal.c_str();
		int pos = 0;
		CString x = tmp.Tokenize(" ;", pos); 
		rcMsgList.left = atoi(x);

		CString y = tmp.Tokenize(" ;", pos); 
		rcMsgList.top = atoi(y);

		CString w = tmp.Tokenize(" ;", pos); 
		rcMsgList.right = rcMsgList.left + atoi(w);

		CString h = tmp.Tokenize(" ;", pos); 
		rcMsgList.bottom = rcMsgList.top + atoi(h);
	}

	
	//********************************************************************

	AppLoadingHelper& LH = AppLoadingHelper::Instance();
	long count = appXML.GetNodesCount(pXMLDoc, "//CONTROLBAR | //LAYER/ITEM");
	////TRACE("\t---- FSDoc nodes count : %d\n", count);

	CString title = "Scheme «" + m_strEmmebedDocTitle + "»";

	long lSecID = LH.AddSection((LPCTSTR)title, pszFileName, count);

	CComPtr<IXMLDOMNodeList> pLayersList;
	hr = pMainNode->selectNodes(CComBSTR("LAYER"), &pLayersList);

	long layerCount = 0;
	pLayersList->get_length(&layerCount);
	pLayersList->reset();

	for(int i = 0; i < layerCount; i++)
	{
		
		CComPtr<IXMLDOMNode>	 pLayerNode;
		pLayersList->get_item(i, &pLayerNode);		

		FSLayer* pLayer = new FSLayer(this);

		if ( !pLayer->CreateFromXML(pLayerNode) )
			delete pLayer;
		else
		{
			CComPtr<IXMLDOMNamedNodeMap> pLayerAttr;
			pLayerNode->get_attributes(&pLayerAttr);	
			
			layers.push_back( pLayer );
		}
	}

	CComPtr<IXMLDOMNode> pBarsNode;
	hr = pMainNode->selectSingleNode(CComBSTR("CONTROLBARS"), &pBarsNode);

	if ( SUCCEEDED(hr) && pBarsNode != NULL )		
	{
		CComPtr<IXMLDOMNodeList> pBarsList;
		hr = pBarsNode->selectNodes(CComBSTR("CONTROLBAR"), &pBarsList);

		long lBarCount;	

		pBarsList->get_length(&lBarCount);		
		pBarsList->reset();

		for(int i = 0; i < lBarCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pBar;
			pBarsList->get_item(i, &pBar);

			std::string val;
			CONTROLBAR_INFO barInfo;

			appXML.GetNodeAttributeByName(pBar, "TYPE", val);
			barInfo.type = val;

			appXML.GetNodeAttributeByName(pBar, "TITLE", val);
			barInfo.title = val;

			appXML.GetNodeAttributeByName(pBar, "ICON", val);
			barInfo.iconPath = val;

			appXML.GetNodeAttributeByName(pBar, "DISABLED", val);
			barInfo.disabledDS = val;
			
			CComPtr<IXMLDOMNodeList>	 pItems;
			pBar->selectNodes( CComBSTR("ITEM"),  &pItems);

			long lItemCount;	

			std::vector<std::string> strItems;

			pItems->get_length(&lItemCount);
			pItems->reset();

			for (int j=0; j < lItemCount; j++)
			{
				CComPtr<IXMLDOMNode>	 pItem;
				pItems->get_item(j, &pItem);
				CONTROLBAR_ITEM_INFO cbii;

				if (!appXML.GetNodeAttributeByName(pItem, "ID", val) )
					continue;

				val = dspref + val;
				cbii.dsid = val.c_str();

				cbii.colorid = 0;
				if ( appXML.GetNodeAttributeByName(pItem, "BKCOLORID", val) )
					cbii.colorid = atoi(val.c_str());

				cbii.precition = 1.0;
				if ( appXML.GetNodeAttributeByName(pItem, "PRECITION", val) )
					cbii.precition = atof(val.c_str());

				cbii.autoapply = false;
				if ( appXML.GetNodeAttributeByName(pItem, "AUTOAPPLY", val) )
					cbii.autoapply = atoi(val.c_str()) > 0;
				
				////TRACE("\n\t%s bar precition (%s) (%f) ", cbii.dsid.c_str(), val.c_str(), cbii.precition );

				barInfo.cbitems.push_back(cbii);
			}


			//**********************************************************
			//PPRESETS
			CComPtr<IXMLDOMNode>	 pPresetsNode;
			hr = pBar->selectSingleNode( CComBSTR("PRESETS"),  &pPresetsNode);
			
			
			if ( SUCCEEDED(hr) && pPresetsNode != NULL )
			{

				CComPtr<IXMLDOMNodeList>	 pPresets;
				hr = pPresetsNode->selectNodes( CComBSTR("PRESET"),  &pPresets);

				long lPresetCount;	

				pPresets->get_length(&lPresetCount);
				pPresets->reset();

				for (int j = 0; j < lPresetCount; j++)
				{
					CComPtr<IXMLDOMNode>	 pPreset;
					pPresets->get_item(j, &pPreset);

					CONTROLBAR_PRESET_INFO cbpi;

					if ( !appXML.GetNodeAttributeByName(pPreset, "TITLE", val) )
							continue;

					cbpi.title = val.c_str();

					cbpi.icon = "";
					if ( appXML.GetNodeAttributeByName(pPreset, "ICON", val) )
						cbpi.icon = NTCCProject::Instance().GetPath(NTCCProject::PATH_IMAGES, val);

					CComPtr<IXMLDOMNodeList>	 pPresetsItems;
					hr = pPreset->selectNodes( CComBSTR("ITEM"),  &pPresetsItems);

					long lItemCount;
					pPresetsItems->get_length(&lItemCount);
					pPresetsItems->reset();

					for (int k=0; k < lItemCount; k++)
					{
						CComPtr<IXMLDOMNode>	 pPresetItem;
						pPresetsItems->get_item(k, &pPresetItem);

						PRESET_ITEM pi;
						if ( !appXML.GetNodeAttributeByName(pPresetItem, "DSID", val) )
							continue;

						val = dspref + val;
						pi.name = val.c_str();
						if ( !appXML.GetNodeAttributeByName(pPresetItem, "VALUE", val) )
							continue;

						pi.val = atof(val.c_str());
						cbpi.presetItems.push_back(pi);								
					}

					barInfo.cbpresets.push_back(cbpi);
				}
			}
			//*********************************************************


			bars.push_back(barInfo);
		}
	}

	CComPtr<IXMLDOMNode> pGraphNode;
	hr = pMainNode->selectSingleNode(CComBSTR("GRAPHICWND"), &pGraphNode);

	if ( SUCCEEDED(hr) && pGraphNode != NULL )		
	{
		CComPtr<IXMLDOMNodeList> pGroupsList;
		hr = pGraphNode->selectNodes(CComBSTR("GROUP"), &pGroupsList);

		long lGroupCount;	

		pGroupsList->get_length(&lGroupCount);		
		pGroupsList->reset();

		for(int i = 0; i < lGroupCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pGroup;
			pGroupsList->get_item(i, &pGroup);

			std::string val;
			GRAPHIC_GROUP grGroup;

			if ( !appXML.GetNodeAttributeByName(pGroup, "TITLE", val))
				continue;
			
			grGroup.title = val.c_str();

			if ( appXML.GetNodeAttributeByName(pGroup, "XATITLE", val))
				grGroup.xAxisTitle = val.c_str();

			if ( appXML.GetNodeAttributeByName(pGroup, "YLATITLE", val))
				grGroup.yLeftAxisTitle = val.c_str();

			if ( appXML.GetNodeAttributeByName(pGroup, "YRATITLE", val))
				grGroup.yRightAxisTitle = val.c_str();

			grGroup.yLeftAxisLblFormat = "%5.2f";
			if ( appXML.GetNodeAttributeByName(pGroup, "YLLBLFMT", val))
				grGroup.yLeftAxisLblFormat = val.c_str();

			grGroup.yRightAxisLblFormat = "%5.2f";
			if ( appXML.GetNodeAttributeByName(pGroup, "YRLBLFMT", val))
				grGroup.yRightAxisLblFormat = val.c_str();
		
			double maxY = 0.0;
			if (appXML.GetNodeAttributeByName(pGroup, "MAXYL", val) )
				maxY = atof(val.c_str());

			double minY = 0.0;
			if (appXML.GetNodeAttributeByName(pGroup, "MINYL", val) )
				minY = atof(val.c_str());

			grGroup.yLeftAxisRange = std::pair<double, double >(minY, maxY);

			maxY = 0.0;
			if (appXML.GetNodeAttributeByName(pGroup, "MAXYR", val) )
				maxY = atof(val.c_str());

			minY = 0.0;
			if (appXML.GetNodeAttributeByName(pGroup, "MINYR", val) )
				minY = atof(val.c_str());
			
			grGroup.yRightAxisRange = std::pair<double, double >(minY, maxY);


			CComPtr<IXMLDOMNodeList> pItems;
			hr = pGroup->selectNodes(CComBSTR("ITEM"), &pItems);

			if ( SUCCEEDED(hr) && pItems != NULL )		
			{
				long lItemCount;
				pItems->get_length(&lItemCount);
				pItems->reset();

				vector<std::string> dsIDsLeft;
				vector<std::string> dsIDsRight;
				vector<int> linesWidths;
				vector<int> linesTypes;
				vector<std::string> linesPatterns;

				

				for (int j=0; j < lItemCount; j++)
				{
					CComPtr<IXMLDOMNode> pItem;
					pItems->get_item(j, &pItem);

					if (!appXML.GetNodeAttributeByName(pItem, "DSID", val) )
						continue;
					
					val = dspref + val;
					std::string tmp;
					if (appXML.GetNodeAttributeByName(pItem, "RYAXIS", tmp) )
					{
						if ( atoi ( tmp.c_str() ) > 0 )
							dsIDsRight.push_back(val);
						else 
							dsIDsLeft.push_back(val);
					}
					else 
						dsIDsLeft.push_back(val);
						

					int lw = 2;
					if (appXML.GetNodeAttributeByName(pItem, "LW", val) )
						lw = atoi(val.c_str());
					linesWidths.push_back(lw);

					int lt = 1;
					if (appXML.GetNodeAttributeByName(pItem, "LT", val) )
						lt = atoi(val.c_str());
					linesTypes.push_back(lt);

					std::string lp = "2 3";
					if (appXML.GetNodeAttributeByName(pItem, "LP", val) )
						lp = val;
					linesPatterns.push_back(lp);
				}			
				grGroup.dslya = dsIDsLeft;
				grGroup.dsrya = dsIDsRight;
				
				grGroup.lp = linesPatterns;
				grGroup.lt = linesTypes;
				grGroup.lw = linesWidths;

				grGroups.push_back(grGroup);
			}
		}

		CComPtr<IXMLDOMNode> pCheckPointsNode;
		hr = pGraphNode->selectSingleNode(CComBSTR("CHECKPOINTS"), &pCheckPointsNode);

		if ( SUCCEEDED(hr) && pCheckPointsNode != NULL )		
		{
			CComPtr<IXMLDOMNodeList> pItems;
			hr = pCheckPointsNode->selectNodes(CComBSTR("ITEM"), &pItems);

			if ( SUCCEEDED(hr) && pItems != NULL )		
			{
				long lItemCount;
				pItems->get_length(&lItemCount);
				pItems->reset();

				std::map<std::string, bool>  dsIDs;

				for (int j=0; j < lItemCount; j++)
				{
					CComPtr<IXMLDOMNode>	 pItem;
					pItems->get_item(j, &pItem);
					
					
					std::string val;
					if (!appXML.GetNodeAttributeByName(pItem, "DSID", val) )
						continue;
					
					val = dspref + val;
					DataSrc* pDS = DataSrc::GetDataSource(val.c_str());

					if ( pDS == NULL || dsIDs.find(val.c_str()) != dsIDs.end() )
						continue;

					string id = val.c_str();
					
					AnalogOutputDataSrc* pADS = dynamic_cast<AnalogOutputDataSrc*>(pDS);
					if ( pADS != NULL )
					{
						dsIDs.insert(std::make_pair(id, true));
						pADS->AddOnOutputValueChange( boost::bind( &CFSDoc::OnAnalogDSChanged,  this, _1) );
					}

					DiscreteDataSrc* pDDS = dynamic_cast<DiscreteDataSrc*>(pDS);
					if ( pDDS != NULL ) 
					{
						dsIDs.insert(std::make_pair(id, true));
						pDDS->AddOnChangeState(boost::bind( &CFSDoc::OnDiscreteDSChanged,  this, _1) );
					}
					
				}
			}	
		}

		CComPtr<IXMLDOMNode> pRegionsNode;
		hr = pGraphNode->selectSingleNode(CComBSTR("REGIONS"), &pRegionsNode);

		if ( SUCCEEDED(hr) && pRegionsNode != NULL )		
		{
			CComPtr<IXMLDOMNodeList> pItems;
			hr = pRegionsNode->selectNodes(CComBSTR("ITEM"), &pItems);

			if ( SUCCEEDED(hr) && pItems != NULL )		
			{
				long lItemCount;
				pItems->get_length(&lItemCount);
				pItems->reset();

				for (int j=0; j < lItemCount; j++)
				{
					CComPtr<IXMLDOMNode>	 pItem;
					pItems->get_item(j, &pItem);
					
					REGIONINFO ri;

					std::string val;
					if (!appXML.GetNodeAttributeByName(pItem, "SRCID", val) )
						continue;

					DataSrc* pDS = DataSrc::GetDataSource(val.c_str());
					if ( pDS != NULL )
						ri.pDS = pDS;

					BaseTimer * pTimer = BaseTimer::GetTimer(val.c_str());
					if ( ri.pDS == NULL && pTimer != NULL )
					{
						ri.pTimer = pTimer;
						pTimer->AddTimerHandler(BaseTimer::STARTHANDLER, boost::bind( &CFSDoc::OnTimerStart,  this, _1) );
						pTimer->AddTimerHandler(BaseTimer::STOPHANDLER, boost::bind( &CFSDoc::OnTimerStop,  this, _1) );
						pTimer->AddTimerHandler(BaseTimer::OVERFLOWHANDLER, boost::bind( &CFSDoc::OnTimerOverflowed,  this, _1) );
					}

					if (appXML.GetNodeAttributeByName(pItem, "TITLE", val) )
						ri.title.text = val.c_str();

					ri.vertical = true;
					if (appXML.GetNodeAttributeByName(pItem, "VERTICAL", val) )
						ri.vertical = atoi( val.c_str() ) > 0;


					CComPtr<IXMLDOMNode>	 pFontNode;
					hr = pItem->selectSingleNode(CComBSTR("FONT"), &pFontNode);

					if (SUCCEEDED(hr) && pFontNode != NULL)
					{
						if (appXML.GetNodeAttributeByName(pFontNode, "NAME", val) )
							ri.title.fontname = val.c_str();

						if (appXML.GetNodeAttributeByName(pFontNode, "SIZE", val) )
							ri.title.fontsize = 2*atoi(val.c_str());

						if (appXML.GetNodeAttributeByName(pFontNode, "COLOR", val) )
							ri.title.color = strtol(val.c_str(), 0, 0);
					}

					CComPtr<IXMLDOMNode>	 pColorNode;
					hr = pItem->selectSingleNode(CComBSTR("COLOR"), &pColorNode);

					if (SUCCEEDED(hr) && pColorNode != NULL)
					{
						if (appXML.GetNodeAttributeByName(pColorNode, "FROM", val) )
							ri.color.from =	strtol(val.c_str(), 0, 0);
							
						ri.color.to = ri.color.from;
						if (appXML.GetNodeAttributeByName(pColorNode, "TO", val) )
							ri.color.to = strtol(val.c_str(), 0, 0);
					}

					CComPtr<IXMLDOMNode>	 pOpacityNode;
					hr = pItem->selectSingleNode(CComBSTR("OPACITY"), &pOpacityNode);

					if (SUCCEEDED(hr) && pOpacityNode != NULL)
					{
						if (appXML.GetNodeAttributeByName(pOpacityNode, "FROM", val) )
							ri.opacity.from =	strtol(val.c_str(), 0, 0);

						ri.opacity.to = ri.opacity.from;
						if (appXML.GetNodeAttributeByName(pOpacityNode, "TO", val) )
							ri.opacity.to = strtol(val.c_str(), 0, 0);
					}

					CComPtr<IXMLDOMNode>	 pHatchNode;
					hr = pItem->selectSingleNode(CComBSTR("HATCH"), &pHatchNode);

					if (SUCCEEDED(hr) && pHatchNode != NULL)
					{
						if (appXML.GetNodeAttributeByName(pHatchNode, "COLOR", val) )
							ri.hatch.color =	strtol(val.c_str(), 0, 0);

						if (appXML.GetNodeAttributeByName(pHatchNode, "STYLE", val) )
							ri.hatch.type = atoi( val.c_str() );
					}

					CComPtr<IXMLDOMNode>	 pBorderNode;
					hr = pItem->selectSingleNode(CComBSTR("BORDER"), &pBorderNode);

					if (SUCCEEDED(hr) && pBorderNode != NULL)
					{
						if (appXML.GetNodeAttributeByName(pBorderNode, "COLOR", val) )
							ri.border.color =	strtol(val.c_str(), 0, 0);

						if (appXML.GetNodeAttributeByName(pBorderNode, "LT", val) )
							ri.border.type = atoi( val.c_str() );

						if (appXML.GetNodeAttributeByName(pBorderNode, "LW", val) )
							ri.border.width = atoi( val.c_str() );

						if (appXML.GetNodeAttributeByName(pBorderNode, "LP", val) )
							ri.border.pat = val.c_str();
					}
					
					regions.push_back(ri);
				}
			}
		}
	}
		
	LH.CloseCurrentSection();

	return TRUE;

}

void CFSDoc::UpdateFSDocTitle()
{
	if ( !m_strEmmebedDocTitle.IsEmpty() )
		SetTitle(m_strEmmebedDocTitle);	
}

void CFSDoc::DeleteContents()
{
	//удаляем все элементы
	std::list<FSLayer*>::iterator it = layers.begin() ;
	
	for(;it != layers.end(); ++it)
		delete (*it);
	
	layers.clear();
	grGroups.clear();
	styles.clear();
	defaults.clear();

	m_docItems.RemoveAll();
	
	CDocument::DeleteContents();
}


void CFSDoc::OnCloseDocument()
{
	m_bAutoDelete = FALSE;
	CDocument::OnCloseDocument();
}


void CFSDoc::GetDSGroup(std::string name, LPGRAPHIC_GROUP grGroup) const
{
	
	for(std::vector<GRAPHIC_GROUP>::const_iterator itGr = grGroups.begin();itGr != grGroups.end(); ++itGr)
	{
		vector<string> dsNames;
		dsNames.insert(dsNames.end(), (*itGr).dslya.begin(), (*itGr).dslya.end());
		dsNames.insert(dsNames.end(), (*itGr).dsrya.begin(), (*itGr).dsrya.end());
		
		for(std::vector<std::string>::const_iterator itDS = dsNames.begin();itDS != dsNames.end(); ++itDS)
		{
			if ( (*itDS) == name )
			{
				(*grGroup) = (*itGr);
				return;
			}
		}
	}
}

void CFSDoc::OnAnalogDSChanged(const AnalogOutputDataSrc* pADS)
{
	//CPtrList lst;
	//lst.AddTail((void*)pADS);
	//UpdateAllViews(NULL, HINT_ADD_CHECKPOINT, &lst);

// 	POSITION pos = GetFirstViewPosition();
// 	while ( pos != NULL )
// 	{
// 		TRACE("\n\tOnAnalogDSChanged");
// 		CView* pView = GetNextView(pos);
// 		::SendMessage(pView->m_hWnd, RM_ADD_CHECKPOINT, 0, (LPARAM)pADS);
// 	}

	SEND_MESSAGE_TO_ALL_VIEWS(RM_ADD_CHECKPOINT, 0, pADS);
}

void CFSDoc::OnDiscreteDSChanged(const DiscreteDataSrc* pDDS)
{
	//CPtrList lst;
	//lst.AddTail((void*)pDDS);
	if( pDDS->IsEnabled() )
	{
		//UpdateAllViews(NULL, HINT_ADD_CHECKPOINT, &lst);

// 		POSITION pos = GetFirstViewPosition();
// 		while ( pos != NULL )
// 		{
// 			TRACE("\n\tOnDiscreteDSChanged");
// 			CView* pView = GetNextView(pos);
// 			::SendMessage(pView->m_hWnd, RM_ADD_CHECKPOINT, 0, (LPARAM)pDDS);
// 		}
		SEND_MESSAGE_TO_ALL_VIEWS(RM_ADD_CHECKPOINT, 0, pDDS);
	}
}

void CFSDoc::OnTimerStart(const BaseTimer* pTimer)
{
	
// 	POSITION pos = GetFirstViewPosition();
// 	while ( pos != NULL )
// 	{
// 		TRACE("\n\tOnDiscreteDSChanged");
// 		CView* pView = GetNextView(pos);
// 		::SendMessage(pView->m_hWnd, RM_TIMER_STARTED, 0, (LPARAM)pTimer);
// 	}

	SEND_MESSAGE_TO_ALL_VIEWS(RM_TIMER_STARTED, 0, pTimer);
}

void CFSDoc::OnTimerStop(const BaseTimer* pTimer)
{
	SEND_MESSAGE_TO_ALL_VIEWS(RM_TIMER_STOPED, 0, pTimer);
}

void CFSDoc::OnTimerOverflowed(const BaseTimer* pTimer)
{
	SEND_MESSAGE_TO_ALL_VIEWS(RM_TIMER_OVERFLOWED, 0, pTimer);
}

Gdiplus::RectF CFSDoc::GetFirstViewRect() const
{
	Gdiplus::RectF rcView(0.0, 0.0, 0.0, 0.0);
	POSITION pos = GetFirstViewPosition(); 

	while ( pos != NULL ) 
	{	
		CView* pView = GetNextView(pos); 
		RECT rc;
		pView->GetClientRect(&rc);

		rcView.Width  = Gdiplus::REAL(rc.right - rc.left);
		rcView.Height = Gdiplus::REAL(rc.bottom - rc.top);
	}

	return rcView;
}

bool CFSDoc::AddStyle(std::string name, const DrawItemStyle& dis) 
{
	if ( GetStyle(name) != NULL )
		return false;

	styles.insert(std::make_pair(name, dis.Clone() ) );
	return true;
}

//получение стиля по имени
DrawItemStyle* CFSDoc::GetStyle(std::string name) const
{
	std::map<std::string, DrawItemStyle*>::const_iterator fnd;
	fnd =  styles.find( name );

	if ( fnd == styles.end() )
		return NULL;
	else
		return (*fnd).second;
}


bool CFSDoc::AddBrush(std::string name, const Gdiplus::Brush& brush) 
{
	if ( GetBrush(name) != NULL )
		return false;

	brushes.insert(std::make_pair(name, brush.Clone() ) );
	return true;
}

//получение кисти по имени
Gdiplus::Brush* CFSDoc::GetBrush(std::string name) const
{
	std::map<std::string, Gdiplus::Brush*>::const_iterator fnd;
	fnd =  brushes.find( name );

	if ( fnd == brushes.end() )
		return NULL;
	else
		return (*fnd).second;
}


bool CFSDoc::AddPen(std::string name, const Gdiplus::Pen& pen) 
{
	if ( GetPen(name) != NULL )
		return false;

	pens.insert(std::make_pair(name, pen.Clone() ) );
	return true;
}


//получение пера по имени
Gdiplus::Pen* CFSDoc::GetPen(std::string name) const
{
	std::map<std::string, Gdiplus::Pen*>::const_iterator fnd;
	fnd =  pens.find( name );

	if ( fnd == pens.end() )
		return NULL;
	else
		return (*fnd).second;
}

//добавить шрифт
bool CFSDoc::AddFont(std::string name, const DrawItemFont& font) 
{
	if ( GetFont(name) != NULL )
		return false;

	fonts.insert(std::make_pair(name, font.Clone() ) );
	return true;
}

//получение шрифта по имени
DrawItemFont* CFSDoc::GetFont(std::string name) const
{
	std::map<std::string, DrawItemFont*>::const_iterator fnd;
	fnd =  fonts.find( name );

	if ( fnd == fonts.end() )
		return NULL;
	else
		return (*fnd).second;
}


//////////////////////////////////////////////////////////////////////////
//  [10/12/2015 Johnny A. Matveichik
DrawItemFont::DrawItemFont(const CFSDoc* doc) :
ang(0.0f), parent(doc), font(NULL), fontbrush(NULL)
{
	
}

DrawItemFont::~DrawItemFont()
{

}

//Создание копии
DrawItemFont* DrawItemFont::Clone() const
{
	DrawItemFont* p = new DrawItemFont(parent);
	return p;
}

//создание шрифта из XML узла
BOOL DrawItemFont::CreateFromXML(IXMLDOMNode* pNode)
{
	ASSERT(parent);

	std::string  strVal;
	AppXMLHelper& appXML = AppXMLHelper::Instance();

	//имя стиля шрифта  
	if( appXML.GetNodeAttributeByName(pNode, "NAME", strVal) )
		return FALSE;	

	//шрифт уже присутствует в списке 
	if ( parent->GetFont( strVal) != NULL )
		return FALSE;

	//Имя шрифта
	std::string family;
	if ( !appXML.GetNodeAttributeByName(pNode, "FAMILY", family) )
		family = parent->GetDefault("FONT.NAME");

	//Размер шрифта
	Gdiplus::REAL fontSz = 0.0f;
	if ( !appXML.GetNodeAttributeByName(pNode, "SIZE", strVal) )
		strVal = parent->GetDefault("FONT.SIZE");

	fontSz = (Gdiplus::REAL)atof(strVal.c_str());					

	//Стиль шрифта
	std::string fStyle;
	if ( !appXML.GetNodeAttributeByName(pNode, "STYLE", fStyle) )
		fStyle = parent->GetDefault("FONT.STYLE");
	
	INT fs = Gdiplus::FontStyleRegular;

	
	boost::algorithm::to_lower(fStyle);
	if ( boost::algorithm::contains(fStyle, "bold") )
		fs |=  Gdiplus::FontStyleBold;

	if ( boost::algorithm::contains(fStyle, "italic") )
		fs |=  Gdiplus::FontStyleItalic;

	if ( boost::algorithm::contains(fStyle, "underline") )
		fs |=  Gdiplus::FontStyleUnderline;

	if ( boost::algorithm::contains(fStyle, "strikeout") )
		fs |=  Gdiplus::FontStyleStrikeout;

	fontstyle = Gdiplus::FontStyle(fs);

	//Угол поворота	
	if ( !appXML.GetNodeAttributeByName(pNode, "ANGLE", strVal) )
		strVal = parent->GetDefault("FONT.ANGLE");

	ang = (Gdiplus::REAL)atof( strVal.c_str() );


	//Выравнивание
	CComPtr<IXMLDOMNode> pAlignNode;  
	HRESULT hr = pNode->selectSingleNode(CComBSTR("ALIGN"), &pAlignNode);

	//Выравнивание текста в прямоугольнике по умолчанию
	sf.SetAlignment(Gdiplus::StringAlignmentCenter);
	sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	sf.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox|
		Gdiplus::StringFormatFlagsMeasureTrailingSpaces|
		Gdiplus::StringFormatFlagsDisplayFormatControl|
		Gdiplus::StringFormatFlagsLineLimit);

	std::string strVAlign = parent->GetDefault("ALIGN.VALIGN");
	std::string strHAlign = parent->GetDefault("ALIGN.HALIGN");

	if (SUCCEEDED(hr) && pAlignNode != NULL)
	{
		if ( appXML.GetNodeAttributeByName(pAlignNode, "HALIGN", strVal) )
			strHAlign = strVal;
		else 
			strHAlign = parent->GetDefault("ALIGN.HALIGN");		

		if ( appXML.GetNodeAttributeByName(pAlignNode, "VALIGN", strVal) )
			strVAlign = strVal;
		else 
			strVAlign = parent->GetDefault("ALIGN.VALIGN");
	}
	
	boost::algorithm::to_lower(strHAlign);

	if (  strHAlign == "left")
		sf.SetAlignment(Gdiplus::StringAlignmentNear);
	else if (strHAlign == "right")
		sf.SetAlignment(Gdiplus::StringAlignmentFar);
	else if (strHAlign == "center")
		sf.SetAlignment(Gdiplus::StringAlignmentCenter);

	boost::algorithm::to_lower(strVAlign);
	if ( strVAlign == "top")
		sf.SetLineAlignment(Gdiplus::StringAlignmentNear);
	else if (strVAlign == "bottom")
		sf.SetLineAlignment(Gdiplus::StringAlignmentFar);
	else if (strVAlign == "center")
		sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	
	return TRUE;
}


//Шрифт 
const Gdiplus::Font* DrawItemFont::GetFont() const
{
	return font;
}

// Выравнивание текста
const Gdiplus::StringFormat& DrawItemFont::GetStringFormat() const
{
	return	sf;
}

// Заполнение 
const Gdiplus::Brush*	DrawItemFont::GetFontBrush() const
{	
	return fontbrush;
}

//Стиль шрифта
const Gdiplus::FontStyle& DrawItemFont::GetFontStyle() const
{
	return fontstyle;
}

//////////////////////////////////////////////////////////////////////////
//  [10/12/2015 Johnny A. Matveichik]
DrawItemStyle::DrawItemStyle(const CFSDoc* doc) :
	font(NULL)	
{
}

DrawItemStyle::~DrawItemStyle(void)
{
	Clear();
}

DrawItemStyle* DrawItemStyle::Clone() const
{
	return new DrawItemStyle(parent);
}

void DrawItemStyle::Clear()
{
	for (std::vector<Gdiplus::Brush*>::iterator pos = brushes.begin(); pos != brushes.end(); ++pos )
	{
		Gdiplus::Brush* pBrush = (*pos);		
		delete pBrush;
	}
	brushes.clear();

	
	for (std::vector<Gdiplus::Pen*>::iterator pos = pens.begin(); pos != pens.end(); ++pos )
	{
		Gdiplus::Pen* pPen = (*pos);
		delete pPen;
	}
	pens.clear();

	if ( font != NULL)
	{
		delete font;
		font = NULL;
	}
	
}

//создание кисти из XML узла
Gdiplus::Brush* DrawItemStyle::BrushFromNode(IXMLDOMNode* pBrushNode)
{
	Gdiplus::Brush* pBrush = NULL; 

	/*
	Gdiplus::REAL ang = 0;
	if( appXML.GetNodeAttributeByName(pStyleNode, "ANGLE",	strVal) )
	ang = boost::lexical_cast<Gdiplus::REAL>((LPCTSTR)strVal);

	std::string colors;
	if( appXML.GetNodeAttributeByName(pStyleNode, "COLORS",	strVal) )
	colors = (LPCTSTR)strVal;

	std::vector<std::string> parts;
	boost::split(parts, colors, std::bind2nd(std::equal_to<char>(), ';'));

	std::map<Gdiplus::REAL, long> gradient; 
	for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
	{
	std::vector<std::string> gradientPoint;
	it->erase( std::remove(it->begin(), it->end(), ' '), it->end() );

	boost::split(gradientPoint, (*it), std::bind2nd(std::equal_to<char>(), ','));

	if ( gradientPoint.size() != 2 )
	continue;

	Gdiplus::REAL pos;
	unsigned long clr;

	try {
	pos = boost::lexical_cast<Gdiplus::REAL>(gradientPoint[1].c_str());
	clr = boost::lexical_cast< hex_to<unsigned long> >(gradientPoint[0].c_str());
	}
	catch(boost::bad_lexical_cast& e)
	{
	e;
	continue;
	}
	gradient.insert( std::make_pair(pos, clr) );
	}

	Gdiplus::LinearGradientBrush* pBrush = new Gdiplus::LinearGradientBrush(Gdiplus::RectF(0.0,0.0,100.0,100.0), Gdiplus::Color::White, Gdiplus::Color::Black, ang);					

	std::vector<Gdiplus::REAL>  pPos;
	std::vector<Gdiplus::Color> pColors;

	for(std::map<Gdiplus::REAL, long>::const_iterator it = gradient.begin(); it != gradient.end(); ++it)
	{
	pPos.push_back(it->first);
	pColors.push_back(it->second);
	}

	pBrush->SetInterpolationColors(pColors.data(), pPos.data(), gradient.size());
	fillstyles.insert( std::make_pair(name, pBrush) );

	*/
	return pBrush;
}

//создание кисти из XML узла
Gdiplus::Pen* DrawItemStyle::PenFromNode(IXMLDOMNode* pBrushNode)
{
	Gdiplus::Pen* pPen = NULL; 
	
	return pPen;
}

//создание стиля из XML узла
BOOL DrawItemStyle::CreateFromXML(IXMLDOMNode* pNode)
{
	std::string strVal;
	AppXMLHelper& appXML = AppXMLHelper::Instance();

	//имя стиля должно быть задано
	if( !appXML.GetNodeAttributeByName(pNode, "NAME",	strVal) )
		return FALSE;	
	
	//имя стиля должно быть уникально для документа
	if (parent->GetStyle(strVal) != NULL)
		return FALSE;

	//********************************************************************
	// ЗАЛИВКИ - ПРИМЕНЯЮТСЯ В ПОРЯДКЕ ОПРЕДЕЛЕНИЯ
#pragma region ЗАЛИВКИ

	CComPtr<IXMLDOMNodeList> pFillNodes;
	HRESULT hr = pNode->selectNodes(CComBSTR("FILL"), &pFillNodes);

	if ( pFillNodes != NULL )
	{
		
		long fillCount = 0;
		pFillNodes->get_length(&fillCount);
		pFillNodes->reset();

		for(int i = 0; i < fillCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pFillNode;
			pFillNodes->get_item(i, &pFillNode);

			Gdiplus::Brush* pBrush = BrushFromNode(pFillNode);
			if ( pBrush == NULL )
				continue;
			else
				brushes.push_back(pBrush);			
		}
	}
#pragma endregion


	//********************************************************************
	// ОБВОДКИ - ПРИМЕНЯЮТСЯ В ПОРЯДКЕ ОПРЕДЕЛЕНИЯ
#pragma region ОБВОДКИ

	CComPtr<IXMLDOMNodeList> pStrokeNodes;
	hr = pNode->selectNodes(CComBSTR("STROKE"), &pStrokeNodes);

	if ( pStrokeNodes != NULL )
	{
		
		long strokeCount = 0;
		pStrokeNodes->get_length(&strokeCount);
		pStrokeNodes->reset();

		for(int i = 0; i < strokeCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pStrokeNode;
			pFillNodes->get_item(i, &pStrokeNode);

			Gdiplus::Pen* pPen = PenFromNode(pStrokeNode);
			if ( pPen == NULL )
				continue;
			else
				pens.push_back(pPen);
		}
	}
#pragma endregion


	return FALSE;
}



//Заливка
std::vector<Gdiplus::Brush*> DrawItemStyle::GetFills() const
{
	return brushes;
}

//Обводка
std::vector<Gdiplus::Pen*> DrawItemStyle::GetStrokes() const
{
	return pens;
}

//Шрифт 
const DrawItemFont* DrawItemStyle::GetFont() const
{
	return font;
}

