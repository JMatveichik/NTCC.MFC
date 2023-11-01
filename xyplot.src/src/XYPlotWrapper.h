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
///@brief Класс-обертка для работы с  2D графикой
///@details  Класс-обертка для работы с объектами отображения 2D графики. Данный класс предоставляет инструменты создания, 
///манипуляции и уничтожение объектов 2D  графики. Класс является сингелтоном , т.е. существует только один экземпляр   
///данного класса, доступ к которому осуществляется через статическую функцию <b>Instance</b>.
///\~english
///@brief 2D plot wrapper class
///@details  Класс-обертка для работы с объектами отображения 2D графики. Данный класс предоставляет инструменты создания, 
///манипуляции и уничтожение объектов 2D  графики. Класс является сингелтоном , т.е. существует только один экземпляр   
///данного класса, доступ к которому осуществляется через статическую функцию <b>Instance</b>.

class XYPlotManager
{

public:

	///\~russian
	///@brief Функция доступа к единственному экземпляру класса
	///Класс XYPlotManager является сингелтоном т.е. существует единственный экземпляр объекта данного класса. 
	///Доступ к нему осуществляется при помощи данной функции.
	///@code
	///	XYPlotManager& pm = XYPlotManager::Instance();
	///@endcode
	///@return Ссылка на экземпляр 
	///\~english
	///@brief Функция доступа к единственному экземпляру класса
	///Класс XYPlotManager является сингелтоном т.е. существует единственный экземпляр объекта данного класса. 
	///Доступ к нему осуществляется при помощи данной функции.
	///@code
	///	XYPlotManager& pm = XYPlotManager::Instance();
	///@endcode
	///@return Ссылка на экземпляр
	static XYPlotManager& Instance();

protected:
	

public:
	
	///\~russian
	///@brief Функция получения кода последней ошибки
	///@return целочисленный код последней ошибки
	///\~english
	///Функция получения кода последней ошибки
	///@return целочисленный код последней ошибки
	int GetLastError() const { return m_nLastError; };

	///\~russian
	///@brief Расшифровывает код ошибки
	///@param code - Код ошибки 
	///@return Возвращает строковое описание ошибки
	///\~english
	///Расшифровывает код ошибки
	///@param code - Код ошибки 
	///@return Возвращает строковое описание ошибки
	std::string TranslateError(int code) const;
	

	///\~russian 
	/// @defgroup group1 Инциализации и освобождение
	/// @brief Функции инциализации и освобождения 2D графики
	/// @details Данный набор функций позволяет инициализировать класс-обертку для манипуляции всеми экземплярами 2D графики, а также 
	///	создавать новые  и уничтожать существующие экземпляры.
	///  @{	

	///\~english 
	/// @defgroup group1 2D plot initialization and destruction function 
	/// This group functions initialize wrapper class  for all 2d plot objects manipulation and for create new and destruct
	///	existing plots.
	///  @{	

	///\~russian
	///@brief Данная функция  инициализирует класса-обертку. 
	///@param pszDllPath - путь к динамически подключаемой библеотеке xyplot.dll
	///@param hwndParent - опциональный параметр, дескриптор окна которому будут присылаться сообщения
	///@return true - в случае если инициализация прошла успешно;<br/>false - в случае если инициализация не удалась.
	BOOL Initialize(const char* pszDllPath, HWND hwndParent = NULL);

	///\~russian
	///@brief Функция создания экземпляра 2D графики. В случае если неудалось создать экземпляр функция генерирует исключение XYPlotRequestFailure
	///@param hwndParent - дескриптор окна в котором будет отображаться графика
	///@return Описатель экземпляра 2D графики
	///@code
	///XYPlotManager& pm = XYPlotManager::Instance();
	///if ( !pm.Initialize("xyplot.dll", ::AfxGetMainWnd()->m_hWnd))
	///	return FALSE;
	///HPLOT plot;
	///CChildView* pView = ...;//получить дескриптор окна в котором будет отображаться графика 
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
	///@brief Функция удаления экземпляра 2D графики. 
	///@details Функция удаляет экземпляра 2D графики. Данную функцию необходимо вызвать перед уничтожением окна, указанного при создании экземпляра в функции CreatePlot 
	///@return true - в случае если освобождение экземпляра графики прошло успешно;
	///@return false - в случае если не удалось освободить экземпляр.	
	BOOL DestroyPlot(HPLOT& hPlot);
	///@}

	///\~russian
	///@brief Функция копирует текущее содержимое экзкемпляра 2D графики в буфер обмена в виде изображения.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param width - ширина результирующего изображения в пикселях
	///@param height - высота результирующего изображения  в пикселях
	///@param dpi - разрешение изображения
	///@return true - в случае если копирование прошло успешно;
	///@return false - в случае если не удалось скопировать экземпляр в буфер обмена.
	BOOL CopyToClipboard(HPLOT& hPlot, unsigned width, unsigned height, unsigned dpi);

	///\~russian
	///@brief Функция копирует текущее содержимое экзкемпляра 2D в буфер битовую карту.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param hBitmap - дескриптор битовой карты
	///@param bDrawWindowBG - флаг отрисовки фона окна
	///@param bDrawClientBG - флаг отрисовки фона клиентской части
	///@return true - в случае если копирование прошло успешно;
	///@return false - в случае если не удалось скопировать экземпляр в буфер обмена.	
	BOOL DrawToBitmap(HPLOT& hPlot, HBITMAP& hBitmap, bool bDrawWindowBG, bool bDrawClientBG);
	

	///\~russian
	/// @brief Функция вызывает диалог настроек экземпляра 2D графики .
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@return true - в случае если запуск диалога прошел успешно;
	///@return false - в случае если не удалось запустить диалог настроек.
	BOOL RunDialog(HPLOT& hPlot);


	///\~russian 
	/// @defgroup grProperties Функции сохранения и загрузки свойств
	/// @brief Хранение параметров 
	/// @details Данный набор функций позволяет сохранять  и загружать свойства 2D графики из файла или реестра.
	///  @{	

