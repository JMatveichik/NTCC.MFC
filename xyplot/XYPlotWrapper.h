#ifndef _XYPLOTWRAPPER_H_
#define _XYPLOTWRAPPER_H_

#include <windef.h>
#include <string>
#include <vector>
#include <stdexcept>
#include "xyplotconst.h"

namespace xyplot
{ 

#ifdef _M_X64
	#define XYPLOTFN
#else
	#define XYPLOTFN __stdcall
#endif

typedef long (XYPLOTFN *LPFNINITIALIZE)(HWND hwndMain, long* lPlotID);
typedef long (XYPLOTFN *LPFNFINALIZE)(long lPlotID);
typedef long (XYPLOTFN *LPFNADDPROFILE)(long lPlotID, const char* name, COLORREF color, int width, int linetype, const char* pszTemplate, BOOL bVisible, BOOL bDataMarks, long XAxis, long YAxis, long* lProfileID);
typedef long (XYPLOTFN *LPFNDELETEPROFILE)(long lPlotID, long lProfileID);
typedef long (XYPLOTFN *LPFNSETDATA) (long lPlotID, long lProfileID, const double *px, const double *py, unsigned size);
typedef long (XYPLOTFN *LPFNGETDATA) (long lPlotID, long lProfileID, double *px, double *py, unsigned* size);
typedef long (XYPLOTFN *LPFNAPPENDDATA)(long lPlotID, long lProfileID, const double *px, const double *py, unsigned size);
typedef long (XYPLOTFN *LPFNSETSTRING)(long lPlotID, long item, long subitem, const char* pszString);
typedef long (XYPLOTFN *LPFNGETSTRING)(long lPlotID, long item, long subitem, long* buflen, char* pszBuffer);
typedef long (XYPLOTFN *LPFNSETCOLOR)(long lPlotID, long item, long subitem, COLORREF color);
typedef long (XYPLOTFN *LPFNGETCOLOR)(long lPlotID, long item, long subitem, COLORREF* pColor);
typedef long (XYPLOTFN *LPFNSETLONG)(long lPlotID, long item, long subitem, long value);
typedef long (XYPLOTFN *LPFNGETLONG)(long lPlotID, long item, long subitem, long* value);
typedef long (XYPLOTFN *LPFNSETDOUBLE)(long lPlotID, long item, long subitem, double value);
typedef long (XYPLOTFN *LPFNGETDOUBLE)(long lPlotID, long item, long subitem, double* value);
typedef long (XYPLOTFN *LPFNSETFONT)(long lPlotID, long item, long subitem, const LOGFONT* plf);
typedef long (XYPLOTFN *LPFNISENABLED)(long lPlotID, long item, long subitem, BOOL* enabled);
typedef long (XYPLOTFN *LPFNENABLEITEM)(long lPlotID, long item, long subitem, BOOL enable);
typedef long (XYPLOTFN *LPFNSELECT)(long lPlotID, long lProfileID, long index);
typedef long (XYPLOTFN *LPFNGETSELECTION)(long lPlotID, long* pID);
typedef long (XYPLOTFN *LPFNSAVEPROPERTIES)(long lPlotID, const char* pszFileName);
typedef long (XYPLOTFN *LPFNLOADPROPERTIES)(long lPlotID, const char* pszFileName);
typedef long (XYPLOTFN *LPFNCOPYTOCLIPBOARD)(long lPlotID, unsigned width, unsigned height, unsigned dpi);
typedef long (XYPLOTFN *LPFNDRAWTOBITMAP)(long lPlotID, HBITMAP& hBitmap, bool bDrawWindowBG, bool bDrawClientBG);
typedef long (XYPLOTFN *LPFNCREATEGROUP)(long lPlotID, long* plGroupID);
typedef long (XYPLOTFN *LPFNDELETEGROUP)(long lPlotID, long lGroupID);
typedef long (XYPLOTFN *LPFNBINDPROFILE)(long lPlotID, long lGroupID, long lProfileID);

typedef long (XYPLOTFN *LPFNGETPROFLIST)(long lPlotID, long* lSize, long* lProfileIDs);

typedef long (XYPLOTFN *LPFNREGRESTOREDEFAULTS)(long lPlotID);
typedef long (XYPLOTFN *LPFNREGLOADSETTINGS)(long lPlotID, const char* pszKeyName, const char* pszValue);
typedef long (XYPLOTFN *LPFNREGSAVESETTINGS)(long lPlotID, const char* pszKeyName, const char* pszValue);
typedef long (XYPLOTFN *LPFNRUNDIALOG)(long lPlotID);

//////////////////////////////
// Regions functions
typedef long (XYPLOTFN *LPFNADDREGION)(long lPlotID, long AxisX, long AxisY, double xFrom, double xTo, double yFrom, double yTo, const char* pszTitle, long* lRegionID);
typedef long (XYPLOTFN *LPFNDELETEREGION)(long lPlotID, long lRegionID);
typedef long (XYPLOTFN *LPFNSETREGION)(long lPlotID, long lRegionID, double x1, double x2, double y1, double y2);

//////////////////////////////////////////////////////////////////////////
// Levels functions
typedef long (XYPLOTFN *LPFNADDLEVEL)(long lPlotID, long axis, double value, const char* pszTitle, COLORREF clr, int width, long* lLevelID);
typedef long (XYPLOTFN *LPFNDELETELEVEL)(long lPlotID, long lLevelID);
typedef long (XYPLOTFN *LPFNSETLEVEL)(long lPlotID, long lLevelID, double value);

class HPLOT;		// Plot handle forward declaration
class HPROFILE;		// Profile handle forward declaration
class HGROUP;		// Forward declaration of a group of profiles 
class HPLOTREGION;	// Forward declaration of a plot regions 
class HLEVEL;		// Forward declaration of a plot level line 


///\~russian
///@brief �����-������� ��� ������ �  2D ��������
///@details  �����-������� ��� ������ � ��������� ����������� 2D �������. ������ ����� ������������� ����������� ��������, 
///����������� � ����������� �������� 2D  �������. ����� �������� ����������� , �.�. ���������� ������ ���� ���������   
///������� ������, ������ � �������� �������������� ����� ����������� ������� <b>Instance</b>.
///\~english
///@brief 2D plot wrapper class
///@details  �����-������� ��� ������ � ��������� ����������� 2D �������. ������ ����� ������������� ����������� ��������, 
///����������� � ����������� �������� 2D  �������. ����� �������� ����������� , �.�. ���������� ������ ���� ���������   
///������� ������, ������ � �������� �������������� ����� ����������� ������� <b>Instance</b>.

class XYPlotManager
{

public:

	///\~russian
	///@brief ������� ������� � ������������� ���������� ������
	///����� XYPlotManager �������� ����������� �.�. ���������� ������������ ��������� ������� ������� ������. 
	///������ � ���� �������������� ��� ������ ������ �������.
	///@code
	///	XYPlotManager& pm = XYPlotManager::Instance();
	///@endcode
	///@return ������ �� ��������� 
	///\~english
	///@brief ������� ������� � ������������� ���������� ������
	///����� XYPlotManager �������� ����������� �.�. ���������� ������������ ��������� ������� ������� ������. 
	///������ � ���� �������������� ��� ������ ������ �������.
	///@code
	///	XYPlotManager& pm = XYPlotManager::Instance();
	///@endcode
	///@return ������ �� ���������
	static XYPlotManager& Instance();

protected:
	

public:
	
