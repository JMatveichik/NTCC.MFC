#pragma once

#include <string>
#include <vector>
#include <gdiplus.h>

#include "Automation/common.h"
#include "Automation/Converters.h"
#include "OBA/obascripter.h"
#include "errors.h"
#include "OutMessage.h"
#include "winmessages.h"
#include "PopupGraphWnd.h"
#include "PopupMessageWnd.h"
#include "LoadingProrgessDlg.h"




class ImagesListExt
{
public:	

	ImagesListExt();	
	~ImagesListExt();

	bool Create(const char* pszFileName, int width = -1);
	bool Create(Gdiplus::Bitmap* image, int width = -1);

	Gdiplus::Bitmap* GetImage(int index);
	int Size() const { return (int)images.size(); }


protected:

	std::vector<Gdiplus::Bitmap*> images;

private:

};


class AppXMLHelper
{
public:
	AppXMLHelper(void);
	~AppXMLHelper(void);

	static BOOL GetNodeAttributeByName(IXMLDOMNode* pNode, const char* pszName, CString& strVal);
	static BOOL GetAttributeByName(IXMLDOMNamedNodeMap* pAttributesMap, const char* pszName, CString& strVal);
	
	static void GetNodesCount(IXMLDOMDocument* pXMLDoc, const char* pszXPath, long& nCount);
	static void GetNodesCount(const char* pszFileName, const char* pszXPath, long& nCount);

	static void GetSchems(IXMLDOMDocument* pXMLDoc, std::vector<std::string>& schems);
	static void GetSchems(const char* pszFileName, std::vector<std::string>& schems);
	

	static BOOL GetDSBaseAtributes(IXMLDOMNode* pNodeDS, std::string& strName, std::string& strDesc, COLORREF& color, bool& bWriteToFile  );
	static BOOL GetAnalogDSBaseAtributes(IXMLDOMNode* pNodeDS, std::string& strUnits, std::string& strOutFmt, double& dmin, double& dmax  );
	static BOOL GetConvertersFormNode(IXMLDOMNode* pNodeDS, bool fwdConv, std::vector<std::string>& conv);
	static BOOL GetSubDataSources(IXMLDOMNode* pNode, std::vector<std::string>& names);

	static BOOL GetAIDataSources(IAnalogInput* pAnalogInputDevice, IXMLDOMNodeList* pAnalogInputsList );
	static BOOL GetAODataSources(IAnalogOutput* pAnalogOutputDevice, IXMLDOMNodeList* pAnalogOutputList );
	static BOOL GetDIDataSources(IDigitalInput* pDigitalInputDevice, IXMLDOMNodeList* pDigitalInputList );
	static BOOL GetDODataSources(IDigitalOutput* pDigitalOutputDevice, IXMLDOMNodeList* pDigitalOutputList );
	static BOOL GetTimeDataSources(IXMLDOMNodeList* pTimersList );
	
	static BOOL GetGdiPlusColor(IXMLDOMNode* pNode, Gdiplus::Color& color, double defOpacity);
};



//////////////////////////////////////////////////////////////////////////
//  [5/17/2012 Johnny A. Matveichik]
class AppMessagesProviderHelper
{
public:

	static AppMessagesProviderHelper& Instance();
	~AppMessagesProviderHelper();

protected:

	AppMessagesProviderHelper() 
	{
		
	}

	AppMessagesProviderHelper(const AppMessagesProviderHelper& );
	const AppMessagesProviderHelper& operator= (const AppMessagesProviderHelper&);

public:

	bool RegisterOutputMessageDestination(CWnd* pWnd);

	void ShowMessage(std::string msg, COutMessage::MESSAGE_TYPE msgType = COutMessage::MSG_OK);
	void ShowMessage(std::istream& msg, COutMessage::MESSAGE_TYPE msgType = COutMessage::MSG_OK);


