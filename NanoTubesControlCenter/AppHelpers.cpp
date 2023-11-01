#include "StdAfx.h"
#include "AppHelpers.h"
#include "Automation\DataSrc.h"
#include "resource.h"
#include "NanoTubesControlCenter.h"
#include "Automation\DataCollector.h"

ImagesListExt::ImagesListExt() 
{
}

ImagesListExt::~ImagesListExt() 
{
	std::for_each(images.begin(), images.end(), delete_object<Gdiplus::Bitmap>());
}

bool ImagesListExt::Create(Gdiplus::Bitmap* image, int width/* = -1*/)
{
	images.clear();

	UINT cy = image->GetHeight();
	UINT cx = (width == -1) ? cy : width;

	if (cx == 0 || cy == 0)
		return false;


	int nCount = image->GetWidth() / cx;

	for (int i = 0; i < nCount; i++)
	{
		Gdiplus::Bitmap* imgPart = image->Clone(cx*i, 0, cx, cy, PixelFormat32bppARGB );
		images.push_back( imgPart );
	}

	return true;
}

bool ImagesListExt::Create(const char* pszFileName, int width)
{
	USES_CONVERSION;
	return Create(Gdiplus::Bitmap::FromFile(A2W(pszFileName)), width);
}

Gdiplus::Bitmap* ImagesListExt::GetImage(int index)
{
	if ( index < 0 || index >= (int)images.size() )
		return NULL;

	return images.at( index );
}


//  [9/27/2010 Johnny A. Matveichik]

AppXMLHelper::AppXMLHelper(void)
{
}

AppXMLHelper::~AppXMLHelper(void)
{
	
}


BOOL AppXMLHelper::GetSubDataSources(IXMLDOMNode* pNode, std::vector<std::string>& names)
{
	CComPtr<IXMLDOMNodeList>	 pItemsList;
	pNode->selectNodes(CComBSTR("ITEM"), &pItemsList);

	long subItems = 0;

	names.clear();

	pItemsList->get_length(&subItems); 
	pItemsList->reset();

	for (int n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList->get_item(n, &pSubItem);

		CComPtr<IXMLDOMNamedNodeMap> pItemAtr;
		pSubItem->get_attributes(&pItemAtr);

		CString sAtr;
		//ИДЕНТИФИКАТОР ИСТОЧНИКА ДАННЫХ
		AppXMLHelper::GetAttributeByName(pItemAtr, "DSID", sAtr);
		names.push_back( (LPCTSTR)sAtr );		
	}
	
	return !names.empty();
}

BOOL AppXMLHelper::GetNodeAttributeByName(IXMLDOMNode* pNode, const char* pszName, CString& strVal)
{
	if (pNode == NULL)
		return FALSE;

	CComPtr<IXMLDOMNamedNodeMap> pNodeAtr;
	pNode->get_attributes(&pNodeAtr);

	return GetAttributeByName(pNodeAtr, pszName, strVal);
}

void AppXMLHelper::GetNodesCount(const char* pszFileName, const char* pszXPath, long& nCount)
{
	CComPtr<IXMLDOMDocument> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) )
		return;

	VARIANT_BOOL bSuccess = false;
	hr = pXMLDoc->load(CComVariant(pszFileName), &bSuccess);

	if (FAILED(hr) || !bSuccess)
		return;

	AppXMLHelper::GetNodesCount(pXMLDoc, pszXPath, nCount);
}

void AppXMLHelper::GetSchems(IXMLDOMDocument* pXMLDoc, std::vector<std::string>& schems)
{
	CComPtr<IXMLDOMNodeList>	 pNodes;
	HRESULT hr = pXMLDoc->selectNodes(CComBSTR("APPCONFIG/WORKSPACE/SCHEMA"), &pNodes);

	if (FAILED(hr))
		return;

	long lNodesCount = 0;
	pNodes->get_length(&lNodesCount);
	pNodes->reset();
	schems.clear();

	for (long i=0;i<lNodesCount; i++)
	{
		CComPtr<IXMLDOMNode> pNode;
		pNodes->get_item(i, &pNode);
		CString sVal;

		if ( AppXMLHelper::GetNodeAttributeByName(pNode, "CONFIGFILE", sVal) )
			schems.push_back((LPCTSTR)sVal);
	}

}

void AppXMLHelper::GetSchems(const char* pszFileName, std::vector<std::string>& schems)
{
	CComPtr<IXMLDOMDocument> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) )
		return;

	VARIANT_BOOL bSuccess = false;
	hr = pXMLDoc->load(CComVariant(pszFileName), &bSuccess);

	if (FAILED(hr) || !bSuccess)
		return;

	AppXMLHelper::GetSchems( pXMLDoc, schems);
}


