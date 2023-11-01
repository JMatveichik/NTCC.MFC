#include "stdafx.h"
#include "ah_loadcfg.h"
#include "ah_xml.h"
#include "ah_project.h"

#include "DLGs/LoadingProrgessDlg.h"

AppLoadingHelper::AppLoadingHelper() : wndLoad(NULL)
{

}

AppLoadingHelper::~AppLoadingHelper()
{
	/*delete wndLoad;*/
}

void AppLoadingHelper::Release()
{
	wndLoad->DestroyWindow();
	delete wndLoad;
	wndLoad = NULL;
	vTitleLines.clear();
	vContentLines.clear();
}

BOOL AppLoadingHelper::Initialize(IXMLDOMDocument* pXMLDoc)
{
	CRect rcWnd;
	CWnd* pMainWnd = ::AfxGetApp()->GetMainWnd();
	pMainWnd->GetWindowRect(&rcWnd);

	CSize cs(rcWnd.Width() / 4, rcWnd.Height() / 4); 
	rcWnd.DeflateRect(cs);

	wndLoad = new CLoadingProrgessDlg();
	if ( !wndLoad->Create(IDD_LOADINGDLG, pMainWnd) )
		return FALSE;

	//wndLoad->MoveWindow(rcWnd);
	wndLoad->CenterWindow();
	wndLoad->ShowWindow(SW_SHOW);


	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	long lTotalNodes = 0;

#ifdef USE_CHEM_LIB_FEATURES
	lTotalNodes = appXML.GetNodesCount(pXMLDoc, "//CMDMANAGER | //CONVERTER | //GAS | //DS | //SCRIPT | //TIMER| //WATCHDOG | //DATALOGER | //SCHEMA");
#else
	lTotalNodes = appXML.GetNodesCount(pXMLDoc, "//CMDMANAGER | //CONVERTER | //DS | //SCRIPT | //TIMER | //WATCHDOG | //DATALOGER | //SCHEMA");
#endif

	CComPtr<IXMLDOMNode> pMainNode;
	HRESULT hr = pXMLDoc->selectSingleNode(CComBSTR("NTCCPROJECT"), &pMainNode);

	CString devSel;
	std::string mode;

	appXML.GetNodeAttributeByName(pMainNode, "MODE", mode);

	if ( mode.length() == 0 )
		devSel = "//DEVICE";
	else
		devSel.Format("NTCCPROJECT/DEVICES[@MODE='%s']/DEVICE", mode.c_str() );

	lTotalNodes += appXML.GetNodesCount(pXMLDoc, devSel);

	CComPtr<IXMLDOMNodeList>	 pShNodes;
	hr = pXMLDoc->selectNodes(CComBSTR("NTCCPROJECT/WORKSPACE/SCHEMA"), &pShNodes);

	if ( SUCCEEDED(hr) )
	{
		long lShCount = 0;
		pShNodes->get_length(&lShCount);

		for (long l = 0; l < lShCount; l++)
		{
			CComPtr<IXMLDOMNode> pNode;
			pShNodes->get_item(l, &pNode);
			
			std::string sVal;

			if ( appXML.GetNodeAttributeByName(pNode, "CONFIGFILE", sVal) )
			{
				std::string shPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_CONFIG, sVal);
				lTotalNodes += appXML.GetNodesCount(shPath.c_str(), "//CONTROLBAR | //LAYER/ITEM");
			}
		}
	}

	::SendMessage(wndLoad->m_hWnd, RM_LOADING_INIT, lTotalNodes, NULL);

	MakeHeader();
	return TRUE;
}

BOOL AppLoadingHelper::Initialize(const char* pszFilePath)
{
	CComPtr<IXMLDOMDocument> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) )
		return FALSE;

	VARIANT_BOOL bSuccess = false;

	hr = pXMLDoc->load(CComVariant(pszFilePath), &bSuccess);

	if ( FAILED(hr) || !bSuccess )
		return FALSE;

	return Initialize(pXMLDoc);
}

UINT AppLoadingHelper::LoadingDlgResult(BOOL bRes/* = false*/)
{
	if ( !bRes )
		return wndLoad->RunModalLoop();
	else
		return 1;
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
		sscanf_s(pVal, "%d,%d,%d,%d", &verinf[0], &verinf[1], &verinf[2], &verinf[3]);


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

	//TRACE("\n\tTITLE : %s\t\t NAME : %s\t\t %04d", title.c_str(), name.c_str(), nSectionSize );
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

		getline(temaplate, line);

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