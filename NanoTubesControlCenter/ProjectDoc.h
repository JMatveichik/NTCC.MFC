#pragma once

// CProjectDoc document
#include "Automation\DevicesEmulators.h"
#include "Automation\DeviceFactory.h"
#include "Automation\CommandManager.h"
#include "Automation\Converters.h"

#include "automation\DataSrc.h"
#include "Automation\DataCollector.h"

typedef struct tagGroupDS
{
	tagGroupDS ()  : strTitle (""), bkColor(0xFFFFFF), iconInd(-1), pParentGroup(NULL) {};

	tagGroupDS ( LPCSTR title, COLORREF color = 0xFFFFFF, int icon = -1, tagGroupDS* pParent = NULL)  : 
	 strTitle (title), bkColor(color), iconInd(icon), pParentGroup(pParent) {};

	CString strTitle;
	COLORREF bkColor;
	int iconInd;
	tagGroupDS* pParentGroup;
	std::vector<std::string> dsID;

}DS_VIEW_GROUP, *LPDS_VIEW_GROUP;

typedef std::vector<LPDS_VIEW_GROUP> DATA_SOURCES_VIEW_GROUPS;



//////////////////////////////////////////////////////////////////////////



class CProjectDoc : public CDocument
{
	DECLARE_DYNCREATE(CProjectDoc)

public:

	enum { 
		UPDATE_ADD_MESSAGE = 0, 
		UPDATE_LOAD_COMPLETE,
		UPDATE_CHANGE_DS_VIEW_TYPE
	};

	enum {
		VIEW_DS_USER_LAYOUT,
		VIEW_DS_BY_TYPE_LAYOUT,
		VIEW_DS_BY_DEVICE_LAYOUT,
		VIEW_DS_BY_SCRIPT_LAYOUT
	};

	CProjectDoc();
	virtual ~CProjectDoc();

#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif

#ifdef _DEBUG
	virtual void AssertValid() const;

#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	BOOL LoadConfig(const char* pszFileName);
	BOOL LoadingResult() const { return m_bRes; }

	std::size_t GetSubDocsCount() const 
	{ 
		return m_vSubDocsPaths.size(); 
	}
	
	std::string GetSubDocPath(int ind) const 
	{ 
		return m_vSubDocsPaths[ind]; 
	}

	const DATA_SOURCES_VIEW_GROUPS& GetLayout(UINT nType) const;

protected:

	virtual BOOL OnNewDocument();
	virtual void DeleteContents();

	BOOL BuildGroups();
	BOOL BuildUserDataSourceGroup(IXMLDOMNode* pGpoupNode, LPDS_VIEW_GROUP parent);	

	DECLARE_MESSAGE_MAP()

protected:

	BOOL m_bRes;
	
	DATA_SOURCES_VIEW_GROUPS		m_dsGroupUser;
	DATA_SOURCES_VIEW_GROUPS		m_dsGroupByType;
	DATA_SOURCES_VIEW_GROUPS		m_dsGroupByDevices;
	DATA_SOURCES_VIEW_GROUPS		m_dsGroupByScript;

	std::vector<std::string>		m_vSubDocsPaths;
	std::vector<std::string>		m_vOnStartScripts;
	std::vector<std::string>		m_vOnCloseScripts;
	
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
public:
	virtual void OnCloseDocument();
};
