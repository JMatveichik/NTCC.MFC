// ProjectDoc.cpp : implementation file
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "ProjectDoc.h"
#include "PopupMessageWnd.h"

#include "ah_xml.h"
#include "ah_msgprovider.h"
#include "ah_errors.h"
#include "ah_loadcfg.h"
#include "ah_project.h"
#include "ah_chem.h"
#include "ah_scripts.h"
#include "ah_sms.h"

#include "FSDoc.h"
#include "ModuleFrm.h"
#include "Automation\DeviceFactory.h"

#include "Automation\BaseFlowController.h"
#include "Automation\Thrermocon011.h"
#include "Automation\ADAM4000Series.h"
#include "Automation\ADAM6000Series.h"
#include "Automation\Timers.h"
#include "Automation\WatchDogs.h"


#ifdef _DEBUG
#define new DEBUG_NEW	
#endif




// CProjectDoc

IMPLEMENT_DYNCREATE(CProjectDoc, CDocument)

CProjectDoc::CProjectDoc() 
{
	
}

BOOL CProjectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CProjectDoc::~CProjectDoc()
{
	
}


BEGIN_MESSAGE_MAP(CProjectDoc, CDocument)
	
END_MESSAGE_MAP()


// CProjectDoc diagnostics

#ifdef _DEBUG
void CProjectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CProjectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CProjectDoc serialization

void CProjectDoc::Serialize(CArchive& ar)
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
#endif

const DATA_SOURCES_VIEW_GROUPS& CProjectDoc::GetLayout(UINT nType) const
{
	switch (nType)
	{
		case VIEW_DS_USER_LAYOUT:
			return m_dsGroupUser;
		
		case VIEW_DS_BY_TYPE_LAYOUT:
			return m_dsGroupByType;

		case VIEW_DS_BY_DEVICE_LAYOUT:
			return  m_dsGroupByDevices;

		case VIEW_DS_BY_SCRIPT_LAYOUT:
			return  m_dsGroupByScript;

		default:
			return m_dsGroupByType;	
	};		
}

void ClearGroup(DATA_SOURCES_VIEW_GROUPS* pGroup)
{
	DATA_SOURCES_VIEW_GROUPS::iterator it = pGroup->begin();
	for(it; it != pGroup->end(); ++it)
		delete (*it);

	pGroup->clear();
}
void CProjectDoc::DeleteContents()
{
	ClearGroup(&m_dsGroupUser);
	ClearGroup(&m_dsGroupByType);
	ClearGroup(&m_dsGroupByDevices);	
	ClearGroup(&m_dsGroupByScript);	

	CDocument::DeleteContents();
}


BOOL CProjectDoc::LoadConfig(const char* pszFileName)
{	

	size_t nWarn = 0;
	size_t nErr  = 0;
	size_t nSectionErr = 0;

	CComPtr<IXMLDOMDocument2> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument60));

	AppErrorsHelper& AppErrors = AppErrorsHelper::Instance();

	CString strMsg;
	const AppMessagesProviderHelper& mh = AppMessagesProviderHelper::Instance();


#define SECTION_LOADING_MSG(msg, brl) mh.ShowMessage(msg, COutMessage::MSG_TEXT, brl, TRUE);

#define SECTION_LOADING_RES() strMsg.Format("%s (%d errors)", (nSectionErr == 0) ? "succeeded" : "failed", nSectionErr); \
							  mh.ShowMessage((LPCTSTR)strMsg, (nSectionErr == 0) ? COutMessage::MSG_OK : COutMessage::MSG_ERROR, TRUE, FALSE );

#pragma region Основные параметры

	
	SECTION_LOADING_MSG("Loading configuration...", TRUE);
	SECTION_LOADING_MSG("------------------------", TRUE);

	
	if ( FAILED(hr) )
	{
		
		mh.ShowMessage("Ошибка создания интерфейса IXMLDOMDocument", COutMessage::MSG_ERROR);
		return FALSE;
	}

	VARIANT_BOOL bSuccess = false;

	
	SYSTEMTIME st1;
	GetSystemTime(&st1);


	hr = pXMLDoc->load(CComVariant(pszFileName), &bSuccess);
	pXMLDoc->setProperty(L"SelectionLanguage", CComVariant(L"XPath"));

	//////////////////////////////////////////////////////////////////////////
	///ПРЕДВАРИТЕЛЬНАЯ ОБРАБОТКА КОНФИГУРАЦИИ
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	appXML.PreprocessDocument(pXMLDoc);
	
	////////////////////////////////////////////////////////////////
	// ИНИЦИАЛИЗАЦИЯ ОТОБРАЖЕНИЕ ПРОЦЕССА ЗАГРУЗКИ 
	AppLoadingHelper& LH =  AppLoadingHelper::Instance();
	LH.Initialize(pXMLDoc);
	long lSecID = 0;


	if ( FAILED(hr) || !bSuccess )
	{
		mh.ShowMessage("Невозможно загрузить файл конфигурации", COutMessage::MSG_ERROR);
		return FALSE;
	}

	CComPtr<IXMLDOMNode> pMainNode;
	hr = pXMLDoc->selectSingleNode(CComBSTR("NTCCPROJECT"), &pMainNode);

	if ( FAILED(hr) || pMainNode == NULL)
	{
		mh.ShowMessage("Некорректный формат файла конфигурации", COutMessage::MSG_ERROR);
		return FALSE;
	}
	

	std::string sVal;
	BOOL bRes; 
	std::string mode;
	appXML.GetNodeAttributeByName(pMainNode, "MODE", mode);

	boost::algorithm::to_upper(mode);
	

#pragma endregion Основные параметры

	///////////////////////////////////////////////////
	///				МЕНЕДЖЕРЫ КОМАНД				///   
	///////////////////////////////////////////////////