void AppXMLHelper::GetNodesCount(IXMLDOMDocument* pXMLDoc, const char* pszXPath, long& nCount)
{
	CComPtr<IXMLDOMNodeList> pSectionsNode;
	HRESULT hr = pXMLDoc->selectNodes(CComBSTR(pszXPath), &pSectionsNode);

	if (FAILED(hr) || pSectionsNode == NULL)
		return;

	long lSections = 0;
	pSectionsNode->get_length(&lSections);
	pSectionsNode->reset();
	
	nCount += lSections;
	
}

BOOL AppXMLHelper::GetAttributeByName(IXMLDOMNamedNodeMap* pAttributesMap, const char* pszName, CString& strVal)
{
	CComPtr<IXMLDOMNode>	pAttrNode;

	VARIANT val;

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

	return TRUE;
}

BOOL AppXMLHelper::GetGdiPlusColor(IXMLDOMNode* pNode, Gdiplus::Color& color, double defOpacity)
{
	CString sVal;

	if ( !AppXMLHelper::GetNodeAttributeByName(pNode, "COLOR", sVal) )
		return FALSE;

	COLORREF clrRed = RGB(255, 0, 0);
	DWORD dw =  0x0000FF; 

	COLORREF clr = strtoul(sVal, 0, 0);
	unsigned char b = GetBValue(clr);
	unsigned char g = GetGValue(clr);
	unsigned char r = GetRValue(clr);

	//непрозрачнорсть 
	unsigned char a = 0xFF;
	double op = defOpacity;

	if ( AppXMLHelper::GetNodeAttributeByName(pNode, "OPACITY", sVal) )
		op = atof(sVal);
	
	a = unsigned char( 255 - 255 * (100 - op) / 100 );
	color  = Gdiplus::Color(a, r, g, b); 
	
	return TRUE;
}

//  [9/27/2010 Johnny A. Matveichik]
AppImagesHelper& AppImagesHelper::Instance()
{
	static AppImagesHelper self;
	return self;
}

AppImagesHelper::~AppImagesHelper()
{
	for (std::map<std::string, Gdiplus::Bitmap*>::iterator it = vImages.begin();  it != vImages.end(); it++)
		delete (*it).second;
	
	for (std::map<std::string, CExtCmdIcon*>::iterator it = mapIcons16.begin(); it != mapIcons16.end(); it++)
		delete (*it).second;

	for (std::map<std::string, CExtCmdIcon*>::iterator it = mapIcons24.begin(); it != mapIcons24.end(); it++)
		delete (*it).second;

	for (std::map<std::string, CExtCmdIcon*>::iterator it = mapIcons32.begin(); it != mapIcons32.end(); it++)
		delete (*it).second;

	for (std::map<std::string, CExtCmdIcon*>::iterator it = mapIcons48.begin(); it != mapIcons48.end(); it++)
		delete (*it).second;

}

Gdiplus::Bitmap* AppImagesHelper::GetImage(std::string name)
{
	std::map<std::string, Gdiplus::Bitmap*>::const_iterator	fnd;

	fnd =  vImages.find( name );

	if ( fnd == vImages.end() )
	{
		TCHAR curDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, curDir);

		CString imgPath = curDir;
		imgPath += "\\Images\\";
		imgPath += name.c_str();

		USES_CONVERSION;
		Gdiplus::Bitmap* pImg = new Gdiplus::Bitmap( A2W(imgPath) );
		
		if ( pImg->GetLastStatus() != Gdiplus::Ok  )
			return NULL;
		else
		{
			vImages.insert(make_pair(name, pImg));
			return pImg;
		}
		
	}
	else
		return (*fnd).second;
}