	///\~russian
	///@brief ������� ��������� ���� ��������� ������
	///@return ������������� ��� ��������� ������
	///\~english
	///������� ��������� ���� ��������� ������
	///@return ������������� ��� ��������� ������
	int GetLastError() const { return m_nLastError; };

	///\~russian
	///@brief �������������� ��� ������
	///@param code - ��� ������ 
	///@return ���������� ��������� �������� ������
	///\~english
	///�������������� ��� ������
	///@param code - ��� ������ 
	///@return ���������� ��������� �������� ������
	std::string TranslateError(int code) const;
	

	///\~russian 
	/// @defgroup group1 ������������ � ������������
	/// @brief ������� ������������ � ������������ 2D �������
	/// @details ������ ����� ������� ��������� ���������������� �����-������� ��� ����������� ����� ������������ 2D �������, � ����� 
	///	��������� �����  � ���������� ������������ ����������.
	///  @{	

	///\~english 
	/// @defgroup group1 2D plot initialization and destruction function 
	/// This group functions initialize wrapper class  for all 2d plot objects manipulation and for create new and destruct
	///	existing plots.
	///  @{	

	///\~russian
	///@brief ������ �������  �������������� ������-�������. 
	///@param pszDllPath - ���� � ����������� ������������ ���������� xyplot.dll
	///@param hwndParent - ������������ ��������, ���������� ���� �������� ����� ����������� ���������
	///@return true - � ������ ���� ������������� ������ �������;<br/>false - � ������ ���� ������������� �� �������.
	BOOL Initialize(const char* pszDllPath, HWND hwndParent = NULL);

	///\~russian
	///@brief ������� �������� ���������� 2D �������. � ������ ���� ��������� ������� ��������� ������� ���������� ���������� XYPlotRequestFailure
	///@param hwndParent - ���������� ���� � ������� ����� ������������ �������
	///@return ��������� ���������� 2D �������
	///@code
	///XYPlotManager& pm = XYPlotManager::Instance();
	///if ( !pm.Initialize("xyplot.dll", ::AfxGetMainWnd()->m_hWnd))
	///	return FALSE;
	///HPLOT plot;
	///CChildView* pView = ...;//�������� ���������� ���� � ������� ����� ������������ ������� 
	///try {
	///		plot = pm.CreatePlot(pView->m_hWnd)
	///	}
	/// catch (XYPlotRequestFailure& e)
	/// {
	///		return FALSE;
	/// }
	///@endcode
 	HPLOT CreatePlot(HWND hwndParent);

	///\~russian
	///@brief ������� �������� ���������� 2D �������. 
	///@details ������� ������� ���������� 2D �������. ������ ������� ���������� ������� ����� ������������ ����, ���������� ��� �������� ���������� � ������� CreatePlot 
	///@return true - � ������ ���� ������������ ���������� ������� ������ �������;
	///@return false - � ������ ���� �� ������� ���������� ���������.	
	BOOL DestroyPlot(HPLOT& hPlot);
	///@}

	///\~russian
	///@brief ������� �������� ������� ���������� ����������� 2D ������� � ����� ������ � ���� �����������.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param width - ������ ��������������� ����������� � ��������
	///@param height - ������ ��������������� �����������  � ��������
	///@param dpi - ���������� �����������
	///@return true - � ������ ���� ����������� ������ �������;
	///@return false - � ������ ���� �� ������� ����������� ��������� � ����� ������.
	BOOL CopyToClipboard(HPLOT& hPlot, unsigned width, unsigned height, unsigned dpi);

	///\~russian
	///@brief ������� �������� ������� ���������� ����������� 2D � ����� ������� �����.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param hBitmap - ���������� ������� �����
	///@param bDrawWindowBG - ���� ��������� ���� ����
	///@param bDrawClientBG - ���� ��������� ���� ���������� �����
	///@return true - � ������ ���� ����������� ������ �������;
	///@return false - � ������ ���� �� ������� ����������� ��������� � ����� ������.	
	BOOL DrawToBitmap(HPLOT& hPlot, HBITMAP& hBitmap, bool bDrawWindowBG, bool bDrawClientBG);
	

	///\~russian
	/// @brief ������� �������� ������ �������� ���������� 2D ������� .
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@return true - � ������ ���� ������ ������� ������ �������;
	///@return false - � ������ ���� �� ������� ��������� ������ ��������.
	BOOL RunDialog(HPLOT& hPlot);


	///\~russian 
	/// @defgroup grProperties ������� ���������� � �������� �������
	/// @brief �������� ���������� 
	/// @details ������ ����� ������� ��������� ���������  � ��������� �������� 2D ������� �� ����� ��� �������.
	///  @{	

	///\~russian	
	///@brief ������� ���������� ���������  ���������� 2D ������� � ��������� �� ���������.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@return true - � ������ ���� ������� ��������� ���������;<br/>false - � ������ ���� �� ������� ��������� ���������.
	BOOL RegRestoreDefaults(HPLOT& hPlot);

	///\~russian
	///@brief ������� ��������� ���������  ���������� 2D ������� �� �������.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param pszKey - ���� �������
	///@param pszValue - ���������� ����� �������
	///@return true - � ������ ���� ������� ��������� ���������;
	///@return false - � ������ ���� �� ������� ��������� ���������.
	BOOL RegLoadSettings(HPLOT& hPlot, const char* pszKey, const char* pszValue);

	///\~russian
	///@brief ������� ��������� ���������  ���������� 2D ������� � �������.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param pszKey - ���� �������
	///@param pszValue - ���������� ����� �������
	///@return true - � ������ ���� ������� ��������� ���������;
	///@return false - � ������ ���� �� ������� ��������� ���������.
	BOOL RegSaveSettings(HPLOT& hPlot, const char* pszKey, const char* pszValue);

	///\~russian
	///@brief ������� ��������� ���������  ���������� 2D ������� � �����.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param pszFileName - ���� � �����
	///@return true - � ������ ���� ������� ��������� ���������;
	///@return false - � ������ ���� �� ������� ��������� ���������.
	BOOL SaveProperties(HPLOT& hPlot, const char* pszFileName);
	
	///\~russian
	///@brief ������� ��������� ���������  ���������� 2D ������� �� �����.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param pszFileName - ���� � �����
	///@return true - � ������ ���� ������� ��������� ���������;
	///@return false - � ������ ���� �� ������� ��������� ���������.
	BOOL LoadProperties(HPLOT& hPlot, const char* pszFileName);

#ifdef _AFX
	///\~russian
	///@brief ������� ��������� ��� ��������� ���������  ���������� 2D ������� � ����� MFC.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param ar - ��������� ������	 MFC
	void Serialize(HPLOT hPlot, CArchive& ar);
#endif
	///@} 

	///\~russian 
	/// @defgroup grSelect ������� ��� ������ � ����������
	/// @brief ������ ����� ������� ������������ 
	///  @{	

	///\~russian
	///@brief ������� ������������� ��������� ���������� �������.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param[in] hProfile  - ���������� �������
	///@param nIndex - ������ � ������� ������
	///@return true - � ������ ���� ��������� �������  ������ �������;
	///@return false - � ������ ���� �� ������� �������� �������.
	BOOL Select(HPLOT& hPlot, HPROFILE& hProfile, unsigned nIndex);