	void ShowPopupMessage(LPCTSTR msg, LPCTSTR title, COutMessage::MESSAGE_TYPE type = COutMessage::MSG_OK);
	void ShowPopupMessage(COutMessage* pMsg);

protected:
	
	std::vector<CWnd*> m_pMsgDest;	
};


//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]

class AppErrorsHelper
{
public:

	static AppErrorsHelper& Instance();
	~AppErrorsHelper();

protected:

	AppErrorsHelper() {};
	AppErrorsHelper(const AppErrorsHelper& );
	const AppErrorsHelper& operator= (const AppErrorsHelper&);
	
public:

	void SetLastError(UINT errorCode, std::string messsage)
	{
		lastError = errorCode;
		lastMessage = messsage;
	}

	std::string GetLastError(UINT& code) const
	{
		code = lastError;
		return lastMessage;
	}

private:

	UINT lastError;
	std::string lastMessage;
};

//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]
//  OBA SCRIPT HELPER
//////////////////////////////////////////////////////////////////////////
typedef struct tagScriptInfo 
{
	tagScriptInfo () : pScript(NULL) 
	{

	};

	std::string name;
	std::string path;

	COBAScripter* pScript;

}SCRIPTINFO,* LPSCRIPTINFO;

typedef struct tagSCRIPTIDLOCATION
{
	unsigned line;
	std::string function;
} SCRIPTIDLOCATION, *LPSCRIPTIDLOCATION;



typedef std::map< std::string, std::vector<SCRIPTIDLOCATION> > IDMap;


class AppOBAScriptsHelper
{
public:
	
	static AppOBAScriptsHelper& Instance();
	~AppOBAScriptsHelper();


protected:

	AppOBAScriptsHelper();	

	AppOBAScriptsHelper(const AppOBAScriptsHelper&);
	const AppOBAScriptsHelper& operator= (const AppOBAScriptsHelper&);

	std::string::size_type ProcessString(IDMap& ids, unsigned line, std::string str);

public:

	bool GetScriptIDs(const char* pszScriptPath, IDMap& ids);

	bool LoadScript(std::string name, std::string path);

	bool RunScript(std::string name, bool bDebug = false);

	bool TerminateScript(std::string name);

	void GetScriptInfo(std::string name, SCRIPTINFO& si); 

	void EnumScripts(std::vector<SCRIPTINFO>& scripts);

	//Вывод сообщения
	static void OnScriptEnd(COBAScripter* pScript, int errCode, void* pParam);

	//Вывод сообщения
	static void Print(const char* strMsg);

	//проверка дискретного состояния
	static int Check(const char* strID);

	//установление дискретного состояния
	static int Switch(const char* strID, bool newState);	

	//получение расхода
	static double FlowGet(const char* strID);

	//задание расхода
	static double FlowSet(const char* strID, double newFlow);

	//получить давление
	static double PressureGet(const char* strID);	

	//получить температуру
	static double TemperatureGet(const char* strID);	

	//получить концентрацию
	static double FractionGet(const char* strID);	

	//задать ток плазматрона
	static double CurrentSet(const char* strID, double newCurrent);

	//всплывающее сообщение
	static void PopupMessage(const char* strTitle, const char* strMessage, int type);


	COBAScripter* ExecuteScript(std::string path, bool debug);

	bool IsActive(COBAScripter* pScript);

private:

	std::map <std::string, SCRIPTINFO> scriptsHost;
	static HWND m_hMainWnd;
};


//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]
//  IMAGES HELPER
//////////////////////////////////////////////////////////////////////////
enum e_tIconSize
{
	ICON_SIZE_16 = 16,
	ICON_SIZE_24 = 24,
	ICON_SIZE_32 = 32,
	ICON_SIZE_48 = 48
};

class AppImagesHelper
{

public:
	static AppImagesHelper& Instance();
	~AppImagesHelper();

protected:

	AppImagesHelper() {};
	AppImagesHelper(const AppImagesHelper& ) {};
	const AppImagesHelper& operator= (const AppImagesHelper&) {};

public:

	Gdiplus::Bitmap* GetImage(std::string fileName);
	CExtCmdIcon* GetIcon(std::string fileName, UINT iconSize = ICON_SIZE_16);

protected:	

	std::map<std::string, Gdiplus::Bitmap*>	vImages;
	std::map<std::string, CExtCmdIcon*>	mapIcons16;
	std::map<std::string, CExtCmdIcon*>	mapIcons24;
	std::map<std::string, CExtCmdIcon*>	mapIcons32;
	std::map<std::string, CExtCmdIcon*>	mapIcons48;
};

//////////////////////////////////////////////////////////////////////////
//  [5/17/2012 Johnny A. Matveichik]
class AppLoadingHelper
{
	
public:
	static AppLoadingHelper& Instance();
	~AppLoadingHelper();

protected:

	AppLoadingHelper();
	AppLoadingHelper(const AppLoadingHelper& );
	const AppLoadingHelper& operator= (const AppLoadingHelper&);

public:	
	
	BOOL Initialize(const char* pszFilePath);
	void Release();

	void MakeHeader();
	long AddSection(std::string title, std::string name, int nSectionSize);
	void CloseCurrentSection();

	long StepSection(int nSecId, bool bOk = true);

	void AddContentMessage(std::string text, COutMessage::e_MessageType mt);

	BOOL MakeReport(std::string templ, std::string outPath, BOOL bRes);

	UINT LoadingDlgResult() 
	{
		return wndLoad->RunModalLoop();
	}
	
protected:
	
	CLoadingProrgessDlg* wndLoad;
	
	std::vector <std::string> vTitleLines;
	std::vector <std::string> vContentLines;
	
};

//////////////////////////////////////////////////////////////////////////
//  [3/31/2011 Johnny A. Matveichik]
//	XYPLOT HELPER
//////////////////////////////////////////////////////////////////////////
class AnalogOutputDataSrc;
class DiscreteDataSrc;

class AppXYPlotWndHelper
{
public:
	
	AppXYPlotWndHelper();
	~AppXYPlotWndHelper();

	static BOOL Init(CWnd* pParent);
	static void ShowGraphWnd(CPoint& pt, bool bShow = true);
	static BOOL IsGraphWndVisible();
	static BOOL AddDataSourceProfile(std::string name, bool bClearContent = true);
	static BOOL AddCheckPoint(std::string name);

	
protected:
	
	

private:
	
	static CPopupGraphWnd pGraphWnd;

	
};
 

//////////////////////////////////////////////////////////////////////////
//class CSpeciesDB;
//class CSpecieSet;
//class CIdealGasMix;
//class MassFlow;
//  [3/31/2011 Johnny A. Matveichik]
//  THERMODINAMIC HELPER
//////////////////////////////////////////////////////////////////////////
#ifdef USE_CHEM_LIB_FEATURES
class AppThermHelper
{

public:
	static AppThermHelper& Instance();
	~AppThermHelper();

protected:

	AppThermHelper(); 	
	AppThermHelper(const AppThermHelper& );
	const AppThermHelper& operator= (const AppThermHelper&);

public:	
	

	bool Init(std::string filePath);
	bool BuildMixtures();
	bool SetMassFlowComposition(MassFlow& mf, std::string gasName);
	bool AddMixture(IXMLDOMNode* pNode, std::pair<std::string, std::string>& mix);
	const CSpeciesDB& GetTHDB() const { return *pTHDB; };
	


protected:

	CSpeciesDB* pTHDB;	
	CSpecieSet* pSS;

	std::map<std::string, CIdealGasMix*>	mapMixtures;
	std::map<std::string, std::string>		mapComp;
	std::string thermFilePath;
};
#endif //USE_CHEM_LIB_FEATURES