CExtCmdIcon* AppImagesHelper::GetIcon(std::string fileName, UINT iconSize/* = ICON_SIZE_16*/)
{
	std::map<std::string, CExtCmdIcon* >* pIconMap;
	switch (iconSize)
	{
	case ICON_SIZE_16:
		pIconMap = &mapIcons16;
		break;
	case ICON_SIZE_24:
		pIconMap = &mapIcons24;
		break;
	case ICON_SIZE_32:
		pIconMap = &mapIcons32;
		break;
	case ICON_SIZE_48:
		pIconMap = &mapIcons48;
		break;
	default:
		iconSize = ICON_SIZE_16;
		pIconMap = &mapIcons16;
		break;
	}

	std::map<std::string, CExtCmdIcon*>::const_iterator	fnd;

	fnd =  pIconMap->find( fileName );

	if ( fnd == pIconMap->end() )
	{
		TCHAR curDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, curDir);

		CString imgPath = curDir;
		imgPath += "\\Images\\";
		imgPath += fileName.c_str();

		HICON hIcon = (HICON) ::LoadImage(NULL, imgPath, IMAGE_ICON, iconSize, iconSize, LR_LOADFROMFILE);
		if ( hIcon == NULL)
			return NULL;
		
		CExtCmdIcon* pIcon = new CExtCmdIcon();		
		pIcon->AssignFromHICON(hIcon, false);

		pIconMap->insert(make_pair(fileName, pIcon));

		DestroyIcon(hIcon);
		return pIcon;
	}
	else
		return (*fnd).second;
}

//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]
AppErrorsHelper& AppErrorsHelper::Instance()
{
	static AppErrorsHelper obj;
	return obj;
}

AppErrorsHelper::~AppErrorsHelper()
{
}

//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]

HWND AppOBAScriptsHelper::m_hMainWnd = NULL;

AppOBAScriptsHelper::AppOBAScriptsHelper()
{
	m_hMainWnd = ::AfxGetMainWnd()->m_hWnd;
	COBAScripter::Initialize( OnScriptEnd, Print, Check, Switch, FlowGet, FlowSet, PressureGet, TemperatureGet, FractionGet, CurrentSet, PopupMessage);
}

AppOBAScriptsHelper::~AppOBAScriptsHelper()
{
	

}

AppOBAScriptsHelper& AppOBAScriptsHelper::Instance()
{
	static AppOBAScriptsHelper obj;
	return obj;
}



std::string::size_type AppOBAScriptsHelper::ProcessString(IDMap& ids, unsigned line, std::string str)
{
	// List of specific function names
	static list<string> funName;
	static string strFirstLetters;
	
	if ( funName.empty() )
	{
		funName.insert(funName.begin(), "CHECK");
		funName.insert(funName.begin(), "CHECK%");
		funName.insert(funName.begin(), "SWITCH");
		funName.insert(funName.begin(), "FLOW");
		funName.insert(funName.begin(), "PRESSURE");
		funName.insert(funName.begin(), "TEMPERATURE");
		funName.insert(funName.begin(), "FRACTION");
		funName.insert(funName.begin(), "PLASMATRON");
		funName.insert(funName.begin(), "EXECUTE");

		set<char> fl;
		for(list<string>::const_iterator it=funName.begin(); it!=funName.end(); ++it)
			fl.insert(it->at(0));

		strFirstLetters.assign(fl.begin(), fl.end());
	}

	string name;
	string::size_type pos = 0;

	for (;;)
	{
		pos = str.find_first_of(strFirstLetters, pos);
		if (pos == string::npos)
			return string::npos;

		for(list<string>::const_iterator it = funName.begin(); it != funName.end(); ++it)
		{
			string::size_type res = str.find(*it, pos);
			if (res == string::npos || res != pos)
				continue;

			res += it->length();

			static string sequence("(\"\"");

			name.clear();
			string::size_type next;
			for (size_t i=0; i<sequence.length(); ++i)
			{
				next = str.find(sequence[i], res);
				if (next == string::npos)
					break;

				name.assign(&str[res], &str[next]);
				res = next + 1;
				if (res >= str.length())
					break;
			}

			if (name.empty())
				continue;

			std::transform(name.begin(), name.end(), name.begin(), toupper);

			SCRIPTIDLOCATION loc;
			loc.function = *it;
			loc.line = line;

			vector<SCRIPTIDLOCATION> locs;
			locs.push_back(loc);
			pair<IDMap::iterator, bool> result = ids.insert(make_pair(name, locs));
			
			if ( !result.second )
				result.first->second.push_back( loc );
			
			if (res < str.length())
				return ProcessString(ids, line, string(&str[res], &str[ str.length() - 1] ));

			break;
		}
		++pos;
	}

	return pos;
}

bool AppOBAScriptsHelper::GetScriptIDs(const char* pszScriptPath, IDMap& ids)
{
	ids.clear();

	ifstream in(pszScriptPath, ios::in|ios::binary);

	if (!in.good())
		return false;

	string line;
	unsigned lineCounter = 0;
	while (getline(in, line).good())
		ProcessString(ids, ++lineCounter, line);

	return true;
}

