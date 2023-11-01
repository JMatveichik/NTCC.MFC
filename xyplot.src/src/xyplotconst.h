/// \file xyplotconst.h    
/// \author Johnny A. Matveichik    
/// \author Alexei N. Migoun    
///\~russian
///\brief �������� ��������� � ������������� ���� .
///\~english
///\brief �������� ��������� � ������������� ���� .
#pragma once

#ifndef _XY_PLOTCONST_H_
#define _XY_PLOTCONST_H_

namespace xyplot {

/// ���� ������ ����������
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

///�������������� ��������� 2D �������
const enum ePLOTELEMENT{
	PI_NOTHING,					///< ������ �������
	PI_BACKGROUND,				///< ������� ���� ����
	PI_BACKGROUND_CLIENT,		///< ������� ���������� ������� �������
	PI_BORDER,					///< ������� ����
	PI_TITLE,					///< ��������� �������
	PI_LEGEND,					///< ������� �������
	PI_AXIS_LEFT,				///< ����� ��� 
	PI_AXIS_RIGHT,				///< ������ ��� 
	PI_AXIS_TOP,				///< ������� ��� 
	PI_AXIS_BOTTOM,				///< ������ ��� 
	PI_SELECTION,				///< ������� ���������
	PI_CANVAS					///< ����� 
};

/// ����������� ���������
const enum eSELECTIONITEMS{
	SP_MODE,				///< ����� ���������
	SP_FLASH_SPEED,			///< �������� �������� � ������ ��������� SM_FLASH
	SP_NOTIFY_MESSAGE		///< ��������� � ��������� �������� �������
};

/// ��� ���������
const enum eSELECTIONTYPE{
	SM_NONE = -1,			///< ��� ���������
	SM_FLASH,				///< ��������� ���������
	SM_VTRACE,				///< ������������ ����������� �������� � �����
	SM_HTRACE,				///< �������������� ����������� �������� � �����
	SM_TRACE				///< ������ ����������� �������� � �����
};


///�������� �������� ������� ��� ������ ��������� ������ ��������� SM_FLASH
const enum eFLASHSPEED{
	FLS_NO_FLASH,			///< ��� ��������
	FLS_VERY_SLOW,			///< ����� �������� (1200 ms - ����������� �������, 600 ms - ������� �� ������������)    
	FLS_SLOW,				///< �������� (600 ms - ����������� �������, 400 ms - ������� �� ������������)    
	FLS_NORMAL,				///< ��������� (400 ms - ����������� �������, 300 ms - ������� �� ������������)    
	FLS_FAST,				///< ������ (300 ms - ����������� �������, 150 ms - ������� �� ������������)    
	FLS_VERY_FAST,			///< ����� ������ (150 ms - ����������� �������, 75 ms - ������� �� ������������)    
	FLS_EXTREME				///< ������������ (10 ms - ����������� �������, 10 ms - ������� �� ������������)    
};

/// �������� ����
const enum eAXISITEMS{
	PAI_TITLE = 200,			///< ��������� ���
	PAI_MAJOR_TICKS,			///< �������� ����� ���
	PAI_MINOR_TICKS,			///< ��������������� ����� ���
	PAI_MAJOR_TICKS_LENGTH,		///< ����� �������� ����� ���
	PAI_MINOR_TICKS_LENGTH,		///< ����� �������������� ����� ���
	PAI_MAJOR_TICKS_COUNT,		///< ���������� �������� ����� ��� 
	PAI_MINOR_TICKS_COUNT,		///< ���������� ��������������� ����� ���
	PAI_MAJOR_TICKS_WIDTH,		///< ������� ����� �������� ����� ���
	PAI_MINOR_TICKS_WIDTH,		///< ������� ����� ��������������� ����� ���
	PAI_LINE,					///< ��� ����� ���
	PAI_LINE_WIDTH,				///< ������� ����� ���
	PAI_LINE_STYLE,				///< ����� �����
	PAI_MAJOR_DIGITS,			///< ������� � �������� ������ ���
	PAI_MINOR_DIGITS,			///< ������� � �������������� ������ ���
	PAI_MAJOR_DIGITS_FORMAT,	///< ������ �������� � �������� ������ ���
	PAI_MINOR_DIGITS_FORMAT,	///< ������ ������� � �������������� ������ ���
	PAI_MAJOR_GRID_LINES,		///< ����� ����� � �������� ������ ���
	PAI_MINOR_GRID_LINES,		///< ����� ����� � �������������������� ���
	PAI_LOWER_LIMIT,			///< ������ ������� ���
	PAI_UPPER_LIMIT,			///< ������� ������� ���
	PAI_AUTOSCALE,				///< ����� ��������������� ��������������� ���
	PAI_LOG10,					///< ����� ���������������� ������������ ���
	PAI_STYLE,					///< �������  ����� ���
	PAI_TIME_HISTORY			///< ����� ����������� ��������� �����
};

/// legend properties
const enum eLEGENDITEMS{
	PLP_ALIGN		= 220,	///< ��������� �������
	PLP_LINELENGTH	= 221,	///< ����� �����  � �������
	PLP_LEFT		= -1,	///< ��������� ������� � ����� ����� 
	PLP_RIGHT		= 1,	///< ��������� ������� � ������ ����� 
	PLP_BOTTOM		= 0		///< ��������� ������� � ������ ����� 
};


///Level label position - ��������� ������� ������
const enum eLEVELLABELPOS{
	LLP_CENTER = 0x001,						///< �������������� ������������ �� ������
	LLP_LEFT   = 0x002,						///< �������������� ������������ �� ������ ����
	LLP_RIGHT  = 0x004,						///< �������������� ������������ �� ������� ����
	LLP_ABOVE  = 0x008,						///< ������������ ������������ ��� ������ 
	LLP_UNDER  = 0x010,						///< ������������ ������������ ��� ������
	LLP_ATLINE = LLP_ABOVE|LLP_UNDER,		///< ������������ ������������ �� ����� 
	LLP_ABOVELEFT   = LLP_LEFT|LLP_ABOVE,	///< ��������� ����� ��� ������
	LLP_ABOVERIGHT  = LLP_RIGHT|LLP_ABOVE,  ///< ��������� ������ ��� ������
	LLP_ABOVECENTER = LLP_CENTER|LLP_ABOVE,	///< ��������� �� �������� ��� ������
	LLP_UNDERLEFT   = LLP_LEFT|LLP_UNDER,	///< ��������� ����� ��� ������
	LLP_UNDERRIGHT  = LLP_RIGHT|LLP_UNDER,	///< ��������� ������ ��� ������
	LLP_UNDERCENTER = LLP_CENTER|LLP_UNDER,	///< ��������� �� �������� ��� ������
	LLP_ATLINELEFT   = LLP_LEFT|LLP_ATLINE, ///< ��������� ����� �� �����
	LLP_ATLINERIGHT  = LLP_RIGHT|LLP_ATLINE, ///< ��������� ������ �� �����
	LLP_ATLINECENTER = LLP_CENTER|LLP_ATLINE ///< ��������� �� ��������� �� �����
};

/// ��� ����� 
const enum LINETYPE{
	PLS_INVISIBLE,	///< ���������
	PLS_SOLID,		///< ��������
	PLS_DASH,		///< ���������
	PLS_DOT,		///< ��������
	PLS_DASHDOT,	///< ������
	PLS_DASHDOTDOT,	///< �����-�����-�����
	PLS_CUSTOM		///< ����������������
};

/// Profile properties
const enum ePROFILEITEMS{
	PRP_COLOR = 1,		///< ���� �����
	PRP_WIDTH,			///< ������� �����
	PRP_STYLE,			///< ��� �����
	PRP_TEMPLATE,		///< ���������������� ������ �����
	PRP_NAME,			///< ��� �������
	PRP_MARKS,			///< ����� ������ �������
	PRP_MARKSFREQ,		///< ������� ����� ������
	PRP_MARKSTYPE,		///< ��� �����
	PRP_MARKSIZE,		///< ������ �����
	PRP_MARKFILLCOLOR,	///< ���� ���������� �����
	PRP_MARKSTROKECOLOR,///< ���� ����� �����
};

/// Level line properties
const enum eLEVELLINE{
	LVL_COLOR = 1,		///< ���� �����
	LVL_WIDTH,			///< ������� �����
	LVL_STYLE,			///< ��� �����
	LVL_TEMPLATE,		///< ���������������� ������ �����
	LVL_TITLE			///< ��� �������
};

/// Background properties
const enum ePLOTREGION{
	PLR_TITLE	 = 430			///< ��������� �������
};

/// ���� ����� ������
const enum eMARKSTYPE{
	PMT_CIRCLE,			///< �������
	PMT_SQUARE,			///< ���������� 
	PMT_DIAMOND,		///< ����
	PMT_TRIANGLEUP,		///< ����������� ������������ �����
	PMT_TRIANGLEDOWN,	///< ����������� ������������ ����
	PMT_CROSS0,			///< �����
	PMT_CROSS45			///< ���������� �����
};

/// ���� ��������� ������� 
const enum eHATCHSTYLE{
	BGHS_NONE = -1,
	BGHS_HORIZONTAL,   ///< ��������������		------
	BGHS_VERTICAL,     ///< ������������		||||||
	BGHS_FDIAGONAL,    ///< ������������ ����� �� �����     /* \\\\\ */
	BGHS_BDIAGONAL,    ///< ������������ ������ �� ����     /* ///// */
	BGHS_CROSS,        ///< �����					/* +++++ */
	BGHS_DIAGCROSS     ///< ������������ �����		/* xxxxx */
};

/// Background properties
const enum eBACKGROUNDITEMS{
	BGP_COLOR_START = 330,		///< ��������� ���� �������
	BGP_COLOR_END,				///< ��������  ���� �������
	BGP_OPACITY_START,			///< ��������� �������������� ����� �������, %
	BGP_OPACITY_END,			///< ��������  �������������� ����� �������, %
	BGP_HATCH_STYLE,			///< ����� ���������
	BGP_HATCH_COLOR,			///< ���� ���������
	BGP_BORDER_STYLE,			///< ����� ����� ������
	BGP_BORDER_COLOR,			///< ���� ����� ������
	BGP_BORDER_WIDTH,			///< ������� ����� ������
	BGP_BORDER_TEMPL,			///< ���������������� ������ ����� �����	
	BGP_DIRECTION,				///< ����������� �������	
	BGP_GRADIENT_VERT = 1,		///< ������������ �������
	BGP_GRADIENT_HORZ = 0		///< �������������� �������
};

typedef int FLASH_SPEED;

};

#endif //_XY_PLOT_CONST_H_