#pragma region Менеджеры команд


	CComPtr<IXMLDOMNode> pManagersNode;
	hr = pMainNode->selectSingleNode(CComBSTR("CMDMANAGERS"), &pManagersNode);
	
	if ( SUCCEEDED(hr) && pManagersNode != NULL )
	{	
		CComPtr<IXMLDOMNodeList> pCmdManagers;
		hr = pManagersNode->selectNodes(CComBSTR("CMDMANAGER"), &pCmdManagers);	
		long lComCount;

		pCmdManagers->get_length(&lComCount);
		pCmdManagers->reset();

		if (SUCCEEDED(hr) && lComCount > 0)
		{
			long lSecID = LH.AddSection("Communication ports", "CMDMANAGERS", lComCount);
			
			SECTION_LOADING_MSG("Communication ports...", FALSE);			

			for(int i = 0; i < lComCount; i++)
			{
				CComPtr<IXMLDOMNode>	 pCmds;
				pCmdManagers->get_item(i, &pCmds);

				CComPtr<IXMLDOMNamedNodeMap> pCmdsAttr;
				pCmds->get_attributes(&pCmdsAttr);

				int comPortNo = 0;
				bRes = appXML.GetAttributeByName(pCmdsAttr, "COM",	sVal);
				comPortNo = atoi(sVal.c_str());

				int nBaudRate = 0;
				bRes = appXML.GetAttributeByName(pCmdsAttr, "BAUD",	sVal);
				nBaudRate = atoi(sVal.c_str());

				int maxRetr = 0;
				bRes = appXML.GetAttributeByName(pCmdsAttr, "MAXRETRIES",	sVal);
				maxRetr = atoi(sVal.c_str());

				std::string logFilePath;
				bRes = appXML.GetAttributeByName(pCmdsAttr, "LOGDIR",	logFilePath);

				logFilePath = NTCCProject::Instance().GetPath( NTCCProject::PATH_LOG, logFilePath);

				auto_ptr<CommandManager>  pCM ( new CommandManager() );

				strMsg.Format("Инициализация COM%d на скорости %d bps...", comPortNo, nBaudRate);
				bool bOk = false;

				if (! pCM->Create(comPortNo, nBaudRate) )
				{
					strMsg += "Ошибка";
					nErr++; nSectionErr++;
				}
				else
				{
					CommandManager::AddCommandManager( pCM.get()  );
					strMsg += "Успешно<br>";
				
					if ( logFilePath.length() != 0)
					{
						strMsg += "Включен журнал команд : ";
						pCM->EnableCommandLog(logFilePath.c_str() );
						strMsg += logFilePath.c_str();
						strMsg += "<br>";

					}
				
					pCM->StartCommandProcessing(maxRetr);
					bOk = true;
					pCM.release();
				}
			
				LH.StepSection(lSecID, bOk);
				LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
			}

			SECTION_LOADING_RES()
			LH.CloseCurrentSection();
		}
	}

	

#pragma endregion Менеджеры команд

	///////////////////////////////////////////////////
	///				SMS РАССЫЛКИ   					///   
	///////////////////////////////////////////////////

	CComPtr<IXMLDOMNode> pSMSNode;
	hr = pMainNode->selectSingleNode(CComBSTR("SMSSENDER"), &pSMSNode);
	if ( SUCCEEDED(hr) )
	{
		if (!AppSMSHelper::Instance().Initialize(pSMSNode) )
		{
			//TRACE ("\n\tSMS Send helper initialization failure!\n");
		}
	}

	///////////////////////////////////////////////////
	///				ВТОРИЧНЫЕ ПРЕОБРАЗОВАТЕЛИ		///   
	///////////////////////////////////////////////////
#pragma region Вторичные преобразователи 


	CComPtr<IXMLDOMNode> pConvNode;
	hr = pMainNode->selectSingleNode(CComBSTR("CONVERTERS"), &pConvNode);
	

	if ( SUCCEEDED(hr) && pConvNode != NULL )		
	{
		SECTION_LOADING_MSG("Secondary converters...", FALSE);	
		nSectionErr = 0;

		CComPtr<IXMLDOMNodeList> pConvList;
		hr = pConvNode->selectNodes(CComBSTR("CONVERTER"), &pConvList);

		long lConvCount;	

		pConvList->get_length(&lConvCount);		
		pConvList->reset();

		long lSecID = LH.AddSection("Secondary converters", "CONVERTERS", lConvCount);

		for(int i = 0; i < lConvCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pConvNode;
			pConvList->get_item(i, &pConvNode);

			
			ATTRIBUTES attrs;
			appXML.GetNodeAttributes(pConvNode, attrs);

			UINT error;
			BaseConverter* pConv = XMLObjectFactory::CreateConverterObject(pConvNode, error);
			
			bool bOk = false;

			if ( pConv != NULL )
			{
				BaseConverter::AddConverter(pConv);				
				strMsg.Format("Добавлен вторичный преобразователь %s - %s", pConv->ToString().c_str(), pConv->Description().c_str() );
				bOk = true;
			}
			else
			{
				std::string err = AppErrors.GetLastError(error); 
				strMsg.Format("Error 0x%04X - %s", error, err.c_str());							
				nErr++; nSectionErr++;
			}				

			LH.StepSection(lSecID, bOk);
			LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
		}	
		
		SECTION_LOADING_RES();
		LH.CloseCurrentSection();
	}
	

	

#pragma endregion Вторичные преобразователи 
	///////////////////////////////////////////////////////
	///		ТЕРМОДИНАМИЧЕСКИЕ ДАННЫЕ	ГАЗОВЫХ СМЕСЕЙ	///   
	///////////////////////////////////////////////////////

#ifdef USE_CHEM_LIB_FEATURES
	
	CComPtr<IXMLDOMNode> pGasesNode;
	hr = pMainNode->selectSingleNode(CComBSTR("GASES"), &pGasesNode);

	
	if ( SUCCEEDED(hr) && pGasesNode != NULL )
	{
		SECTION_LOADING_MSG("Thermodynamic library data...", FALSE);
		nSectionErr = 0;

		CString filePath;
		if ( !appXML.GetNodeAttributeByName(pGasesNode, "THERMFILE", filePath) ||
			 !AppThermHelper::Instance().Init((LPCTSTR)filePath) )
		{
			//	error
		}


		CComPtr<IXMLDOMNodeList> pGasesList;
		pGasesNode->selectNodes( CComBSTR("GAS"), &pGasesList);

		long lGasCount;	

		pGasesList->get_length(&lGasCount);		
		pGasesList->reset();


		long lSecID = LH.AddSection("Составы газовых смесей", "GASES", lGasCount);
		
		for(int i = 0; i < lGasCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pGas;
			pGasesList->get_item(i, &pGas);

			bool bOk = false;
			std::pair<std::string, std::string> mix;

			if ( AppThermHelper::Instance().AddMixture(pGas, mix) )
			{
				strMsg.Format("Добавлена газовая смесь %s (%s)", mix.first.c_str(), mix.second.c_str() );
				bOk = true;
			}
			else
			{
				strMsg.Format("Ошибка создания газовой смеси  %s (%s)", mix.first.c_str(), mix.second.c_str() );
				nErr++; nSectionErr++;
			}
			LH.StepSection(lSecID, bOk);
			LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
		}

		SECTION_LOADING_RES();
		LH.CloseCurrentSection();
	}
	

	AppThermHelper& th = AppThermHelper::Instance();
	th.BuildMixtures();