void AppOBAScriptsHelper::GetScriptInfo(std::string name, SCRIPTINFO& si)
{
	si.name = "";
	si.path = "";
	si.pScript = NULL;

	std::map <std::string, SCRIPTINFO>::const_iterator fnd = scriptsHost.find(name);
	
	if ( fnd != scriptsHost.end() )
		si = (*fnd).second;
}

void AppOBAScriptsHelper::EnumScripts(std::vector<SCRIPTINFO>& scripts)
{
	scripts.clear();
	std::map <std::string, SCRIPTINFO>::iterator it;
	for(it  = scriptsHost.begin(); it != scriptsHost.end(); it++)
		scripts.push_back((*it).second);

}

bool AppOBAScriptsHelper::LoadScript(std::string name, std::string path)
{
	std::map <std::string, SCRIPTINFO>::const_iterator fnd = scriptsHost.find(name);

	if ( fnd != scriptsHost.end() )
		return false;
	
	if ( !path.length() )
		return false;

	SCRIPTINFO si;

	std::ifstream inp(path.c_str());
	if ( inp.fail() )
		return false;

	inp.close();
	
	si.name = name;
	si.path = path;
	
	if ( !scriptsHost.insert( std::make_pair(name, si) ).second  )
		return false;

	return true;
}

bool AppOBAScriptsHelper::RunScript(std::string name, bool bDebug /*= false*/)
{
	std::map <std::string, SCRIPTINFO>::iterator fnd = scriptsHost.find(name);
	if (fnd == scriptsHost.end())
		return false;

	SCRIPTINFO& si = (*fnd).second;
	si.pScript = COBAScripter::Create();

	si.pScript->Execute(si.path, NULL);
	return true;
}

bool AppOBAScriptsHelper::TerminateScript(std::string name)
{
	std::map <std::string, SCRIPTINFO>::iterator fnd = scriptsHost.find(name);
	if (fnd == scriptsHost.end())
		return false;

	SCRIPTINFO& si = (*fnd).second;

	si.pScript->Terminate();
	return true;
}

//после завершение выполнения скрипта
void AppOBAScriptsHelper::OnScriptEnd(COBAScripter* pScript, int errCode, void* pParam)
{
	
	CString str;
	str.Format("\nSCRIPT terminate with code %d (%s)\n", errCode, pScript->GetErrorMessage(errCode).c_str());
	AppMessagesProviderHelper::Instance().ShowMessage((LPCTSTR)str, COutMessage::MSG_OBA_SCRIPT);
}

//Вывод сообщения
void AppOBAScriptsHelper::Print(const char* strMsg)
{
	/*TRACE("\t=> %s\n", strMsg);*/
	COutMessage msg(strMsg, "", COutMessage::MSG_OBA_SCRIPT); 
	::SendMessage(m_hMainWnd, RM_SHOW_USER_MESSAGE, 0, (LPARAM)&msg);
	
}

void AppOBAScriptsHelper::PopupMessage(const char* strTitle, const char* strMessage, int type)
{
	COutMessage msg(strMessage, strTitle, (COutMessage::MESSAGE_TYPE)type); 
	::SendMessage(m_hMainWnd, RM_SHOW_USER_MESSAGE, 1, (LPARAM)&msg);
}

//проверка дискретного состояния
int AppOBAScriptsHelper::Check(const char* strID)
{
	DiscreteDataSrc* pDDS = dynamic_cast<DiscreteDataSrc*>(DataSrc::GetDataSource(string(strID)));
	if ( pDDS == NULL )
		throw oba_err(97);
	else
		return pDDS->IsEnabled();
}

//установление дискретного состояния
int AppOBAScriptsHelper::Switch(const char* strID, bool newState)
{
	DiscreteOutputDataSrc* pDDS = dynamic_cast<DiscreteOutputDataSrc*>(DataSrc::GetDataSource(string(strID)));
	if ( pDDS == NULL )
		throw oba_err(98);
	else
	{
		pDDS->Enable(newState);
		return pDDS->IsEnabled();
	}	
}

//получение расхода
double AppOBAScriptsHelper::FlowGet(const char* strID)
{
	AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(DataSrc::GetDataSource(string(strID)));

	if ( pADS == NULL )
		throw oba_err(99);
	else
		return pADS->GetValue();
}

//задание расхода
double AppOBAScriptsHelper::FlowSet(const char* strID, double newFlow)
{
	AnalogOutputDataSrc* pADS = dynamic_cast<AnalogOutputDataSrc*>(DataSrc::GetDataSource(string(strID)));
	
	if ( pADS == NULL )
		throw oba_err(99);
	else
		return pADS->SetValue(newFlow);
}

