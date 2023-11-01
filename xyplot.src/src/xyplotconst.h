/// \file xyplotconst.h    
/// \author Johnny A. Matveichik    
/// \author Alexei N. Migoun    
///\~russian
///\brief Содержит константы и перечисляемые типы .
///\~english
///\brief Содержит константы и перечисляемые типы .
#pragma once

#ifndef _XY_PLOTCONST_H_
#define _XY_PLOTCONST_H_

namespace xyplot {

/// Коды ошибок библеотеки
const enum ePLOTERRORS{
	PE_NOERROR,					///\~english < No errors
	PE_UNKNOWN,					///\~english < Unknown error occured
	PE_UNEXPECTED,				///\~english < Unexpected exception occured
	PE_INVALID_PLOT,			///\~english < Invalid plot handle provided
	PE_INVALID_PROFILE,			///\~english < Invalid profile handle provided
	PE_INVALID_WINDOW,			///\~english < Invalid window handle provided
	PE_ALREADY_HOOKED,			///\~english < Window is already hooked by a plot
	PE_INSUFFICIENT_MEMORY,		///\~english < Not enough memory
	PE_INVALID_RANGE,			///\~english < Invalid range provided
	PE_INVALID_ARG,				///\~english < Invalid argument provided
	PE_INSUFFICIENT_STORAGE,	///\~english < Storage size is not enough to receive requested data
	PE_OUT_OF_RANGE,			///\~english < Provided index is out of range
	PE_INVALID_GROUP,			///\~english < Invalid group handle provided
	PE_RTFTEXT,					///\~english < Inapplicable due to RTF text formatting
	PE_INVALID_DIALOG_DLL,		///\~english < Dialog DLL is incompatible with this version of plot library
	PE_DIALOG_DLL_NOT_FOUND		///\~english < Dialog DLL was not found
};

///Идентификаторы элементов 2D графика
const enum ePLOTELEMENT{
	PI_NOTHING,					///< Пустой элемент
	PI_BACKGROUND,				///< Заливка фона ркна
	PI_BACKGROUND_CLIENT,		///< Заливка клиентской области графика
	PI_BORDER,					///< Граница окна
	PI_TITLE,					///< Заголовок графика
	PI_LEGEND,					///< Легенда графика
	PI_AXIS_LEFT,				///< Левая ось 
	PI_AXIS_RIGHT,				///< Правая ось 
	PI_AXIS_TOP,				///< Верхняя ось 
	PI_AXIS_BOTTOM,				///< Нижняя ось 
	PI_SELECTION,				///< Текущее выделение
	PI_CANVAS					///< Холст 
};

/// Подэлементы выделения
const enum eSELECTIONITEMS{
	SP_MODE,				///< Режим выделения
	SP_FLASH_SPEED,			///< Скорость мерцания в режиме выделения SM_FLASH
	SP_NOTIFY_MESSAGE		///< Сообщение о выделении элемента графика
};

/// Тип выделения
const enum eSELECTIONTYPE{
	SM_NONE = -1,			///< Нет выделения
	SM_FLASH,				///< Выделение мерцанием
	SM_VTRACE,				///< Вертикальная трассировка профилей и групп
	SM_HTRACE,				///< Горизонтальная трассировка профилей и групп
	SM_TRACE				///< Полная трассировка профилей и групп
};


///Скорость мерцания профиля или группы профилейв режиме выделения SM_FLASH
const enum eFLASHSPEED{
	FLS_NO_FLASH,			///< Нет мерцания
	FLS_VERY_SLOW,			///< Очень медленно (1200 ms - отображение профиля, 600 ms - профиль не отображается)    
	FLS_SLOW,				///< Медленно (600 ms - отображение профиля, 400 ms - профиль не отображается)    
	FLS_NORMAL,				///< Нормально (400 ms - отображение профиля, 300 ms - профиль не отображается)    
	FLS_FAST,				///< Быстро (300 ms - отображение профиля, 150 ms - профиль не отображается)    
	FLS_VERY_FAST,			///< Очень быстро (150 ms - отображение профиля, 75 ms - профиль не отображается)    
	FLS_EXTREME				///< Экстремально (10 ms - отображение профиля, 10 ms - профиль не отображается)    
};

/// Элементы осей
const enum eAXISITEMS{
	PAI_TITLE = 200,			///< Заголовок оси
	PAI_MAJOR_TICKS,			///< Основные метки оси
	PAI_MINOR_TICKS,			///< Вспомогательные метки оси
	PAI_MAJOR_TICKS_LENGTH,		///< Длина основных меток оси
	PAI_MINOR_TICKS_LENGTH,		///< Длина дополнительных меток оси
	PAI_MAJOR_TICKS_COUNT,		///< Количество основных меток оси 
	PAI_MINOR_TICKS_COUNT,		///< Количество вспомогательных меток оси
	PAI_MAJOR_TICKS_WIDTH,		///< Толщина линии основных меток оси
	PAI_MINOR_TICKS_WIDTH,		///< Толщина линии вспомогательных меток оси
	PAI_LINE,					///< Тип линии оси
	PAI_LINE_WIDTH,				///< Толщина линии оси
	PAI_LINE_STYLE,				///< Стиль линии
	PAI_MAJOR_DIGITS,			///< Подпись к основным меткам оси
	PAI_MINOR_DIGITS,			///< Подпись к дополнительным меткам оси
	PAI_MAJOR_DIGITS_FORMAT,	///< Формат подписей к основным меткам оси
	PAI_MINOR_DIGITS_FORMAT,	///< Формат подписи к дополнительным меткам оси
	PAI_MAJOR_GRID_LINES,		///< Линии сетки к основным меткам оси
	PAI_MINOR_GRID_LINES,		///< Линии сетки к дополнительнымметкам оси
	PAI_LOWER_LIMIT,			///< Нижняя граница оси
	PAI_UPPER_LIMIT,			///< Верхняя граница оси
	PAI_AUTOSCALE,				///< Режим автоматического масштабирования оси
	PAI_LOG10,					///< Режим логарифмического оттображения оси
	PAI_STYLE,					///< Текущий  стиль оси
	PAI_TIME_HISTORY			///< Режим отображения временной шкалы
};

/// legend properties
const enum eLEGENDITEMS{
	PLP_ALIGN		= 220,	///< Положение легенды
	PLP_LINELENGTH	= 221,	///< Длина линии  в легенде
	PLP_LEFT		= -1,	///< Положение легенды в левой части 
	PLP_RIGHT		= 1,	///< Положение легенды в правой части 
	PLP_BOTTOM		= 0		///< Положение легенды в нижней части 
};


///Level label position - положение надписи уровня
const enum eLEVELLABELPOS{
	LLP_CENTER = 0x001,						///< Горизонтальное выравнивание по центру
	LLP_LEFT   = 0x002,						///< Горизонтальное выравнивание по левому краю
	LLP_RIGHT  = 0x004,						///< Горизонтальное выравнивание по правому краю
	LLP_ABOVE  = 0x008,						///< Вертикальное выравнивание над линией 
	LLP_UNDER  = 0x010,						///< Вертикальное выравнивание под линией
	LLP_ATLINE = LLP_ABOVE|LLP_UNDER,		///< Вертикальное выравнивание по линии 
	LLP_ABOVELEFT   = LLP_LEFT|LLP_ABOVE,	///< Положение слева над линией
	LLP_ABOVERIGHT  = LLP_RIGHT|LLP_ABOVE,  ///< Положение справа над линией
	LLP_ABOVECENTER = LLP_CENTER|LLP_ABOVE,	///< Положение по середине над линией
	LLP_UNDERLEFT   = LLP_LEFT|LLP_UNDER,	///< Положение слева под линией
	LLP_UNDERRIGHT  = LLP_RIGHT|LLP_UNDER,	///< Положение справа под линией
	LLP_UNDERCENTER = LLP_CENTER|LLP_UNDER,	///< Положение по середине под линией
	LLP_ATLINELEFT   = LLP_LEFT|LLP_ATLINE, ///< Положение слева по линии
	LLP_ATLINERIGHT  = LLP_RIGHT|LLP_ATLINE, ///< Положение справа по линии
	LLP_ATLINECENTER = LLP_CENTER|LLP_ATLINE ///< Положение по серединие по линии
};

/// Тип линии 
const enum LINETYPE{
	PLS_INVISIBLE,	///< Невидимая
	PLS_SOLID,		///< Сплошная
	PLS_DASH,		///< Штриховая
	PLS_DOT,		///< Точечная
	PLS_DASHDOT,	///< Осевая
	PLS_DASHDOTDOT,	///< Штрих-точка-точка
	PLS_CUSTOM		///< Пользовательская
};

/// Profile properties
const enum ePROFILEITEMS{
	PRP_COLOR = 1,		///< Цвет линии
	PRP_WIDTH,			///< Толщина линии
	PRP_STYLE,			///< Тип линии
	PRP_TEMPLATE,		///< Пользовательский шаблон линии
	PRP_NAME,			///< Имя профиля
	PRP_MARKS,			///< Метки данных профиля
	PRP_MARKSFREQ,		///< Частота меток данных
	PRP_MARKSTYPE,		///< Тип меток
	PRP_MARKSIZE,		///< Размер меток
	PRP_MARKFILLCOLOR,	///< Цвет заполнения меток
	PRP_MARKSTROKECOLOR,///< Цвет линий меток
};

/// Level line properties
const enum eLEVELLINE{
	LVL_COLOR = 1,		///< Цвет линии
	LVL_WIDTH,			///< Толщина линии
	LVL_STYLE,			///< Тип линии
	LVL_TEMPLATE,		///< Пользовательский шаблон линии
	LVL_TITLE			///< Имя профиля
};

/// Background properties
const enum ePLOTREGION{
	PLR_TITLE	 = 430			///< Заголовок региона
};

/// Типы меток данных
const enum eMARKSTYPE{
	PMT_CIRCLE,			///< Круглая
	PMT_SQUARE,			///< Квадратная 
	PMT_DIAMOND,		///< Ромб
	PMT_TRIANGLEUP,		///< Треугольник направленный вверх
	PMT_TRIANGLEDOWN,	///< Треугольник направленный вниз
	PMT_CROSS0,			///< Крест
	PMT_CROSS45			///< Повернутый крест
};

/// Типы штриховок заливки 
const enum eHATCHSTYLE{
	BGHS_NONE = -1,
	BGHS_HORIZONTAL,   ///< Горизонтальная		------
	BGHS_VERTICAL,     ///< Вертикальная		||||||
	BGHS_FDIAGONAL,    ///< Диагональная слева на право     /* \\\\\ */
	BGHS_BDIAGONAL,    ///< Диагональная справа на лево     /* ///// */
	BGHS_CROSS,        ///< Сетка					/* +++++ */
	BGHS_DIAGCROSS     ///< Диагональная сетка		/* xxxxx */
};

/// Background properties
const enum eBACKGROUNDITEMS{
	BGP_COLOR_START = 330,		///< Начальный цвет заливки
	BGP_COLOR_END,				///< Конечный  цвет заливки
	BGP_OPACITY_START,			///< Начальная непрозрачность цвета заливки, %
	BGP_OPACITY_END,			///< Конечная  непрозрачность цвета заливки, %
	BGP_HATCH_STYLE,			///< Стиль штриховки
	BGP_HATCH_COLOR,			///< Цвет штриховки
	BGP_BORDER_STYLE,			///< Стиль линии границ
	BGP_BORDER_COLOR,			///< Цвет линии границ
	BGP_BORDER_WIDTH,			///< Толщина линии границ
	BGP_BORDER_TEMPL,			///< Пользовательский шаблон стиля линии	
	BGP_DIRECTION,				///< Направление заливки	
	BGP_GRADIENT_VERT = 1,		///< Вертикальная заливка
	BGP_GRADIENT_HORZ = 0		///< Горизонтальная заливка
};

typedef int FLASH_SPEED;

};

#endif //_XY_PLOT_CONST_H_