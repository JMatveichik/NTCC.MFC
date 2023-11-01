
#include "stdafx.h"

#include <Winnetwk.h>

#include "ah_xml.h"
#include "ah_network.h"

#include "Automation\DataSrc.h"
#include "Automation\common.h"
#include "NanoTubesControlCenter.h"

#pragma comment(lib, "mpr.lib")

AppXMLHelper::AppXMLHelper() 
{

}

AppXMLHelper::~AppXMLHelper()
{

}


void AppXMLHelper::GetDrawObjectDefaults(IXMLDOMNode* pNode, std::map<std::string, std::string>& defaults, bool useAppDefaults /*= true*/) const
{
	std::string strVal;
	CComPtr<IXMLDOMNode> pDefaultsNode;
	HRESULT hr = pNode->selectSingleNode(CComBSTR("DEFAULTS"), &pDefaultsNode);

	defaults.clear();

	if ( hr == S_OK )
	{
		CComPtr<IXMLDOMNode> pDefFont;
		hr = pDefaultsNode->selectSingleNode(CComBSTR("FONT"), &pDefFont);

		if ( hr == S_OK )
		{	
			if ( AppXMLHelper::GetNodeAttributeByName( pDefFont,"NAME", strVal) )
				defaults["FONT.NAME"] = strVal;
			else if ( useAppDefaults)
				defaults["FONT.NAME"] = "Tahoma";

			if ( AppXMLHelper::GetNodeAttributeByName( pDefFont,"SIZE", strVal) )
				defaults["FONT.SIZE"] = strVal;
			else if ( useAppDefaults)
				defaults["FONT.SIZE"] = "14";

			if ( AppXMLHelper::GetNodeAttributeByName( pDefFont,"COLOR", strVal) )
				defaults["FONT.COLOR"] = strVal;
			else if ( useAppDefaults)
				defaults["FONT.COLOR"] = "0x000000";

			if ( AppXMLHelper::GetNodeAttributeByName( pDefFont, "OPACITY", strVal) )
				defaults["FONT.OPACITY"] = strVal;
			else if ( useAppDefaults)
				defaults["FONT.OPACITY"] = "100";

			if ( AppXMLHelper::GetNodeAttributeByName( pDefFont, "STYLE", strVal) )
				defaults["FONT.STYLE"] = strVal;
			else if ( useAppDefaults)
				defaults["FONT.STYLE"] = "Normal";

			if ( AppXMLHelper::GetNodeAttributeByName( pDefFont, "ANGLE", strVal) )
				defaults["FONT.ANGLE"] = strVal;
			else if ( useAppDefaults)
				defaults["FONT.ANGLE"] = "0";
		}
		else if (useAppDefaults)
		{
			defaults["FONT.NAME"] = "Tahoma";
			defaults["FONT.SIZE"] = "14";
			defaults["FONT.COLOR"] = "0x000000";
			defaults["FONT.OPACITY"] = "100";
			defaults["FONT.STYLE"] = "Normal";
			defaults["FONT.ANGLE"] = "0";
		}

		CComPtr<IXMLDOMNode> pDefAlign;
		hr = pDefaultsNode->selectSingleNode(CComBSTR("ALIGN"), &pDefAlign);

		if (hr == S_OK)
		{	
			if ( AppXMLHelper::GetNodeAttributeByName( pDefAlign,"HALIGN", strVal) )
				defaults["ALIGN.HALIGN"] = strVal;
			else if (useAppDefaults)
				defaults["ALIGN.HALIGN"] = "Left";


			if ( AppXMLHelper::GetNodeAttributeByName( pDefAlign,"VALIGN", strVal) )
				defaults["ALIGN.VALIGN"] =  strVal;
			else if (useAppDefaults)
				defaults["ALIGN.VALIGN"] = "Center";
		}
		else if (useAppDefaults)
		{
			defaults["ALIGN.HALIGN"] = "CENTER";
			defaults["ALIGN.VALIGN"] = "CENTER";
		}

		CComPtr<IXMLDOMNode> pDefStroke;
		hr = pDefaultsNode->selectSingleNode(CComBSTR("STROKE"), &pDefStroke);

		if (hr == S_OK)
		{	
			if ( AppXMLHelper::GetNodeAttributeByName( pDefStroke,"COLOR", strVal) )
				defaults["STROKE.COLOR"] = strVal;
			else if (useAppDefaults)
				defaults["STROKE.COLOR"] =  "0x000000";

			if ( AppXMLHelper::GetNodeAttributeByName( pDefStroke, "OPACITY", strVal) )
				defaults["STROKE.OPACITY"] = strVal;
			else if (useAppDefaults)
				defaults["STROKE.OPACITY"] = "0";

			if ( AppXMLHelper::GetNodeAttributeByName( pDefStroke, "WIDTH", strVal) )
				defaults["STROKE.WIDTH"] = strVal;
			else if (useAppDefaults)
				defaults["STROKE.WIDTH"] = "0";
		}
		else if (useAppDefaults)
		{
			defaults["STROKE.COLOR"] = "0x000000";
			defaults["STROKE.OPACITY"] = "0";
			defaults["STROKE.WIDTH"] = "0";
		}

		CComPtr<IXMLDOMNode> pDefFill;
		hr = pDefaultsNode->selectSingleNode(CComBSTR("FILL"), &pDefFill);

		if (hr == S_OK)
		{	
			if ( AppXMLHelper::GetNodeAttributeByName( pDefFill,"COLOR", strVal) )
				defaults["FILL.COLOR"] = strVal;
			else if (useAppDefaults)
				defaults["FILL.COLOR"] = "0x000000";

			if ( AppXMLHelper::GetNodeAttributeByName( pDefFill, "OPACITY", strVal) )
				defaults["FILL.OPACITY"] = strVal;
			else if (useAppDefaults)
				defaults["FILL.OPACITY"] = "0";
		}
		else if (useAppDefaults)
		{
			defaults["FILL.COLOR"] = "0x000000";
			defaults["FILL.OPACITY"] = "0";
		}
	}
// 	else {
// 		defaults["FONT.NAME"] = "Tahoma";
// 		defaults["FONT.SIZE"] = "14";
// 		defaults["FONT.COLOR"] = "0x000000";
// 		defaults["FONT.OPACITY"] = "100";
// 		defaults["FONT.STYLE"] = "Normal";
// 		defaults["FONT.ANGLE"] = "0";
// 	
// 		defaults["FILL.COLOR"] = "0x000000";
// 		defaults["FILL.OPACITY"] = "0";
// 	
// 		defaults["STROKE.COLOR"] = "0x000000";
// 		defaults["STROKE.OPACITY"] = "0";
// 		defaults["STROKE.WIDTH"] = "0";
// 
// 		defaults["ALIGN.HALIGN"] = "CENTER";
// 		defaults["ALIGN.VALIGN"] = "CENTER";
// 	}
}