	///\~russian	
	///@brief Функция сбрасывает настройки  экземпляра 2D графики в настройки по умолчанию.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@return true - в случае если удалось загрузить настройки;<br/>false - в случае если не удалось загрузить настройки.
	BOOL RegRestoreDefaults(HPLOT& hPlot);

	///\~russian
	///@brief Функция загружает настройки  экземпляра 2D графики из реестра.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param pszKey - ключ реестра
	///@param pszValue - переменная ветки реестра
	///@return true - в случае если удалось загрузить настройки;
	///@return false - в случае если не удалось загрузить настройки.
	BOOL RegLoadSettings(HPLOT& hPlot, const char* pszKey, const char* pszValue);

	///\~russian
	///@brief Функция сохраняет настройки  экземпляра 2D графики в реестре.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param pszKey - ключ реестра
	///@param pszValue - переменная ветки реестра
	///@return true - в случае если удалось сохранить настройки;
	///@return false - в случае если не удалось сохранить настройки.
	BOOL RegSaveSettings(HPLOT& hPlot, const char* pszKey, const char* pszValue);

	///\~russian
	///@brief Функция сохраняет настройки  экземпляра 2D графики в файле.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param pszFileName - путь к файлу
	///@return true - в случае если удалось сохранить настройки;
	///@return false - в случае если не удалось сохранить настройки.
	BOOL SaveProperties(HPLOT& hPlot, const char* pszFileName);
	
	///\~russian
	///@brief Функция загружает настройки  экземпляра 2D графики из файла.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param pszFileName - путь к файлу
	///@return true - в случае если удалось загрузить настройки;
	///@return false - в случае если не удалось загрузить настройки.
	BOOL LoadProperties(HPLOT& hPlot, const char* pszFileName);

#ifdef _AFX
	///\~russian
	///@brief Функция загружает или сохраняет настройки  экземпляра 2D графики в архив MFC.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param ar - экземпляр архива	 MFC
	void Serialize(HPLOT hPlot, CArchive& ar);
#endif
	///@} 

	///\~russian 
	/// @defgroup grSelect Функции для работы с выделением
	/// @brief Данный набор функций обеспечивает 
	///  @{	

	///\~russian
	///@brief Функция устанавливает выделение указанного профиля.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param[in] hProfile  - дескриптор профиля
	///@param nIndex - индекс в массиве данных
	///@return true - в случае если выделение профиля  прошло успешно;
	///@return false - в случае если не удалось выделить профиль.
	BOOL Select(HPLOT& hPlot, HPROFILE& hProfile, unsigned nIndex);

	///\~russian
	///@brief Функция устанавливает выделение указанной группы профилей.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param hGroup   - дескриптор группы профилей
	///@param nIndex - индекс в массиве данных
	///@return true - в случае если выделение группы профилей  прошло успешно;
	///@return false - в случае если не удалось выделить группу профилей.
	BOOL Select(HPLOT& hPlot, HGROUP& hGroup, unsigned nIndex);

	///\~russian
	///@brief Функция задает режим выделения для профилей и групп профилей
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param mode - режим выделения один из 
	///SM_NONE = -1	- нет выделения
	///SM_FLASH = 0 - выделение мерцанием
	///SM_VTRACE = 1 - вертикальная трассировка
	///SM_HTRACE = 2 - горизонтальная трассировка
	///SM_TRACE = 3 - полная трассировка
	///@return true - в случае если удалось установить режим выделения;
	///@return false - в случае если не удалось установить выделение.
	BOOL SetSelectionMode(HPLOT& hPlot, int mode);

	///\~russian
	///@brief Функция возвращает текущий режим выделения для профилей и групп профилей
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@return	SM_NONE = -1	- нет выделения
	///@return	SM_FLASH = 0 - выделение мерцанием
	///@return	SM_VTRACE = 1 - вертикальная трассировка
	///@return	SM_HTRACE = 2 - горизонтальная трассировка
	///@return	SM_TRACE = 3 - полная трассировка
	long GetSelectionMode(HPLOT& hPlot) const;

	///\~russian
	///@brief Функция задает частоту мерцания профиля либо группы профилей в режиме  выделения SM_FLASH
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param speed - режим выделения один из \li FLS_NO_FLASH = 0 \li FLS_VERY_SLOW = 1 \li FLS_SLOW = 2 \li FLS_NORMAL = 3 \li FLS_FAST = 4 \li FLS_VERY_FAST = 5 \li FLS_EXTREME = 6
	///@return true - в случае если удалось установить частоту мерцания;
	///@return false - в случае если не удалось установить частоту мерцания.	
	BOOL SetFlashSpeed(HPLOT& hPlot, int speed);
	
	///@}


	
	///\~russian
	///@brief Функция разрешает либо запрещает перерисовку экземпляра 2D графики
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param bRedraw - флаг разрешения отрисовки если bRedraw = TRUE окно обновляется после каждой операции. 
	///В случае если bRedraw = FALSE окно не обновляется до того момента вызова функции с параметром bRedraw = TRUE
	///@code
	///XYPlotManager& pm = XYPlotManager::Instance();
	///double* pdX = NULL;
	///double* pdY = NULL;
	///HPLOT hPlot = ...;//Получить дескриптор 2D графики
	///pm.SetRedraw(hPlot, FALSE); 
	///for (int i = 0; i < nProfileCount; i++)
	///{
	///		//получить данные для профиля
	///     ...
	///    HPROFILE hProf = ...;//получить дескриптор профиля
	///    //установить данные профиля
	///    pm.SetData(hPlot, hProf, pdX, pdY, nSize);
	///    ...//прочие операции
	///}
	///pm.SetRedraw(hPlot, TRUE);
	///@endcode
	BOOL SetRedraw(HPLOT& hPlot, BOOL bRedraw = TRUE);


	///\~russian 
	/// @defgroup grElements Функции для работы с элементами графика
	/// @brief Данный набор функций обеспечивает работу с получением и заданием свойств элементов  графика.
	///  @{	

	///\~english
	/// @defgroup grElements Plot elements functions
	///  This group functions work with properties of 2D plot elements
	///  @{	