	///\~russian
	///@brief ������� ������������� ��������� ��������� ������ ��������.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param hGroup   - ���������� ������ ��������
	///@param nIndex - ������ � ������� ������
	///@return true - � ������ ���� ��������� ������ ��������  ������ �������;
	///@return false - � ������ ���� �� ������� �������� ������ ��������.
	BOOL Select(HPLOT& hPlot, HGROUP& hGroup, unsigned nIndex);

	///\~russian
	///@brief ������� ������ ����� ��������� ��� �������� � ����� ��������
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param mode - ����� ��������� ���� �� 
	///SM_NONE = -1	- ��� ���������
	///SM_FLASH = 0 - ��������� ���������
	///SM_VTRACE = 1 - ������������ �����������
	///SM_HTRACE = 2 - �������������� �����������
	///SM_TRACE = 3 - ������ �����������
	///@return true - � ������ ���� ������� ���������� ����� ���������;
	///@return false - � ������ ���� �� ������� ���������� ���������.
	BOOL SetSelectionMode(HPLOT& hPlot, int mode);

	///\~russian
	///@brief ������� ���������� ������� ����� ��������� ��� �������� � ����� ��������
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@return	SM_NONE = -1	- ��� ���������
	///@return	SM_FLASH = 0 - ��������� ���������
	///@return	SM_VTRACE = 1 - ������������ �����������
	///@return	SM_HTRACE = 2 - �������������� �����������
	///@return	SM_TRACE = 3 - ������ �����������
	long GetSelectionMode(HPLOT& hPlot) const;

	///\~russian
	///@brief ������� ������ ������� �������� ������� ���� ������ �������� � ������  ��������� SM_FLASH
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param speed - ����� ��������� ���� �� \li FLS_NO_FLASH = 0 \li FLS_VERY_SLOW = 1 \li FLS_SLOW = 2 \li FLS_NORMAL = 3 \li FLS_FAST = 4 \li FLS_VERY_FAST = 5 \li FLS_EXTREME = 6
	///@return true - � ������ ���� ������� ���������� ������� ��������;
	///@return false - � ������ ���� �� ������� ���������� ������� ��������.	
	BOOL SetFlashSpeed(HPLOT& hPlot, int speed);
	
	///@}


	
	///\~russian
	///@brief ������� ��������� ���� ��������� ����������� ���������� 2D �������
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param bRedraw - ���� ���������� ��������� ���� bRedraw = TRUE ���� ����������� ����� ������ ��������. 
	///� ������ ���� bRedraw = FALSE ���� �� ����������� �� ���� ������� ������ ������� � ���������� bRedraw = TRUE
	///@code
	///XYPlotManager& pm = XYPlotManager::Instance();
	///double* pdX = NULL;
	///double* pdY = NULL;
	///HPLOT hPlot = ...;//�������� ���������� 2D �������
	///pm.SetRedraw(hPlot, FALSE); 
	///for (int i = 0; i < nProfileCount; i++)
	///{
	///		//�������� ������ ��� �������
	///     ...
	///    HPROFILE hProf = ...;//�������� ���������� �������
	///    //���������� ������ �������
	///    pm.SetData(hPlot, hProf, pdX, pdY, nSize);
	///    ...//������ ��������
	///}
	///pm.SetRedraw(hPlot, TRUE);
	///@endcode
	BOOL SetRedraw(HPLOT& hPlot, BOOL bRedraw = TRUE);


	///\~russian 
	/// @defgroup grElements ������� ��� ������ � ���������� �������
	/// @brief ������ ����� ������� ������������ ������ � ���������� � �������� ������� ���������  �������.
	///  @{	

	///\~english
	/// @defgroup grElements Plot elements functions
	///  This group functions work with properties of 2D plot elements
	///  @{	

	///\~russian
	///@brief ������� ������ �������� �������� ��� ���������� 2D �������. 
	///@details ������ ������� �������� ������ � ������ ���� ��� ��� �� ����� ����� ��������������� ���������������.
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param axis - ������������� ��� 
	///\li PI_AXIS_LEFT = 6
	///\li PI_AXIS_RIGHT = 7
	///\li PI_AXIS_TOP = 8
	///\li PI_AXIS_BOTTOM = 9
	///@param dMin - ��������� �������� ���������
	///@param dMax - �������� �������� ���������
	BOOL SetAxisRange(HPLOT& hPlot, long axis, double dMin, double dMax);

	///\~russian
	///@brief ������� ��������� ��������� �������� ��� ���������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param axis - ������������� ��� 
	///\li PI_AXIS_LEFT = 6
	///\li PI_AXIS_RIGHT = 7
	///\li PI_AXIS_TOP = 8
	///\li PI_AXIS_BOTTOM = 9
	///@param[out] dMin - ��������� �������� ���������
	///@param[out] dMax - �������� �������� ���������
	BOOL GetAxisRange(HPLOT& hPlot, long axis, double& dMin, double& dMax) const;

	///\~russian
	///@brief ������� ��������� ������ �������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@return ������ ��������� � ������������ 2D �������. 
	std::string GetString(HPLOT& hPlot, long item, long subitem) const;

	///\~russian
	///@brief ������� ������� ������ �������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@param pszString - ������ ��� ����������� 2D �������. 
	///@return true - � ������ ���� ������� ���������� ������ ��������;<br/>false - � ������ ���� �� ������� ���������� ������ ��������.	
	BOOL SetString(HPLOT& hPlot, long item, long subitem, const char* pszString);

	///\~russian
	///������� ��������� �������������� �������� ���������� � ��������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@return ������������� �������� ��������� � ������������ 2D �������. 
	long GetLong(HPLOT& hPlot, long item, long subitem) const;

	///\~russian
	///@brief ������� ������� �������������� �������� ���������� � ��������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@param value - ������������� �������� ��������� � ������������ 2D �������. 
	///@return true - � ������ ���� ������� ���������� �������������� �������� ��������;<br/>false - � ������ ���� �� ������� ���������� �������� ��������.
	BOOL SetLong(HPLOT& hPlot, long item, long subitem, long value);

	///\~russian
	///������� ������� �������� � ��������� ������ ���������� � ��������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@return �������� � ��������� ������ ��������� � ������������ 2D �������. 
	double GetDouble(HPLOT& hPlot, long item, long subitem) const;

	///\~russian
	///@brief ������� ������� �������� � ��������� ������ ���������� � ��������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@param value - � ��������� ������ �������� ��������� � ������������ 2D �������. 
	///@return true - � ������ ���� ������� ���������� �������� � ��������� ������ ��������;<br/>false - � ������ ���� �� ������� ���������� �������� � ��������� ������ ��������.
	BOOL SetDouble(HPLOT& hPlot, long item, long subitem, double value);

	///\~english
	///Function enables/disables 2D plot element. 
	///@param[in] hPlot - 2D plot handle
	///@param item - plot item identificator
	///@param subitem - plot subitem identificator
	///@param enabled - enable flag  
	///@return true - if element state was changed;
	///@return false - if element state was not changed.
	///\~russian
	///@brief ������� ���������/���������� �������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@param enabled - ���� ��������� ����������� 2D ������� 
	///@return true - � ������ ���� ������� ����������� ��������� ��������;
	///@return false - � ������ ���� �� ������� ���������� �������� � ��������� ������ ��������.
	BOOL EnableItem(HPLOT& hPlot, long item, long subitem, BOOL enabled);