BOOL AppXMLHelper::GetProjectSubPath(IXMLDOMNode* pMainNode, const char* pszType, std::string& dir) const
{
	if (pMainNode == NULL)
		return FALSE;

	CString basePath  = dir.c_str();
	dir = ""; 

	CString xPath;
	xPath.Format("PATHS//PATH[@TYPE='%s']", pszType);

	CComPtr<IXMLDOMNodeList> pPaths;
	HRESULT hr = pMainNode->selectNodes(CComBSTR((LPCTSTR)xPath), &pPaths);	

	if ( FAILED(hr) )
		return FALSE;

	long lCount;
	pPaths->get_length(&lCount);
	pPaths->reset();

	if ( lCount == 0 ) 
		return FALSE;

	CComPtr<IXMLDOMNode> pPath;
	pPaths->get_item(0, &pPath);

	
	std::string sval;
	if ( AppXMLHelper::GetNodeAttributeByName(pPath, "DIR", sval ) )
	{
		CString strProjectSubPath = sval.c_str();
		CString strDir;

		if ( strProjectSubPath.Find(":") != -1 )
			strDir = strProjectSubPath;
		else
			strDir = basePath + strProjectSubPath;

		dir = strDir.GetBuffer();
		strDir.ReleaseBuffer();
	}
	
	return (dir.size() != 0);
}