	///\~russian
	///@brief Функция задает диапазон значений оси экземпляра 2D графики. 
	///@details Данная функция работает только в случае если для оси не задан режим автоматического масштабирования.
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param axis - идентификатор оси 
	///\li PI_AXIS_LEFT = 6
	///\li PI_AXIS_RIGHT = 7
	///\li PI_AXIS_TOP = 8
	///\li PI_AXIS_BOTTOM = 9
	///@param dMin - начальное значение диапазона
	///@param dMax - конечное значение диапазона
	BOOL SetAxisRange(HPLOT& hPlot, long axis, double dMin, double dMax);

	///\~russian
	///@brief Функция получения диапазона значений оси экземпляра 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param axis - идентификатор оси 
	///\li PI_AXIS_LEFT = 6
	///\li PI_AXIS_RIGHT = 7
	///\li PI_AXIS_TOP = 8
	///\li PI_AXIS_BOTTOM = 9
	///@param[out] dMin - начальное значение диапазона
	///@param[out] dMax - конечное значение диапазона
	BOOL GetAxisRange(HPLOT& hPlot, long axis, double& dMin, double& dMax) const;

	///\~russian
	///@brief Функция получения строки элемента 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@return строку связанную с подэлементом 2D графики. 
	std::string GetString(HPLOT& hPlot, long item, long subitem) const;

	///\~russian
	///@brief Функция задания строки элемента 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@param pszString - строка для подэлемента 2D графики. 
	///@return true - в случае если удалось установить строку элемента;<br/>false - в случае если не удалось установить строку элемента.	
	BOOL SetString(HPLOT& hPlot, long item, long subitem, const char* pszString);

	///\~russian
	///Функция получения целочисленного значения связанного с элементом 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@return целочисленное значения связанное с подэлементом 2D графики. 
	long GetLong(HPLOT& hPlot, long item, long subitem) const;

	///\~russian
	///@brief Функция задания целочисленного значения связанного с элементом 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@param value - целочисленное значения связанное с подэлементом 2D графики. 
	///@return true - в случае если удалось установить цедлочисленное значение элемента;<br/>false - в случае если не удалось установить значение элемента.
	BOOL SetLong(HPLOT& hPlot, long item, long subitem, long value);

	///\~russian
	///Функция задания значения с плавающей точкой связанного с элементом 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@return значения с плавающей точкой связанное с подэлементом 2D графики. 
	double GetDouble(HPLOT& hPlot, long item, long subitem) const;

	///\~russian
	///@brief Функция задания значения с плавающей точкой связанного с элементом 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@param value - с плавающей точкой значения связанное с подэлементом 2D графики. 
	///@return true - в случае если удалось установить значение с плавающей точкой элемента;<br/>false - в случае если не удалось установить значение с плавающей точкой элемента.
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
	///@brief Функция включения/отключения элемента 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@param enabled - флаг включения подэлемента 2D графики 
	///@return true - в случае если удалось переключить состояние элемента;
	///@return false - в случае если не удалось установить значение с плавающей точкой элемента.
	BOOL EnableItem(HPLOT& hPlot, long item, long subitem, BOOL enabled);

	///\~english
	///@brief Function return enables/disables state 2D plot element. 
	///@param[in] hPlot - 2D plot handle
	///@param item - plot item identificator
	///@param subitem - plot subitem identificator
	///@return true - plot item enabled now;
	///@return false - plot item not enabled now.
	///\~russian
	///@brief Функция возвращает флаг состояния элемента 2D графики (включен/выключен). 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 	
	///@return true - в случае если элемент экземпляра графики включен;
	///@return false - в случае если элемент экземпляра графики выключен.
	BOOL IsEnabled(HPLOT& hPlot, long item, long subitem) const;



	///\~english
	///@brief Function return color for 2D plot element. 
	///@param[in] hPlot - 2D plot handle
	///@param item - plot item identificator
	///@param subitem - plot subitem identificator
	///@return - plot item current color as \b COLORREF value;	
	///\~russian
	///@brief Функция включения/отключения элемента 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@return текущий цвет элемента 2D графики.
	COLORREF GetColor(HPLOT& hPlot, long item, long subitem) const;
		
	///\~russian
	///@brief Функция задания цвета элемента 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@param color	- новый цвет подэлемента 2D графики 
	///@return true - в случае если удалось задать цвет элемента;
	///@return false - в случае если не удалось задать цвет элемента.
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
	///@brief Функция задания шрифта  элемента 2D графики. 
	///@param[in] hPlot - дескриптор экземпляра 2D графики 
	///@param item - идентификатор элемента 2D графики 
	///@param subitem - идентификатор подэлемента 2D графики 
	///@param font	- указатель на новый шрифт подэлемента 2D графики 
	///@return true - в случае если удалось задать шрифт элемента;
	///@return false - в случае если не удалось задать шрифт элемента.
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
	/// @defgroup grOutput Функции для работы с профилями и группами.
	/// @brief  Данный набор функций обеспечивает создание, удаление профилей и групп профилей.
	///  @{	

	///\~english
	/// @defgroup grOutput Plot profiles/groups manipulation functions.
	///  This group functions work with plot profiles/groups.
	///  @{	

