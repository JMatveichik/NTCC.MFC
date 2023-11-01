#include "StdAfx.h"
#include <assert.h>
#include "xyplotwrapper.h"

using namespace xyplot;

#ifdef _M_X64
	#define XYFUNNAME_INITIALIZE "Initialize"
	#define XYFUNNAME_FINALIZE	 "Finalize"
	#define XYFUNNAME_ADDPROFILE "AddProfile"
	#define XYFUNNAME_DELPROFILE "DeleteProfile"
	#define XYFUNNAME_SETDATA	 "SetData"
	#define XYFUNNAME_GETDATA	 "GetData"
	#define XYFUNNAME_APPENDDATA "AppendData"
	#define XYFUNNAME_SETPROFCLR "SetProfileColor"
	#define XYFUNNAME_GETPROFCLR "GetProfileColor"
	#define XYFUNNAME_SETPROFSTL "ModifyProfile"
	#define XYFUNNAME_ENABLEMARK "EnablePointsMarker"
	#define XYFUNNAME_SETSTRING	 "SetString"
	#define XYFUNNAME_GETSTRING	 "GetString"
	#define XYFUNNAME_SETCOLOR	 "SetColor"
	#define XYFUNNAME_GETCOLOR	 "GetColor"
	#define XYFUNNAME_SETLONG	 "SetLong"
	#define XYFUNNAME_GETLONG	 "GetLong"
	#define XYFUNNAME_SETDOUBLE	 "SetDouble"
	#define XYFUNNAME_GETDOUBLE	 "GetDouble"
	#define XYFUNNAME_SETSELMODE "SetSelectionMode"
	#define XYFUNNAME_GETSELMODE "GetSelectionMode"
	#define XYFUNNAME_ISENABLED  "IsEnabled"
// 	#define XYFUNNAME_ENABLEITEM "EnablePlotItem"
// 	#define XYFUNNAME_ENAXISITEM "EnableAxisItem"
	#define XYFUNNAME_SETAXRANGE "SetAxisRange"
	#define XYFUNNAME_GETAXRANGE "GetAxisRange"
	#define XYFUNNAME_SETMARKPOS "SetMarkerPos"
	#define XYFUNNAME_GETMARKPOS "GetMarkerPos"
	#define XYFUNNAME_GETSELPROF "GetSelection"
	#define XYFUNNAME_SETFLASH   "SetFlashSpeed"
	#define XYFUNNAME_CLIPCOPY   "CopyToClipboard"
	#define XYFUNNAME_SETREDRAW	 "SetRedraw"
	#define XYFUNNAME_CREATEGRP	 "CreateGroup"
	#define XYFUNNAME_DELETEGRP	 "DeleteGroup"
	#define XYFUNNAME_BINDPROF	 "BindProfile"
	#define XYFUNNAME_GETPROFLIST "GetProfileList"
	#define XYFUNNAME_SAVEPROP	 "SaveProperties"
	#define XYFUNNAME_LOADPROP	 "LoadProperties"
	#define XYFUNNAME_REGRESTORE "RegRestoreDefaults"
	#define XYFUNNAME_REGLOAD	 "RegLoadSettings"
	#define XYFUNNAME_REGSAVE	 "RegSaveSettings"
	#define XYFUNNAME_RUNDIALOG	 "RunDialog"
	#define XYFUNNAME_ADDLEVEL	 "AddLevel"
	#define XYFUNNAME_DELLEVEL	 "DeleteLevel"
	#define XYFUNNAME_SETLEVEL	 "SetLevel"
	#define XYFUNNAME_ADDREGION	 "AddRegion"
	#define XYFUNNAME_DELREGION	 "DeleteRegion"
	#define XYFUNNAME_SETREGION	 "SetRegion"
	#define XYFUNNAME_SETFONT	 "SetFont"
	#define XYFUNNAME_ENITEM	 "EnableItem"
	#define XYFUNNAME_FLASHPROF	 "Select"
	#define XYFUNNAME_DRAWTOBITMAP "DrawToBitmap"