	///\~english
	///@brief Function return enables/disables state 2D plot element. 
	///@param[in] hPlot - 2D plot handle
	///@param item - plot item identificator
	///@param subitem - plot subitem identificator
	///@return true - plot item enabled now;
	///@return false - plot item not enabled now.
	///\~russian
	///@brief ������� ���������� ���� ��������� �������� 2D ������� (�������/��������). 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 	
	///@return true - � ������ ���� ������� ���������� ������� �������;
	///@return false - � ������ ���� ������� ���������� ������� ��������.
	BOOL IsEnabled(HPLOT& hPlot, long item, long subitem) const;



	///\~english
	///@brief Function return color for 2D plot element. 
	///@param[in] hPlot - 2D plot handle
	///@param item - plot item identificator
	///@param subitem - plot subitem identificator
	///@return - plot item current color as \b COLORREF value;	
	///\~russian
	///@brief ������� ���������/���������� �������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@return ������� ���� �������� 2D �������.
	COLORREF GetColor(HPLOT& hPlot, long item, long subitem) const;
		
	///\~russian
	///@brief ������� ������� ����� �������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@param color	- ����� ���� ����������� 2D ������� 
	///@return true - � ������ ���� ������� ������ ���� ��������;
	///@return false - � ������ ���� �� ������� ������ ���� ��������.
	///\~english
	///@brief Function set color 2D plot element. 
	///@param[in] hPlot - 2D plot handle
	///@param item - plot item identificator
	///@param subitem - plot subitem identificator
	///@param color - new item color
	///@return true - if element color was changed;
	///@return false - if element color was not changed.
	BOOL SetColor(HPLOT& hPlot, long item, long subitem, COLORREF color);

	///\~russian
	///@brief ������� ������� ������  �������� 2D �������. 
	///@param[in] hPlot - ���������� ���������� 2D ������� 
	///@param item - ������������� �������� 2D ������� 
	///@param subitem - ������������� ����������� 2D ������� 
	///@param font	- ��������� �� ����� ����� ����������� 2D ������� 
	///@return true - � ������ ���� ������� ������ ����� ��������;
	///@return false - � ������ ���� �� ������� ������ ����� ��������.
	///\~english
	///@brief Function set font 2D plot element. 
	///@param[in] hPlot - 2D plot handle
	///@param item - plot item identificator
	///@param subitem - plot subitem identificator
	///@param font - new item font pointer
	///@return true - if element color was changed;
	///@return false - if element color was not changed.
	BOOL SetFont(HPLOT& hPlot, long item, long subitem, const LOGFONT* font);

	///@}

	


	///\~russian 
	/// @defgroup grOutput ������� ��� ������ � ��������� � ��������.
	/// @brief  ������ ����� ������� ������������ ��������, �������� �������� � ����� ��������.
	///  @{	

	///\~english
	/// @defgroup grOutput Plot profiles/groups manipulation functions.
	///  This group functions work with plot profiles/groups.
	///  @{	

	///\~russian
	///@brief �������� ���������� �������
	///@param[in] hPlot	���������� ���������� 2D ������� ��� �������� ��������� �������
	///@param name ��� ������� (������������ � �������)
	///@param color ���� �������
	///@param width ������� ����� ������� � ��������
	///@param lineType ��� ����� ������� 
	///\li PLS_INVISIBLE - ��������� \li PLS_SOLID - �������� \li PLS_DASH - ��������� \li PLS_DOT - �������� \li PLS_DASHDOT - ������ \li PLS_DASHDOTDOT - �����-�����-����� \li PLS_CUSTOM - �����������������
	///@param szLineTemplate ���������������� ������ �����. ������ �������� ������������ ������ � ������ ���� ������ ���������������� ��� �����. 
	///������ ������������ ����� ������������������ ������������� �������� ��� ������� ���������� ���������� � ����������� � ��������. 
	///�������� �������� ������� "5 2" ��������,  ��� 5px �������������� �������� ������ ����� ������� ���������� 2px.
	///@param visible ����� �� ������������ ����� ������� �������  
	///@param showmarks ���������� �� ����� ������ �������
	///@param xParentAxis ��� X, � ������� ������������� ������� 
	///@param yParentAxis ��� Y, � ������� ������������� ������� 
	///@return  ���������� ���������� �������
	///\~english
	///@brief Profile handle creation
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param name profile name (show in legend)
	///@param color profile color
	///@param width profile line width in pixels
	///@param lineType ��� ����� ������� 
	///\li PLS_INVISIBLE - ��������� \li PLS_SOLID - �������� \li PLS_DASH - ��������� \li PLS_DOT - �������� \li PLS_DASHDOT - ������ \li PLS_DASHDOTDOT - �����-�����-����� \li PLS_CUSTOM - �����������������
	///@param szLineTemplate ���������������� ������ �����. ������ �������� ������������ ������ � ������ ���� ������ ���������������� ��� �����. 
	///������ ������������ ����� ������������������ ������������� �������� ��� ������� ���������� ���������� � ����������� � ��������. 
	///�������� �������� ������� "5 2" ��������,  ��� 5px �������������� �������� ������ ����� ������� ���������� 2px.
	///@param visible ����� �� ������������ ����� ������� �������  
	///@param showmarks ���������� �� ����� ������ �������
	///@param xParentAxis ��� X, � ������� ������������� ������� 
	///@param yParentAxis ��� Y, � ������� ������������� ������� 
	///@return  ���������� ���������� �������		
	///\~	
	///\include ex_001.cpp
	HPROFILE CreateProfile(HPLOT& hPlot, const char* name, COLORREF color, int width, int lineType = xyplot::PLS_SOLID, const char* szLineTemplate = 0, BOOL visible = TRUE, BOOL showmarks = FALSE, const long xParentAxis = xyplot::PI_AXIS_BOTTOM, const long yParentAxis = xyplot::PI_AXIS_LEFT);


	///\~russian
	///@brief �������� ���������� �������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� �������
	///@param[in] hProfile ���������� �������
	///@return true ���� ������� ������
	///@return false ���� ������� �� ������.
	///\~english
	///@brief Delete profile
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfile profile handle
	///@return true if profile was delete
	///@return false if profile was not delete
	BOOL DeleteProfile(HPLOT& hPlot, HPROFILE& hProfile);


	///\~russian
	///@brief �������� ������ ��������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������ ��������
	///@return ���������� ������	
	///\~english
	///Create profiles group
	///@param[in] hPlot	parent 2D plot handle for profile
	///@return profiles group handle
	///\~
	///@sa CreateProfile()
	///@sa BindProfileToGroup()	
	HGROUP CreateGroup(HPLOT& hPlot);
	
	///\~russian
	///@brief �������� ������ ��������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������ ��������
	///@param hGroup ���������� ������ 
	///@return true ���� ������ �������� �������
	///@return false ���� ������  �������� �� �������
	///\~english
	///@brief Delete profiles group
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param hGroup profile handle
	///@return true if profiles group was delete
	///@return false if profiles group was not delete
	BOOL DeleteGroup(HPLOT& hPlot, HGROUP& hGroup);

	///\~russian
	///@brief ���������� ������� � ������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������ ��������
	///@param[in] hProfile ���������� �������
	///@param hGroup ���������� ������ 
	///@return true ���� ������ �������� �������
	///@return false ���� ������  �������� �� �������
	///\~english
	///@brief Add profile group
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfile profile handle
	///@param hGroup profile handle
	///@return true if profile was added to group
	///@return false if profile was not added to group
	BOOL BindProfileToGroup(HPLOT& hPlot, HGROUP& hGroup, HPROFILE& hProfile);