BOOL AppXMLHelper::GetNodeAttributeByName(IXMLDOMNode* pNode, const char* pszName, std::string& sval) const
{
	if (pNode == NULL)
		return FALSE;

	CComPtr<IXMLDOMNamedNodeMap> pNodeAtr;
	pNode->get_attributes(&pNodeAtr);

	return GetAttributeByName(pNodeAtr, pszName, sval);
}

BOOL AppXMLHelper::GetNodeAttributes(IXMLDOMNode* pNode, ATTRIBUTES& attrs) const
{
	attrs.clear();

	if (pNode == NULL)
		return FALSE;

	CComPtr<IXMLDOMNamedNodeMap> pNodeAtr;
	pNode->get_attributes(&pNodeAtr);
	
	long count;
	pNodeAtr->get_length(&count);
	pNodeAtr->reset();

	if (count  == 0)
		return FALSE;

	USES_CONVERSION;
	CComBSTR txt;
	CString iText;

	pNode->get_text(&txt);
	iText = W2A(txt);
	iText = iText.Trim();
	iText.Replace('\n',' ');
	iText.Replace('\t',' ');

	attrs.insert(std::make_pair("ITEXT", (LPCTSTR)iText));
	
	for(long c = 0; c < count; c++)
	{
		CComPtr<IXMLDOMNode> aItem;
		pNodeAtr->get_item(c, &aItem);

		
		CComBSTR bstrName;
		aItem->get_nodeName(&bstrName);

		USES_CONVERSION;
		CString strName = W2A( bstrName );
		
		VARIANT val;
		aItem->get_nodeValue(&val); 
		CString strVal = W2A( val.bstrVal );

		attrs.insert(std::make_pair((LPCTSTR)strName, (LPCTSTR)strVal));
	}

	

	return TRUE;
}

BOOL AppXMLHelper::ImportNodes(IXMLDOMNode* pParentNode, const char* pszImportNodesName, const char* pszXML) const 
{
	CComPtr<IXMLDOMDocument2> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument60));

	if ( FAILED(hr) )
		return FALSE;

	pXMLDoc->setProperty(L"SelectionLanguage", CComVariant(L"XPath"));

	VARIANT_BOOL bSuccess = false;
	
	//Пробуем загрузить из файла pszXML.
	if ( PathFileExists( pszXML) )
	{
		hr = pXMLDoc->load(CComVariant(pszXML), &bSuccess);
		if ( SUCCEEDED(hr) )
			return ImportNodes(pParentNode, pszImportNodesName, pXMLDoc);
	}
	else {

		Globals::CheckError(TRUE);

		//если не получилось пробуем загрузить pszXML текст
		hr = pXMLDoc->loadXML(CComBSTR(pszXML), &bSuccess);

		if ( SUCCEEDED(hr) )
			return ImportNodes(pParentNode, pszImportNodesName, pXMLDoc);
	}	

	return FALSE;
}

BOOL AppXMLHelper::ImportNodes(IXMLDOMNode* pParentNode, const char* pszImportNodesName, IXMLDOMDocument* pXMLDocFrom) const 
{
	//выбираем все узлы
	CString strXPath;
	strXPath.Format("//%s", pszImportNodesName);

	CComPtr<IXMLDOMNodeList> pImportedNodes;

	HRESULT hr = pXMLDocFrom->selectNodes(CComBSTR((LPCTSTR)strXPath), &pImportedNodes);
	if ( FAILED(hr) )
		return FALSE;

	long lImportNodesCount = 0;
	pImportedNodes->get_length(&lImportNodesCount);
	pImportedNodes->reset();

	for(long i = 0; i < lImportNodesCount; i++)
	{
		CComPtr<IXMLDOMNode>	 pNode;
		pImportedNodes->get_item(i, &pNode);

		CComPtr<IXMLDOMNode>	 pNewNode;
		hr  = pParentNode->appendChild(pNode, &pNewNode);
		
		if ( FAILED(hr) )
			continue;
	}

	return TRUE;
}