#else
	#define XYFUNNAME_INITIALIZE "_Initialize@8"
	#define XYFUNNAME_FINALIZE	 "_Finalize@4"
	#define XYFUNNAME_ADDPROFILE "_AddProfile@44"
	#define XYFUNNAME_DELPROFILE "_DeleteProfile@8"
	#define XYFUNNAME_SETDATA	 "_SetData@20"
	#define XYFUNNAME_GETDATA	 "_GetData@20"
	#define XYFUNNAME_APPENDDATA "_AppendData@20"
	#define XYFUNNAME_SETPROFCLR "_SetProfileColor@12"
	#define XYFUNNAME_GETPROFCLR "_GetProfileColor@12"
	#define XYFUNNAME_SETPROFSTL "_ChangeProfileStyle@24"
	#define XYFUNNAME_ENABLEMARK "_EnablePointsMarker@12"
	#define XYFUNNAME_SETSTRING	 "_SetString@16"
	#define XYFUNNAME_GETSTRING	 "_GetString@20"
	#define XYFUNNAME_SETCOLOR	 "_SetColor@16"
	#define XYFUNNAME_GETCOLOR	 "_GetColor@16"
	#define XYFUNNAME_SETLONG	 "_SetLong@16"
	#define XYFUNNAME_GETLONG	 "_GetLong@16"
	#define XYFUNNAME_SETDOUBLE	 "_SetDouble@20"
	#define XYFUNNAME_GETDOUBLE	 "_GetDouble@16"
	#define XYFUNNAME_SETSELMODE "_SetSelectionMode@8"
	#define XYFUNNAME_GETSELMODE "_GetSelectionMode@8"
	#define XYFUNNAME_ISENABLED  "_IsEnabled@16"
// 	#define XYFUNNAME_ENABLEITEM "_EnablePlotItem@12"
// 	#define XYFUNNAME_ENAXISITEM "_EnableAxisItem@16"
	#define XYFUNNAME_SETAXRANGE "_SetAxisRange@24"
	#define XYFUNNAME_GETAXRANGE "_GetAxisRange@16"
	#define XYFUNNAME_GETMARKPOS "_GetMarkerPos@12"
	#define XYFUNNAME_SETMARKPOS "_SetMarkerPos@20"
	#define XYFUNNAME_GETSELPROF "_GetSelection@8"
	#define XYFUNNAME_SETFLASH   "_SetFlashSpeed@8"
	#define XYFUNNAME_CLIPCOPY   "_CopyToClipboard@16"
	#define XYFUNNAME_SETREDRAW	 "_SetRedraw@8"
	#define XYFUNNAME_CREATEGRP	 "_CreateGroup@8"
	#define XYFUNNAME_DELETEGRP	 "_DeleteGroup@8"
	#define XYFUNNAME_BINDPROF	 "_BindProfile@12"
	#define XYFUNNAME_GETPROFLIST "_GetProfileList@12"
	#define XYFUNNAME_SAVEPROP	 "_SaveProperties@8"
	#define XYFUNNAME_LOADPROP	 "_LoadProperties@8"
	#define XYFUNNAME_REGRESTORE "_RegRestoreDefaults@4"
	#define XYFUNNAME_REGLOAD	 "_RegLoadSettings@12"
	#define XYFUNNAME_REGSAVE	 "_RegSaveSettings@12"
	#define XYFUNNAME_RUNDIALOG	 "_RunDialog@4"
	#define XYFUNNAME_ADDLEVEL	 "_AddLevel@32"
	#define XYFUNNAME_DELLEVEL	 "_DeleteLevel@8"
	#define XYFUNNAME_SETLEVEL	 "_SetLevel@16"
	#define XYFUNNAME_ADDREGION	 "_AddRegion@52"
	#define XYFUNNAME_DELREGION	 "_DeleteRegion@8"
	#define XYFUNNAME_SETREGION	 "_SetRegion@40"
	#define XYFUNNAME_SETFONT	 "_SetFont@16"
	#define XYFUNNAME_ENITEM	 "_EnableItem@16"
	#define XYFUNNAME_FLASHPROF	 "_Select@12"
	#define XYFUNNAME_DRAWTOBITMAP "_DrawToBitmap@16"
#endif

XYPlotManager& XYPlotManager::Instance()
{
	static XYPlotManager obj; 
	return obj; 
}

