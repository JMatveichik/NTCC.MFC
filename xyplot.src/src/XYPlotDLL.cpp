// XYPlotDLL.cpp : Defines the initialization routines for the DLL.
#include "stdafx.h"

#define _DLL_MASTER_

#include "Main.h"
#include "Global.h"
#include "xyplot.h"

using namespace std;
using namespace xyplot;

//#define DNP_PROTECT
#define DNP_SOFTWARE_NAME "xyplot"
#define DNP_SOFTWARE_CRC 0x00
#include ".\dnp_checkout.h"

using namespace dnp_checkout;

BEGIN_CHECKOUT_MAP
CHECKOUT_FUNCT(AddProfile)		// 0
CHECKOUT_FUNCT(SetData)			// 1
CHECKOUT_FUNCT(AppendData)		// 2
CHECKOUT_FUNCT(SetString)		// 3
CHECKOUT_FUNCT(Initialize)		// 4
CHECKOUT_FUNCT(SaveProperties)	// 5
CHECKOUT_FUNCT(LoadProperties)	// 6
CHECKOUT_FUNCT(RegLoadSettings)	// 7
CHECKOUT_FUNCT(RegSaveSettings)	// 8
END_CHECKOUT_MAP

typedef map< long, XYPlot* > PlotMap;
typedef PlotMap::iterator PlotMapIterator;
PlotMap g_plotmap;
CRITICAL_SECTION g_cs;
HMODULE g_hmod;


#define DECLARE_PLOT_PTR(p_p, id) \
	EnterCriticalSection(&g_cs); \
	PlotMapIterator piter_ = g_plotmap.find(id); \
	if (piter_ == g_plotmap.end()) { LeaveCriticalSection(&g_cs); return PE_INVALID_PLOT; } \
	XYPlot* p_p = piter_->second; LeaveCriticalSection(&g_cs);



/************************************************************************/

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID Reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		InitializeCriticalSection(&g_cs);

		g_hmod = LoadLibrary("riched20.dll");
		if (!g_hmod)
			return FALSE;
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		EnterCriticalSection(&g_cs);

		for(PlotMapIterator it = g_plotmap.begin(); it != g_plotmap.end(); ++it)
			delete it->second;

		LeaveCriticalSection(&g_cs);

		DeleteCriticalSection(&g_cs);

		FreeLibrary(g_hmod);
	}

	return TRUE;
}

/************************************************************************/

long CALLCONV Initialize(HWND hwndMain, long* lPlotID)
{
	DNP_CHECKPOINT_BEGIN(4, Initialize)
		return PE_UNEXPECTED;
	DNP_CHECKPOINT_END

	if( !hwndMain || !IsWindow( hwndMain ) )
		return PE_INVALID_WINDOW;

	*lPlotID = 0;
	try
	{ 
		std::auto_ptr<XYPlot> pPlot(new XYPlot(g_hmod));

		pPlot->Create( hwndMain );

		*lPlotID = Global::Pointer2ID(pPlot.get());

		EnterCriticalSection(&g_cs);
		
		g_plotmap[*lPlotID] = pPlot.release();

		LeaveCriticalSection(&g_cs);
	}
	catch ( ... )
	{ 
		return PE_UNEXPECTED;
	}

	return PE_NOERROR;
}

long CALLCONV Finalize(long lPlotID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{
		pPlot->Destroy();
		EnterCriticalSection(&g_cs);
		g_plotmap.erase(piter_);
		LeaveCriticalSection(&g_cs);
		delete pPlot;
	}
	catch( ... ) 
	{ 
		return PE_UNEXPECTED;
	}

	return PE_NOERROR;
}

long CALLCONV AddProfile(long lPlotID, const char* name, COLORREF color, int width, int linetype, const char* line_template, BOOL bVisible, BOOL bMarked, BOOL bottom_axis, BOOL left_axis, long* lProfileID)
{	
	DNP_CHECKPOINT_BEGIN(0, AddProfile)
		return PE_UNEXPECTED;
	DNP_CHECKPOINT_END

	DECLARE_PLOT_PTR(pPlot, lPlotID);

	return pPlot->AddProfile( name, color, width, linetype, line_template, bVisible, bMarked, bottom_axis, left_axis, lProfileID );
}

long CALLCONV DeleteProfile(long lPlotID, long lProfileID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	return pPlot->DeleteProfile( PROFILE_KEY(lProfileID) );
}

long CALLCONV SetData(long lPlotID, long lProfileID, const double *px, const double *py, unsigned size)
{
	DNP_CHECKPOINT_BEGIN(1, SetData)
	DNP_CHECKPOINT_END

	DECLARE_PLOT_PTR(pPlot, lPlotID);
	
	if( !px || !py )
		return PE_INVALID_ARG;

	return pPlot->SetData( PROFILE_KEY(lProfileID), px, py, size );
}

long CALLCONV AppendData(long lPlotID, long lProfileID, const double *px, const double *py, int size)
{
	DNP_CHECKPOINT_BEGIN(2, AppendData)
	DNP_CHECKPOINT_END

	DECLARE_PLOT_PTR(pPlot, lPlotID);

	if( !px || !py )
		return PE_INVALID_ARG;

	return pPlot->AppendData( PROFILE_KEY(lProfileID), px, py, size );
}

long CALLCONV GetData(long lPlotID, long lProfileID, double *px, double *py, unsigned* size)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	if( !px || !py )
		return PE_INVALID_ARG;

	Profile *profile = pPlot->FindProfile( PROFILE_KEY(lProfileID) );
	if (!profile)
		return PE_INVALID_PROFILE;

	return profile->GetData(size, px, py);
}