//получить давление
double AppOBAScriptsHelper::PressureGet(const char* strID)
{

	AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(DataSrc::GetDataSource(string(strID)));

	if ( pADS == NULL )
		throw oba_err(100);
	else
		return pADS->GetValue();

	return 0.0;
}

//получить температуру
double AppOBAScriptsHelper::TemperatureGet(const char* strID)
{
	AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(DataSrc::GetDataSource(string(strID)));

	if ( pADS == NULL )
		throw oba_err(101);
	else
		return pADS->GetValue();

}

//получить концентрацию
double AppOBAScriptsHelper::FractionGet(const char* strID)
{
	AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(DataSrc::GetDataSource(string(strID)));

	if ( pADS == NULL )
		throw oba_err(102);
	else
		return pADS->GetValue();

	return 0.0;
}

//задать ток плазматрона
double AppOBAScriptsHelper::CurrentSet(const char* strID, double newCurrent)
{
	AnalogOutputDataSrc* pADS = dynamic_cast<AnalogOutputDataSrc*>(DataSrc::GetDataSource( string(strID) ));

	if ( pADS == NULL )
		throw oba_err(103);
	else
		return pADS->SetValue(newCurrent);

}

COBAScripter* AppOBAScriptsHelper::ExecuteScript(std::string path, bool debug)
{
	return NULL;
}

bool AppOBAScriptsHelper::IsActive(COBAScripter* pScript)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////

AppMessagesProviderHelper& AppMessagesProviderHelper::Instance()
{
	static AppMessagesProviderHelper obj;
	return obj;
}

AppMessagesProviderHelper::~AppMessagesProviderHelper()
{
	
}

//////////////////////////////////////////////////////////
//регистрирует окно которому будут отправляться информационные сообщения
bool AppMessagesProviderHelper::RegisterOutputMessageDestination(CWnd* pWnd)
{
	if ( pWnd == NULL )
		return false;

	if ( !IsWindow(pWnd->m_hWnd) )
		return false;

	m_pMsgDest.push_back(pWnd);
	return true;
}


void AppMessagesProviderHelper::ShowPopupMessage(COutMessage* pMsg)
{
	
	for (std::vector<CWnd*>::iterator it = m_pMsgDest.begin(); it != m_pMsgDest.end(); ++it)
	{	
		ASSERT( *it != NULL && IsWindow((*it)->m_hWnd) );
		::SendMessage((*it)->m_hWnd, RM_SHOW_USER_MESSAGE, 1, (LPARAM)pMsg);
	}
	
}

void AppMessagesProviderHelper::ShowPopupMessage(LPCTSTR text, LPCTSTR title, COutMessage::MESSAGE_TYPE type/* = COutMessage::MSG_OK*/ )
{
	COutMessage msg(text, title, type);
	ShowPopupMessage(&msg);
}

void AppMessagesProviderHelper::ShowMessage(std::string msg, COutMessage::MESSAGE_TYPE msgType/* = COutMessage::MSG_OK*/) 
{
	
	std::auto_ptr<COutMessage> pMsgOut ( new COutMessage( msg.c_str(), "", msgType  ) );
	for (std::vector<CWnd*>::iterator it = m_pMsgDest.begin(); it != m_pMsgDest.end(); ++it)
	{	
		ASSERT( *it != NULL && IsWindow((*it)->m_hWnd) );
		(*it)->SendMessage(RM_SHOW_USER_MESSAGE, 0, (LPARAM)pMsgOut.get());
	}

	
}

void AppMessagesProviderHelper::ShowMessage(std::istream& msg, COutMessage::MESSAGE_TYPE msgType/* = COutMessage::MSG_OK*/) 
{
	

	do 
	{
		std::string line;		
		getline(msg, line);
		
		if ( line.empty() )
			continue;

		std::auto_ptr<COutMessage> pMsgOut ( new COutMessage( line.c_str(), "", msgType  ) );

		for (std::vector<CWnd*>::iterator it = m_pMsgDest.begin(); it != m_pMsgDest.end(); ++it)
		{	
			ASSERT( *it != NULL && IsWindow((*it)->m_hWnd) );		
			(*it)->SendMessage(RM_SHOW_USER_MESSAGE, 0, (LPARAM)pMsgOut.get());
		}

	} while (msg);
		
}


#pragma region AppLoadingHelper
//////////////////////////////////////////////////////////////////////////
//Класс отображающий окно процесса загрузки и формирующий файл отчета

AppLoadingHelper::AppLoadingHelper() : wndLoad(NULL)
{

}