#endif //USE_CHEM_LIB_FEATURES

	///////////////////////////////////////////////////
	///						УСТРОЙСТВА				///   
	///////////////////////////////////////////////////

#pragma region Промышленная сеть

	

	CString devSel;

	if ( mode.length() == 0 )
		devSel = "DEVICES/DEVICE";
	else
		devSel.Format("DEVICES[@MODE='%s']/DEVICE", mode.c_str());

	CComPtr<IXMLDOMNodeList> pDevicesList;
	hr = pMainNode->selectNodes(CComBSTR((LPCTSTR)devSel), &pDevicesList);

	//////////////////////////////////////////////////////////////////////////
	if ( SUCCEEDED(hr) || pDevicesList != NULL)
	{
		SECTION_LOADING_MSG("Industrial network...", FALSE);		
		nSectionErr = 0;

		long lDevCount;	

		pDevicesList->get_length(&lDevCount);		
		pDevicesList->reset();

		long lSecID = LH.AddSection("Industrial net", "DEVICE", lDevCount);

		for(int i = 0; i < lDevCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pDevNode;
			pDevicesList->get_item(i, &pDevNode);
			
			UINT error;
			IdentifiedPhysDevice* pDev = XMLObjectFactory::CreateDeviceObject(pDevNode, error);
			
			bool bOk = false;

			if ( pDev != NULL )
			{
						
				IdentifiedPhysDevice::AddDevice(pDev);
				pDev->Start();

				strMsg.Format("Добавлено устройство (%s %s) (Name : %s)", pDev->Manufacturer().c_str(), pDev->Model().c_str(), pDev->Name().c_str());
				bOk = true;
			}
			else
			{
				std::string err = AppErrors.GetLastError(error); 
				strMsg.Format("Error 0x%04X - %s", error, err.c_str());							
				nErr++; nSectionErr++;
			}	

			LH.StepSection(lSecID, bOk);
			LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
		}

		SECTION_LOADING_RES();

		LH.CloseCurrentSection();
	}
	

#pragma endregion Промышленная сеть
	

#pragma region ИСТОЧНИКИ ДАННЫХ 
	
	CComPtr<IXMLDOMNode> pDSRootNode;
	hr = pMainNode->selectSingleNode(CComBSTR("DATASOURCES"), &pDSRootNode);

	if ( SUCCEEDED(hr) && pDSRootNode != NULL )
	{
// 		CString importPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_CONFIG, "import.xml");
// 		appXML.ImportNodes(pDSRootNode, "DS", (LPCTSTR)importPath);

		SECTION_LOADING_MSG("Inner variables...", FALSE);
		nSectionErr = 0;

		std::string logPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_LOG, "DS\\", FALSE);
		DataSrc::EnableLog( logPath.c_str(), false);

		CComPtr<IXMLDOMNodeList> pDSNodesList;
		hr = pDSRootNode->selectNodes(CComBSTR("DS"), &pDSNodesList);

		long lDSCount;
		
		pDSNodesList->get_length(&lDSCount);
		pDSNodesList->reset();

		
		long lSecID = LH.AddSection("Inner variables", "DATASOURCES", lDSCount);

		for (long num = 0; num < lDSCount; num++ )
		{
			CComPtr<IXMLDOMNode> pDSNode;
			pDSNodesList->get_item(num, &pDSNode);

			UINT error;
			DataSrc* pDS = XMLObjectFactory::CreateDataSourceObject(pDSNode, error);
			bool bOk = false;
			if ( pDS != NULL )
			{
				if ( !DataSrc::AddDataSource(pDS) )
				{
					strMsg.Format("Источник данных %s : %s не может быть добавлен.", pDS->Name().c_str(), pDS->Description().c_str());
					bOk = false;
					nErr++; nSectionErr++;
					delete pDS; 
				}
				else {
					strMsg.Format("Добавлен источник данных %s :%s", pDS->Name().c_str(), pDS->Description().c_str());
					bOk = true;
				}
			}
			else
			{
				std::string str = AppErrors.GetLastError(error);
				strMsg.Format("Error 0x%04X - %s", error, str.c_str());
				nErr++;
			}

			LH.StepSection(lSecID, bOk);
			LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
		}
		
		SECTION_LOADING_RES();
		LH.CloseCurrentSection();
	}
	

#pragma endregion ИСТОЧНИКИ ДАННЫХ 

#pragma region Повторная обработка устройств после создания источников данных

	CComPtr<IXMLDOMNodeList> pDevList;
	hr = pMainNode->selectNodes(CComBSTR((LPCTSTR)devSel), &pDevList);

	//////////////////////////////////////////////////////////////////////////

	if ( SUCCEEDED(hr) || pDevList != NULL)
	{
		long lDevCount;	

		pDevicesList->get_length(&lDevCount);		
		pDevicesList->reset();

		for(int i = 0; i < lDevCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pDevNode;
			pDevicesList->get_item(i, &pDevNode);

			std::string name;
			if ( appXML.GetNodeAttributeByName(pDevNode, "NAME", name) )
			{
				IdentifiedPhysDevice* pDev = IdentifiedPhysDevice::GetDevice(name);
				if (pDev != NULL)
					pDev->PostConfigProcessing(pDevNode);
			}
		}
	}

#pragma endregion