std::string XYPlotManager::TranslateError(int code) const
{
	std::string message;
	switch (code) {
		case PE_NOERROR:
			message = "No errors";
			break;

		case PE_UNKNOWN:
			message = "Unknown error occured";
			break;

		case PE_UNEXPECTED:
			message = "Unexpected exception occured";
			break;

		case PE_INVALID_PLOT:
			message = "Invalid plot handle provided";
			break;

		case PE_INVALID_PROFILE:
			message = "Invalid profile handle provided";
			break;

		case PE_ALREADY_HOOKED:
			message = "Window is already hooked by a plot";
			break;

		case PE_INSUFFICIENT_MEMORY:
			message = "Not enough memory";
			break;

		case PE_INVALID_RANGE:
			message = "Invalid range provided";
			break;

		case PE_INVALID_ARG:
			message = "Invalid argument provided";
			break;

		case PE_INSUFFICIENT_STORAGE:
			message = "Storage size is not enough to receive requested data";
			break;

		case PE_OUT_OF_RANGE:
			message = "Provided value is out of range";
			break;

		case PE_INVALID_GROUP:
			message = "Invalid group handle provided";
			break;

		case PE_RTFTEXT:
			message = "Inapplicable due to RTF text formatting";
			break;

		default:
			message = "Unknown error occured";
			break;
	}

	return message;
}

BOOL XYPlotManager::Initialize(const char* pszDllPath, HWND hwndParent)
{
	if (m_hDllInstance) 
	{
		FreeLibrary(m_hDllInstance);
		m_hDllInstance = NULL;
	}

	m_hDllInstance = LoadLibrary(pszDllPath);

	if (!m_hDllInstance)
	{
		if (!hwndParent)
			return FALSE;

		LPVOID lpMsgBuf;
		if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL))
		{
			return FALSE;
		}
		if (GetLastError() == ERROR_SUCCESS)
			MessageBox(hwndParent, "XYPlot dynamic library was not found", "XYPlot initialization error", MB_OK|MB_ICONHAND);
		else
			MessageBox(hwndParent, (LPCTSTR)lpMsgBuf, "XYPlot initialization error", MB_OK|MB_ICONHAND);
		LocalFree(lpMsgBuf);
		return FALSE;
	}