AppLoadingHelper::~AppLoadingHelper()
{
	delete wndLoad;
}

void AppLoadingHelper::Release()
{
	wndLoad->DestroyWindow();
	delete wndLoad;
	wndLoad = NULL;
}


AppLoadingHelper& AppLoadingHelper::Instance()
{
	static AppLoadingHelper obj;
	return obj;
}

BOOL AppLoadingHelper::Initialize(const char* pszFilePath)
{

	CRect rcWnd;
	CWnd* pMainWnd = ::AfxGetApp()->GetMainWnd();
	pMainWnd->GetWindowRect(&rcWnd);
	
	CSize cs(rcWnd.Width()/6, rcWnd.Height()/6); 
	rcWnd.DeflateRect(cs);

	wndLoad = new CLoadingProrgessDlg();	
	if ( !wndLoad->Create(IDD_LOADINGDLG, pMainWnd) )
		return FALSE;

	wndLoad->MoveWindow(rcWnd);
	wndLoad->ShowWindow(SW_SHOW);

	CComPtr<IXMLDOMDocument> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) )
		return FALSE;

	VARIANT_BOOL bSuccess = false;

	hr = pXMLDoc->load(CComVariant(pszFilePath), &bSuccess);

	if ( FAILED(hr) || !bSuccess )
		return FALSE;

	long lNodesCount = 0;
	AppXMLHelper::GetNodesCount(pXMLDoc, "//CMDMANAGER | //CONVERTER | //GAS | //DEVICE | //DS | //SCRIPT | //WATCHDOG | //DATALOGER | //SCHEMA", lNodesCount);

	std::vector<std::string> sh;
	AppXMLHelper::GetSchems(pXMLDoc, sh );

	for(std::size_t i=0; i<sh.size(); i++)
		AppXMLHelper::GetNodesCount(sh[i].c_str(), "//CONTROLBAR | //LAYER/ITEM", lNodesCount );

	::SendMessage(wndLoad->m_hWnd, RM_LOADING_INIT, lNodesCount, NULL);
	
	MakeHeader();
	return true;
}

