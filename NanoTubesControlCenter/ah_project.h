#pragma once

#include "singletone.h"

#ifndef _APP_PROJECT_HELPER_H_INCLUDED_
#define _APP_PROJECT_HELPER_H_INCLUDED_

//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]
//////////////////////////////////////////////////////////////////////////

class NTCCProject : public Singletone<NTCCProject>
{
	friend class Singletone<NTCCProject>;

public: 

	virtual ~NTCCProject();

protected:

	NTCCProject();
	NTCCProject(const NTCCProject& );
	const NTCCProject& operator= (const NTCCProject&);

public:

	typedef enum PathType{
		
		PATH_BASE,		
		PATH_IMAGES,
		PATH_SCRIPTS,
		PATH_LOG,
		PATH_DATA,
		PATH_HTML,
		PATH_CONFIG,
		PATH_BIN,
		PATH_FONTS

	}PathType;

	

public:

	bool Init(const char* pszProjectPath, std::ofstream& log);

	std::string GetPath(PathType type, bool asUNC = FALSE) const;
	std::string GetPath(PathType type, std::string fileName, bool asUNC = FALSE) const;
	std::string CreateLogFilePath(std::string sub, std::string id = "") const;

	bool IsDebugMode() const;
	std::string Name() const;
	std::string Version() const;

private:

	bool BuildPath(IXMLDOMNode* pMainNode,  PathType pt, std::string share);

	std::string name;
	std::string ver;
	
	std::string basePath;
	std::string shareName;

	std::map <PathType, std::string> pathKey;
	mutable std::map <PathType, std::string> localPaths;
	mutable std::map <PathType, std::string> uncSharePaths;

	bool bDebug;
};

#endif //_APP_PROJECT_HELPER_H_INCLUDED_
