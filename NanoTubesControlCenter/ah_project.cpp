#include "stdafx.h"
#include <Dbghelp.h>
#include "ah_project.h"
#include "ah_xml.h"


NTCCProject::NTCCProject()
{
	pathKey[PATH_BASE]   = "BASE";
	pathKey[PATH_IMAGES] = "IMAGES";
	pathKey[PATH_SCRIPTS]= "SCRIPTS";
	pathKey[PATH_LOG]    = "LOG";
	pathKey[PATH_DATA]   = "DATA";
	pathKey[PATH_HTML]   = "HTML";
	pathKey[PATH_CONFIG] = "CONFIG";
	pathKey[PATH_FONTS] = "FONTS";
	pathKey[PATH_BIN] = "BIN";
}

NTCCProject::~NTCCProject()
{

}

bool NTCCProject::BuildPath(IXMLDOMNode* pMainNode,  PathType pt, std::string share)
{
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	std::string path;
	if ( !appXML.GetProjectSubPath(pMainNode, pathKey[pt].c_str(), path) )
		return false;

	if ( share.size() != 0) {
		char szHostName[MAX_PATH];
		gethostname(szHostName, MAX_PATH);

		CString uncPath;
		uncPath.Format("\\\\%s\\%s\\%s", szHostName, share.c_str(), path.c_str() );
		if ( GetFileAttributes( uncPath ) != INVALID_FILE_ATTRIBUTES )
		{
			uncSharePaths[pt] = uncPath;
			return true;
		}
		else {
			CString msg; 
			msg.Format("Invalid file path : %s", (LPCTSTR)uncPath);
			MessageBox(NULL, msg, "Path error", MB_ICONERROR);
			return false;
		}

	}
	else 
	{
		std::string strProjectSubPath = this->basePath + path;

		if ( GetFileAttributes( strProjectSubPath.c_str() ) != INVALID_FILE_ATTRIBUTES )
		{
			localPaths[pt] = strProjectSubPath;
			return true;
		}
		else {
			CString msg; 
			msg.Format("Invalid file path : %s", strProjectSubPath.c_str());
			MessageBox(NULL, msg, "Path error", MB_ICONERROR);

			return false;
		}
	}

	return false;
}