void AppLoadingHelper::MakeHeader()
{
	
	//Получение информации о версии приложения
	CString appPath = ::AfxGetApp()->m_pszExeName;
	appPath  += ".exe";

	const CString strGet[10] = 
	{ 
		"CompanyName",  "FileDescription",  "FileVersion", "InternalName",  "LegalCopyright",  
		"LegalTrademarks", "OriginalFilename", "ProductName",  "ProductVersion",  "Comments" 
	};



	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSize(appPath, &dwHandle);

	char* pData = (char*)::LocalAlloc(LMEM_FIXED, dwSize);	

	if ( ::GetFileVersionInfo(appPath, 0, dwSize, pData))
	{
		struct LANGANDCODEPAGE 
		{
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;


		UINT len; //
		VerQueryValue(pData, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &len);

		CString str;
		LPCSTR pVal;

		str.Format("\\StringFileInfo\\%04X%04X\\ProductName",lpTranslate->wLanguage, lpTranslate->wCodePage );
		VerQueryValue(pData, str.GetBuffer(MAX_PATH), (LPVOID*)&pVal, &len);
		str.ReleaseBuffer();
		str = pVal;
		str += " ";

		vTitleLines.push_back((LPCTSTR)str);

		str.Format("\\StringFileInfo\\%04X%04X\\ProductVersion",lpTranslate->wLanguage, lpTranslate->wCodePage );	
		VerQueryValue(pData, str.GetBuffer(MAX_PATH), (LPVOID*)&pVal, &len);
		str.ReleaseBuffer();
		str = pVal; 
		
		int verinf[4];
		sscanf_s(pVal, "%d.%d.%d.%d", &verinf[0], &verinf[1], &verinf[2], &verinf[3]);

		
		str.Format("v.%d.%d Build %d<br>", verinf[0], verinf[1],  verinf[3]);
		vTitleLines.push_back((LPCTSTR)str);

		str.Format("\\StringFileInfo\\%04X%04X\\CompanyName",lpTranslate->wLanguage, lpTranslate->wCodePage );
		VerQueryValue(pData, str.GetBuffer(MAX_PATH), (LPVOID*)&pVal, &len);
		str.ReleaseBuffer();
		str = pVal;
		str += "<br>";
		vTitleLines.push_back((LPCTSTR)str);

		str.Format("\\StringFileInfo\\%04X%04X\\LegalCopyright",lpTranslate->wLanguage, lpTranslate->wCodePage );
		VerQueryValue(pData, str.GetBuffer(MAX_PATH), (LPVOID*)&pVal, &len);
		str.ReleaseBuffer();
		str = pVal;
		str += "<br>";
		vTitleLines.push_back((LPCTSTR)str);

		str = CTime::GetCurrentTime().Format("%d %B %Y<br>%A %H:%M");		
		vTitleLines.push_back((LPCTSTR)str);
	}

	::LocalFree(pData);
}

long AppLoadingHelper::AddSection(std::string title, std::string name, int nSectionSize)
{
	if ( wndLoad == NULL )
		return -1L;

	DOCUMENT_SECTION_INFO dsi;
	dsi.title = title.c_str();
	dsi.subItemsCount = nSectionSize;

	CString str;
	str.Format("<h3><a name=\"%s\">%s</a></h3>", name.c_str(), title.c_str() );
	vContentLines.push_back((LPCTSTR)str);
	vContentLines.push_back("<div>");

	return ::SendMessage(wndLoad->m_hWnd, RM_ADD_LOADING_SECTION, 0, (LPARAM)&dsi);
}

void AppLoadingHelper::CloseCurrentSection()
{
	vContentLines.push_back("</div>");
}

long AppLoadingHelper::StepSection(int nSecId, bool bOk /*= true*/)
{
	if ( wndLoad == NULL )
		return -1L;

	::SendMessage(wndLoad->m_hWnd, RM_STEP_SECTION, nSecId, (LPARAM)&bOk);
	return 0L;
}

void AppLoadingHelper::AddContentMessage(std::string text, COutMessage::e_MessageType mt)
{
	CString cssClass;
	switch ( mt )
	{	
		case COutMessage::MSG_WARNING: 
			cssClass = "message warning";
			break;

		case COutMessage::MSG_ERROR:
			cssClass = "message error";
			break;

		default:
			cssClass = "message ok";
			break;
	}

	CString str;
	str.Format("<p class=\"%s\">%s</p>", (LPCTSTR)cssClass, text.c_str() );
	vContentLines.push_back((LPCTSTR)str);
}

BOOL AppLoadingHelper::MakeReport(std::string templ, std::string outPath, BOOL bRes)
{
	::SendMessage(wndLoad->m_hWnd, RM_LOADING_COMPLETE, bRes, NULL);

	std::ifstream temaplate(templ);
	if ( !temaplate.good() )
		return false;


	std::vector<std::string> strOut; 

	int iTitle;
	int iContent;	
	
	while ( !temaplate.eof() )
	{
		std::string line;
			
		getline(temaplate, line).good();
			
		if( line.find("end .top") != string::npos )
			iTitle =  strOut.size();
			
		if( line.find("end .content") != string::npos )
			iContent =   strOut.size();			

		strOut.push_back(line);
	}
		
	strOut.insert( strOut.begin() + iContent, vContentLines.begin(), vContentLines.end());
	strOut.insert( strOut.begin() + iTitle, vTitleLines.begin(), vTitleLines.end());
	
	std::ofstream outHTML(outPath);
		
	if ( !outHTML.good() )
		return false;

	copy(strOut.begin(), strOut.end(), ostream_iterator<string>(outHTML,"\n") );
	return true;
}
#pragma endregion 


//////////////////////////////////////////////////////////////////////////

CPopupGraphWnd AppXYPlotWndHelper::pGraphWnd;

AppXYPlotWndHelper::AppXYPlotWndHelper()
{
	pGraphWnd.DestroyWindow();
}
AppXYPlotWndHelper::~AppXYPlotWndHelper()
{

}

BOOL AppXYPlotWndHelper::Init(CWnd* pParent)
{

	CRect rc;
	GetWindowRect(GetDesktopWindow(), &rc);

	int xOffset = rc.Width() / 5;
	int yOffset = rc.Height() / 4;

	rc.DeflateRect(xOffset, yOffset);

	HCURSOR hCur = ::LoadCursor(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CUR_ARROW));

	if ( !pGraphWnd.CreateEx(WS_EX_TOPMOST, ::AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, hCur) , "Graph popup", WS_POPUPWINDOW, rc, pParent, IDC_STATIC ) )
	{
		TRACE( "\nPopup graphic window creation failed" );		
		return FALSE;
	}
	
	return TRUE;
}



BOOL AppXYPlotWndHelper::IsGraphWndVisible()
{
	return pGraphWnd.IsWindowVisible();
}

void AppXYPlotWndHelper::ShowGraphWnd(CPoint& pt, bool bShow)
{
	pGraphWnd.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	//pGraphWnd.MoveWindow(pt.x, pt.y, 600, 400);	
}