	///\~russian
	///��������� ������ �������� ���������� 2D �������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������ ��������
	///@param[in] hProfiles ��������� ���������� ������ ���� �������� 
	///@return true ���� ������� �������� ������ �������� 
	///@return false ���� �� ������� �������� ������ ��������
	///\~english
	///@brief Get list of all profiles for 2D plot 
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfiles profile handle
	///@return true if profile list was added to group
	///@return false if profile list was not added to group
	BOOL GetProfileList(HPLOT& hPlot, std::vector<HPROFILE>& hProfiles);
	///@}


	///\~russian 
	/// @defgroup grOutputData ������� ��� ������ � �������.
	/// @brief ������ ����� ������� ������������ ������� ���������� ��������� ��������.
	///  @{	

	///\~english
	/// @defgroup grOutputData Plot profiles/groups data manipulation functions.
	///  This group functions work with plot profiles/groups data.
	///  @{	
	
	///\~russian
	///@brief ���������� ������ ��� �������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������ ��������
	///@param[in] hProfile ���������� ������� ��� �������� �������� ������
	///@param px ��������� �� ������ ������, ���������� ���������� ������� �� ��� X 
	///@param py ��������� �� ������ ������, ���������� ���������� ������� �� ��� Y 
	///@param size ������ ������� ������ px � py
	///@return true ���� ������� ������ ������ ��� �������
	///@return false ���� �� ������� ������ ������ ��� �������
	///@remark ������� �������� px � py ������ ���� �����������.
	///\~english
	///@brief ���������� ������ ��� �������
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfile profile handle
	///@param px ��������� �� ������ ������, ���������� ���������� ������� �� ��� X 
	///@param py ��������� �� ������ ������, ���������� ���������� ������� �� ��� Y 
	///@param size ������ ������� ������ px � py
	///@return true ���� ������� ������ ������ ��� �������
	///@return false ���� �� ������� ������ ������ ��� �������
	///@remark ������� �������� px � py ������ ���� �����������.
	BOOL SetData(HPLOT& hPlot, HPROFILE& hProfile, const double *px, const double *py, unsigned size);

	///\~russian
	///@brief �������� ������ ������� � �������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������ ��������
	///@param[in] hProfile ���������� ������� �� �������� ���������� ������
	///@param[out] px ��������� �� ������ ������, ���������� ���������� ������� �� ��� X 
	///@param[out] py ��������� �� ������ ������, ���������� ���������� ������� �� ��� Y 
	///@param[out] size ������ ������� ������ px � py
	///@return true ���� ������� ��������  ������ ��� �������
	///@return false ���� �� ������� ��������  ������ ��� �������
	///@remark ��� ����������� ����������� �������� �������� ���������� ������� ������� � ����������� px = NULL � py = NULL.  
	///\~english
	///@brief ���������� ������ ��� �������
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfile profile handle for data copy 
	///@param[out] px ��������� �� ������ ������, ���������� ���������� ������� �� ��� X 
	///@param[out] py ��������� �� ������ ������, ���������� ���������� ������� �� ��� Y 
	///@param[out] size ������ ������� ������ px � py
	///@return true ���� ������� ������ ������ ��� �������
	///@return false ���� �� ������� ������ ������ ��� �������
	///@remark ������� �������� px � py ������ ���� �����������.
	BOOL GetData(HPLOT& hPlot, HPROFILE& hProfile, double *px, double *py, unsigned* size);

	///\~russian
	///@brief �������� ������ ��� �������
	///@details ������� ���������� ������ ������������ � �������� px � py ��� �������.
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� �������
	///@param[in] hProfile ���������� ������� 
	///@param px ��������� �� ������ ������, ���������� ���������� ������� �� ��� X 
	///@param py ��������� �� ������ ������, ���������� ���������� ������� �� ��� Y 
	///@param size ���������� ����� ������������ �� ��������
	///@return true ���� ������� �������� ������ ��� �������
	///@return false ���� �� ������� �������� ������ ��� �������
	///@remark ������� �������� px � py ������ ���� �����������.
	///\~english
	///@brief ���������� ������ ��� �������
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfiles profile handle
	///@param px ��������� �� ������ ������, ���������� ���������� ������� �� ��� X 
	///@param py ��������� �� ������ ������, ���������� ���������� ������� �� ��� Y 
	///@param size ������ ������� ������ px � py
	///@return true ���� ������� ������ ������ ��� �������
	///@return false ���� �� ������� ������ ������ ��� �������
	///@remark ������� �������� px � py ������ ���� �����������.
	BOOL AppendData(HPLOT& hPlot, HPROFILE& hProfile, const double *px, const double *py, unsigned size);
	///@}

	///\~russian 
	/// @defgroup grProfEx C������� �������
	/// @brief ������� ��� ������ �� ���������� �������.
	///  @{	
	
	///\~russian
	///@brief �������� ������� ���������� �������
	///@param[in] hPlot	parent 2D plot handle for profile
	///@return ���������� ����������� ������� ������� 
	///@return INVALID_PROFILE ���� � ������ ������ �� ���� ������� �� ������ ������� ���������� .
	HPROFILE GetSelectedProfile(HPLOT& hPlot) const;

	///\~russian
	///@brief �������� ���� �������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� �������
	///@param[in] hProfile ���������� ������� 
	COLORREF GetColor(HPLOT& hPlot, HPROFILE& hProfile) const;

	///\~russian
	///@brief ������ ���� �������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� �������
	///@param[in] hProfile ���������� ������� 
	///@param[in] color ���������� ������� 
	///@return true ���� ������� ������ ���� �������
	///@return false ���� �� ������� ������ ���� �������
	BOOL SetColor(HPLOT& hPlot, HPROFILE& hProfile, COLORREF color);

	///@}

	
	///\~russian 
	/// @defgroup grLvlEx ����� �������.
	/// @brief ������� ��� ������ ������� �������.
	/// @details ��������, �������� � ��������� ��������� ���������� ����� ������
	///  @{

	///\~russian
	///@brief �������� ���������� ����� ������
	///@param[in] hPlot	���������� ���������� 2D ������� ��� �������� ��������� �������
	///@param axis ���, ��� ������� ��������� ����� ������
	///@param val	�������� ������ � ���������� �������� 
	///@param pszTitle ������� ����� ������
	///@param clr	���� ����� 
	///@param width ������� �����  � ��������
	///@return  ���������� ���������� ����� ������	
	///\~english	
	///@brief �������� ���������� ����� ������
	///@param[in] hPlot	���������� ���������� 2D ������� ��� �������� ��������� �������
	///@param axis ���, ��� ������� ��������� ����� ������
	///@param val	�������� ������ � ���������� �������� 
	///@param pszTitle ������� ����� ������
	///@param clr	���� ����� 
	///@param width ������� �����  � ��������
	///@return  ���������� ���������� ����� ������
	HLEVEL CreateLevel(HPLOT& hPlot, long axis, double val, const char* pszTitle, COLORREF clr, int width);

	///\~russian
	///@brief �������� ����� ������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������ ��������
	///@param hLevel ���������� ����� ������
	///@return true ���� ����� ������ �������
	///@return false ���� ����� ������ �� �������
	///\~english
	///@brief Delete level line
	///@param[in] hPlot	parent 2D plot handle for level line
	///@param hLevel level line handle
	///@return true if level line was delete
	///@return false if level line was not delete
	BOOL DeleteLevel(HPLOT& hPlot, HLEVEL& hLevel);

