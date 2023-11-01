#if !defined(AFX_DEVICE_FACTORY_H_INCLUDED_)
#define AFX_DEVICE_FACTORY_H_INCLUDED_

#include "..\FSLayer.h"

class IdentifiedPhysDevice;
class DataSrc;
class BaseWatchDog;
class BaseConverter;
class BaseTimer;
class CDrawObj;
class BaseFilter;


typedef IdentifiedPhysDevice* (*LPCREATEDEVFROMXML)(IXMLDOMNode* pNode, UINT& nError);
typedef DataSrc*			  (*LPCREATEDSFROMXML)(IXMLDOMNode* pNode, UINT& nError);
typedef BaseWatchDog*		  (*LPCREATEWDFROMXML)(IXMLDOMNode* pNode, UINT& nError);
typedef CDrawObj*			  (*LPCREATEFSITEMFROMXML)(IXMLDOMNode* pNode, FSLayer& parentLayer, UINT& nError);
typedef BaseConverter*		  (*LPCREATECONVERTERFROMXML)(IXMLDOMNode* pNode, UINT& nError);
typedef BaseTimer*		  (*LPCREATETIMERFROMXML)(IXMLDOMNode* pNode, UINT& nError);
typedef BaseFilter*			  (*LPCREATEFILTERFROMXML)(IXMLDOMNode* pNode, UINT& nError);

class XMLObjectFactory
{

public:
	virtual ~XMLObjectFactory(void);


protected:
	XMLObjectFactory(void);

public:
	
	static bool Init();
	
	static IdentifiedPhysDevice* CreateDeviceObject(IXMLDOMNode* pNode, UINT& error);
	static DataSrc* CreateDataSourceObject(IXMLDOMNode* pNode, UINT& nError);
	static BaseWatchDog* CreateWatchDogObject(IXMLDOMNode* pNode, UINT& nError);
	static CDrawObj* CreateDrawObject(IXMLDOMNode* pNode, FSLayer& parentLayer, UINT& nError);
	static BaseConverter* CreateConverterObject(IXMLDOMNode* pNode, UINT& nError);
	static BaseTimer* CreateTimerObject(IXMLDOMNode* pNode, UINT& nError);
	static BaseFilter*  CreateFilterObject(IXMLDOMNode* pNode, UINT& nError);

protected:
	static bool RegisterDevice(std::string id, LPCREATEDEVFROMXML creator);
	static bool RegisterDataSource(std::string id, LPCREATEDSFROMXML creator);
	static bool RegisterWatchDog(std::string id, LPCREATEWDFROMXML creator);
	static bool RegisterDrawObject(std::string id, LPCREATEFSITEMFROMXML creator);
	static bool RegisterConverter(std::string id, LPCREATECONVERTERFROMXML creator);
	static bool RegisterTimer(std::string id, LPCREATETIMERFROMXML creator);
	static bool RegisterFilter(std::string id, LPCREATEFILTERFROMXML creator);


	typedef std::map<std::string, LPCREATEDEVFROMXML> AssocDevMap;
	static AssocDevMap m_devCreators; 

	typedef std::map<std::string, LPCREATEDSFROMXML> AssocDSMap;
	static AssocDSMap m_dsCreators; 

	typedef std::map<std::string, LPCREATEWDFROMXML> AssocWDMap;
	static AssocWDMap m_wdCreators; 

	typedef std::map<std::string, LPCREATEFSITEMFROMXML> AssocFSDIMap;
	static AssocFSDIMap m_fsdiCreators; 

	typedef std::map<std::string, LPCREATECONVERTERFROMXML> AssocConverterMap;
	static AssocConverterMap m_convCreators; 

	typedef std::map<std::string, LPCREATETIMERFROMXML> AssocTimerMap;
	static AssocTimerMap m_timerCreators; 

	typedef std::map<std::string, LPCREATEFILTERFROMXML> AssocFilterMap;
	static AssocFilterMap m_filterCreators; 
};

#endif