	///\~russian
	///@brief Создания экземпляра профиля
	///@param[in] hPlot	дескриптор экземпляра 2D графика для которого создается профиль
	///@param name имя профиля (отображается в легенде)
	///@param color цвет профиля
	///@param width толщина линии профиля в пикселях
	///@param lineType тип линии профиля 
	///\li PLS_INVISIBLE - невидимая \li PLS_SOLID - сплошная \li PLS_DASH - штриховая \li PLS_DOT - точечная \li PLS_DASHDOT - осевая \li PLS_DASHDOTDOT - штрих-точка-точка \li PLS_CUSTOM - пользовательсякая
	///@param szLineTemplate пользовательский шаблон линии. Данный аргумент используется только в случае если выбран пользовательский тип линии. 
	///Шаблон представляет собой последовательность целочисленных значений для задания интервалов заполнения и промежутков в пикселях. 
	///Например значение шаблона "5 2" означает,  что 5px отрисовываются сплошной линией затем следует промежуток 2px.
	///@param visible будет ли отображаться линия данного профиля  
	///@param showmarks отображать ли метки данных профиля
	///@param xParentAxis ось X, к которой прикрепляется профиль 
	///@param yParentAxis ось Y, к которой прикрепляется профиль 
	///@return  дескриптор экземпляра профиля
	///\~english
	///@brief Profile handle creation
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param name profile name (show in legend)
	///@param color profile color
	///@param width profile line width in pixels
	///@param lineType тип линии профиля 
	///\li PLS_INVISIBLE - невидимая \li PLS_SOLID - сплошная \li PLS_DASH - штриховая \li PLS_DOT - точечная \li PLS_DASHDOT - осевая \li PLS_DASHDOTDOT - штрих-точка-точка \li PLS_CUSTOM - пользовательсякая
	///@param szLineTemplate пользовательский шаблон линии. Данный аргумент используется только в случае если выбран пользовательский тип линии. 
	///Шаблон представляет собой последовательность целочисленных значений для задания интервалов заполнения и промежутков в пикселях. 
	///Например значение шаблона "5 2" означает,  что 5px отрисовываются сплошной линией затем следует промежуток 2px.
	///@param visible будет ли отображаться линия данного профиля  
	///@param showmarks отображать ли метки данных профиля
	///@param xParentAxis ось X, к которой прикрепляется профиль 
	///@param yParentAxis ось Y, к которой прикрепляется профиль 
	///@return  дескриптор экземпляра профиля		
	///\~	
	///\include ex_001.cpp
	HPROFILE CreateProfile(HPLOT& hPlot, const char* name, COLORREF color, int width, int lineType = xyplot::PLS_SOLID, const char* szLineTemplate = 0, BOOL visible = TRUE, BOOL showmarks = FALSE, const long xParentAxis = xyplot::PI_AXIS_BOTTOM, const long yParentAxis = xyplot::PI_AXIS_LEFT);


	///\~russian
	///@brief Удаление экземпляра профиля
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий профиль
	///@param[in] hProfile дескриптор профиля
	///@return true если профиль удален
	///@return false если профиль не удален.
	///\~english
	///@brief Delete profile
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfile profile handle
	///@return true if profile was delete
	///@return false if profile was not delete
	BOOL DeleteProfile(HPLOT& hPlot, HPROFILE& hProfile);


	///\~russian
	///@brief Создание группы профилей
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий группу профилей
	///@return дескриптор группы	
	///\~english
	///Create profiles group
	///@param[in] hPlot	parent 2D plot handle for profile
	///@return profiles group handle
	///\~
	///@sa CreateProfile()
	///@sa BindProfileToGroup()	
	HGROUP CreateGroup(HPLOT& hPlot);
	
	///\~russian
	///@brief Удаление группы профилей
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий группу профилей
	///@param hGroup дескриптор группы 
	///@return true если группа профилей удалена
	///@return false если группа  профилей не удалена
	///\~english
	///@brief Delete profiles group
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param hGroup profile handle
	///@return true if profiles group was delete
	///@return false if profiles group was not delete
	BOOL DeleteGroup(HPLOT& hPlot, HGROUP& hGroup);

	///\~russian
	///@brief Добавление профиля к группе
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий группу профилей
	///@param[in] hProfile дескриптор профиля
	///@param hGroup дескриптор группы 
	///@return true если группа профилей удалена
	///@return false если группа  профилей не удалена
	///\~english
	///@brief Add profile group
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfile profile handle
	///@param hGroup profile handle
	///@return true if profile was added to group
	///@return false if profile was not added to group
	BOOL BindProfileToGroup(HPLOT& hPlot, HGROUP& hGroup, HPROFILE& hProfile);

	///\~russian
	///Получение списка профилей экземпляра 2D графика
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий группу профилей
	///@param[in] hProfiles контейнер содержащий список всех профилей 
	///@return true если удалось получить список профилей 
	///@return false если не удалось получить список профилей
	///\~english
	///@brief Get list of all profiles for 2D plot 
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfiles profile handle
	///@return true if profile list was added to group
	///@return false if profile list was not added to group
	BOOL GetProfileList(HPLOT& hPlot, std::vector<HPROFILE>& hProfiles);
	///@}


	///\~russian 
	/// @defgroup grOutputData Функции для работы с данными.
	/// @brief Данный набор функций обеспечивает задание физических координат профилей.
	///  @{	

	///\~english
	/// @defgroup grOutputData Plot profiles/groups data manipulation functions.
	///  This group functions work with plot profiles/groups data.
	///  @{	
	
	///\~russian
	///@brief Установить данные для профиля
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий группу профилей
	///@param[in] hProfile дескриптор профиля для которого задаются данные
	///@param px указатель на массив данных, содержащий координаты профиля по оси X 
	///@param py указатель на массив данных, содержащий координаты профиля по оси Y 
	///@param size размер масивов данных px и py
	///@return true если удалось задать данные для профиля
	///@return false если не удалось задать данные для профиля
	///@remark Размеры массивов px и py должны быть одинаковыми.
	///\~english
	///@brief Установить данные для профиля
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfile profile handle
	///@param px указатель на массив данных, содержащий координаты профиля по оси X 
	///@param py указатель на массив данных, содержащий координаты профиля по оси Y 
	///@param size размер масивов данных px и py
	///@return true если удалось задать данные для профиля
	///@return false если не удалось задать данные для профиля
	///@remark Размеры массивов px и py должны быть одинаковыми.
	BOOL SetData(HPLOT& hPlot, HPROFILE& hProfile, const double *px, const double *py, unsigned size);