bool NTCCProject::Init(const char* pszProjectPath, std::ofstream& log)
{
	DWORD res = GetFileAttributes( pszProjectPath );
	if (res == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if ( res & FILE_ATTRIBUTE_DIRECTORY )
	{
		CString cfgPath = pszProjectPath;
		
		if (cfgPath[cfgPath.GetLength() - 1] != '\\')
			cfgPath += '\\'; 

		cfgPath += "Config\\";
		CString cfgFileName = cfgPath;
		cfgFileName += "config.xml";

		log << " Load configuration from file => " << cfgFileName << std::endl;

		WIN32_FIND_DATA fd;
		memset(&fd, 0, sizeof(WIN32_FIND_DATA));

		HANDLE hCfgFile =  ::FindFirstFile(cfgFileName, &fd);
		if (hCfgFile == INVALID_HANDLE_VALUE)
			return false;

		
		CComPtr<IXMLDOMDocument> pXMLDoc;
		HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument));
		
		if ( FAILED(hr) )
		{
			log << " XML Document object creation failed! " << std::endl;
			return false;
		}
	
		VARIANT_BOOL bSuccess = false;

		hr = pXMLDoc->load(CComVariant((LPCTSTR)cfgFileName), &bSuccess);

		if ( FAILED(hr) || !bSuccess )
		{
			log << " XML Document loading failed!" << std::endl;
			std::string error = Globals::CheckError(true);
			return false;
		}
		

		CComPtr<IXMLDOMNode> pMainNode;
		hr = pXMLDoc->selectSingleNode(CComBSTR("NTCCPROJECT"), &pMainNode);

		if ( FAILED(hr) || pMainNode == NULL)
		{
			log << " NTCCPROJECT node not found !" << std::endl;
			return false;
		}

		
		const AppXMLHelper& appXML = AppXMLHelper::Instance();

		this->basePath = pszProjectPath;
		
		if (this->basePath[this->basePath.size() - 1] != '\\')
			this->basePath += '\\';

		localPaths[PATH_BASE] = this->basePath;

		if ( !appXML.GetNodeAttributeByName(pMainNode, "PROJECTNAME", this->name ) )
			name = "NTCC project";

		if ( !appXML.GetNodeAttributeByName(pMainNode, "VERSION", this->ver ) )
			ver = "v.1.0";

		std::string mode;
		if ( appXML.GetNodeAttributeByName(pMainNode, "MODE", mode ) )
		{
			this->bDebug = (mode == "DEBUG");
		}

		CComPtr<IXMLDOMNode> pPaths;
		hr = pMainNode->selectSingleNode(CComBSTR("PATHS"), &pPaths);	

		std::string share;
		appXML.GetNodeAttributeByName(pPaths, "SHARE", share );
		
		if ( share.size() != 0) 
		{
			BuildPath(pMainNode,  PATH_CONFIG, share.c_str());
			BuildPath(pMainNode,  PATH_IMAGES, share.c_str());
			BuildPath(pMainNode,  PATH_SCRIPTS, share.c_str());
			BuildPath(pMainNode,  PATH_LOG, share.c_str());
			BuildPath(pMainNode,  PATH_DATA, share.c_str());
			BuildPath(pMainNode,  PATH_HTML, share.c_str());
			BuildPath(pMainNode,  PATH_FONTS, share.c_str());
			BuildPath(pMainNode,  PATH_BIN, share.c_str());
		}
				

		log << " Build project paths... " << std::endl;

		if (! BuildPath(pMainNode,  PATH_CONFIG, "") )
		{
			log << " Config path build failed..." << std::endl;
			return false;
		}

		if (! BuildPath(pMainNode,  PATH_IMAGES, "") )
		{
			log << " Images path build failed..." << std::endl;
			return false;
		}

		if (! BuildPath(pMainNode,  PATH_SCRIPTS, ""))
		{
			log << " OBA scripts path build failed..." << std::endl;
			return false;
		}
		
		if (! BuildPath(pMainNode,  PATH_LOG, ""))
		{
			log << " Log path build failed..." << std::endl;
			return false;
		}
		
		if (! BuildPath(pMainNode,  PATH_DATA, ""))
		{
			log << " Data path build failed..." << std::endl;
			return false;
		}

		if (! BuildPath(pMainNode,  PATH_HTML, ""))
		{
			log << " HTML path build failed..." << std::endl;
			return false;
		}

		BuildPath(pMainNode,  PATH_FONTS, "");

		BuildPath(pMainNode,  PATH_BIN, "");

		return true;
	}

	return false;    // this is not a directory!
}

bool NTCCProject::IsDebugMode() const
{
	return bDebug;
}

std::string NTCCProject::Name() const
{
	return name;
}

std::string NTCCProject::Version() const
{
	return ver;
}

std::string NTCCProject::GetPath(PathType type, bool asUNC /*= false*/) const
{
	std::string path;
	std::map <PathType, std::string>* pathsMap = ( asUNC ) ? &uncSharePaths : &localPaths;
	
	if ( pathsMap->find(type) != pathsMap->end() )
		path = pathsMap->at(type);	
	
	if (path.length() != 0 && path[path.length() - 1] != '\\')
		path +=  '\\';

	return path;
}

std::string NTCCProject::GetPath(PathType type, std::string fileName, bool asUNC/* = false*/) const
{
	std::string path = GetPath(type, asUNC);

	if ( path[path.size() - 1] != '\\')
		path +=  '\\';

	path += fileName;
	return path;
}

std::string NTCCProject::CreateLogFilePath(std::string sub, std::string id /*=""*/) const
{
	std::string path = GetPath(PATH_LOG, false);

	if ( path[path.size() - 1] != '\\')
		path +=  '\\';

	path += sub;

	//создаем поддиректории с текущим годом и месяцем
	SYSTEMTIME curTime;
	::GetLocalTime( &curTime );
	
	char szTmpPath[MAX_PATH];
	sprintf_s(szTmpPath, MAX_PATH, "\\%04d\\%02d\\", curTime.wYear, curTime.wMonth );
	
	path += szTmpPath;

	//если невозможно создать директорию
	if ( !MakeSureDirectoryPathExists(path.c_str()) )	
		return "";	

	char szBuffer[MAX_PATH];
	if (id.empty())
		sprintf_s(szBuffer, MAX_PATH, "%02d%02d%04d.log", curTime.wDay, curTime.wMonth, curTime.wYear);
	else
		sprintf_s(szBuffer, MAX_PATH, "%s_%02d%02d%04d.log", id.c_str(), curTime.wDay, curTime.wMonth, curTime.wYear);

	path += szBuffer;
	return path;
}