#pragma region DATACOLLECTOR


	CComPtr<IXMLDOMNode> pDataCollectorNode;
	hr = pMainNode->selectSingleNode( CComBSTR("DATACOLLECTOR"), &pDataCollectorNode );
	
	
	//время жизни даных по умолчанию 1 час
	double liveInterval = 3600.0;  

	//интервал обновления по умолчанию 2 сек
	double updateInterval = 2.0;

	if ( SUCCEEDED(hr) )
	{
		std::string sVal;

		if ( appXML.GetNodeAttributeByName(pDataCollectorNode, "LIVEINTERVAL", sVal) )
			liveInterval = atof(sVal.c_str());

		if ( appXML.GetNodeAttributeByName(pDataCollectorNode, "UPDATEINTERVAL", sVal) )
			updateInterval = atof(sVal.c_str());
	}
	
	if ( !DataCollector::Instance().Initialize(liveInterval, updateInterval) ) 
	{	
		AfxMessageBox("Ошибка инициализации сбора данных.", MB_OK|MB_ICONEXCLAMATION );
	}
		
	
	//запускаем сборщик данных (интервал опроса переводим в милисекунды)
	if (!DataCollector::Instance().Start(unsigned long(updateInterval*1000) ) )
	{	
		AfxMessageBox("Ошибка запуска сбора данных.", MB_OK|MB_ICONEXCLAMATION );
	}

#pragma endregion

#pragma region Управляющие алгоритмы

		
	CComPtr<IXMLDOMNode> pWorkScriptsRootNode;
	hr = pMainNode->selectSingleNode(CComBSTR("SCRIPTS"), &pWorkScriptsRootNode);
		
	if (SUCCEEDED(hr) && pWorkScriptsRootNode != NULL)
	{
		SECTION_LOADING_MSG("OBA Scripts...", TRUE);
		nSectionErr = 0;

		CComPtr<IXMLDOMNodeList> pScriptsList;
		hr = pWorkScriptsRootNode->selectNodes(CComBSTR("SCRIPT"), &pScriptsList);

		long lScriptsCount;	

		pScriptsList->get_length(&lScriptsCount);
		pScriptsList->reset();

		long lSecID = LH.AddSection("OBA Scripts", "SCRIPTS", lScriptsCount);

		for(int i = 0; i < lScriptsCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pScriptItem;
			pScriptsList->get_item(i, &pScriptItem);

			std::string sAtr;

			SCRIPTINFO si;
			
			if ( appXML.GetNodeAttributeByName(pScriptItem, "NAME", sAtr ))
				si.name = sAtr;

			if ( appXML.GetNodeAttributeByName(pScriptItem, "PATH", sAtr ))
				si.path = NTCCProject::Instance().GetPath(NTCCProject::PATH_SCRIPTS, sAtr );

			if ( appXML.GetNodeAttributeByName(pScriptItem, "AUTOSTART", sAtr ))
				si.autostart = atoi(sAtr.c_str() ) > 0;

			bool bOk = false;
				
			if ( !appXML.GetNodeAttributeByName(pScriptItem, "DSID", sAtr ) )
			{
				strMsg.Format("Не задан флаг управления скриптом (%s) тэг DSID", si.name.c_str() );
				LH.AddContentMessage((LPCTSTR)strMsg, COutMessage::MSG_ERROR);				
			}
			else {
				DiscreteOutputDataSrc* pDS = dynamic_cast<DiscreteOutputDataSrc*>(DataSrc::GetDataSource(sAtr));
				if ( pDS == NULL)
				{
					strMsg.Format("Не найден источник данных флага управления скриптом  (%s)", sAtr.c_str() );
					LH.AddContentMessage((LPCTSTR)strMsg, COutMessage::MSG_ERROR);
					nErr++; nSectionErr++;
				}
				else {
					VirtualDevice* pDev = dynamic_cast<VirtualDevice*>( pDS->GetParentDevice() );
					if ( pDev == NULL )
					{
						strMsg.Format("Устройство с которым связан флаг управления скриптом (%s) не является виртуальным", sAtr.c_str() );
						LH.AddContentMessage((LPCTSTR)strMsg, COutMessage::MSG_ERROR);
						nErr++; nSectionErr++;
					}
					else
						si.ds = sAtr;
				}
			}
			
			
			if ( GetFileAttributes( si.path.c_str()) != INVALID_FILE_ATTRIBUTES )
			{
				if ( AppOBAScriptsHelper::Instance().LoadScript( &si ) )
				{	
					strMsg.Format("Управляющий скрипт %s (%s) загружен", si.name.c_str(), si.path.c_str() );
					bOk = true;
				}
				else
				{
					strMsg.Format("Обнаружены ошибки в управляющем скрипте %s (%s)", si.name.c_str(), si.path.c_str() );
					nErr++; nSectionErr++;
					bOk = false;
				}
			}
			else
			{
				strMsg.Format("Файл %s не найден",  si.path.c_str() );
				nErr++; nSectionErr++;
			}
			
			LH.StepSection(lSecID, bOk);
			LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
		}
		

		std::vector<SCRIPTINFO> scripts;
		AppOBAScriptsHelper& scrHelper = AppOBAScriptsHelper::Instance();
		scrHelper.EnumScripts(scripts);

		for(std::vector<SCRIPTINFO>::const_iterator it = scripts.begin(); it != scripts.end(); ++it )
		{
			int iStart = nErr;
			scrHelper.CheckScript( (*it).name, nErr, nWarn);

			nSectionErr = nSectionErr + (nErr - iStart);
		}

		SECTION_LOADING_RES();
		LH.CloseCurrentSection();
	}
	
#pragma endregion