#define CHECK_FUNCTION_POINTER(a) if (!(a)) return FALSE;

	// Initialize function pointers
	CHECK_FUNCTION_POINTER(m_lpfnInitialize = (LPFNINITIALIZE) GetProcAddress(m_hDllInstance, XYFUNNAME_INITIALIZE));
	CHECK_FUNCTION_POINTER(m_lpfnFinalize = (LPFNFINALIZE) GetProcAddress(m_hDllInstance, XYFUNNAME_FINALIZE));
	CHECK_FUNCTION_POINTER(m_lpfnAddProfile = (LPFNADDPROFILE) GetProcAddress(m_hDllInstance, XYFUNNAME_ADDPROFILE));
	CHECK_FUNCTION_POINTER(m_lpfnDeleteProfile = (LPFNDELETEPROFILE) GetProcAddress(m_hDllInstance, XYFUNNAME_DELPROFILE));
	CHECK_FUNCTION_POINTER(m_lpfnSetData = (LPFNSETDATA) GetProcAddress(m_hDllInstance, XYFUNNAME_SETDATA));
	CHECK_FUNCTION_POINTER(m_lpfnGetData = (LPFNGETDATA) GetProcAddress(m_hDllInstance, XYFUNNAME_GETDATA));
	CHECK_FUNCTION_POINTER(m_lpfnAppendData = (LPFNAPPENDDATA) GetProcAddress(m_hDllInstance, XYFUNNAME_APPENDDATA));
	CHECK_FUNCTION_POINTER(m_lpfnSetString = (LPFNSETSTRING) GetProcAddress(m_hDllInstance, XYFUNNAME_SETSTRING));
	CHECK_FUNCTION_POINTER(m_lpfnGetString = (LPFNGETSTRING) GetProcAddress(m_hDllInstance, XYFUNNAME_GETSTRING));
	CHECK_FUNCTION_POINTER(m_lpfnSetColor = (LPFNSETCOLOR) GetProcAddress(m_hDllInstance, XYFUNNAME_SETCOLOR));
	CHECK_FUNCTION_POINTER(m_lpfnGetColor = (LPFNGETCOLOR) GetProcAddress(m_hDllInstance, XYFUNNAME_GETCOLOR));
	CHECK_FUNCTION_POINTER(m_lpfnSetLong = (LPFNSETLONG) GetProcAddress(m_hDllInstance, XYFUNNAME_SETLONG));
	CHECK_FUNCTION_POINTER(m_lpfnGetLong = (LPFNGETLONG) GetProcAddress(m_hDllInstance, XYFUNNAME_GETLONG));
	CHECK_FUNCTION_POINTER(m_lpfnSetDouble = (LPFNSETDOUBLE) GetProcAddress(m_hDllInstance, XYFUNNAME_SETDOUBLE));
	CHECK_FUNCTION_POINTER(m_lpfnGetDouble = (LPFNGETDOUBLE) GetProcAddress(m_hDllInstance, XYFUNNAME_GETDOUBLE));
	CHECK_FUNCTION_POINTER(m_lpfnGetSelection = (LPFNGETSELECTION) GetProcAddress(m_hDllInstance, XYFUNNAME_GETSELPROF));

	CHECK_FUNCTION_POINTER(m_lpfnCopyToClipboard = (LPFNCOPYTOCLIPBOARD) GetProcAddress(m_hDllInstance, XYFUNNAME_CLIPCOPY));
	CHECK_FUNCTION_POINTER(m_lpfnDrawToBitmap = (LPFNDRAWTOBITMAP)GetProcAddress(m_hDllInstance, XYFUNNAME_DRAWTOBITMAP));

	CHECK_FUNCTION_POINTER(m_lpfnCreateGroup = (LPFNCREATEGROUP) GetProcAddress(m_hDllInstance, XYFUNNAME_CREATEGRP));
	CHECK_FUNCTION_POINTER(m_lpfnDeleteGroup = (LPFNDELETEGROUP) GetProcAddress(m_hDllInstance, XYFUNNAME_DELETEGRP));
	CHECK_FUNCTION_POINTER(m_lpfnBindProfile = (LPFNBINDPROFILE) GetProcAddress(m_hDllInstance, XYFUNNAME_BINDPROF));
	
	CHECK_FUNCTION_POINTER(m_lpfnGetProfileList = (LPFNGETPROFLIST) GetProcAddress(m_hDllInstance, XYFUNNAME_GETPROFLIST));

	CHECK_FUNCTION_POINTER(m_lpfnSelect = (LPFNSELECT) GetProcAddress(m_hDllInstance, XYFUNNAME_FLASHPROF));
	CHECK_FUNCTION_POINTER(m_lpfnSaveProperties = (LPFNSAVEPROPERTIES) GetProcAddress(m_hDllInstance, XYFUNNAME_SAVEPROP));
	CHECK_FUNCTION_POINTER(m_lpfnLoadProperties = (LPFNLOADPROPERTIES) GetProcAddress(m_hDllInstance, XYFUNNAME_LOADPROP));
	CHECK_FUNCTION_POINTER(m_lpfnRegRestoreDefaults = (LPFNREGRESTOREDEFAULTS) GetProcAddress(m_hDllInstance, XYFUNNAME_REGRESTORE));
	CHECK_FUNCTION_POINTER(m_lpfnRegLoadSettings = (LPFNREGLOADSETTINGS) GetProcAddress(m_hDllInstance, XYFUNNAME_REGLOAD));
	CHECK_FUNCTION_POINTER(m_lpfnRegSaveSettings = (LPFNREGSAVESETTINGS) GetProcAddress(m_hDllInstance, XYFUNNAME_REGSAVE));
	CHECK_FUNCTION_POINTER(m_lpfnRunDialog = (LPFNRUNDIALOG) GetProcAddress(m_hDllInstance, XYFUNNAME_RUNDIALOG));
	CHECK_FUNCTION_POINTER(m_lpfnAddLevel		= (LPFNADDLEVEL)GetProcAddress(m_hDllInstance, XYFUNNAME_ADDLEVEL));
	CHECK_FUNCTION_POINTER(m_lpfnDeleteLevel	= (LPFNDELETELEVEL)GetProcAddress(m_hDllInstance, XYFUNNAME_DELLEVEL));
	CHECK_FUNCTION_POINTER(m_lpfnSetLevel		= (LPFNSETLEVEL)GetProcAddress(m_hDllInstance, XYFUNNAME_SETLEVEL));
	CHECK_FUNCTION_POINTER(m_lpfnAddRegion = (LPFNADDREGION)GetProcAddress(m_hDllInstance, XYFUNNAME_ADDREGION));
	CHECK_FUNCTION_POINTER(m_lpfnDeleteRegion = (LPFNDELETEREGION)GetProcAddress(m_hDllInstance, XYFUNNAME_DELREGION));
	CHECK_FUNCTION_POINTER(m_lpfnSetRegion = (LPFNSETREGION)GetProcAddress(m_hDllInstance, XYFUNNAME_SETREGION));
	CHECK_FUNCTION_POINTER(m_lpfnSetFont = (LPFNSETFONT)GetProcAddress(m_hDllInstance, XYFUNNAME_SETFONT));
	CHECK_FUNCTION_POINTER(m_lpfnEnableItem = (LPFNENABLEITEM)GetProcAddress(m_hDllInstance, XYFUNNAME_ENITEM));
	CHECK_FUNCTION_POINTER(m_lpfnIsEnabled = (LPFNISENABLED)GetProcAddress(m_hDllInstance, XYFUNNAME_ISENABLED));
	CHECK_FUNCTION_POINTER(m_lpfnDrawToBitmap = (LPFNDRAWTOBITMAP)GetProcAddress(m_hDllInstance, XYFUNNAME_DRAWTOBITMAP));

	m_nLastError = PE_NOERROR;

	return TRUE;
}