BOOL AppXYPlotWndHelper::AddDataSourceProfile(std::string name, bool bClearContent/* = true*/)
{
	return pGraphWnd.AddDataSrc(name, bClearContent);
}

BOOL AppXYPlotWndHelper::AddCheckPoint(std::string name)
{
	std::vector<double> vals;
	DataCollector::Instance().GetTimeBuffer(vals);

	if ( vals.size() == 0 )
		return FALSE;

	return pGraphWnd.AddCheckPoint(name, vals.back());
}

//////////////////////////////////////////////////////////////////////////
//  [3/10/2011 Johnny A. Matveichik]

#ifdef USE_CHEM_LIB_FEATURES

AppThermHelper& AppThermHelper::Instance()
{
	static AppThermHelper obj;
	return obj;
}

AppThermHelper::AppThermHelper() 
{
	pTHDB = new CSpeciesDB();	
	pSS   = new CSpecieSet();	
}

AppThermHelper::~AppThermHelper()
{

	std::map<std::string, CIdealGasMix*>::iterator it = mapMixtures.begin();
	for( ; it != mapMixtures.end(); ++it)
	{
		delete it->second;
		//pTHDB->Detach(*(it->second));
	}
		

	pSS->Detach(pTHDB);
	delete pSS;
	delete pTHDB;	
}

bool AppThermHelper::AddMixture(IXMLDOMNode* pNode, std::pair<std::string, std::string>& mix)
{
	

	if ( pNode == NULL )
		return false;

	CComPtr<IXMLDOMNodeList> pSpeciesList;
	pNode->selectNodes( CComBSTR("SPEC"), &pSpeciesList);

	long lSpecCount;	

	pSpeciesList->get_length(&lSpecCount);		
	pSpeciesList->reset();

	CString gasName;
	if ( !AppXMLHelper::GetNodeAttributeByName(pNode, "ID", gasName) || gasName.IsEmpty())
		return false;

	mix.first = (LPCTSTR)gasName;

	CString strComp;

	for(int i = 0; i < lSpecCount; i++)
	{

		CComPtr<IXMLDOMNode>	 pSpec;
		pSpeciesList->get_item(i, &pSpec);

		CString val;
		
		if ( !AppXMLHelper::GetNodeAttributeByName(pSpec, "NAME", val) || val.IsEmpty())
			return false;
		
		if ( pTHDB->FindSpecie((LPCTSTR)val) < 0 )
		{
			strComp.Format(IDS_CHEM_CHECK0, val); //Species <%s> was not found in thermal database file
			mix.second = (LPCTSTR)strComp;
			return false;
		}

		pSS->Add((LPCTSTR) val);

		strComp += val; 
		strComp += '='; 

		if ( !AppXMLHelper::GetNodeAttributeByName(pSpec, "FRACTION", val) || val.IsEmpty())
			return false;
	
		strComp += val; 
		strComp += ' ';
		double frac  = atof((LPCTSTR)val);		
		
	}
	mix = std::make_pair( (LPCTSTR)gasName, strComp);
	return mapComp.insert( mix ).second;
}

bool AppThermHelper::Init(std::string filePath)
{
	std::ifstream inp( filePath.c_str() ); 
	if ( inp.good() )
	{
		std::strstream str;

		if ( pTHDB->Create(filePath.c_str(), &str) != ckcl::NOERRORS )
		{
			AppMessagesProviderHelper::Instance().ShowMessage(str, COutMessage::MSG_ERROR);
			return false;
		}	
		return true;
	}
	return false;
}

bool AppThermHelper::BuildMixtures()
{
	if ( pSS->Attach(pTHDB) != ckcl::NOERRORS )
		return false;

	mapMixtures.clear();

	for ( std::map<std::string, std::string>::iterator it = mapComp.begin(); it != mapComp.end(); ++it )
	{
		CIdealGasMix* pGM = new CIdealGasMix(*pTHDB);
		if ( pGM->SetComposition( (*it).second  ) != ckcl::NOERRORS )
			return false;

		mapMixtures.insert( std::make_pair( (*it).first, pGM) );
	}
	return true;
}

bool AppThermHelper::SetMassFlowComposition(MassFlow& mf, std::string gasName)
{
	std::map<std::string, CIdealGasMix*>::iterator itFind = mapMixtures.find(gasName);
	if ( itFind == mapMixtures.end() )
		return false;

	mf.SetComposition( *(itFind->second) );	

	return true;
}
#endif //USE_CHEM_LIB_FEATURES