	///\~russian
	///@brief ������� ������ �������� ����� ������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������ ��������
	///@param hLevel ���������� ����� ������
	///@param value ����� �������� ������
	///@return true ���� ����� �������� ����� ������ ������
	///@return false ���� ����� �������� ����� ������ �� ������
	///\~english
	///@brief Set new level position
	///@param[in] hPlot	parent 2D plot handle for level line
	///@param hLevel level line handle
	///@param value new level line position 
	///@return true if new position for level line was set
	///@return false if new position for level line not set
	BOOL SetLevel(HPLOT& hPlot, HLEVEL& hLevel, double value);
	///@}

	/// @defgroup grRgnEx �������.
	/// @brief ������� ��� ������ ���������.
	///  @{	

	///\~russian
	///@brief �������� ���������� �������
	///@details �������� ������������� ������� ������� ������������� ������� � ������������ [xFrom, yFrom] � [xTo, yTo].
	///@param[in] hPlot	���������� ���������� 2D ������� ��� �������� ��������� ������
	///@param axisX ��� X, ��� ������� ��������� ������
	///@param axisY ��� Y, ��� ������� ��������� ������
	///@param xFrom ��������� �������� ������� ���������� �������� �� ��� X
	///@param xTo �������� �������� ������� ���������� �������� �� ��� X
	///@param yFrom ��������� �������� ������� ���������� �������� �� ��� Y
	///@param yTo �������� �������� ������� ���������� �������� �� ��� Y
	///@param pszTitle ������� �������
	///@return  ���������� ���������� �������
	///\~english	
	///@brief �������� ���������� ����� ������
	///@param[in] hPlot	���������� ���������� 2D ������� ��� �������� ��������� �������
	///@param axis ���, ��� ������� ��������� ����� ������
	///@param val	�������� ������ � ���������� �������� 
	///@param pszTitle ������� ����� ������
	///@param clr	���� ����� 
	///@param width ������� �����  � ��������
	///@return  ���������� ���������� ����� ������
	HPLOTREGION CreateRegion(HPLOT& hPlot, long axisX, long axisY, double xFrom, double xTo, double yFrom, double yTo, const char* pszTitle );

	///\~russian
	///@brief �������� �������
	///@param[in] hPlot	���������� ���������� 2D ������� ���������� ������
	///@param hRegion ���������� �������
	///@return true ���� ������ ������
	///@return false ���� ������ �� ������
	///\~english
	///@brief Delete level line
	///@param[in] hPlot	parent 2D plot handle for level line
	///@param hLevel level line handle
	///@return true if level line was delete
	///@return false if level line was not delete
	BOOL DeleteRegion(HPLOT& hPlot, HPLOTREGION& hRegion);

	///\~russian
	///@brief ������� ����� ������ �������
	///@details ������� ������� ������������� ������� ������� ������������ � ����� ��������� xFrom, yFrom, xTo, yTo.
	///@param[in] hPlot	���������� ���������� 2D ������� ��� �������
	///@param hRegion ���������� �������
	///@param xFrom ����� ��������� �������� ������� � ���������� �������� �� ��� X
	///@param xTo ����� �������� �������� ������� � ���������� �������� �� ��� X
	///@param yFrom ����� ��������� �������� ������� � ���������� �������� �� ��� Y
	///@param yTo ����� �������� �������� ������� � ���������� �������� �� ��� Y
	///@return true ���� ������� �������� �������� ������ �������
	///@return false ���� �� ������� �������� �������� ������ �������
	///\~english	
	///@brief �������� ���������� ����� ������
	///@param[in] hPlot	���������� ���������� 2D ������� ��� �������� ��������� �������
	///@param hRegion ���������� �������
	///@param axis ���, ��� ������� ��������� ����� ������
	///@param val	�������� ������ � ���������� �������� 
	///@param pszTitle ������� ����� ������
	///@param clr	���� ����� 
	///@param width ������� �����  � ��������
	///@return  ���������� ���������� ����� ������
	BOOL SetRegion(HPLOT& hPlot, HPLOTREGION& hRegion, double xFrom, double xTo, double yFrom,double yTo);
	///@}

	///���������� ������ �������
	~XYPlotManager() {}

protected:


	/// @defgroup grConstruct ������������ ������.
	///  @{	

	///@brief ����������� �� ����������
	///@details ����������� �� ���������� ������� � ���������� ������. ��� ����������� �������� ��������� ���������� ������� ������.
	///@sa Instance
	XYPlotManager() : m_hDllInstance(NULL), m_nLastError(xyplot::PE_NOERROR) {};

	///@brief ����������� �����������
	///@details ����������� �����������, ����� ��� ����������� �� ���������� ������� � ���������� ������. ��� ����������� �������� ��������� ���������� ������� ������ ��������� ������������ ���������.
	///@sa Instance()
	///@sa XYPlotManager()
	XYPlotManager(const XYPlotManager& rhs);

	///@}


	HMODULE m_hDllInstance;		///< ���������� ������ 
		
	mutable int m_nLastError;	///< ��� ��������� ������


	/// @defgroup grFuncPointers ��������� �� �������.
	///  @{	
	xyplot::LPFNINITIALIZE			m_lpfnInitialize;			///< ��������� �� ������� Initialize � ������ m_hDllInstance.
	xyplot::LPFNFINALIZE			m_lpfnFinalize;				///< ��������� �� ������� Finalize  � ������ m_hDllInstance.
	xyplot::LPFNADDPROFILE			m_lpfnAddProfile;			///< ��������� �� ������� AddProfile � ������ m_hDllInstance.		 
	xyplot::LPFNDELETEPROFILE		m_lpfnDeleteProfile;		///< ��������� �� ������� DeleteProfile � ������ m_hDllInstance.	
	xyplot::LPFNSETDATA				m_lpfnSetData;				///< ��������� �� ������� SetData � ������ m_hDllInstance.			
	xyplot::LPFNGETDATA				m_lpfnGetData;				///< ��������� �� ������� GetData � ������ m_hDllInstance.			
	xyplot::LPFNAPPENDDATA			m_lpfnAppendData;			///< ��������� �� ������� AppendData � ������ m_hDllInstance.		
	xyplot::LPFNSETSTRING			m_lpfnSetString;			///< ��������� �� ������� SetString � ������ m_hDllInstance.		
	xyplot::LPFNGETSTRING			m_lpfnGetString;			///< ��������� �� ������� GetString � ������ m_hDllInstance.		
	xyplot::LPFNSETCOLOR			m_lpfnSetColor;				///< ��������� �� ������� SetColor � ������ m_hDllInstance.			
	xyplot::LPFNGETCOLOR			m_lpfnGetColor;				///< ��������� �� ������� GetColor � ������ m_hDllInstance.			
	xyplot::LPFNSETFONT				m_lpfnSetFont;				///< ��������� �� ������� SetFont � ������ m_hDllInstance.			
	xyplot::LPFNSETLONG				m_lpfnSetLong;				///< ��������� �� ������� SetLong � ������ m_hDllInstance.			
	xyplot::LPFNGETLONG				m_lpfnGetLong;				///< ��������� �� ������� GetLong � ������ m_hDllInstance.			
	xyplot::LPFNSETDOUBLE			m_lpfnSetDouble;			///< ��������� �� ������� SetDouble � ������ m_hDllInstance.		
	xyplot::LPFNGETDOUBLE			m_lpfnGetDouble;			///< ��������� �� ������� GetDouble � ������ m_hDllInstance.		
	xyplot::LPFNISENABLED			m_lpfnIsEnabled;			///< ��������� �� ������� IsEnabled � ������ m_hDllInstance.		
	xyplot::LPFNENABLEITEM			m_lpfnEnableItem;			///< ��������� �� ������� EnableItem � ������ m_hDllInstance.		
	xyplot::LPFNSAVEPROPERTIES		m_lpfnSaveProperties;		///< ��������� �� ������� SaveProperties � ������ m_hDllInstance.	
	xyplot::LPFNLOADPROPERTIES		m_lpfnLoadProperties;		///< ��������� �� ������� LoadProperties � ������ m_hDllInstance.	
	xyplot::LPFNSELECT				m_lpfnSelect;				///< ��������� �� ������� Select � ������ m_hDllInstance.			
	xyplot::LPFNGETSELECTION		m_lpfnGetSelection;			///< ��������� �� ������� GetSelection � ������ m_hDllInstance.		
	xyplot::LPFNCOPYTOCLIPBOARD		m_lpfnCopyToClipboard;		///< ��������� �� ������� CopyToClipboard � ������ m_hDllInstance.	
	xyplot::LPFNDRAWTOBITMAP		m_lpfnDrawToBitmap;			///< ��������� �� ������� DrawToBitmap � ������ m_hDllInstance.		
	xyplot::LPFNCREATEGROUP			m_lpfnCreateGroup;			///< ��������� �� ������� CreateGroup � ������ m_hDllInstance.		
	xyplot::LPFNDELETEGROUP			m_lpfnDeleteGroup;			///< ��������� �� ������� DeleteGroup � ������ m_hDllInstance.		
	xyplot::LPFNBINDPROFILE			m_lpfnBindProfile;			///< ��������� �� ������� BindProfile � ������ m_hDllInstance.		
																						  						