#pragma region Автостарт

	CComPtr<IXMLDOMNode> pOnStartNode;
	hr = pMainNode->selectSingleNode(CComBSTR("ONSTART"), &pOnStartNode);
	
	if (SUCCEEDED(hr) && pOnStartNode != NULL)
	{
		SECTION_LOADING_MSG("Autorun scripts...", FALSE);	
		nSectionErr = 0;

		CComPtr<IXMLDOMNodeList> pScriptsList;
		hr = pOnStartNode->selectNodes(CComBSTR("SCRIPT"), &pScriptsList);

		long lScriptsCount;	

		pScriptsList->get_length(&lScriptsCount);
		pScriptsList->reset();

		long lSecID = LH.AddSection("Autorun", "ONSTART/SCRIPT", lScriptsCount);

		for(int i = 0; i < lScriptsCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pScriptItem;
			pScriptsList->get_item(i, &pScriptItem);

			std::string sAtr;

			std::string name;
			if ( appXML.GetNodeAttributeByName(pScriptItem, "NAME", sAtr ))
				name = sAtr;

			SCRIPTINFO si;
			AppOBAScriptsHelper::Instance().GetScriptInfo(name, si);
			
			bool bOk = false;

			if ( si.autostart )
			{
				strMsg.Format("Управляющий скрипт %s (%s) добавлен в автозапуск", si.name.c_str(), si.path.c_str() );
				m_vOnStartScripts.push_back(si.name);
				bOk = true;
			}
			else
			{
				strMsg.Format("Управляющий скрипт %s не может быть добавлен в автозапуск.", name.c_str() );
				bOk = false;
				nErr++;nSectionErr++;
			}			

			LH.StepSection(lSecID, bOk);
			LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
		}
		
		SECTION_LOADING_RES()
		LH.CloseCurrentSection();
	}

#pragma endregion

#pragma region Автостарт

	CComPtr<IXMLDOMNode> pOnCloseNode;
	hr = pMainNode->selectSingleNode(CComBSTR("ONCLOSE"), &pOnCloseNode);

	if ( SUCCEEDED(hr) && pOnCloseNode!= NULL )
	{
		SECTION_LOADING_MSG("Shutdown scripts...", FALSE);	
		nSectionErr = 0;

		CComPtr<IXMLDOMNodeList> pScriptsList;
		hr = pOnCloseNode->selectNodes(CComBSTR("SCRIPT"), &pScriptsList);

		long lScriptsCount;	

		pScriptsList->get_length(&lScriptsCount);
		pScriptsList->reset();

		long lSecID = LH.AddSection("Preclose application", "ONCLOSE/SCRIPT", lScriptsCount);

		for(int i = 0; i < lScriptsCount; i++)
		{
			CComPtr<IXMLDOMNode>	 pScriptItem;
			pScriptsList->get_item(i, &pScriptItem);

			std::string sAtr;

			std::string name;
			if ( appXML.GetNodeAttributeByName(pScriptItem, "NAME", sAtr ))
				name = sAtr.c_str();

			SCRIPTINFO si;
			AppOBAScriptsHelper::Instance().GetScriptInfo(name, si);

			bool bOk = false;

			if ( si.autostart )
			{
				strMsg.Format("Управляющий скрипт %s (%s) добавлен в выполнение при закрытии", si.name.c_str(), si.path.c_str() );
				m_vOnCloseScripts.push_back(si.name);
				bOk = true;
			}
			else
			{
				strMsg.Format("Управляющий скрипт %s не может быть добавлен в в выполнение при закрытии.", name.c_str() );
				bOk = false;
				nErr++;nSectionErr++;
			}			

			LH.StepSection(lSecID, bOk);
			LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
		}

		SECTION_LOADING_RES()
		LH.CloseCurrentSection();
	}

#pragma endregion

#pragma region Таймеры событий

	CComPtr<IXMLDOMNode> pWorkTimersRootNode;
	hr = pMainNode->selectSingleNode(CComBSTR("TIMERS"), &pWorkTimersRootNode);

	if (SUCCEEDED(hr) && pWorkTimersRootNode != NULL)
	{
		SECTION_LOADING_MSG("Events timers...", FALSE);
		nSectionErr = 0;

		CComPtr<IXMLDOMNodeList> pTMList;
		hr = pWorkTimersRootNode->selectNodes(CComBSTR("TIMER"), &pTMList);

		long lTMCount;	

		pTMList->get_length(&lTMCount);
		pTMList->reset();

		if (lTMCount > 0 )
		{
			long lSecID = LH.AddSection("Events timers", "TIMERS", lTMCount);

			for(int i = 0; i < lTMCount; i++)
			{
				CComPtr<IXMLDOMNode>	 pTMItem;
				pTMList->get_item(i, &pTMItem);

				UINT error;
				BaseTimer* pST = XMLObjectFactory::CreateTimerObject(pTMItem, error);
				bool bOk = false;

				if ( pST != NULL )
				{
					BaseTimer::AddTimer(pST);
					strMsg.Format("Добавлен таймер объект (%s)", pST->Name().c_str());
					bOk = true;
				}
				else
				{
					std::string str = AppErrors.GetLastError(error);
					strMsg.Format("Error 0x%04X - %s", error, str.c_str());
					nErr++; nSectionErr++;
				}

				LH.StepSection(lSecID, bOk);
				LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
			}						
		}
		SECTION_LOADING_RES()
		LH.CloseCurrentSection();
	}

#pragma endregion

#pragma region Сторожевые объекты

	CComPtr<IXMLDOMNode> pWorkWDRootNode;
	hr = pMainNode->selectSingleNode(CComBSTR("WATCHDOGS"), &pWorkWDRootNode);
	
	if (SUCCEEDED(hr) && pWorkWDRootNode != NULL)
	{
		CComPtr<IXMLDOMNodeList> pWDList;
		hr = pWorkWDRootNode->selectNodes(CComBSTR("WATCHDOG"), &pWDList);

		long lWDCount;	

		pWDList->get_length(&lWDCount);
		pWDList->reset();

		if ( lWDCount > 0 )
		{
			SECTION_LOADING_MSG("Watchdogs objects...", FALSE);
			nSectionErr = 0;
		
			long lSecID = LH.AddSection("Watchdogs", "WATCHDOGS", lWDCount);

			for(int i = 0; i < lWDCount; i++)
			{
				CComPtr<IXMLDOMNode>	 pWDItem;
				pWDList->get_item(i, &pWDItem);

				UINT error;
				BaseWatchDog* pWD = XMLObjectFactory::CreateWatchDogObject(pWDItem, error);
				bool bOk = false;

				if ( pWD != NULL )
				{
					pWD->AddWatchDog(pWD);
					strMsg.Format("Добавлен сторожевой объект (%s)", pWD->Name().c_str());
					bOk = true;
				}
				else
				{
					std::string str = AppErrors.GetLastError(error);
					strMsg.Format("Error 0x%04X - %s", error, str.c_str());
					nErr++; nSectionErr++;
				}

				LH.StepSection(lSecID, bOk);
				LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
			}
		
			SECTION_LOADING_RES()
			LH.CloseCurrentSection();
		}
	}