HPLOT XYPlotManager::CreatePlot(HWND hWndParent)
{
	assert(m_lpfnInitialize != NULL);
	HPLOT plot;
	if (m_nLastError = m_lpfnInitialize(hWndParent, &plot) != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);
	return plot;
}

BOOL XYPlotManager::DestroyPlot(HPLOT& hPlot)
{
	assert(m_lpfnFinalize != NULL);
	return m_nLastError = m_lpfnFinalize(hPlot) != PE_NOERROR;
}

HPROFILE XYPlotManager::CreateProfile(HPLOT& hPlot, const char* name, COLORREF color, int width, int linetype, const char* pszTemplate,
	BOOL bVisible, BOOL bDataMarks, const long XAxis, const long YAxis)
{	
	assert(m_lpfnAddProfile != NULL);
	HPROFILE hProfile;
	m_nLastError = m_lpfnAddProfile(hPlot, name, color, width, linetype, pszTemplate, bVisible, bDataMarks, XAxis, YAxis, &hProfile);
	return hProfile;
}

BOOL XYPlotManager::DeleteProfile(HPLOT& hPlot, HPROFILE& hProfile)
{
	assert(m_lpfnDeleteProfile != NULL);
	return m_nLastError = m_lpfnDeleteProfile(hPlot, hProfile) == PE_NOERROR;
}

BOOL XYPlotManager::SetData(HPLOT& hPlot, HPROFILE& hProfile, const double *px, const double *py, unsigned size)
{
	assert(m_lpfnSetData != NULL);
	return m_nLastError = m_lpfnSetData(hPlot, hProfile, px, py, size) == PE_NOERROR;
}

BOOL XYPlotManager::GetData(HPLOT& hPlot, HPROFILE& hProfile, double *px, double *py, unsigned* size)
{
	assert(m_lpfnGetData != NULL);
	return m_nLastError = m_lpfnGetData(hPlot, hProfile, px, py, size) == PE_NOERROR;
}

BOOL XYPlotManager::AppendData(HPLOT& hPlot, HPROFILE& hProfile, const double *px, const double *py, unsigned size)
{
	assert(m_lpfnAppendData != NULL);
	return m_nLastError = m_lpfnAppendData(hPlot, hProfile, px, py, size) == PE_NOERROR;
}

BOOL XYPlotManager::SetFont(HPLOT& hPlot, long item, long subitem, const LOGFONT* font)
{
	assert(m_lpfnSetFont != NULL);
	return m_nLastError = m_lpfnSetFont(hPlot, item, subitem, font) == PE_NOERROR;
}

//////////////////////////////////////////////////////////////////////////
// REGIONS

HPLOTREGION XYPlotManager::CreateRegion( HPLOT& hPlot, long AxisX, long AxisY, double xFrom, double xTo, double yFrom, double yTo, const char* pszTitle )
{
	assert(m_lpfnAddRegion != NULL);
	HPLOTREGION hRegion;
	if (m_nLastError = m_lpfnAddRegion(hPlot, AxisX, AxisY, xFrom, xTo, yFrom, yTo, pszTitle, &hRegion) != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);

	return hRegion;	
}

BOOL XYPlotManager::DeleteRegion( HPLOT& hPlot, HPLOTREGION& hRegion )
{
	assert(m_lpfnDeleteRegion != NULL);
	return m_nLastError = m_lpfnDeleteRegion(hPlot, hRegion ) == PE_NOERROR;
}

BOOL XYPlotManager::SetRegion( HPLOT& hPlot, HPLOTREGION& hRegion, double x1, double x2, double y1, double y2 )
{
	assert(m_lpfnSetRegion != NULL);
	return m_nLastError = m_lpfnSetRegion(hPlot, hRegion, x1, x2, y1, y2 ) == PE_NOERROR;
}