	xyplot::LPFNGETPROFLIST		m_lpfnGetProfileList;			///< ��������� �� ������� GetProfileList � ������ m_hDllInstance.		
																						  						
	xyplot::LPFNREGRESTOREDEFAULTS	m_lpfnRegRestoreDefaults;	///< ��������� �� ������� RegRestoreDefaults
	xyplot::LPFNREGLOADSETTINGS		m_lpfnRegLoadSettings;		///< ��������� �� ������� RegLoadSettings � ������ m_hDllInstance.	
	xyplot::LPFNREGSAVESETTINGS		m_lpfnRegSaveSettings;		///< ��������� �� ������� RegSaveSettings � ������ m_hDllInstance.	
	xyplot::LPFNRUNDIALOG			m_lpfnRunDialog; 			///< ��������� �� ������� RunDialog � ������ m_hDllInstance. 		
																						  						
	xyplot::LPFNADDREGION			m_lpfnAddRegion;			///< ��������� �� ������� AddRegion � ������ m_hDllInstance.		
	xyplot::LPFNDELETEREGION		m_lpfnDeleteRegion;			///< ��������� �� ������� DeleteRegion � ������ m_hDllInstance.		
	xyplot::LPFNSETREGION			m_lpfnSetRegion;			///< ��������� �� ������� SetRegion � ������ m_hDllInstance.		
																						  						
	xyplot::LPFNADDLEVEL			m_lpfnAddLevel;				///< ��������� �� ������� AddLevel � ������ m_hDllInstance.			
	xyplot::LPFNDELETELEVEL			m_lpfnDeleteLevel;			///< ��������� �� ������� DeleteLevel � ������ m_hDllInstance.		
	xyplot::LPFNSETLEVEL			m_lpfnSetLevel;				///< ��������� �� ������� SetLevel � ������ m_hDllInstance.			
		
	/// @}	
};

///\~russian
///@brief ������� ����� ������������ 2D �������
///\~english
///@brief Base 2d plot descriptors class
class PlotHandle
{
	long p;

public:

	///\~russian
	///����������
	///\~english
	///Destructor
	virtual ~PlotHandle() {};
	
	///\~russian
	///�������� ������������
	///@param rhs ������ �� ������������ ����������
	///\~english
	///�������� ������������
	///@param rhs exist handle reference
	const PlotHandle& operator = (const PlotHandle& rhs) 
	{ 
		if (this == &rhs) 
			return *this; 
		p = rhs.p; 
		return *this;
	};

	///\~russian
	///�������� ���������� ���
	///@return true ���� ���������� ������ ( = 0) 
	///@return false ���� ���������� �� ������� 
	///\~english
	///Logical not operator
	///@return true if handle is empty ( = 0) 
	///@return false if handle not empty
	bool operator !() const 
	{ 
		return !p; 
	}
	
	///\~russian
	///�������� �� �����. ���������� � ������������ ������������ 
	///@param rhs ������ �� ������������ ����������
	///@return true ���� ����������� �� ������
	///@return false ���� ����������� ������
	///\~english
	///Operator not equal. Compare current handle with another one
	///@param rhs exist handle reference
	///@return true if descriptors not equal 
	///@return false if descriptors equal 
	bool operator !=(const PlotHandle& rhs) const 
	{ 
		return p != rhs.p; 
	}
	
	///\~russian
	///O������� ���������� � \b long. 
	///@return ���������� � ���� \b long
	///\~english
	///Typecast to \b long operator 
	///@return handle as \b long
	operator long() const 
	{ 
		return p; 
	}

	///\~russian
	///������� �������� & ���������� ����� �����������. 
	///@return ���������� ����� �����������
	///\~english
	///Unary operator &  return handle address. 
	///@return return handle address
	long* operator&() 
	{ 
		return &p; 
	}

protected:

	///\~russian
	///����������� �� ���������. �������������� ������ ���������� (= 0) 
	///\~english
	///Default constructor. Initialize empty handle (= 0) 
	PlotHandle() : p(NULL) {};

public:

	///\~russian
	///����������� � ����������.
	///@param n - ???
	///\~english
	///Parametric constructor
	///@param n - ???
	PlotHandle(long n) : p(n) {};

	///\~russian
	///����������� �����������.
	///@param rhs ������ �� ������������ ����������
	///\~english
	///Copy constructor
	///@param rhs exist handle reference
	PlotHandle(const PlotHandle& rhs) { *this = rhs; };
};

///\~russian
///@brief ����� ����������� 2D �������
///\~english
///@brief 2D plot descriptors class
class HPLOT : public PlotHandle
{
public:

	///\~russian
	///����������� �� ���������. �������������� ������ ���������� (= 0) 
	///\~english
	///Default constructor. Initialize empty handle (= 0) 
	HPLOT() {};

protected:

	///\~russian
	///������������� ������� �� ������ XYPlotManager
	///@param hwnd ���������� ���� ��� �������� ��������� ��������� 2D �������
	///@return   ���������� ���������� 2D �������
	///\~english
	///Friend function from class XYPlotManager
	///@param hwnd window handle 
	///@return  handle 2D plot object 
	///\~ 
	///@code
	///XYPlotManager& pm = XYPlotManager::Instance();
	///if ( !pm.Initialize("xyplot.dll", ::AfxGetMainWnd()->m_hWnd))
	///	return FALSE;
	///HPLOT plot;
	///CChildView* pView = ...;//�������� ���������� ���� � ������� ����� ������������ ������� 
	///try {
	///		plot = pm.CreatePlot(pView->m_hWnd)
	///	}
	/// catch (XYPlotRequestFailure& e)
	/// {
	///		return FALSE;
	/// }
	///@endcode
	friend HPLOT XYPlotManager::CreatePlot(HWND hwnd);