long AppXMLHelper::GetNodesCount(const char* pszFileName, const char* pszXPath) const
{

	CComPtr<IXMLDOMDocument> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) )
		return 0;

	VARIANT_BOOL bSuccess = false;
	hr = pXMLDoc->load(CComVariant(pszFileName), &bSuccess);

	if (FAILED(hr) || !bSuccess)
		return 0;

	return GetNodesCount(pXMLDoc, pszXPath);
}

long AppXMLHelper::GetNodesCount(IXMLDOMDocument* pXMLDoc, const char* pszXPath) const
{
	CComPtr<IXMLDOMNodeList> pSectionsNode;
	HRESULT hr = pXMLDoc->selectNodes(CComBSTR(pszXPath), &pSectionsNode);

	if (FAILED(hr) || pSectionsNode == NULL)
		return 0;

	long lSections = 0;
	pSectionsNode->get_length(&lSections);
	pSectionsNode->reset();

	return lSections;
}
BOOL AppXMLHelper::GetAttributeByName(IXMLDOMNamedNodeMap* pAttributesMap, const char* pszName, std::string& sval) const
{
	CComPtr<IXMLDOMNode>	pAttrNode;

	VARIANT val;
	CString strVal;

	HRESULT hr = pAttributesMap->getNamedItem(CComBSTR(pszName), &pAttrNode);
	if (FAILED(hr) || pAttrNode == NULL)
	{
		strVal = "";
		return FALSE;
	}

	pAttrNode->get_nodeValue(&val); 
	pAttrNode.p->Release();
	pAttrNode.p = NULL;

	USES_CONVERSION;
	strVal = W2A( val.bstrVal );
	strVal.Trim();
	
	sval = strVal.GetBuffer();
	strVal.ReleaseBuffer();

	return TRUE;
}

BOOL AppXMLHelper::GetGdiPlusColor(IXMLDOMNode* pNode, Gdiplus::Color& color, double defOpacity) const
{
	std::string sVal;

	if ( !AppXMLHelper::GetNodeAttributeByName(pNode, "COLOR", sVal) )
		return FALSE;

	COLORREF clr = strtoul(sVal.c_str(), 0, 0);
	BYTE b = GetBValue(clr);
	BYTE g = GetGValue(clr);
	BYTE r = GetRValue(clr);

	//непрозрачнорсть 
	BYTE a = 0xFF;
	double op = defOpacity;

	if ( AppXMLHelper::GetNodeAttributeByName(pNode, "OPACITY", sVal) )
		op = atof(sVal.c_str());

	a = BYTE( 255 - 255 * (100 - op) / 100 );
	color  = Gdiplus::Color(a, r, g, b); 

	return TRUE;
}

Gdiplus::Color AppXMLHelper::GetGdiPlusColor(CString strClr, CString strOp) const
{
	COLORREF clr = strtoul(strClr, 0, 0);
	BYTE b = GetBValue(clr);
	BYTE g = GetGValue(clr);
	BYTE r = GetRValue(clr);

	//непрозрачнорсть 
	BYTE a = 0xFF;
	double op = atof( strOp );

	if ( op < 0 ) 
		op = 0.0;

	if ( op > 100 ) 
		op = 100.0;

	a = BYTE( 255 - 255 * (100 - op) / 100 );
	return Gdiplus::Color(a, r, g, b); 
}

BOOL AppXMLHelper::AddNodeAtrribute(IXMLDOMDocument* pXMLDoc, IXMLDOMNode* pNode, CString strName, CString strVal) const
{
// 	if (strVal.IsEmpty())
// 		return FALSE;

	CComPtr<IXMLDOMAttribute> pAttr;
	CComPtr<IXMLDOMNamedNodeMap> pNodeAtrs;

	HRESULT hr = pNode->get_attributes(&pNodeAtrs);
	
	if (FAILED(hr))
		return FALSE;
	
	hr = pXMLDoc->createAttribute( CComBSTR(strName), &pAttr );
	
	if( FAILED(hr) )
		return FALSE;

	hr = pAttr->put_nodeValue( CComVariant(strVal) );
	if( FAILED(hr) )
		return FALSE;

	hr = pNodeAtrs->setNamedItem(pAttr, NULL);
	if( FAILED(hr) )
		return FALSE;

	return TRUE;
}