//////////////////////////////////////////////////////////////////////////
/// LEVELS
HLEVEL XYPlotManager::CreateLevel( HPLOT& hPlot, long axis, double val, const char* pszTitle, COLORREF clr, int width )
{
	assert(m_lpfnAddLevel != NULL);
	HLEVEL hLevel;

	if (m_nLastError = m_lpfnAddLevel(hPlot, axis, val,  pszTitle, clr, width, &hLevel) != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);

	return hLevel;	
}

BOOL XYPlotManager::DeleteLevel( HPLOT& hPlot, HLEVEL& hLevel )
{
	assert(m_lpfnDeleteLevel != NULL);
    return m_nLastError = m_lpfnDeleteLevel(hPlot, hLevel) == PE_NOERROR;
}

BOOL XYPlotManager::SetLevel(HPLOT& hPlot, HLEVEL& hLevel, double val)
{
	assert(m_lpfnSetLevel != NULL);
	return m_nLastError = m_lpfnSetLevel (hPlot, hLevel, val) == PE_NOERROR;
}

//////////////////////////////////////////////////////////////////////////
BOOL XYPlotManager::SetColor(HPLOT& hPlot, long item, long subitem, COLORREF color)
{
	assert(m_lpfnSetColor != NULL);
	return m_nLastError = m_lpfnSetColor(hPlot, item, subitem, color) == PE_NOERROR;
}

BOOL XYPlotManager::SetString(HPLOT& hPlot, long item, long subitem, const char* pszString)
{
	assert(m_lpfnSetString != NULL);
	return m_nLastError = m_lpfnSetString(hPlot, item, subitem, pszString) == PE_NOERROR;
}

BOOL XYPlotManager::SetSelectionMode(HPLOT& hPlot, int mode)
{
	assert(m_lpfnSetLong != NULL);
	return m_nLastError = m_lpfnSetLong(hPlot, PI_SELECTION, 0, mode) == PE_NOERROR;
}

long XYPlotManager::GetSelectionMode(HPLOT& hPlot) const
{
	assert(m_lpfnSetLong != NULL);
	long mode;
	if (m_nLastError = m_lpfnGetLong(hPlot, PI_SELECTION, 0, &mode) != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);
	return mode;
}

BOOL XYPlotManager::SetLong(HPLOT& hPlot, long item, long subitem, long value)
{
	assert(m_lpfnSetLong != NULL);
	return m_nLastError = m_lpfnSetLong(hPlot, item, subitem, value) == PE_NOERROR;
}

long XYPlotManager::GetLong(HPLOT& hPlot, long item, long subitem) const
{
	assert(m_lpfnGetLong != NULL);
	long value;
	if (m_nLastError = m_lpfnGetLong(hPlot, item, subitem, &value) != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);
	return value;
}

BOOL XYPlotManager::SetDouble(HPLOT& hPlot, long item, long subitem, double value)
{
	assert(m_lpfnSetDouble != NULL);
	return m_nLastError = m_lpfnSetDouble(hPlot, item, subitem, value) == PE_NOERROR;
}

double XYPlotManager::GetDouble(HPLOT& hPlot, long item, long subitem) const
{
	assert(m_lpfnGetDouble != NULL);
	double value;
	if (m_nLastError = m_lpfnGetDouble(hPlot, item, subitem, &value) != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);
	return value;
}


BOOL XYPlotManager::EnableItem(HPLOT& hPlot, long item, long subitem, BOOL enabled)
{
	assert(m_lpfnEnableItem != NULL);
	return m_nLastError = m_lpfnEnableItem(hPlot, item, subitem, enabled) == PE_NOERROR;
}

BOOL XYPlotManager::SetAxisRange(HPLOT& hPlot, long item, double dMin, double dMax)
{
	assert(m_lpfnSetDouble != NULL);

	m_nLastError = m_lpfnSetDouble(hPlot, item, PAI_LOWER_LIMIT, dMin);
	if (m_nLastError != PE_NOERROR)
		return FALSE;

	return m_nLastError = m_lpfnSetDouble(hPlot, item, PAI_UPPER_LIMIT, dMax) == PE_NOERROR;
}

BOOL XYPlotManager::GetAxisRange(HPLOT& hPlot, long item, double& dMin, double& dMax) const
{
	assert(m_lpfnGetDouble != NULL);

	m_nLastError = m_lpfnGetDouble(hPlot, item, PAI_LOWER_LIMIT, &dMin);
	if (m_nLastError != PE_NOERROR)
		return FALSE;

	return m_nLastError = m_lpfnGetDouble(hPlot, item, PAI_UPPER_LIMIT, &dMax) == PE_NOERROR;
}