/*Задать строку для элемента графика*/
long CALLCONV SetString(long lPlotID, long item, long subitem, const char* pszString)
{
	DNP_CHECKPOINT_BEGIN(3, SetString)
	DNP_CHECKPOINT_END

	DECLARE_PLOT_PTR(pPlot, lPlotID);

	if( !pszString )
		return PE_INVALID_ARG;

	switch( item )
	{
	case PI_TITLE:
		
		pPlot->GetTitle()->SetText( pszString );
		break;

	case PI_AXIS_BOTTOM:
	case PI_AXIS_LEFT:
	case PI_AXIS_RIGHT:
	case PI_AXIS_TOP:
		{
			Axis* pAxis = pPlot->GetAxis(item);

			switch (subitem)
			{
			case PAI_TITLE:
				pAxis->SetLabel( pszString );
				break;

			case PAI_MAJOR_DIGITS_FORMAT:
				pAxis->SetDigitsFormat( pszString, TRUE );
				break;
			
			case PAI_MINOR_DIGITS_FORMAT:
				pAxis->SetDigitsFormat( pszString, FALSE );
				break;
			
			case PAI_MAJOR_GRID_LINES:
				pAxis->GetGridLine(TRUE).SetTemplate( pszString );
				break;
			
			case PAI_MINOR_GRID_LINES:
				pAxis->GetGridLine(FALSE).SetTemplate( pszString );
				break;
			
			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	default:
		Profile* pProfile = pPlot->FindProfile(item);
		if (pProfile != NULL)
		{	

			switch (subitem)
			{
			case PRP_NAME:
				pProfile->SetName(pszString);
				break;
			case PRP_TEMPLATE:
				pProfile->GetLineData().SetTemplate(pszString);
				break;
			default:
				return PE_INVALID_ARG;
			}
			return PE_NOERROR;
		}
		
		LevelLine* pLevelLine = pPlot->FindLevelLine(item);
		if (pLevelLine != NULL)
		{
			switch (subitem)
			{
			case LVL_TITLE:
				pLevelLine->SetLabel(pszString);
				break;
			case LVL_TEMPLATE:
				pLevelLine->GetLine().SetTemplate(pszString);
				break;
			default:
				return PE_INVALID_ARG;
			}
			return PE_NOERROR;
		}
		
		PlotRegion* pRgn = pPlot->FindRegion(item);
		if (pRgn != NULL)
		{
			switch (subitem)
			{
			case PLR_TITLE:
				pRgn->SetTitle(pszString);
				break;			
			default:
				return PE_INVALID_ARG;
			}
			return PE_NOERROR;
		}
	}

	return PE_NOERROR;
}

/*Получить строку для элемента графика*/
long CALLCONV GetString(long lPlotID, long item, long subitem, int* buf_size, char* buf)
{	
	DECLARE_PLOT_PTR(pPlot, lPlotID);
	
	string str;
	try
	{
		switch( item )
		{
		case PI_TITLE:
			str = pPlot->GetTitle()->GetText();
			break;
		
		case PI_AXIS_BOTTOM:
		case PI_AXIS_LEFT:
		case PI_AXIS_RIGHT:
		case PI_AXIS_TOP:
			{
				Axis* pAxis = pPlot->GetAxis(item);

				switch (subitem)
				{
				case PAI_TITLE:
					str = pAxis->GetLabel();
					break;

				case PAI_MAJOR_DIGITS_FORMAT:
					str = pAxis->GetDigitsFormat( TRUE );
					break;
				
				case PAI_MINOR_DIGITS_FORMAT:
					str = pAxis->GetDigitsFormat( FALSE );
					break;
				
				case PAI_MAJOR_GRID_LINES:
					str = pAxis->GetGridLine(TRUE).GetTemplate();
					break;
				
				case PAI_MINOR_GRID_LINES:
					str = pAxis->GetGridLine(FALSE).GetTemplate();
					break;
				
				default:
					return PE_INVALID_ARG;
				}
			}
			break;

		default:

			Profile* pProfile = pPlot->FindProfile(item);
			if (pProfile != NULL)
			{

				switch (subitem)
				{
				case PRP_NAME:
					str = pProfile->GetName();
					break;
				case PRP_TEMPLATE:
					str = pProfile->GetLineData().GetTemplate();
					break;
				default:
					return PE_INVALID_ARG;
				}				
			}

			LevelLine* pLevelLine = pPlot->FindLevelLine(item);
			if (pLevelLine != NULL)
			{
				switch (subitem)
				{
				case LVL_TITLE:
					str = pLevelLine->GetLabel();
					break;
				case LVL_TEMPLATE:
					str = pLevelLine->GetLine().GetTemplate();
					break;
				default:
					return PE_INVALID_ARG;
				}				
			}

			PlotRegion* pRgn = pPlot->FindRegion(item);
			if (pRgn != NULL)
			{
				switch (subitem)
				{
				case PLR_TITLE:
					str = pRgn->GetTitle();
					break;			
				default:
					return PE_INVALID_ARG;
				}				
			}
		}		
	}
	catch (...)
	{
		return PE_UNEXPECTED;
	}

	int nRequired = int(str.length())+1;
	if (buf == NULL)
	{
		*buf_size = nRequired;
		return PE_NOERROR;
	}

	if (*buf_size < nRequired) {
		if (!buf)
		{
			*buf_size = nRequired;
			buf[0] = 0;
			return PE_NOERROR;
		}
		else 
		{
			return PE_INSUFFICIENT_STORAGE;
		}
	}

	strcpy_s(buf, *buf_size, str.c_str());

	return PE_NOERROR;
}

/*Задать шрифт для элемента графика*/
long CALLCONV SetFont(long lPlotID, long item, long subitem, const LOGFONT* plf)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	if( !plf )
		return PE_INVALID_ARG;

	switch( item )
	{
	case PI_TITLE:
		
		pPlot->GetTitle()->SetFont( plf );
		break;

	case PI_LEGEND:
		pPlot->GetLegend()->SetFont( plf );
		break;

	case PI_AXIS_BOTTOM:
	case PI_AXIS_LEFT:
	case PI_AXIS_RIGHT:
	case PI_AXIS_TOP:
		{
			Axis* pAxis = pPlot->GetAxis(item);
			switch (subitem)
			{
			case PAI_TITLE:
				pAxis->SetLabelFont( plf );
				break;

			case PAI_MAJOR_DIGITS:
				pAxis->SetDigitsFont( *plf, TRUE );
				break;
				
			case PAI_MINOR_DIGITS:
				pAxis->SetDigitsFont( *plf, FALSE );
				break;
				
			default:
				return PE_INVALID_ARG;
			}
			break;
		}

	//TODO: Level line & region processing

	default:
		return PE_INVALID_ARG;
	}

	return PE_NOERROR;
}

/*Задать цвет элемента графика*/
long CALLCONV SetColor(long lPlotID, long item, long subitem, COLORREF color)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	switch( item )
	{
	case PI_TITLE:		
		 pPlot->GetTitle()->SetColor( color );
		break;

	case PI_BORDER:
		pPlot->GetBorder()->GetLineData().SetColor( color );
		break;

	case PI_BACKGROUND:
	case PI_BACKGROUND_CLIENT:
		{
			BackGround& bk = (item != PI_BACKGROUND) ? *pPlot->GetClientBG() : *pPlot->GetPlotBG();

			switch(subitem)
			{
				case BGP_COLOR_START:
					{
						COLORREF left, right;
						bk.GetFillColors(left, right);
						bk.SetFillColors(color, right);
					}
					break;
			
				case BGP_COLOR_END:
					{
						COLORREF left, right;
						bk.GetFillColors(left, right);
						bk.SetFillColors(left, color);
					}
					break;
				default:
					return PE_INVALID_ARG;
			}
			break;
		}
		break;

	case PI_AXIS_TOP:
	case PI_AXIS_RIGHT:
	case PI_AXIS_LEFT:
	case PI_AXIS_BOTTOM:
		{
			Axis *pAxis = pPlot->GetAxis(item);
			switch (subitem)
			{
			case PAI_TITLE:
				pAxis->SetLabelColor(color);
				break;
			
			case PAI_LINE:
				pAxis->GetLine().SetColor(color);
				break;
			
			case PAI_MAJOR_TICKS:
				pAxis->GetTicksLine(TRUE).SetColor(color);
				break;
			
			case PAI_MINOR_TICKS:
				pAxis->GetTicksLine(FALSE).SetColor(color);
				break;
			
			case PAI_MAJOR_DIGITS:
				pAxis->SetDigitsColor(color, TRUE);
				break;
			
			case PAI_MINOR_DIGITS:
				pAxis->SetDigitsColor(color, FALSE);
				break;
			
			case PAI_MAJOR_GRID_LINES:
				pAxis->GetGridLine(TRUE).SetColor(color);
				break;
			
			case PAI_MINOR_GRID_LINES:
				pAxis->GetGridLine(FALSE).SetColor(color);
				break;
			
			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	default:

		if ( pPlot->FindProfile(item) )
		{
			Profile* pProfile = pPlot->FindProfile(item);

			switch (subitem)
			{
			case PRP_MARKSTROKECOLOR:
				pProfile->GetMarks().SetLineColor(color);
				break;
			case PRP_MARKFILLCOLOR:
				pProfile->GetMarks().SetFillColor(color);
				break;
			default:
				pProfile->GetLineData().SetColor(color);

			}
		}
		else if ( pPlot->FindLevelLine(item) )
		{
			LevelLine* pLevelLine = pPlot->FindLevelLine(item);
			pLevelLine->GetLine().SetColor(color);
			return PE_NOERROR;
		}
		else if ( pPlot->FindRegion(item) )
		{
			PlotRegion* pRgn = pPlot->FindRegion(item);
			switch (subitem)
			{
			case BGP_COLOR_START:
				{
				COLORREF clrFrom, clrTo;
				BackGround& bg = pRgn->GetBG();
				bg.GetFillColors(clrFrom, clrTo);
				bg.SetFillColors(color, clrTo);
				break;
				}
			case BGP_COLOR_END:
				{
				COLORREF clrFrom, clrTo;
				BackGround& bg = pRgn->GetBG();
				bg.GetFillColors(clrFrom, clrTo);
				bg.SetFillColors(clrFrom, color);
				break;
				}

			case BGP_BORDER_COLOR:
				pRgn->GetLineData().SetColor(color);
				break;

			case BGP_HATCH_COLOR:
				pRgn->GetBG().SetHatchColor(color);
				break;
			
			default:
				return PE_INVALID_ARG;			
			}

			return PE_NOERROR;
		}


		return PE_INVALID_ARG;
	}
	
	pPlot->Update();

	return PE_NOERROR;
}

/*Получить цвет элемента графика*/
long CALLCONV GetColor(long lPlotID, long item, long subitem, COLORREF* color)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	switch( item )
	{
	case PI_TITLE:
		*color = 0;
		return PE_RTFTEXT;

	case PI_BORDER:
		*color = pPlot->GetBorder()->GetLineData().GetColor();
		break;

	case PI_BACKGROUND:
	case PI_BACKGROUND_CLIENT:
		{
			BackGround& bk = (item != PI_BACKGROUND) ? *pPlot->GetClientBG() : *pPlot->GetPlotBG();

			switch(subitem)
			{
				case BGP_COLOR_START:
					{
						COLORREF left, right;
						bk.GetFillColors(left, right);
						*color = left;
					}
					break;
			
				case BGP_COLOR_END:
					{
						COLORREF left, right;
						bk.GetFillColors(left, right);
						*color = right;
					}
					break;
				default:
					return PE_INVALID_ARG;
			}
			break;
		}
		break;
	
	case PI_AXIS_TOP:
	case PI_AXIS_RIGHT:
	case PI_AXIS_LEFT:
	case PI_AXIS_BOTTOM:
		{
			Axis *pAxis = pPlot->GetAxis(item);
			switch (subitem)
			{
			case PAI_TITLE:
				return PE_RTFTEXT;
			
			case PAI_LINE:
				*color = pAxis->GetLine().GetColor();
				break;
			
			case PAI_MAJOR_TICKS:
				*color = pAxis->GetTicksLine(TRUE).GetColor();
				break;
			
			case PAI_MINOR_TICKS:
				*color = pAxis->GetTicksLine(FALSE).GetColor();
				break;
			
			case PAI_MAJOR_DIGITS:
				*color = pAxis->GetDigitsColor(TRUE);
				break;
			
			case PAI_MINOR_DIGITS:
				*color = pAxis->GetDigitsColor(FALSE);
				break;
			
			case PAI_MAJOR_GRID_LINES:
				*color = pAxis->GetGridLine(TRUE).GetColor();
				break;
			
			case PAI_MINOR_GRID_LINES:
				*color = pAxis->GetGridLine(FALSE).GetColor();
				break;
			
			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	//TODO: Level line & region processing

	default:
		
		if ( pPlot->FindProfile(item) )
		{
			Profile* pProfile = pPlot->FindProfile(item);

			switch (subitem)
			{
			case PRP_MARKSTROKECOLOR:
				*color = pProfile->GetMarks().GetLineColor();
				break;
			case PRP_MARKFILLCOLOR:
				*color = pProfile->GetMarks().GetFillColor();
				break;
			default:
				*color = pProfile->GetLineData().GetColor();

			}
		}
		else if ( pPlot->FindLevelLine(item) )
		{
			LevelLine* pLevelLine = pPlot->FindLevelLine(item);
			*color = pLevelLine->GetLine().GetColor();
			return PE_NOERROR;
		}
		else if ( pPlot->FindRegion(item) )
		{
			PlotRegion* pRgn = pPlot->FindRegion(item);
			COLORREF clr;
			switch (subitem)
			{
			case BGP_BORDER_COLOR:
				*color = pRgn->GetLineData().GetColor();
				break;

			case BGP_HATCH_COLOR:
				*color = pRgn->GetBG().GetHatchColor();
				break;

			case BGP_COLOR_START:
				pRgn->GetBG().GetFillColors(*color, clr);
				break;

			case BGP_COLOR_END:
				pRgn->GetBG().GetFillColors(clr, *color);
				break;
				
			default:
				return PE_INVALID_ARG;
			}

			return PE_NOERROR;
		}
		return PE_INVALID_ARG;
	}

	return PE_NOERROR;
}

/*Задать значение элемента графика*/
long CALLCONV SetLong(long lPlotID, long item, long subitem, int value)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	switch( item )
	{
	case PI_BACKGROUND:
	case PI_BACKGROUND_CLIENT:
		{
			BackGround& bk = (item == PI_BACKGROUND) ? *pPlot->GetPlotBG() : *pPlot->GetClientBG();

			switch(subitem)
			{
				case BGP_DIRECTION:
					{
						switch (value)
						{
						case BGP_GRADIENT_VERT:
							bk.SetGradientDirection( TRUE );
							break;
						case BGP_GRADIENT_HORZ:
							bk.SetGradientDirection( FALSE );
							break;
						default:
							return PE_INVALID_ARG;
						}
						bk.SetGradientDirection( value > 0 );
					}
					break;
				default:
					return PE_INVALID_ARG;
			}
			break;
		}
		break;

	case PI_SELECTION:
		{
			switch (subitem) 
			{
			case SP_MODE:
				pPlot->GetSelection()->SetMode(value);
				break;
			case SP_FLASH_SPEED:
				pPlot->GetSelection()->SetFlashSpeed(value);
				break;
			case SP_NOTIFY_MESSAGE:
				pPlot->GetSelection()->SetNotifyMessageID(value);
				break;
			default:
				return PE_INVALID_ARG;
			}
		}
		break;
	case PI_LEGEND:
		{
			switch (subitem) 
			{
			case PLP_ALIGN:
				pPlot->GetLegend()->SetAlign( value );
				break;
			case PLP_LINELENGTH:
				pPlot->GetLegend()->SetLineLength( value );
				break;
			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	case PI_AXIS_LEFT:
	case PI_AXIS_RIGHT:
	case PI_AXIS_TOP:
	case PI_AXIS_BOTTOM:
		{
			Axis *pAxis = pPlot->GetAxis(item);

			switch (subitem)
			{
			case PAI_MAJOR_TICKS_LENGTH:
				pAxis->SetMajorTicksLength( value );
				break;
			
			case PAI_MINOR_TICKS_LENGTH:
				pAxis->SetMinorTicksLength( value );
				break;
			
			case PAI_MAJOR_TICKS_COUNT:
				pAxis->SetMajorTicksCount( value );
				break;
			
			case PAI_MAJOR_TICKS_WIDTH:
				pAxis->GetTicksLine().SetWidth( value );
				break;

			case PAI_MINOR_TICKS_WIDTH:
				pAxis->GetTicksLine(FALSE).SetWidth( value );
				break;
			
			case PAI_MINOR_TICKS_COUNT:
				pAxis->SetMinorTicksCount( value );
				break;
			
			case PAI_LINE_WIDTH:
				pAxis->GetLine().SetWidth( value );
				break;

			case PAI_LINE_STYLE:
				pAxis->GetLine().SetType( value );
				break;

			case PAI_STYLE:
				pAxis->SetStyle( value );
				break;

			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	default:

#pragma region Profile properties		

		Profile* pProfile = pPlot->FindProfile(item);

		if ( pProfile )
		{
			switch (subitem)
			{
			case PRP_WIDTH:
				pProfile->GetLineData().SetWidth(value);
				break;
			case PRP_STYLE:
				pProfile->GetLineData().SetType(value);
				break;
			case PRP_MARKSTYPE:
				pProfile->GetMarks().SetMarkType(value);
				break;
			case PRP_MARKSFREQ:
				pProfile->GetMarks().SetFrequency(value);
				break;
			case PRP_MARKSIZE:
				pProfile->GetMarks().SetSize(value);
				break;
			default:
				return PE_INVALID_ARG;
			}
			return PE_NOERROR;
		}
#pragma endregion 

#pragma region Region properties

		PlotRegion* pRegion = pPlot->FindRegion(item);

		if ( pRegion )
		{
			switch (subitem)
			{
			//толщина границы региона
			case BGP_BORDER_WIDTH:
				pRegion->GetLineData().SetWidth(value);
				break;

			//стиль границы региона
			case BGP_BORDER_STYLE:
				pRegion->GetLineData().SetType(value);
				break;

			case BGP_HATCH_STYLE:
				pRegion->GetBG().SetHatchStyle(value);
				break;
			
			case BGP_DIRECTION:
				{
					BackGround& bk = pRegion->GetBG();

					switch (value)
					{
					case BGP_GRADIENT_VERT:
						bk.SetGradientDirection( TRUE );
						break;
					case BGP_GRADIENT_HORZ:
						bk.SetGradientDirection( FALSE );
						break;
					default:
						return PE_INVALID_ARG;
					}
					bk.SetGradientDirection( value > 0 );
				}

			default:
				return PE_INVALID_ARG;
			}
			return PE_NOERROR;
		}

#pragma endregion

	}

	pPlot->Update();

	return PE_NOERROR;
}

long CALLCONV GetLong(long lPlotID, long item, long subitem, int* value)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	switch( item )
	{
	case PI_BACKGROUND:
	case PI_BACKGROUND_CLIENT:
		{
			BackGround& bk = (item != PI_BACKGROUND) ? *pPlot->GetClientBG() : *pPlot->GetPlotBG();

			switch(subitem)
			{
				case BGP_DIRECTION:
					{
						*value = bk.GetGradientDirection() ? BGP_GRADIENT_VERT : BGP_GRADIENT_HORZ;
					}
					break;
				default:
					return PE_INVALID_ARG;
			}
			break;
		}
		break;

	case PI_SELECTION:
		{
			switch (subitem) 
			{
			case SP_MODE:
				*value = pPlot->GetSelection()->GetMode();
				break;
			case SP_FLASH_SPEED:
				*value = pPlot->GetSelection()->GetFlashSpeed();
				break;
			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	case PI_LEGEND:
		{
			switch (subitem) 
			{
			case PLP_ALIGN:
				*value = pPlot->GetLegend()->GetAlign();
				break;
			case PLP_LINELENGTH:
				*value = pPlot->GetLegend()->GetLineLength();
				break;
			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	case PI_AXIS_LEFT:
	case PI_AXIS_RIGHT:
	case PI_AXIS_TOP:
	case PI_AXIS_BOTTOM:
		{
			Axis *pAxis = pPlot->GetAxis(item);

			switch (subitem)
			{
			case PAI_MAJOR_TICKS_LENGTH:
				*value = pAxis->GetMajorTicksLength();
				break;
			
			case PAI_MINOR_TICKS_LENGTH:
				*value = pAxis->GetMinorTicksLength();
				break;
			
			case PAI_MAJOR_TICKS_COUNT:
				*value = pAxis->GetMajorTicksCount();
				break;
			
			case PAI_MINOR_TICKS_COUNT:
				*value = pAxis->GetMinorTicksCount();
				break;
			
			case PAI_MAJOR_TICKS_WIDTH:
				*value = pAxis->GetTicksLine().GetWidth();
				break;

			case PAI_MINOR_TICKS_WIDTH:
				*value = pAxis->GetTicksLine(FALSE).GetWidth();
				break;
			
			case PAI_LINE:
			case PAI_LINE_WIDTH:
				*value = pAxis->GetLine().GetWidth();
				break;

			case PAI_LINE_STYLE:
				*value = pAxis->GetLine().GetType();
				break;

			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	default:
		Profile* pProfile = pPlot->FindProfile(item);		
		if ( pProfile )
		{
			switch (subitem)
			{
			case PRP_WIDTH:
				*value = pProfile->GetLineData().GetWidth();
				break;
			case PRP_STYLE:
				*value = pProfile->GetLineData().GetType();
				break;
			case PRP_MARKSTYPE:
				*value = pProfile->GetMarks().GetMarkType();
				break;
			case PRP_MARKSFREQ:
				*value = pProfile->GetMarks().GetFrequency();
				break;
			case PRP_MARKSIZE:
				*value = pProfile->GetMarks().GetSize();
				break;
			default:
				return PE_INVALID_ARG;
			}
		}

		PlotRegion* pRegion = pPlot->FindRegion(item);

		if ( pRegion )
		{
			switch (subitem)
			{

			case BGP_BORDER_WIDTH:				
				*value = pRegion->GetLineData().GetWidth();
				break;

			case BGP_BORDER_STYLE:
				*value = pRegion->GetLineData().GetType();
				break;

			case BGP_DIRECTION:
				*value = (long)(pRegion->GetBG().GetGradientDirection());
				break;

			case BGP_HATCH_STYLE:
				*value = (long)pRegion->GetBG().GetHatchStyle();
				break;
			

			default:
				return PE_INVALID_ARG;

			}
			return PE_NOERROR;
		}
	}

	return PE_NOERROR;
}

/*Задать значение элемента графика*/
long CALLCONV SetDouble(long lPlotID, long item, long subitem, double value)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	switch( item )
	{
	case PI_AXIS_LEFT:
	case PI_AXIS_RIGHT:
	case PI_AXIS_TOP:
	case PI_AXIS_BOTTOM:
		{
			Axis *pAxis = pPlot->GetAxis(item);

			switch (subitem)
			{
			case PAI_LOWER_LIMIT:
				{
					double min, max;
					pAxis->GetRange( min, max );
					pAxis->SetRange( value, max );
				}
				break;
			case PAI_UPPER_LIMIT:
				{
					double min, max;
					pAxis->GetRange( min, max );
					pAxis->SetRange( min, value );
				}
				break;
			
			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	default:

		PlotRegion* pRgn = pPlot->FindRegion(item);

		if ( pRgn )
		{
			switch (subitem)
			{
			case BGP_OPACITY_START:
				{
					double opFrom, opTo;
					BackGround& bg = pRgn->GetBG();
					bg.GetOpacity(opFrom, opTo);
					bg.SetOpacity(value, opTo);
				}
			case BGP_OPACITY_END:
				{
					double opFrom, opTo;
					BackGround& bg = pRgn->GetBG();
					bg.GetOpacity(opFrom, opTo);
					bg.SetOpacity(opFrom, value);
				}

			default:
				return PE_INVALID_ARG;
			}

			return PE_NOERROR;
		}

		return PE_INVALID_ARG;
	}

	pPlot->Update();

	return PE_NOERROR;
}

long CALLCONV GetDouble(long lPlotID, long item, long subitem, double* value)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	switch( item )
	{
	case PI_AXIS_LEFT:
	case PI_AXIS_RIGHT:
	case PI_AXIS_TOP:
	case PI_AXIS_BOTTOM:
		{
			Axis *pAxis = pPlot->GetAxis(item);

			switch (subitem)
			{
			case PAI_LOWER_LIMIT:
				{
					double max;
					pAxis->GetRange(*value, max);
					break;
				}
			
			case PAI_UPPER_LIMIT:
				{
					double min;
					pAxis->GetRange(min, *value);
					break;
				}
			
			default:
				return PE_INVALID_ARG;
			}
		}
		break;

	default:
		
		PlotRegion* pRgn = pPlot->FindRegion(item);
		
		if ( pRgn )
		{
			switch (subitem)
			{
			case BGP_OPACITY_START:
				{
					double op;
					BackGround& bg = pRgn->GetBG();
					bg.GetOpacity(*value, op);
				}
			case BGP_OPACITY_END:
				{
					double op;
					BackGround& bg = pRgn->GetBG();
					bg.GetOpacity(op, *value);
				}
			default:
				return PE_INVALID_ARG;
			}

			return PE_NOERROR;
		}


		return PE_INVALID_ARG;
	}

	return PE_NOERROR;
}

long CALLCONV EnableItem(long lPlotID, long item, long subitem, BOOL enabled)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	switch (item)
	{

	case PI_CANVAS:
		pPlot->SetRedraw( enabled );
		break;

	case PI_BORDER:
		pPlot->GetBorder()->Enable( enabled );
		break;

	case PI_TITLE:
		pPlot->GetTitle()->Enable( enabled );
		break;

	case PI_LEGEND:
		pPlot->GetLegend()->Enable( enabled );
		break;

	case PI_AXIS_TOP:
	case PI_AXIS_BOTTOM:
	case PI_AXIS_LEFT:
	case PI_AXIS_RIGHT:
		{
			Axis* pAxis = pPlot->GetAxis(item);
			if ( !pAxis )
				return PE_INVALID_ARG; 

			switch (subitem)
			{
			case PAI_TITLE:
				pAxis->ModifyStyle(AF_AXIS_TITLED, enabled);
				break;

			case PAI_MAJOR_TICKS:
				pAxis->ModifyStyle(AF_AXIS_MAJOR_TICKS, enabled);
				break;

			case PAI_MINOR_TICKS:
				pAxis->ModifyStyle(AF_AXIS_MINOR_TICKS, enabled);
				break;
			
			case PAI_MAJOR_DIGITS:
				pAxis->ModifyStyle(AF_AXIS_MAJOR_LABELS, enabled);
				break;

			case PAI_MINOR_DIGITS:
				pAxis->ModifyStyle(AF_AXIS_MINOR_LABELS, enabled);
				break;
			
			case PAI_MAJOR_GRID_LINES:
				pAxis->ModifyStyle(AF_AXIS_MAJOR_GRIDLINES, enabled);
				break;

			case PAI_MINOR_GRID_LINES:
				pAxis->ModifyStyle(AF_AXIS_MINOR_GRIDLINES, enabled);
				break;

			case PAI_AUTOSCALE:
				pAxis->ModifyStyle(AF_AXIS_AUTOSCALED, enabled);
				break;

			case PAI_LOG10:
				pAxis->ModifyStyle(AF_AXIS_LOG_MODE, enabled);
				break;

			case PAI_TIME_HISTORY:
				pAxis->ModifyStyle(AF_AXIS_TIME_HISTORY, enabled);
				break;

			default :
				return PE_INVALID_ARG;
			}
		}
		break;
	
	default:
		return PE_INVALID_ARG;
	}

	return PE_NOERROR;
}

long CALLCONV IsEnabled(long lPlotID, long item, long subitem, BOOL* enabled)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	switch (item)
	{
	case PI_BORDER:
		*enabled = pPlot->GetBorder()->IsEnabled();
		break;

	case PI_TITLE:
		*enabled = pPlot->GetTitle()->IsEnabled();
		break;

	case PI_LEGEND:
		*enabled = pPlot->GetLegend()->IsEnabled();
		break;

	case PI_AXIS_TOP:
	case PI_AXIS_BOTTOM:
	case PI_AXIS_LEFT:
	case PI_AXIS_RIGHT:
		{
			Axis* pAxis = pPlot->GetAxis(item);
			if ( !pAxis )
				return PE_INVALID_ARG; 

			switch (subitem)
			{
			case PAI_TITLE:
				*enabled = pAxis->IsLabelEnabled();
				break;

			case PAI_MAJOR_TICKS:
				*enabled = pAxis->IsMajorTicksEnabled();
				break;

			case PAI_MINOR_TICKS:
				*enabled = pAxis->IsMinorTicksEnabled();
				break;
			
			case PAI_MAJOR_DIGITS:
				*enabled = pAxis->IsMajorDigitsEnabled();
				break;

			case PAI_MINOR_DIGITS:
				*enabled = pAxis->IsMinorDigitsEnabled();
				break;
			
			case PAI_MAJOR_GRID_LINES:
				*enabled = pAxis->IsMajorGridLinesEnabled();
				break;

			case PAI_MINOR_GRID_LINES:
				*enabled = pAxis->IsMinorGridLinesEnabled();
				break;

			case PAI_AUTOSCALE:
				*enabled = pAxis->IsAutoScaled();
				break;

			case PAI_LOG10:
				*enabled = pAxis->IsLogarithmic();
				break;

			case PAI_TIME_HISTORY:
				*enabled = pAxis->IsTimeHistory();
				break;

			default :
				return PE_INVALID_ARG;
			}
		}
		break;
	
	default:
		return PE_INVALID_ARG;
	}

	return PE_NOERROR;
}

long CALLCONV SaveProperties(long lPlotID, const char* fileName)
{
	DNP_CHECKPOINT_BEGIN(5, SaveProperties)
	DNP_CHECKPOINT_END

	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{ 
		pPlot->SaveProperties( fileName );
	}	
	catch( ... ) 
	{
		return PE_UNEXPECTED; 
	}

	return PE_NOERROR;
}

long CALLCONV LoadProperties(long lPlotID, const char* fileName)
{
	DNP_CHECKPOINT_BEGIN(6, LoadProperties)
	DNP_CHECKPOINT_END

	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{ 
		pPlot->LoadProperties( fileName );		
	}	
	catch( ... ) 
	{ 
		return PE_UNEXPECTED;
	}

	return PE_NOERROR;
}

long CALLCONV SetRedraw(long lPlotID, BOOL bRedraw)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	pPlot->SetRedraw( bRedraw );

	return PE_NOERROR;
}

long CALLCONV RegRestoreDefaults(long lPlotID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{
		pPlot->RegLoadSettings( NULL, NULL );
	}	
	catch( ... ) 
	{ 
		return PE_UNEXPECTED; 
	}

	return PE_NOERROR;
}

long CALLCONV RegLoadSettings(long lPlotID, const char* keyString, const char* pszValue)
{
	DNP_CHECKPOINT_BEGIN(7, RegLoadSettings)
	DNP_CHECKPOINT_END

	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{		
		pPlot->RegLoadSettings( keyString, pszValue );
	}	
	catch( ... ) 
	{ 
		return PE_UNEXPECTED; 
	}

	return PE_NOERROR;
}

long CALLCONV RegSaveSettings(long lPlotID, const char* keyString, const char* pszValue)
{
	DNP_CHECKPOINT_BEGIN(8, RegSaveSettings)
	DNP_CHECKPOINT_END

	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{
		pPlot->RegSaveSettings( keyString , pszValue);
	}	
	catch( ... )
	{ 
		return PE_UNEXPECTED; 
	}

	return PE_NOERROR;
}

//////////////////////////////////////////////////////////////////////////
/// Properties dialog

typedef long (__stdcall *LPFNDIALOG)(HWND hwndMain, long ID);

#ifdef _M_X64
	#define EXTFUNNAME "Run"
#else
	#define EXTFUNNAME "_Run@8"
#endif

long CALLCONV RunDialog(long lPlotID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	HMODULE hmod = LoadLibrary("xyplotdlg.dll");
	if (!hmod)
		return PE_DIALOG_DLL_NOT_FOUND;

	try {
		LPFNDIALOG m_lpfnRun = (LPFNDIALOG) GetProcAddress(hmod, EXTFUNNAME);
		if (!m_lpfnRun)
			return PE_INVALID_DIALOG_DLL;

		m_lpfnRun(pPlot->GetParent(), lPlotID);
	}
	catch (...)
	{
		return PE_INVALID_DIALOG_DLL;
	}

	FreeLibrary(hmod);
	
	return PE_NOERROR;
}

//////////////////////////////////////////////////////////////////////////
/// Region functions
long CALLCONV AddRegion(long lPlotID, long AxisX, long AxisY, double xFrom, double xTo, double yFrom, double yTo, const char* pszTitle, long* lRegionID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	return pPlot->AddRegion(AxisX, AxisY, lRegionID, xFrom, xTo, yFrom, yTo, pszTitle);
}

long CALLCONV DeleteRegion(long lPlotID, long lRegionID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	return pPlot->DeleteRegion ( lRegionID );	

}

long CALLCONV SetRegion(long lPlotID, long lRegionID, double x1, double x2, double y1, double y2)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	return pPlot->SetRegionLimits( lRegionID, x1, x2, y1, y2 );	
}

//////////////////////////////////////////////////////////////////////////
/// Level functions
long CALLCONV AddLevel(long lPlotID, long axis, double value, const char* pszTitle, COLORREF clr, int width, long* lLevelID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	return pPlot->AddLevelLine(pszTitle, clr, lLevelID, width, axis, value);
}

long CALLCONV DeleteLevel(long lPlotID, long lLevelID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	return pPlot->DeleteLevelLine(lLevelID);
}

long CALLCONV SetLevel(long lPlotID, long lLevelID, double value)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	return pPlot->SetLevelValue(lLevelID, value);
}

long CALLCONV CreateGroup(long lPlotID, long* pGroupID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{ 
		*pGroupID = pPlot->GetSelection()->CreateGroup();
	}
	catch( ... ) 
	{ 
		return PE_UNEXPECTED; 
	}

	return PE_NOERROR;
}

long CALLCONV DeleteGroup(long lPlotID, long pGroupID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{
		pPlot->GetSelection()->DeleteGroup(pGroupID);
	}
	catch( ... ) 
	{ 
		return PE_UNEXPECTED; 
	}

	return PE_NOERROR;
}

long CALLCONV BindProfile(long lPlotID, long lGroupID, long lProfileID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{
		pPlot->GetSelection()->AddProfile(lGroupID, lProfileID);
	}	
	catch( ... ) 
	{
		return PE_UNEXPECTED; 
	}

	return PE_NOERROR;
}

long CALLCONV GetProfileList(long lPlotID, long* lSize, long* lProfileIDs)
{
	if (!lSize)
		return PE_INVALID_ARG;

	DECLARE_PLOT_PTR(pPlot, lPlotID);
	
	const ProfileMap& pm = pPlot->GetProfileMap();

	if ( *lSize < (long)pm.size() ) 
	{
		if (lProfileIDs)
			return PE_INVALID_ARG;
		else{
			*lSize = (long)pm.size();
			return PE_NOERROR;
		}
	}

	for(ProfileMap::const_iterator it = pm.begin(); it != pm.end(); ++it)
	{
		*lProfileIDs = (*it).first;
		lProfileIDs++;
	}
	return PE_NOERROR;
}

long CALLCONV Select(long lPlotID, long ID, long index)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	if (pPlot->GetSelection()->Select(ID, index))
		return PE_NOERROR;

	return PE_INVALID_ARG;
}

long CALLCONV GetSelection(long lPlotID, long* ID)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	*ID = pPlot->GetSelection()->GetSelection();

	return PE_NOERROR;
}

long CALLCONV SetFlashSpeed(long lPlotID, int speed)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);

	pPlot->GetSelection()->SetFlashSpeed( speed );

	return PE_NOERROR;
}

long CALLCONV DrawToBitmap(long lPlotID, HBITMAP hBmp, bool bDrawWindowBG, bool bDrawClientBG)
{
	DECLARE_PLOT_PTR(pPlot, lPlotID);
	pPlot->DrawToBitmap(hBmp, bDrawWindowBG, bDrawClientBG);
	return PE_NOERROR;
}
///////////////////////////////////////////////////////////////////
// Convert DDB into DIB
///////////////////////////////////////////////////////////////////

HANDLE DDBToDIB(HBITMAP bitmap, DWORD dwCompression, HPALETTE hPal, int dpi) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;

	assert(bitmap != NULL);

	// The function has no arg for bitfields
	if (dwCompression == BI_BITFIELDS) 
		return NULL;

	// If a palette has not been supplied use defaul palette
	if (hPal==NULL) 
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	GetObject(bitmap, sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize			= sizeof(BITMAPINFOHEADER);
	bi.biWidth			= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= int((double)dpi/2.54f*100);
	bi.biYPelsPerMeter	= int((double)dpi/2.54f*100);
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if (nColors > 256) nColors = 0;
	dwLen = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB) {
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;
	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, bitmap, 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0) {
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) * bi.biHeight;
		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB) bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else {
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, bitmap,
		0L,				// Start scan line
		(DWORD)bi.biHeight,		// # of scan lines
		(LPBYTE)lpbi 			// address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,		// address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if(!bGotBits) {
		GlobalFree(hDIB);

		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}

long CALLCONV CopyToClipboard(long lPlotID, int width, int height, int dpi)
{
	assert(false);

	DECLARE_PLOT_PTR(pPlot, lPlotID);

	try
	{
		RECT rc, rcOld;

		HDC hdc = GetDC( pPlot->GetParent() );

		int nLogWidth = (int)((double)width/254 * dpi);
		int nLogHeight = (int)((double)height/254 * dpi);

		HDC hdcClip = CreateCompatibleDC( hdc );

		int nDCWidth  = ::GetDeviceCaps(hdcClip, HORZRES);
		int nDCHeight = ::GetDeviceCaps(hdcClip, VERTRES);

		HBITMAP hbmpClip = CreateCompatibleBitmap( hdc, nLogWidth, nLogHeight );

		ReleaseDC( pPlot->GetParent(), hdc );

		if( !hbmpClip )
		{
			DeleteDC( hdcClip );
			return -1; //E_ABORT;
		}

		HBITMAP hold = (HBITMAP)SelectObject( hdcClip, hbmpClip );

		rc.left = rc.top = 0;
		rc.right = nLogWidth;
		rc.bottom = nLogHeight;

		if( rc.right > nDCWidth )
			rc.right = nDCWidth;
		if( rc.bottom > nDCHeight )
			rc.bottom = nDCHeight;

		rcOld = pPlot->TotalRect();
		pPlot->TotalRect() = rc;	

		pPlot->PushClipboard( hdcClip );

		SelectObject( hdcClip, hold );
		pPlot->TotalRect() = rcOld;		

		// Create logical palette if source device support a palette
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;
		pLP->palNumEntries = GetSystemPaletteEntries( hdcClip, 0, 255, pLP->palPalEntry);

		// Create the palette
		HPALETTE pal = CreatePalette(pLP);
		delete [] pLP;

		// Convert the bitmap to a DIB
		HANDLE hDIB = DDBToDIB(hbmpClip, BI_RGB, pal, dpi);

		OpenClipboard( pPlot->GetParent() );
		SetClipboardData( CF_DIB, hDIB );
		CloseClipboard();
		
		DeleteObject(hbmpClip);
		DeleteDC( hdcClip );
	}
	catch( ... ) 
	{
		return PE_UNEXPECTED; 
	}

	return PE_NOERROR;
}