bool GetDSInfo(const DataSrc* pDS, CString& sType, CString& sDev, int& nWire)
{
	const AnalogInputDataSrc*  pAIDS = dynamic_cast<const AnalogInputDataSrc*>(pDS);

	if (  pAIDS != NULL )
	{
		IdentifiedPhysDevice* pDev = dynamic_cast<IdentifiedPhysDevice*>( pAIDS->GetParentDevice() );
		nWire = (int)pAIDS->GetChannelNo();
		sType = "AIN";
		sDev = pDev ? pDev->Name().c_str() : "NULL";
		return TRUE;
	}

	const AnalogOutputDataSrc*  pAODS = dynamic_cast<const AnalogOutputDataSrc*>(pDS);	
	if (  pAODS != NULL )
	{
		IdentifiedPhysDevice* pDev = dynamic_cast<IdentifiedPhysDevice*>( pAODS->GetParentDevice() );
		nWire = (int)pAODS->GetChannelNo();
		sType = "AOUT";
		sDev = pDev ? pDev->Name().c_str() : "NULL";
		return TRUE;
	}

	const DiscreteInputDataSrc*  pDIDS = dynamic_cast<const DiscreteInputDataSrc*>(pDS);	
	if (  pDIDS != NULL )
	{
		IdentifiedPhysDevice* pDev = dynamic_cast<IdentifiedPhysDevice*>( pDIDS->GetParentDevice() );
		nWire = (int)pDIDS->GetChannelNo();
		sType = "DIN";
		sDev = pDev ? pDev->Name().c_str() : "NULL";
		return TRUE;
	}
	const DiscreteOutputDataSrc*  pDODS = dynamic_cast<const DiscreteOutputDataSrc*>(pDS);	
	if (  pDODS != NULL )
	{
		IdentifiedPhysDevice* pDev = dynamic_cast<IdentifiedPhysDevice*>( pDODS->GetParentDevice() );
		nWire = (int)pDODS->GetChannelNo();
		sType = "DOUT";
		sDev = pDev ? pDev->Name().c_str() : "NULL";
		return TRUE;
	}

	const DiscreteDataSrc*  pDDS = dynamic_cast<const DiscreteDataSrc*>(pDS);	
	if (  pDDS != NULL )
	{
		sType = "DIN";		
		return TRUE;
	}

	const AnalogDataSrc*  pADS = dynamic_cast<const AnalogDataSrc*>(pDS);	
	if (  pADS != NULL )
	{
		sType = "AIN";		
		return TRUE;
	}

	return TRUE;
}

BOOL AppXMLHelper::SaveDataSourceToXMLNode(IXMLDOMDocument* pXMLDoc, IXMLDOMNode* pParentNode, const DataSrc* pDS, const char* pszName/* = NULL*/, const char* pszDev/* = NULL*/, int nWire/* = -1*/) const
{
	
	CComPtr<IXMLDOMNode> pDSNode;

	HRESULT hr = pXMLDoc->createNode(CComVariant(NODE_ELEMENT), CComBSTR(L"DS"), CComBSTR(L""), &pDSNode);
	
	if ( FAILED(hr) )
		return FALSE;
	
	const AnalogDataSrc*   pADS = dynamic_cast<const AnalogDataSrc*>(pDS);
	const DiscreteDataSrc* pDDS = dynamic_cast<const DiscreteDataSrc*>(pDS);
	
	CString val;
	CString sType, sDev;
	int wire;

	if ( !GetDSInfo(pDS, sType, sDev, wire) )
		return FALSE;

	///Тип источника данных в строковом виде
	if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "TYPE", sType) )
		return FALSE;

	if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "DEV", (pszDev == NULL) ? sDev : pszDev ) )
		return FALSE;

	if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "NAME", (pszName == NULL) ? pDS->Name().c_str() : pszName ) )
		return FALSE;
	
	val.Format("%d", (nWire == -1) ? wire : nWire); 
	if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "WIRE", val) )
		return FALSE;
	
	///Цвет отображения на графиках
	val.Format("0x%06X", pDS->Color());
	if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "COLOR", val) )
		return FALSE;

	if ( pADS != NULL )
	{
		///Формат вывода
		val = pADS->OutFormat().c_str();
		if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "FMT", val) )
			return FALSE;
		
		///Единицы измерения
		val = pADS->Units().c_str();
		if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "UNITS", val) )
			return FALSE;

		///Минимальное и максимальное значение
		///Единицы измерения
		double minv, maxv;
		pADS->GetRange(minv, maxv);

		if ( minv != DBL_MIN && maxv != DBL_MAX) 
		{
			val.Format(pADS->OutFormat().c_str(), minv);
			if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "MINV", val) )
				return FALSE;

			val.Format(pADS->OutFormat().c_str(), maxv);
			if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "MAXV", val) )
				return FALSE;
		}	
	}
	///
	if ( pDDS != NULL )
	{
		val = pDDS->ToString(true).c_str();
		val += ";";
		val += pDDS->ToString(false).c_str();

		if ( !AddNodeAtrribute(pXMLDoc, pDSNode, "TOSTR", val) )
			return FALSE;
	}

	CComPtr<IXMLDOMNode> pNewNode;
	hr = pParentNode->appendChild(pDSNode, &pNewNode);
	pDSNode->put_text(CComBSTR(pDS->Description().c_str()));

	return TRUE;
}