	///\~russian
	///@brief Копирует данные профиля в массивы
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий группу профилей
	///@param[in] hProfile дескриптор профиля из которого копируются данные
	///@param[out] px указатель на массив данных, содержащий координаты профиля по оси X 
	///@param[out] py указатель на массив данных, содержащий координаты профиля по оси Y 
	///@param[out] size размер масивов данных px и py
	///@return true если удалось получить  данные для профиля
	///@return false если не удалось получить  данные для профиля
	///@remark Для определения необходимых размеров массивов необходимо вызвать функцию с параметрами px = NULL и py = NULL.  
	///\~english
	///@brief Установить данные для профиля
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfile profile handle for data copy 
	///@param[out] px указатель на массив данных, содержащий координаты профиля по оси X 
	///@param[out] py указатель на массив данных, содержащий координаты профиля по оси Y 
	///@param[out] size размер масивов данных px и py
	///@return true если удалось задать данные для профиля
	///@return false если не удалось задать данные для профиля
	///@remark Размеры массивов px и py должны быть одинаковыми.
	BOOL GetData(HPLOT& hPlot, HPROFILE& hProfile, double *px, double *py, unsigned* size);

	///\~russian
	///@brief Дописать данные для профиля
	///@details Функция дописывает данные содержащиеся в массивах px и py для профиля.
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий профиль
	///@param[in] hProfile дескриптор профиля 
	///@param px указатель на массив данных, содержащий координаты профиля по оси X 
	///@param py указатель на массив данных, содержащий координаты профиля по оси Y 
	///@param size количество точек дописываемых из массивов
	///@return true если удалось дописать данные для профиля
	///@return false если не удалось дописать данные для профиля
	///@remark Размеры массивов px и py должны быть одинаковыми.
	///\~english
	///@brief Установить данные для профиля
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param[in] hProfiles profile handle
	///@param px указатель на массив данных, содержащий координаты профиля по оси X 
	///@param py указатель на массив данных, содержащий координаты профиля по оси Y 
	///@param size размер масивов данных px и py
	///@return true если удалось задать данные для профиля
	///@return false если не удалось задать данные для профиля
	///@remark Размеры массивов px и py должны быть одинаковыми.
	BOOL AppendData(HPLOT& hPlot, HPROFILE& hProfile, const double *px, const double *py, unsigned size);
	///@}

	///\~russian 
	/// @defgroup grProfEx Cвойства профиля
	/// @brief Функции для работы со свойствами профиля.
	///  @{	
	
	///\~russian
	///@brief Получить текущий выделенный профиль
	///@param[in] hPlot	parent 2D plot handle for profile
	///@return дескриптор выделенного профиля профиля 
	///@return INVALID_PROFILE если в данный момент ни один профиль не выбран функция возвращает .
	HPROFILE GetSelectedProfile(HPLOT& hPlot) const;

	///\~russian
	///@brief Получить цвет профиля
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий профиль
	///@param[in] hProfile дескриптор профиля 
	COLORREF GetColor(HPLOT& hPlot, HPROFILE& hProfile) const;

	///\~russian
	///@brief Задать цвет профиля
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий профиль
	///@param[in] hProfile дескриптор профиля 
	///@param[in] color дескриптор профиля 
	///@return true если удалось задать цвет профиля
	///@return false если не удалось задать цвет профиля
	BOOL SetColor(HPLOT& hPlot, HPROFILE& hProfile, COLORREF color);

	///@}

	
	///\~russian 
	/// @defgroup grLvlEx Линии уровней.
	/// @brief Функции для работы линиями уровней.
	/// @details Создание, удаление и изменение положения экземпляра линии уровня
	///  @{

	///\~russian
	///@brief Создания экземпляра линии уровня
	///@param[in] hPlot	дескриптор экземпляра 2D графика для которого создается профиль
	///@param axis ось, для которой создается линия уровня
	///@param val	значение уровня в физических единицах 
	///@param pszTitle подпись линии уровня
	///@param clr	цвет линии 
	///@param width толщина линии  в пикселях
	///@return  дескриптор экземпляра линии уровня	
	///\~english	
	///@brief Создания экземпляра линии уровня
	///@param[in] hPlot	дескриптор экземпляра 2D графика для которого создается профиль
	///@param axis ось, для которой создается линия уровня
	///@param val	значение уровня в физических единицах 
	///@param pszTitle подпись линии уровня
	///@param clr	цвет линии 
	///@param width толщина линии  в пикселях
	///@return  дескриптор экземпляра линии уровня
	HLEVEL CreateLevel(HPLOT& hPlot, long axis, double val, const char* pszTitle, COLORREF clr, int width);

	///\~russian
	///@brief Удаление линии уровня
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий группу профилей
	///@param hLevel дескриптор линии уровня
	///@return true если линия уровня удалена
	///@return false если линия уровня не удалена
	///\~english
	///@brief Delete level line
	///@param[in] hPlot	parent 2D plot handle for level line
	///@param hLevel level line handle
	///@return true if level line was delete
	///@return false if level line was not delete
	BOOL DeleteLevel(HPLOT& hPlot, HLEVEL& hLevel);

	///\~russian
	///@brief Задание нового значения линии уровня
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий группу профилей
	///@param hLevel дескриптор линии уровня
	///@param value новое значение уровня
	///@return true если новое значение линия уровня задано
	///@return false если новое значение линия уровня не задано
	///\~english
	///@brief Set new level position
	///@param[in] hPlot	parent 2D plot handle for level line
	///@param hLevel level line handle
	///@param value new level line position 
	///@return true if new position for level line was set
	///@return false if new position for level line not set
	BOOL SetLevel(HPLOT& hPlot, HLEVEL& hLevel, double value);
	///@}

	/// @defgroup grRgnEx Регионы.
	/// @brief Функции для работы регионами.
	///  @{	

