#pragma once

#ifndef __XYPLOT_H_DEFINED__
#define __XYPLOT_H_DEFINED__

#include "xyplotconst.h"

#ifdef _DLL_MASTER_
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif
#define CALLCONV __stdcall

#ifdef __cplusplus
extern "C" {
#endif

// Initialization
IMPEXP long CALLCONV Initialize(HWND hwndMain, long* lPlotID);
IMPEXP long CALLCONV Finalize(long lPlotID);

// Plot manipulation
IMPEXP long CALLCONV EnableItem(long lPlotID, long item, long subitem, BOOL enable);
IMPEXP long CALLCONV IsEnabled(long lPlotID, long item, long subitem, BOOL* enabled);

IMPEXP long CALLCONV Select(long lPlotID, long ID, long index);
IMPEXP long CALLCONV GetSelection(long lPlotID, long* ID);
IMPEXP long CALLCONV RunDialog(long lPlotID);
IMPEXP long CALLCONV CopyToClipboard(long lPlotID, int width, int height, int dpi);

// Profile/group manipulation
IMPEXP long CALLCONV AddProfile(long lPlotID, const char* name, COLORREF color, int width, int linetype, const char* line_template, BOOL bVisible, BOOL bMarked, BOOL bottom_axis, BOOL left_axis, long* lProfileID);
IMPEXP long CALLCONV DeleteProfile(long lPlotID, long lProfileID);
IMPEXP long CALLCONV CreateGroup(long lPlotID, long* lGroupID);
IMPEXP long CALLCONV BindProfile(long lPlotID, long lGroupID, long lProfileID);
IMPEXP long CALLCONV DeleteGroup(long lPlotID, long lGroupID);
IMPEXP long CALLCONV GetProfileList(long lPlotID, long* lSize, long* lProfileIDs);

// Data manipulation
IMPEXP long CALLCONV SetData(long lPlotID, long lProfileID, const double *px, const double *py, unsigned size);
IMPEXP long CALLCONV GetData(long lPlotID, long lProfileID, double *px, double *py, unsigned* size);
IMPEXP long CALLCONV AppendData(long lPlotID, long lProfileID, const double *pfx, const double *pfy, int size);

// Plot properties
IMPEXP long CALLCONV SaveProperties(long lPlotID, const char* szFileName);
IMPEXP long CALLCONV LoadProperties(long lPlotID, const char* szFileName);
IMPEXP long CALLCONV RegLoadSettings(long lPlotID, const char* keyString, const char* szValue);
IMPEXP long CALLCONV RegSaveSettings(long lPlotID, const char* keyString, const char* szValue);
IMPEXP long CALLCONV RegRestoreDefaults(long lPlotID);

IMPEXP long CALLCONV SetString(long lPlotID, long item, long subitem, const char* pszString);
IMPEXP long CALLCONV GetString(long lPlotID, long item, long subitem, int* buf_size, char* buf);
IMPEXP long CALLCONV SetColor(long lPlotID, long item, long subitem, COLORREF color);
IMPEXP long CALLCONV GetColor(long lPlotID, long item, long subitem, COLORREF* color);
IMPEXP long CALLCONV SetLong(long lPlotID, long item, long subitem, int value);
IMPEXP long CALLCONV GetLong(long lPlotID, long item, long subitem, int* value);
IMPEXP long CALLCONV SetDouble(long lPlotID, long item, long subitem, double value);
IMPEXP long CALLCONV GetDouble(long lPlotID, long item, long subitem, double* value);

IMPEXP long CALLCONV SetFont(long lPlotID, long item, long subitem, const LOGFONT* plf);


//////////////////////////////////////////////////////////////////////////
IMPEXP long CALLCONV AddLevel(long lPlotID, long axis, double value, const char* pszTitle, COLORREF clr, int width, long* lLevelID);
IMPEXP long CALLCONV DeleteLevel(long lPlotID, long lLevelID);
IMPEXP long CALLCONV SetLevel(long lPlotID, long lLevelID, double value);

//////////////////////////////////////////////////////////////////////////
/// Regions Functions
IMPEXP long CALLCONV AddRegion(long lPlotID, long AxisX, long AxisY, double xFrom, double xTo, double yFrom, double yTo, const char* pszTitle, long* lRegionID);
IMPEXP long CALLCONV DeleteRegion(long lPlotID, long lRegionID);
IMPEXP long CALLCONV SetRegion(long lPlotID, long lRegionID, double x1, double x2, double y1, double y2);

IMPEXP long CALLCONV DrawToBitmap(long lPlotID, HBITMAP hBmp, bool bDrawWindowBG, bool bDrawClientBG);

#ifdef __cplusplus
}
#endif

#endif //__XYPLOT_H_DEFINED__