std::string XYPlotManager::GetString(HPLOT& hPlot, long item, long subitem) const
{
	assert(m_lpfnGetString != NULL);
	
	std::string str;
	long size;

	m_nLastError = m_lpfnGetString(hPlot, item, subitem, &size, NULL);
	if (m_nLastError != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);
		
	char* pBuf = new char [size];
	m_nLastError = m_lpfnGetString(hPlot, item, subitem, &size, pBuf);
	if (m_nLastError == PE_NOERROR)
	{
		str = pBuf;
		delete [] pBuf;
	}
	else
	{
		delete [] pBuf;
		throw XYPlotRequestFailure(__FUNCTION__);
	}

	return str;
}

BOOL XYPlotManager::SaveProperties(HPLOT& hPlot, const char* pszFileName)
{
	assert(m_lpfnSaveProperties != NULL);
	return m_nLastError = m_lpfnSaveProperties(hPlot, pszFileName) == PE_NOERROR;
}

BOOL XYPlotManager::LoadProperties(HPLOT& hPlot, const char* pszFileName)
{
	assert(m_lpfnLoadProperties != NULL);
	return m_nLastError = m_lpfnLoadProperties(hPlot, pszFileName) == PE_NOERROR;
}

BOOL XYPlotManager::Select(HPLOT& hPlot, HPROFILE& hProfile, unsigned nIndex)
{
	assert(m_lpfnSelect != NULL);
	return m_nLastError = m_lpfnSelect(hPlot, hProfile, nIndex) == PE_NOERROR;
}

BOOL XYPlotManager::Select(HPLOT& hPlot, HGROUP& hGroup, unsigned nIndex)
{
	assert(m_lpfnSelect != NULL);
	return m_nLastError = m_lpfnSelect(hPlot, hGroup, nIndex) == PE_NOERROR;
}

HPROFILE XYPlotManager::GetSelectedProfile(HPLOT& hPlot) const
{
	assert(m_lpfnGetSelection != NULL);
	HPROFILE hProfile;
	if (m_nLastError = m_lpfnGetSelection(hPlot, &hProfile) != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);
	return hProfile; 
}

BOOL XYPlotManager::SetFlashSpeed(HPLOT& hPlot, int speed)
{
	assert(m_lpfnSetLong != NULL);
	return m_nLastError = m_lpfnSetLong( hPlot, PI_SELECTION, SP_FLASH_SPEED, speed ) == PE_NOERROR;
}

BOOL XYPlotManager::IsEnabled(HPLOT& hPlot, long item, long subitem) const
{
	assert(m_lpfnIsEnabled != NULL);
	BOOL bState;
	if (m_nLastError = m_lpfnIsEnabled(hPlot, item, subitem, &bState) != PE_NOERROR)
		throw XYPlotRequestFailure("IsEnabled");
	return bState;
}

BOOL XYPlotManager::CopyToClipboard(HPLOT& hPlot, unsigned width, unsigned height, unsigned dpi)
{
	assert(m_lpfnCopyToClipboard != NULL);
	return m_nLastError = m_lpfnCopyToClipboard(hPlot, width, height, dpi) == PE_NOERROR;
}


BOOL XYPlotManager::DrawToBitmap(HPLOT& hPlot, HBITMAP& hBitmap, bool bDrawWindowBG, bool bDrawClientBG)
{
	assert(m_lpfnDrawToBitmap != NULL);
	return m_nLastError = m_lpfnDrawToBitmap(hPlot, hBitmap, bDrawWindowBG, bDrawClientBG) == PE_NOERROR;
}

COLORREF XYPlotManager::GetColor(HPLOT& hPlot, long item, long subitem) const
{
	assert(m_lpfnGetColor != NULL);
	COLORREF color;
	if (m_nLastError = m_lpfnGetColor(hPlot, item, subitem, &color) != PE_NOERROR)
		throw XYPlotRequestFailure("GetColor");
	return color;
}

BOOL XYPlotManager::SetRedraw(HPLOT& hPlot, BOOL bRedraw)
{
	assert(m_lpfnEnableItem != NULL);
	return m_nLastError = m_lpfnEnableItem(hPlot, PI_CANVAS, 0, bRedraw) == PE_NOERROR;
}

HGROUP XYPlotManager::CreateGroup(HPLOT& hPlot)
{
	assert(m_lpfnCreateGroup != NULL);
	HGROUP hGroup;
	if (m_nLastError = m_lpfnCreateGroup(hPlot, &hGroup) != PE_NOERROR)
		throw XYPlotRequestFailure("CreateGroup");
	return hGroup;
}

