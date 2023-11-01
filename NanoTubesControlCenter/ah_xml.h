#pragma once

#include "singletone.h"

#ifndef _APP_XML_HELPER_H_INCLUDED_
#define _APP_XML_HELPER_H_INCLUDED_



typedef std::map<std::string, std::string> ATTRIBUTES;

class DataSrc;
class AppXMLHelper : public Singletone<AppXMLHelper>
{
	friend class Singletone<AppXMLHelper>;

public:
	virtual ~AppXMLHelper();

protected:

	AppXMLHelper();
	AppXMLHelper(const AppXMLHelper& );
	const AppXMLHelper& operator= (const AppXMLHelper&);

public:

	 BOOL GetNodeAttributes(IXMLDOMNode* pNode, ATTRIBUTES& attrs) const;

	 BOOL GetNodeAttributeByName(IXMLDOMNode* pNode, const char* pszName, std::string& sval) const;
	 BOOL GetAttributeByName(IXMLDOMNamedNodeMap* pAttributesMap, const char* pszName, std::string& sval) const;

	 BOOL PreprocessDocument(IXMLDOMDocument* pXMLDoc) const;
	 BOOL ImportNodes(IXMLDOMNode* pParentNode, const char* pszImportNodesName, const char* pszSrcFilePath) const ;
	 BOOL ImportNodes(IXMLDOMNode* pParentNode, const char* pszImportNodesName, IXMLDOMDocument* pXMLDocFrom) const ;

	 void GetSubNodesAtributes(IXMLDOMNode* pNode, const char* pszSubNodeName, const char* pszAttrName, std::vector<std::string>& attrs) const;

	 BOOL AddNodeAtrribute(IXMLDOMDocument* pXMLDoc, IXMLDOMNode* pNode, CString strName, CString strVal) const;
	 BOOL SaveDataSourceToXMLNode(IXMLDOMDocument* pXMLDoc, IXMLDOMNode* pParentNode, const DataSrc* pDS, const char* pszName/* = NULL*/, const char* pszDev = NULL, int nWire = -1) const;

	 //возвращает число узлов соответствующих шаблону XPath	  
	 long GetNodesCount(IXMLDOMDocument* pXMLDoc, const char* pszXPath) const; 

	 //возвращает число узлов соответствующих шаблону XPath
	 long GetNodesCount(const char* pszFileName, const char* pszXPath) const;


	 void GetDrawObjectDefaults(IXMLDOMNode* pNode, std::map<std::string, std::string>& defaults, bool useAppDefaults = true) const;

	 BOOL GetProjectSubPath(IXMLDOMNode* pMainNode, const char* pszType, std::string& strDir) const;

	
	 BOOL GetGdiPlusColor(IXMLDOMNode* pNode, Gdiplus::Color& color, double defOpacity) const;
	 Gdiplus::Color GetGdiPlusColor(CString strClr, CString strOp) const;

};

#endif //_APP_XML_HELPER_H_INCLUDED_