	///\~russian
	///����������� � ����������.
	///@param n - ???
	///\~english
	///Parametric constructor
	///@param n - ???
	explicit HPLOT(long n) : PlotHandle(n) {};
};


///\~russian
///@brief ����� ����������  ��� ����  ����������� 2D �������
///\~english
///@brief 2D plot handle class
class ALL_PLOTS_ : public HPLOT
{
public:
	///\~russian
	///����������� �� ���������.
	///\~english
	///Default constructor. 
	ALL_PLOTS_() : HPLOT(0) {};
};

#define ALL_PLOTS ALL_PLOTS_()

///\~russian
///@brief ����� ����������� ������� 
///\~english
///@brief Plot profile descriptors class
class HPROFILE : public PlotHandle
{
public:

	///\~russian
	///����������� �� ���������.
	///\~english
	///Default constructor. 
	HPROFILE() {};

protected:

	///\~russian
	///������������� ������� �� ������ XYPlotManager ��� �������� ���������� �������
	///@param[in] hPlot	���������� ���������� 2D ������� ��� �������� ��������� �������
	///@param name ��� ������� (������������ � �������)
	///@param color ���� �������
	///@param width ������� ����� ������� � ��������
	///@param lineType ��� ����� ������� 
		///\li PLS_INVISIBLE - ��������� \li PLS_SOLID - �������� \li PLS_DASH - ��������� \li PLS_DOT - �������� \li PLS_DASHDOT - ������ \li PLS_DASHDOTDOT - �����-�����-����� \li PLS_CUSTOM - �����������������
	///@param szLineTemplate ���������������� ������ �����. ������ �������� ������������ ������ � ������ ���� ������ ���������������� ��� �����. 
	///������ ������������ ����� ������������������ ������������� �������� ��� ������� ���������� ���������� � ����������� � ��������. 
	///�������� �������� ������� "5 2" ��������,  ��� 5px �������������� �������� ������ ����� ������� ���������� 2px.
	///@param visible ����� �� ������������ ����� ������� �������  
	///@param showmarks ���������� �� ����� ������ �������
	///@param xParentAxis ��� X, � ������� ������������� ������� 
	///@param yParentAxis ��� Y, � ������� ������������� ������� 
	///@return  ���������� ���������� �������
	///\~english
	///Friend function from class XYPlotManager for profile handle creation
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param name profile name (show in legend)
	///@param color profile color
	///@param width profile line width in pixels
	///@param lineType ��� ����� ������� 
	///\li PLS_INVISIBLE - ��������� \li PLS_SOLID - �������� \li PLS_DASH - ��������� \li PLS_DOT - �������� \li PLS_DASHDOT - ������ \li PLS_DASHDOTDOT - �����-�����-����� \li PLS_CUSTOM - �����������������
	///@param szLineTemplate ���������������� ������ �����. ������ �������� ������������ ������ � ������ ���� ������ ���������������� ��� �����. 
	///������ ������������ ����� ������������������ ������������� �������� ��� ������� ���������� ���������� � ����������� � ��������. 
	///�������� �������� ������� "5 2" ��������,  ��� 5px �������������� �������� ������ ����� ������� ���������� 2px.
	///@param visible ����� �� ������������ ����� ������� �������  
	///@param showmarks ���������� �� ����� ������ �������
	///@param xParentAxis ��� X, � ������� ������������� ������� 
	///@param yParentAxis ��� Y, � ������� ������������� ������� 
	///@return  ���������� ���������� �������
	///\~ 
	///@code
	///XYPlotManager& pm = XYPlotManager::Instance();
	///if ( !pm.Initialize("xyplot.dll", ::AfxGetMainWnd()->m_hWnd))
	///	return FALSE;
	///HPLOT plot;
	///CChildView* pView = ...;//�������� ���������� ���� � ������� ����� ������������ ������� 
	///try {
	///		plot = pm.CreatePlot(pView->m_hWnd)
	///	}
	/// catch (XYPlotRequestFailure& e)
	/// {
	///		return FALSE;
	/// }
	///@endcode
	friend HPROFILE XYPlotManager::CreateProfile(HPLOT& hPlot, const char* name, COLORREF color, int width, int lineType, const char* szLineTemplate, BOOL visible, BOOL showmarks, long xParentAxis, long yParentAxis);

	friend BOOL XYPlotManager::GetProfileList(HPLOT&, std::vector<HPROFILE>&);
	
	explicit HPROFILE(long n) : PlotHandle(n) {};
};

///\~russian
///@brief ����� ����������  ��� ����  �������� 2D �������
///\~english
///@brief 2D plot all profiles handle class
class ALL_PROFILES : public HPROFILE
{
public:
	ALL_PROFILES() : HPROFILE(0) {};
};

///\~russian
///@brief ����� ����������  ������� ������� 2D �������
///\~english
///@brief 2D plot invalid profiles handle class
class INVALID_PROFILE : public HPROFILE
{
public:
	INVALID_PROFILE() : HPROFILE(-1) {};
};

//#define ALL_PROFILES ALL_PROFILES_()

///\~russian
///@brief ����� ����������� ������ �������� 
///\~english
///@brief Plot profile group descriptors class
class HGROUP : public PlotHandle
{
public:
	HGROUP() {};
protected:
	friend HGROUP XYPlotManager::CreateGroup(HPLOT&);
	explicit HGROUP(long n) : PlotHandle(n) {};
};

///\~russian
///@brief ����� ����������  ������ ������ �������� 2D �������
///\~english
///@brief 2D plot invalid profiles group  handle class
class INVALID_GROUP : public HGROUP
{
public:
	INVALID_GROUP() : HGROUP(-1) {};
};


///\~russian
///@brief ����� ����������� �������
///\~english
///@brief Plot region handle class
class HPLOTREGION : public PlotHandle
{
public:
	HPLOTREGION() {};
protected:
	friend HPLOTREGION XYPlotManager::CreateRegion(HPLOT& , long , long , double , double , double , double , const char* );
	explicit HPLOTREGION(long n) : PlotHandle(n) {};
};

///\~russian
///@brief ����� ���������� ���� ��������
///\~english
///@brief Plot all regions descriptor class
class ALL_REGIONS_ : public HPLOTREGION
{
public:
	ALL_REGIONS_() : HPLOTREGION(0) {};
};

#define ALL_REGIONS ALL_REGIONS_()


///\~russian
///@brief ����� ���������� ����� ������
///\~english
///@brief Plot line level descriptors class
class HLEVEL : public PlotHandle
{
public:
	HLEVEL() {};
	
protected:

	friend HLEVEL XYPlotManager::CreateLevel(HPLOT& , long , double , const char* , COLORREF , int );
	explicit HLEVEL(long n) : PlotHandle(n) {};
};


///\~russian
///@brief ����� ������ ������� ���������� ��� ������� 
///\~english
///@brief Plot runtime error class
class XYPlotError : public std::runtime_error
{
public:
	XYPlotError(const std::string& message) : std::runtime_error(message) {};
};

///\~russian
///@brief ����� ������ ���������� ������� ��� ������� 
///\~english
///@brief Plot runtime request error class
class XYPlotRequestFailure : public XYPlotError
{
public:
	XYPlotRequestFailure(const std::string& msg) : XYPlotError(msg + " was unable to complete request. Check last error for possible reason.") {};
};

}

#endif //_XYPLOTWRAPPER_H_