BOOL XYPlotManager::DeleteGroup(HPLOT& hPlot, HGROUP& hGroup)
{
	assert(m_lpfnDeleteGroup != NULL);
	return m_nLastError = m_lpfnDeleteGroup(hPlot, hGroup) == PE_NOERROR;
}

BOOL XYPlotManager::BindProfileToGroup(HPLOT& hPlot, HGROUP& hGroup, HPROFILE& hProfile)
{
	assert(m_lpfnBindProfile != NULL);
	return m_nLastError = m_lpfnBindProfile(hPlot, hGroup, hProfile) == PE_NOERROR;
}

BOOL XYPlotManager::GetProfileList(HPLOT& hPlot, std::vector<HPROFILE>& hProfiles)
{
	hProfiles.clear();
	assert(m_lpfnGetProfileList != NULL);

	long size;

	m_nLastError = m_lpfnGetProfileList(hPlot, &size, NULL);
	if (m_nLastError != PE_NOERROR)
		throw XYPlotRequestFailure(__FUNCTION__);

	long* pIDs = new long [size];
	
	m_nLastError = m_lpfnGetProfileList(hPlot, &size, pIDs);

	if (m_nLastError == PE_NOERROR)
	{
		for (long i = 0; i < size; i++)
			hProfiles.push_back(HPROFILE(pIDs[i]));
		
		delete [] pIDs;
	}
	else
	{
		delete [] pIDs;
		throw XYPlotRequestFailure(__FUNCTION__);
	}

	return m_nLastError == PE_NOERROR;

}

BOOL XYPlotManager::RegRestoreDefaults(HPLOT& hPlot)
{
	assert(m_lpfnRegRestoreDefaults != NULL);
	return m_nLastError = m_lpfnRegRestoreDefaults(hPlot) == PE_NOERROR;
}

BOOL XYPlotManager::RegLoadSettings(HPLOT& hPlot, const char* pszKeyName, const char* pszValue)
{
	assert(m_lpfnRegLoadSettings != NULL);
	return m_nLastError = m_lpfnRegLoadSettings(hPlot, pszKeyName, pszValue) == PE_NOERROR;
}
BOOL XYPlotManager::RegSaveSettings(HPLOT& hPlot, const char* pszKeyName, const char* pszValue)
{
	assert(m_lpfnRegSaveSettings != NULL);
	return m_nLastError = m_lpfnRegSaveSettings(hPlot, pszKeyName, pszValue) == PE_NOERROR;
}

BOOL XYPlotManager::RunDialog(HPLOT& hPlot)
{
	assert(m_lpfnRunDialog != NULL);
	return m_nLastError = m_lpfnRunDialog(hPlot) == PE_NOERROR;
}

#ifdef _AFX
void XYPlotManager::Serialize(HPLOT hPlot, CArchive& ar)
{
	SYSTEMTIME st;
	GetLocalTime( &st );

	char name[MAX_PATH];
	GetTempPath( sizeof( name ), name );

	std::string tempFileName = name;
	if( tempFileName.length() < 1 )
		tempFileName = ".";
	GetTempFileName( tempFileName.c_str(), "~us", st.wMinute + st.wHour + st.wSecond + st.wMilliseconds, name );			
	
	if (ar.IsStoring())
	{
        SaveProperties(hPlot, name);
		HANDLE file = CreateFile( name, FILE_ALL_ACCESS, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		
		DWORD dwSize = SetFilePointer(file, 0, 0, FILE_END);

		char* szBuffer = new char[dwSize];

		SetFilePointer(file, 0, 0, FILE_BEGIN);
		
		DWORD dwRead;
		ReadFile(file, szBuffer, dwSize, &dwRead, NULL);
		
		CloseHandle(file);

		ar << dwSize;
		ar.Write(szBuffer, dwSize);

		delete [] szBuffer; 
	}
	else
	{
		DWORD dwSize;
		ar >> dwSize;
		if (dwSize > 100 * 1024)
			throw XYPlotError( "XYPlot: failed to serialize!" );

        char* szBuffer = new char[dwSize];

		ar.Read(szBuffer, dwSize);
		
		HANDLE file = CreateFile( name, FILE_ALL_ACCESS, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL );

		DWORD dwWritten;

		WriteFile(file, szBuffer, dwSize, &dwWritten, NULL);
		CloseHandle(file);		
		delete [] szBuffer;

		LoadProperties(hPlot, name);		
	}
	DeleteFile(name);
}




#endif