void AppXMLHelper::GetSubNodesAtributes(IXMLDOMNode* pNode, const char* pszSubNodeName, const char* pszAttrName, std::vector<std::string>& attrs) const
{
	CComPtr<IXMLDOMNodeList>	 pItemsList;
	pNode->selectNodes(CComBSTR(pszSubNodeName), &pItemsList);

	long subItems = 0;

	attrs.clear();

	pItemsList->get_length(&subItems); 
	pItemsList->reset();

	for (long n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList->get_item(n, &pSubItem);

		CComPtr<IXMLDOMNamedNodeMap> pItemAtr;
		pSubItem->get_attributes(&pItemAtr);

		std::string sAtr;
		//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
		if ( !AppXMLHelper::Instance().GetAttributeByName(pItemAtr, pszAttrName, sAtr))
			sAtr = "";

		attrs.push_back( sAtr );
	}
}

BOOL AppXMLHelper::PreprocessDocument(IXMLDOMDocument* pXMLDoc) const
{
	CComPtr<IXMLDOMNodeList>	 pImportNodes;
	HRESULT hr = pXMLDoc->selectNodes(CComBSTR("NTCCPROJECT/DATASOURCES/IMPORT"), &pImportNodes);
	if ( FAILED (hr) )
		return FALSE;

	CComPtr<IXMLDOMNode>	 pDSRootNode;
	hr = pXMLDoc->selectSingleNode(CComBSTR("NTCCPROJECT/DATASOURCES"), &pDSRootNode);
	if ( FAILED (hr) )
		return FALSE;

	long lImportCount;
	pImportNodes->get_length(&lImportCount);
	pImportNodes->reset();
	for (long i = 0; i < lImportCount; i++ )
	{
		CComPtr<IXMLDOMNode>	 pImportNode;
		pImportNodes->get_item(i, &pImportNode);

		std::string srv;
		if (!GetNodeAttributeByName(pImportNode, "SERVER", srv) )
			continue;

		std::string name;
		if (!GetNodeAttributeByName(pImportNode, "NAME", name) )
			continue;

		std::string user;
		if ( !GetNodeAttributeByName(pImportNode, "USER", user) )
			continue;

		std::string psw;
		if ( !GetNodeAttributeByName(pImportNode, "PSW", psw) )
			continue;

		DWORD  cbRead;
		char   szBuf[4096];
		CString cmd("get:importds");

		CString strPipePath;
		strPipePath.Format("\\\\%s\\pipe\\%s", srv.c_str(), name.c_str());

		
		if ( !AppNetworkHelper::Instance().Connect(srv.c_str(), user.c_str(), psw.c_str()) )
			return false;

		if ( CallNamedPipe(strPipePath, (LPVOID)((LPCTSTR)cmd), cmd.GetLength() + 1, szBuf, 4096, &cbRead, 20 ) )					
			ImportNodes(pDSRootNode, "DS", szBuf);
		else 
			continue;
	}

	return TRUE;
}