#pragma endregion

#pragma region Рабочее пространство

	CComPtr<IXMLDOMNode> pWorkSpaceRootNode;
	hr = pMainNode->selectSingleNode(CComBSTR("WORKSPACE"), &pWorkSpaceRootNode);
	
	std::map<CFSDoc*, CModuleFrame*> frames;

	if ( SUCCEEDED(hr) && pWorkSpaceRootNode != NULL )
	{
		
		//////////////////////////////////////////////////////////////////////////
		CComPtr<IXMLDOMNode> pPopupSettings;
		hr = pWorkSpaceRootNode->selectSingleNode(CComBSTR("POPUPMSGS"), &pPopupSettings);

		std::string sVal;
		if ( appXML.GetNodeAttributeByName(pPopupSettings, "OPACITY", sVal) )
		{
			double dVal = atof(sVal.c_str());
			
			if (dVal < 0.0)
				dVal = 0.0;

			if (dVal > 100.0)
				dVal = 100.0;

			CPopupMessageWnd::transparencyLevel = 255 - (BYTE)(255 * ( 100.0 - dVal ) / 100.0);
		}



		//////////////////////////////////////////////////////////////////////////
		CComPtr<IXMLDOMNodeList> pDataLoggers;
		hr = pWorkSpaceRootNode->selectNodes(CComBSTR("DATALOGER"), &pDataLoggers);

		if (SUCCEEDED(hr) && pDataLoggers != NULL) 
		{			
			SECTION_LOADING_MSG("Data archives...", FALSE);
			nSectionErr = 0;

			long lDLDocCount;	

			pDataLoggers->get_length(&lDLDocCount);
			pDataLoggers->reset();

			long lSecID = LH.AddSection("Data archives", "DATALOGERS", lDLDocCount);

			for(int i = 0; i < lDLDocCount; i++)
			{
				CComPtr<IXMLDOMNode>	 pDLNode;
				pDataLoggers->get_item(i, &pDLNode);
			
				UINT error;
				DataLogger* pDL = DataLogger::CreateFromXML(pDLNode, error);

				bool bOk = false;
				if ( pDL != NULL )
				{
					DataLogger::AddDataLogger(pDL);
					pDL->Start();
					strMsg.Format("Запущен архив (%s)", pDL->Name().c_str());
					bOk = true;
				}
				else
				{	
					delete pDL;
					std::string str = AppErrors.GetLastError(error);
					strMsg.Format("Error 0x%04X - %s", error, str.c_str());
					nErr++; nSectionErr++;
					bOk = false;
				}

				LH.StepSection(lSecID, bOk);
				LH.AddContentMessage((LPCTSTR)strMsg, bOk ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
			}

			SECTION_LOADING_RES()
			LH.CloseCurrentSection();
		}

		CComPtr<IXMLDOMNodeList> pFSList;
		hr = pWorkSpaceRootNode->selectNodes(CComBSTR("SCHEMA"), &pFSList);

		if ( SUCCEEDED(hr) && pFSList != NULL )
		{
		
			SECTION_LOADING_MSG("Output schemes...", FALSE);			
			nSectionErr = 0;

			long lSubDocCount;	

			pFSList->get_length(&lSubDocCount);
			pFSList->reset();

			lSecID = LH.AddSection("Output schemes", "SCHEMS", lSubDocCount);
					
			for(int i = 0; i < lSubDocCount; i++)
			{
				CComPtr<IXMLDOMNode>	 pSubDoc;
				pFSList->get_item(i, &pSubDoc);

				std::string sAtr;
			
				std::string name;			
				if ( appXML.GetNodeAttributeByName(pSubDoc, "NAME", sAtr ))
					name = sAtr.c_str();

				std::string path;
				if ( appXML.GetNodeAttributeByName(pSubDoc, "CONFIGFILE", sAtr ))
					path = NTCCProject::Instance().GetPath(NTCCProject::PATH_CONFIG, sAtr);
				else
					continue;

				//СОЗДАЕМ ВСЕ ОПИСАННЫЕ СХЕМЫ
			
			
				//ПОЛУЧАЕМ ШАБЛОН ДОКУМЕНТА ОТОБРАЖЕНИЯ ФУНКЦИОНАЛЬНОЙ СХЕМЫ
				POSITION pos  = ::AfxGetApp()->GetFirstDocTemplatePosition();
				CMultiDocTemplate* pDocOutShemaTemplate = NULL;

				CString strFSName;
				strFSName.LoadString(IDR_NANOTUBESCC_TYPE);
				while (pos)
				{
					CMultiDocTemplate* pTempl = (CMultiDocTemplate*)::AfxGetApp()->GetNextDocTemplate(pos);
					CString str;
					pTempl->GetDocString(str, CDocTemplate::docName );
					if ( strFSName.Find(str) != -1 )
					{
						pDocOutShemaTemplate = pTempl;
						break;
					}
				}
				
				CFSDoc* pFSDoc = (CFSDoc*)pDocOutShemaTemplate->OpenDocumentFile( path.c_str(), FALSE, TRUE);	

				if ( pFSDoc != NULL )
				{
					pFSDoc->UpdateFSDocTitle();
					CNanoTubesControlCenterApp::AddDocument(pFSDoc);
				}
				else {
					nSectionErr++;
					continue;
				}

				vector<CONTROLBAR_INFO> cbi;
				pFSDoc->EnumDescribedControlBars(cbi);
			
				pos = pFSDoc->GetFirstViewPosition();
				CView* pView = pFSDoc->GetNextView(pos);
				ASSERT (pView != NULL);

				CModuleFrame* pMF = (CModuleFrame*)pView->GetParentFrame();
				frames.insert(std::make_pair(pFSDoc, pMF));

				if ( !cbi.empty() )
				{

					for (vector<CONTROLBAR_INFO>::iterator it = cbi.begin(); it != cbi.end(); ++it)
					{
						bool bAdd = true;
						for(size_t i = 0; i < (*it).cbitems.size(); i++)
						{
							if ( DataSrc::GetDataSource( (*it).cbitems.at(i).dsid) == NULL )
							{
								bAdd = false;
								strMsg.Format("Ошибка создания панели управления %s. Не найден источник данных с идентификатором %s", (*it).title.c_str(), (*it).cbitems.at(i).dsid.c_str() );
								nSectionErr++;
								break;
							}
						}
					
						if ( bAdd )
						{
							pMF->AddControlBar(*it);
							strMsg.Format("Cоздана панель управления %s  (%d элементов управления)", (*it).title.c_str(), (*it).cbitems.size()  );
						}

						LH.StepSection(-1, bAdd);
						LH.AddContentMessage((LPCTSTR)strMsg, bAdd ? COutMessage::MSG_OK:COutMessage::MSG_ERROR);
					}
				}
			
				//pMF->ActivateFrame(SW_SHOWMAXIMIZED);

				LH.StepSection(lSecID, true);
				strMsg.Format("Мнемосхема %s (%s) загружена", name.c_str(), path.c_str());
				LH.AddContentMessage((LPCTSTR)strMsg, COutMessage::MSG_OK);
			}

		}

		SECTION_LOADING_RES()
		LH.CloseCurrentSection();
	}
	

	CComPtr<IXMLDOMNode> pUserGroupsNode;
	hr = pWorkSpaceRootNode->selectSingleNode(CComBSTR("GROUPS"), &pUserGroupsNode);

	if (SUCCEEDED(hr) && pUserGroupsNode != NULL)
	{
		BuildUserDataSourceGroup(pUserGroupsNode, NULL);
	}

	BuildGroups();


	for (std::map<CFSDoc*, CModuleFrame*>::iterator it = frames.begin(); it != frames.end(); ++it)
	{
		(*it).second->InitialUpdateFrame((*it).first, FALSE);
		(*it).second->ActivateFrame(SW_SHOWMAXIMIZED);
	}
	
	strMsg.Format("\t\t%d - errors; %d - warnings", nErr, nWarn);
	SECTION_LOADING_MSG((LPCTSTR)strMsg, TRUE);

	
	for (std::vector<std::string>::const_iterator it = m_vOnStartScripts.begin(); it < m_vOnStartScripts.end(); ++it)
		AppOBAScriptsHelper::Instance().RunScript((*it), false);	

	SYSTEMTIME st2;
	GetSystemTime(&st2);
	double sec = Milliseconds(st1, st2) / 1000.0;
	TRACE("\nDocument loading time : %f\n\n", sec);

	if ( nErr > 0)
		return FALSE;

	return TRUE;
}