	///\~russian
	///@brief Создания экземпляра региона
	///@details Создания прямоугольной области графика ограниченного линиями с координатами [xFrom, yFrom] и [xTo, yTo].
	///@param[in] hPlot	дескриптор экземпляра 2D графика для которого создается регион
	///@param axisX ось X, для которой создается регион
	///@param axisY ось Y, для которой создается регион
	///@param xFrom начальное значение региона физических единицах по оси X
	///@param xTo конечное значение региона физических единицах по оси X
	///@param yFrom начальное значение региона физических единицах по оси Y
	///@param yTo конечное значение региона физических единицах по оси Y
	///@param pszTitle подпись региона
	///@return  дескриптор экземпляра региона
	///\~english	
	///@brief Создания экземпляра линии уровня
	///@param[in] hPlot	дескриптор экземпляра 2D графика для которого создается профиль
	///@param axis ось, для которой создается линия уровня
	///@param val	значение уровня в физических единицах 
	///@param pszTitle подпись линии уровня
	///@param clr	цвет линии 
	///@param width толщина линии  в пикселях
	///@return  дескриптор экземпляра линии уровня
	HPLOTREGION CreateRegion(HPLOT& hPlot, long axisX, long axisY, double xFrom, double xTo, double yFrom, double yTo, const char* pszTitle );

	///\~russian
	///@brief Удаление региона
	///@param[in] hPlot	дескриптор экземпляра 2D графика содержащий регион
	///@param hRegion дескриптор региона
	///@return true если регион удален
	///@return false если регион не удален
	///\~english
	///@brief Delete level line
	///@param[in] hPlot	parent 2D plot handle for level line
	///@param hLevel level line handle
	///@return true if level line was delete
	///@return false if level line was not delete
	BOOL DeleteRegion(HPLOT& hPlot, HPLOTREGION& hRegion);

	///\~russian
	///@brief Задание новых границ региона
	///@details Границы региона прямоугольной области графика перемещаются в новое положение xFrom, yFrom, xTo, yTo.
	///@param[in] hPlot	дескриптор экземпляра 2D графика для региона
	///@param hRegion дескриптор региона
	///@param xFrom новое начальное значение региона в физических единицах по оси X
	///@param xTo новое конечное значение региона в физических единицах по оси X
	///@param yFrom новое начальное значение региона в физических единицах по оси Y
	///@param yTo новое конечное значение региона в физических единицах по оси Y
	///@return true если удалось поменять значение границ региона
	///@return false если не удалось поменять значение границ региона
	///\~english	
	///@brief Создания экземпляра линии уровня
	///@param[in] hPlot	дескриптор экземпляра 2D графика для которого создается профиль
	///@param hRegion дескриптор региона
	///@param axis ось, для которой создается линия уровня
	///@param val	значение уровня в физических единицах 
	///@param pszTitle подпись линии уровня
	///@param clr	цвет линии 
	///@param width толщина линии  в пикселях
	///@return  дескриптор экземпляра линии уровня
	BOOL SetRegion(HPLOT& hPlot, HPLOTREGION& hRegion, double xFrom, double xTo, double yFrom,double yTo);
	///@}

	///Деструктор класса обертки
	~XYPlotManager() {}

protected:


	/// @defgroup grConstruct Конструкторы класса.
	///  @{	

	///@brief Конструктор по усмолчанию
	///@details Конструктор по усмолчанию вынесен в защищенную секцию. Нет возможности напрямую создавать экземпляры данного класса.
	///@sa Instance
	XYPlotManager() : m_hDllInstance(NULL), m_nLastError(xyplot::PE_NOERROR) {};

	///@brief Конструктор копирования
	///@details Конструктор копирования, также как конструктор по усмолчанию вынесен в защищенную секцию. Нет возможности напрямую создавать экземпляры данного класса использую существующий экземпляр.
	///@sa Instance()
	///@sa XYPlotManager()
	XYPlotManager(const XYPlotManager& rhs);

	///@}


	HMODULE m_hDllInstance;		///< Дескриптор модуля 
		
	mutable int m_nLastError;	///< Код последней ошибки


	/// @defgroup grFuncPointers Указатели на функции.
	///  @{	
	xyplot::LPFNINITIALIZE			m_lpfnInitialize;			///< Указатель на функцию Initialize в модуле m_hDllInstance.
	xyplot::LPFNFINALIZE			m_lpfnFinalize;				///< Указатель на функцию Finalize  в модуле m_hDllInstance.
	xyplot::LPFNADDPROFILE			m_lpfnAddProfile;			///< Указатель на функцию AddProfile в модуле m_hDllInstance.		 
	xyplot::LPFNDELETEPROFILE		m_lpfnDeleteProfile;		///< Указатель на функцию DeleteProfile в модуле m_hDllInstance.	
	xyplot::LPFNSETDATA				m_lpfnSetData;				///< Указатель на функцию SetData в модуле m_hDllInstance.			
	xyplot::LPFNGETDATA				m_lpfnGetData;				///< Указатель на функцию GetData в модуле m_hDllInstance.			
	xyplot::LPFNAPPENDDATA			m_lpfnAppendData;			///< Указатель на функцию AppendData в модуле m_hDllInstance.		
	xyplot::LPFNSETSTRING			m_lpfnSetString;			///< Указатель на функцию SetString в модуле m_hDllInstance.		
	xyplot::LPFNGETSTRING			m_lpfnGetString;			///< Указатель на функцию GetString в модуле m_hDllInstance.		
	xyplot::LPFNSETCOLOR			m_lpfnSetColor;				///< Указатель на функцию SetColor в модуле m_hDllInstance.			
	xyplot::LPFNGETCOLOR			m_lpfnGetColor;				///< Указатель на функцию GetColor в модуле m_hDllInstance.			
	xyplot::LPFNSETFONT				m_lpfnSetFont;				///< Указатель на функцию SetFont в модуле m_hDllInstance.			
	xyplot::LPFNSETLONG				m_lpfnSetLong;				///< Указатель на функцию SetLong в модуле m_hDllInstance.			
	xyplot::LPFNGETLONG				m_lpfnGetLong;				///< Указатель на функцию GetLong в модуле m_hDllInstance.			
	xyplot::LPFNSETDOUBLE			m_lpfnSetDouble;			///< Указатель на функцию SetDouble в модуле m_hDllInstance.		
	xyplot::LPFNGETDOUBLE			m_lpfnGetDouble;			///< Указатель на функцию GetDouble в модуле m_hDllInstance.		
	xyplot::LPFNISENABLED			m_lpfnIsEnabled;			///< Указатель на функцию IsEnabled в модуле m_hDllInstance.		
	xyplot::LPFNENABLEITEM			m_lpfnEnableItem;			///< Указатель на функцию EnableItem в модуле m_hDllInstance.		
	xyplot::LPFNSAVEPROPERTIES		m_lpfnSaveProperties;		///< Указатель на функцию SaveProperties в модуле m_hDllInstance.	
	xyplot::LPFNLOADPROPERTIES		m_lpfnLoadProperties;		///< Указатель на функцию LoadProperties в модуле m_hDllInstance.	
	xyplot::LPFNSELECT				m_lpfnSelect;				///< Указатель на функцию Select в модуле m_hDllInstance.			
	xyplot::LPFNGETSELECTION		m_lpfnGetSelection;			///< Указатель на функцию GetSelection в модуле m_hDllInstance.		
	xyplot::LPFNCOPYTOCLIPBOARD		m_lpfnCopyToClipboard;		///< Указатель на функцию CopyToClipboard в модуле m_hDllInstance.	
	xyplot::LPFNDRAWTOBITMAP		m_lpfnDrawToBitmap;			///< Указатель на функцию DrawToBitmap в модуле m_hDllInstance.		
	xyplot::LPFNCREATEGROUP			m_lpfnCreateGroup;			///< Указатель на функцию CreateGroup в модуле m_hDllInstance.		
	xyplot::LPFNDELETEGROUP			m_lpfnDeleteGroup;			///< Указатель на функцию DeleteGroup в модуле m_hDllInstance.		
	xyplot::LPFNBINDPROFILE			m_lpfnBindProfile;			///< Указатель на функцию BindProfile в модуле m_hDllInstance.		
																						  						