BOOL CProjectDoc::BuildUserDataSourceGroup(IXMLDOMNode* pGpoupNode, LPDS_VIEW_GROUP parent)
{
	ASSERT( pGpoupNode != NULL);

	CComPtr<IXMLDOMNodeList> pSubGroups;
	HRESULT hr = pGpoupNode->selectNodes(CComBSTR("GROUP"), &pSubGroups);

	long lGroupCount;

	pSubGroups->get_length(&lGroupCount);
	pSubGroups->reset();
	
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	for(int i = 0; i < lGroupCount; i++)
	{
		CComPtr<IXMLDOMNode> pGpoupNode;
		pSubGroups->get_item(i, &pGpoupNode);

		CComPtr<IXMLDOMNamedNodeMap> pAtrs;
		pGpoupNode->get_attributes(&pAtrs);

		std::string sAtr;

		if (! appXML.GetAttributeByName(pAtrs, "TITLE", sAtr) )
			continue;

		CString title = sAtr.c_str();

		COLORREF clr = 0xFFFFFF;
		if (appXML.GetAttributeByName(pAtrs, "BKCOLOR", sAtr) )
			clr = strtol(sAtr.c_str(), 0, 0);

		CString icon;
		if (appXML.GetAttributeByName(pAtrs, "ICON", sAtr) )
			icon = sAtr.c_str();
			
		DS_VIEW_GROUP* dsGroup = new DS_VIEW_GROUP( title, clr, -1);		
		
		m_dsGroupUser.push_back( dsGroup );

		dsGroup->pParentGroup = parent;

		BuildUserDataSourceGroup(pGpoupNode, dsGroup);

		appXML.GetSubNodesAtributes(pGpoupNode, "ITEM", "DSID", dsGroup->dsID);
	}

	return (lGroupCount == m_dsGroupUser.size());
}


BOOL CProjectDoc::BuildGroups()
{
	
	std::vector<const DataSrc*> ds;

	/////////////////////////////////////////////////////////
	//Группировка по типам источников данных

	DS_VIEW_GROUP* groupMain = new DS_VIEW_GROUP( "Inner variables", 0xFFFFFF, -1);
	m_dsGroupByType.push_back(groupMain);
	
	DATA_SOURCES_ENUM_PARAMS ep;
	ep.mask = DSEM_TYPE;	

	DS_VIEW_GROUP* groupAnalog = new DS_VIEW_GROUP("Analog", 0xFFFFFF, -1, groupMain);
	m_dsGroupByType.push_back(groupAnalog);

	ep.dstype = DS_ANALOG;
	DataSrc::EnumDataSources(ds, &ep);

	for (std::vector<const DataSrc*>::iterator it = ds.begin(); it != ds.end(); it++)
		groupAnalog->dsID.push_back((*it)->Name());


	DS_VIEW_GROUP* groupDiscrete = new DS_VIEW_GROUP("Discrete", 0xFFFFFF, -1, groupMain);
	m_dsGroupByType.push_back(groupDiscrete);

	ep.dstype = DS_DISCRETE;
	DataSrc::EnumDataSources(ds, &ep);
	for (std::vector<const DataSrc*>::iterator it = ds.begin(); it != ds.end(); it++)
		groupDiscrete->dsID.push_back((*it)->Name());

	
	/////////////////////////////////////////////////////////
	//Группировка по устройствам промышленной сети

	ep.mask = DSEM_TYPE|DSEM_DEV;	

	DS_VIEW_GROUP* groupMainDev = new DS_VIEW_GROUP( "Devices", 0xFFFFFF, -1);
	m_dsGroupByDevices.push_back(groupMainDev);

	std::vector< const IdentifiedPhysDevice* > devs;

	IdentifiedPhysDevice::EnumDevices(devs);

	for (std::vector< const IdentifiedPhysDevice* >::const_iterator itDev = devs.begin(); itDev != devs.end(); itDev++)
	{
		ep.device = (*itDev)->Name();

		DS_VIEW_GROUP* pDev = new DS_VIEW_GROUP( (*itDev)->Name().c_str(), 0xFFFFFF, -1, groupMainDev);
		m_dsGroupByDevices.push_back(pDev);
	
		IAnalogInput* pAI = dynamic_cast<IAnalogInput*>( IdentifiedPhysDevice::GetDevice( (*itDev)->Name() ) );
		if ( pAI != NULL )
		{			
			DS_VIEW_GROUP* pAIGroup = new DS_VIEW_GROUP( "Analog inputs", 0xFFFFFF, -1, pDev);
			ep.dstype = DS_AI;
			DataSrc::EnumDataSources(ds, &ep);
			for (std::vector<const DataSrc*>::iterator it = ds.begin(); it != ds.end(); it++)
				pAIGroup->dsID.push_back((*it)->Name());
			
			if ( !ds.empty() )
				m_dsGroupByDevices.push_back(pAIGroup);
			else
				delete pAIGroup;
		}

		IAnalogOutput* pAO = dynamic_cast<IAnalogOutput*>( IdentifiedPhysDevice::GetDevice( (*itDev)->Name() ) );
		if ( pAO != NULL )
		{
			DS_VIEW_GROUP* pAOGroup = new DS_VIEW_GROUP( "Analog outputs", 0xFFFFFF, -1, pDev);
			ep.dstype = DS_AO;
			DataSrc::EnumDataSources(ds, &ep);
			for (std::vector<const DataSrc*>::iterator it = ds.begin(); it != ds.end(); it++)
				pAOGroup->dsID.push_back((*it)->Name());

			if ( !ds.empty() )
				m_dsGroupByDevices.push_back(pAOGroup);
			else
				delete pAOGroup;

		}

		IDigitalInput* pDI = dynamic_cast<IDigitalInput*>( IdentifiedPhysDevice::GetDevice( (*itDev)->Name() ) );
		if ( pDI != NULL )
		{
			DS_VIEW_GROUP* pDIGroup = new DS_VIEW_GROUP( "Discrete inputs", 0xFFFFFF, -1, pDev);
			ep.dstype = DS_DI;
			DataSrc::EnumDataSources(ds, &ep);
			for (std::vector<const DataSrc*>::iterator it = ds.begin(); it != ds.end(); it++)
				pDIGroup->dsID.push_back((*it)->Name());
			
			if ( !ds.empty() )
				m_dsGroupByDevices.push_back(pDIGroup);
			else
				delete pDIGroup;

		}

		IDigitalOutput* pDO = dynamic_cast<IDigitalOutput*>( IdentifiedPhysDevice::GetDevice( (*itDev)->Name() ) );
		if ( pDO != NULL )
		{
			DS_VIEW_GROUP* pDOGroup = new DS_VIEW_GROUP( "Discrete otputs", 0xFFFFFF, -1, pDev);
			ep.dstype = DS_DO;
			DataSrc::EnumDataSources(ds, &ep);

			for (std::vector<const DataSrc*>::iterator it = ds.begin(); it != ds.end(); it++)
				pDOGroup->dsID.push_back((*it)->Name());

			if ( !ds.empty() )
				m_dsGroupByDevices.push_back(pDOGroup);
			else
				delete pDOGroup;
		}
	}

	/////////////////////////////////////////////////////////
	//Группировка по управляющим скриптам
	std::vector<SCRIPTINFO> scripts;
	AppOBAScriptsHelper&  obah = AppOBAScriptsHelper::Instance();
	obah.EnumScripts(scripts);

	DS_VIEW_GROUP* pGroupScripts = new DS_VIEW_GROUP( "Control scripts", 0xFFFFFF, -1);
	m_dsGroupByScript.push_back(pGroupScripts);

	for (std::vector<SCRIPTINFO>::const_iterator it = scripts.begin(); it != scripts.end(); ++it)
	{
		SCRIPTINFO scr = (*it);

		DS_VIEW_GROUP* pGroupScript = new DS_VIEW_GROUP(scr.name.c_str(), 0xFFFFFF, -1, pGroupScripts );
		m_dsGroupByScript.push_back(pGroupScript);

		DS_VIEW_GROUP* pADSGroup = new DS_VIEW_GROUP( "Analog", 0xFFFFFF, -1, pGroupScript);
		DS_VIEW_GROUP* pDDSGroup = new DS_VIEW_GROUP( "Discrete", 0xFFFFFF, -1, pGroupScript);

		IDMap ids;
		obah.GetScriptIDs(scr.name.c_str(), ids);

		for (IDMap::const_iterator it1 = ids.begin(); it1 != ids.end(); ++it1)
		{
			const DataSrc* pDS = DataSrc::GetDataSource(it1->first);
			
			if ( dynamic_cast<const AnalogDataSrc*>(pDS) != NULL )
				pADSGroup->dsID.push_back(it1->first);
			else if (dynamic_cast<const DiscreteDataSrc*>(pDS) != NULL )
				pDDSGroup->dsID.push_back(it1->first);
		}

		if (! pADSGroup->dsID.empty() )
			m_dsGroupByScript.push_back(pADSGroup);
		else
			delete pADSGroup;
		
		if (! pDDSGroup->dsID.empty() )
			m_dsGroupByScript.push_back(pDDSGroup);
		else
			delete pDDSGroup;
	}

	
	
	return TRUE; 
}

// CProjectDoc commands




BOOL CProjectDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	//ЗАГРУЖАЕМ КОНФИГУРАЦИЮ
	//TODO: Генерирует std::exception. Нужно разобраться! BR
	m_bRes = LoadConfig(lpszPathName);

	return TRUE;
}

void CProjectDoc::OnCloseDocument()
{
	m_bAutoDelete = FALSE;
	CDocument::OnCloseDocument();
}