	xyplot::LPFNGETPROFLIST		m_lpfnGetProfileList;			///< Указатель на функцию GetProfileList в модуле m_hDllInstance.		
																						  						
	xyplot::LPFNREGRESTOREDEFAULTS	m_lpfnRegRestoreDefaults;	///< Указатель на функцию RegRestoreDefaults
	xyplot::LPFNREGLOADSETTINGS		m_lpfnRegLoadSettings;		///< Указатель на функцию RegLoadSettings в модуле m_hDllInstance.	
	xyplot::LPFNREGSAVESETTINGS		m_lpfnRegSaveSettings;		///< Указатель на функцию RegSaveSettings в модуле m_hDllInstance.	
	xyplot::LPFNRUNDIALOG			m_lpfnRunDialog; 			///< Указатель на функцию RunDialog в модуле m_hDllInstance. 		
																						  						
	xyplot::LPFNADDREGION			m_lpfnAddRegion;			///< Указатель на функцию AddRegion в модуле m_hDllInstance.		
	xyplot::LPFNDELETEREGION		m_lpfnDeleteRegion;			///< Указатель на функцию DeleteRegion в модуле m_hDllInstance.		
	xyplot::LPFNSETREGION			m_lpfnSetRegion;			///< Указатель на функцию SetRegion в модуле m_hDllInstance.		
																						  						
	xyplot::LPFNADDLEVEL			m_lpfnAddLevel;				///< Указатель на функцию AddLevel в модуле m_hDllInstance.			
	xyplot::LPFNDELETELEVEL			m_lpfnDeleteLevel;			///< Указатель на функцию DeleteLevel в модуле m_hDllInstance.		
	xyplot::LPFNSETLEVEL			m_lpfnSetLevel;				///< Указатель на функцию SetLevel в модуле m_hDllInstance.			
		
	/// @}	
};

///\~russian
///@brief Базовый класс дескрипторов 2D графики
///\~english
///@brief Base 2d plot descriptors class
class PlotHandle
{
	long p;

public:

	///\~russian
	///Деструктор
	///\~english
	///Destructor
	virtual ~PlotHandle() {};
	
	///\~russian
	///Оператор присваивания
	///@param rhs ссылка на существующий дескриптор
	///\~english
	///Оператор присваивания
	///@param rhs exist handle reference
	const PlotHandle& operator = (const PlotHandle& rhs) 
	{ 
		if (this == &rhs) 
			return *this; 
		p = rhs.p; 
		return *this;
	};

	///\~russian
	///Оператор логическое нет
	///@return true если дескриптор пустой ( = 0) 
	///@return false если дескриптор не нулевой 
	///\~english
	///Logical not operator
	///@return true if handle is empty ( = 0) 
	///@return false if handle not empty
	bool operator !() const 
	{ 
		return !p; 
	}
	
	///\~russian
	///Оператор не равно. Сравнивает с существующим дескриптором 
	///@param rhs ссылка на существующий дескриптор
	///@return true если дескрипторы не равены
	///@return false если дескрипторы равены
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
	///Oператор приведения к \b long. 
	///@return дескриптор в виде \b long
	///\~english
	///Typecast to \b long operator 
	///@return handle as \b long
	operator long() const 
	{ 
		return p; 
	}

	///\~russian
	///Унарный оператор & возвращает адрес дескриптора. 
	///@return возвращает адрес дескриптора
	///\~english
	///Unary operator &  return handle address. 
	///@return return handle address
	long* operator&() 
	{ 
		return &p; 
	}

protected:

	///\~russian
	///Конструктор по умолчанию. Инициализирует пустой дескриптор (= 0) 
	///\~english
	///Default constructor. Initialize empty handle (= 0) 
	PlotHandle() : p(NULL) {};

public:

	///\~russian
	///Конструктор с параметром.
	///@param n - ???
	///\~english
	///Parametric constructor
	///@param n - ???
	PlotHandle(long n) : p(n) {};

	///\~russian
	///Конструктор копирования.
	///@param rhs ссылка на существующий дескриптор
	///\~english
	///Copy constructor
	///@param rhs exist handle reference
	PlotHandle(const PlotHandle& rhs) { *this = rhs; };
};

///\~russian
///@brief Класс дескриптора 2D графики
///\~english
///@brief 2D plot descriptors class
class HPLOT : public PlotHandle
{
public:

	///\~russian
	///Конструктор по умолчанию. Инициализирует пустой дескриптор (= 0) 
	///\~english
	///Default constructor. Initialize empty handle (= 0) 
	HPLOT() {};

protected:

	///\~russian
	///Дружественная функция из класса XYPlotManager
	///@param hwnd дескриптор окна для которого создается экземпляр 2D графики
	///@return   дескриптор экземпляра 2D графики
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
	///CChildView* pView = ...;//получить дескриптор окна в котором будет отображаться графика 
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
	///Конструктор с параметром.
	///@param n - ???
	///\~english
	///Parametric constructor
	///@param n - ???
	explicit HPLOT(long n) : PlotHandle(n) {};
};


///\~russian
///@brief Класс дескриптор  для всех  экземпляров 2D графики
///\~english
///@brief 2D plot handle class
class ALL_PLOTS_ : public HPLOT
{
public:
	///\~russian
	///Конструктор по умолчанию.
	///\~english
	///Default constructor. 
	ALL_PLOTS_() : HPLOT(0) {};
};

#define ALL_PLOTS ALL_PLOTS_()

///\~russian
///@brief Класс дескриптора профиля 
///\~english
///@brief Plot profile descriptors class
class HPROFILE : public PlotHandle
{
public:

	///\~russian
	///Конструктор по умолчанию.
	///\~english
	///Default constructor. 
	HPROFILE() {};

protected:

	///\~russian
	///Дружественная функция из класса XYPlotManager для создания экземпляра профиля
	///@param[in] hPlot	дескриптор экземпляра 2D графика для которого создается профиль
	///@param name имя профиля (отображается в легенде)
	///@param color цвет профиля
	///@param width толщина линии профиля в пикселях
	///@param lineType тип линии профиля 
		///\li PLS_INVISIBLE - невидимая \li PLS_SOLID - сплошная \li PLS_DASH - штриховая \li PLS_DOT - точечная \li PLS_DASHDOT - осевая \li PLS_DASHDOTDOT - штрих-точка-точка \li PLS_CUSTOM - пользовательсякая
	///@param szLineTemplate пользовательский шаблон линии. Данный аргумент используется только в случае если выбран пользовательский тип линии. 
	///Шаблон представляет собой последовательность целочисленных значений для задания интервалов заполнения и промежутков в пикселях. 
	///Например значение шаблона "5 2" означает,  что 5px отрисовываются сплошной линией затем следует промежуток 2px.
	///@param visible будет ли отображаться линия данного профиля  
	///@param showmarks отображать ли метки данных профиля
	///@param xParentAxis ось X, к которой прикрепляется профиль 
	///@param yParentAxis ось Y, к которой прикрепляется профиль 
	///@return  дескриптор экземпляра профиля
	///\~english
	///Friend function from class XYPlotManager for profile handle creation
	///@param[in] hPlot	parent 2D plot handle for profile
	///@param name profile name (show in legend)
	///@param color profile color
	///@param width profile line width in pixels
	///@param lineType тип линии профиля 
	///\li PLS_INVISIBLE - невидимая \li PLS_SOLID - сплошная \li PLS_DASH - штриховая \li PLS_DOT - точечная \li PLS_DASHDOT - осевая \li PLS_DASHDOTDOT - штрих-точка-точка \li PLS_CUSTOM - пользовательсякая
	///@param szLineTemplate пользовательский шаблон линии. Данный аргумент используется только в случае если выбран пользовательский тип линии. 
	///Шаблон представляет собой последовательность целочисленных значений для задания интервалов заполнения и промежутков в пикселях. 
	///Например значение шаблона "5 2" означает,  что 5px отрисовываются сплошной линией затем следует промежуток 2px.
	///@param visible будет ли отображаться линия данного профиля  
	///@param showmarks отображать ли метки данных профиля
	///@param xParentAxis ось X, к которой прикрепляется профиль 
	///@param yParentAxis ось Y, к которой прикрепляется профиль 
	///@return  дескриптор экземпляра профиля
	///\~ 
	///@code
	///XYPlotManager& pm = XYPlotManager::Instance();
	///if ( !pm.Initialize("xyplot.dll", ::AfxGetMainWnd()->m_hWnd))
	///	return FALSE;
	///HPLOT plot;
	///CChildView* pView = ...;//получить дескриптор окна в котором будет отображаться графика 
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
///@brief Класс дескриптор  для всех  профилей 2D графики
///\~english
///@brief 2D plot all profiles handle class
class ALL_PROFILES : public HPROFILE
{
public:
	ALL_PROFILES() : HPROFILE(0) {};
};

///\~russian
///@brief Класс дескриптор  пустого профиля 2D графики
///\~english
///@brief 2D plot invalid profiles handle class
class INVALID_PROFILE : public HPROFILE
{
public:
	INVALID_PROFILE() : HPROFILE(-1) {};
};

//#define ALL_PROFILES ALL_PROFILES_()

///\~russian
///@brief Класс дескриптора группы профилей 
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
///@brief Класс дескриптор  пустой группы профилей 2D графики
///\~english
///@brief 2D plot invalid profiles group  handle class
class INVALID_GROUP : public HGROUP
{
public:
	INVALID_GROUP() : HGROUP(-1) {};
};


///\~russian
///@brief Класс дескриптора региона
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
///@brief Класс дескриптор всех регионов
///\~english
///@brief Plot all regions descriptor class
class ALL_REGIONS_ : public HPLOTREGION
{
public:
	ALL_REGIONS_() : HPLOTREGION(0) {};
};

#define ALL_REGIONS ALL_REGIONS_()


///\~russian
///@brief Класс дескриптор линий уровня
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
///@brief Класс ошибки времени исполнения для графика 
///\~english
///@brief Plot runtime error class
class XYPlotError : public std::runtime_error
{
public:
	XYPlotError(const std::string& message) : std::runtime_error(message) {};
};

///\~russian
///@brief Класс ошибки завершения запроса для графика 
///\~english
///@brief Plot runtime request error class
class XYPlotRequestFailure : public XYPlotError
{
public:
	XYPlotRequestFailure(const std::string& msg) : XYPlotError(msg + " was unable to complete request. Check last error for possible reason.") {};
};

}

#endif //_XYPLOTWRAPPER_H_