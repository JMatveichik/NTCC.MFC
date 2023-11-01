#pragma once
#include <string>
#include "LineData.h"
#include "Ticks.h"
#include "Digits.h"
#include "Label.h"
#include "Gridlines.h"
#include "CoordConverter.h"
//#include <map>
#include <set>

class RTFString;
class XYPlot;

// axis flags
const unsigned long	AF_AXIS_TITLED			= 0x0001;
const unsigned long	AF_AXIS_MAJOR_TICKS		= 0x0002;
const unsigned long	AF_AXIS_MINOR_TICKS		= 0x0004;
const unsigned long	AF_AXIS_MAJOR_GRIDLINES = 0x0008;
const unsigned long	AF_AXIS_MINOR_GRIDLINES = 0x0010;
const unsigned long	AF_AXIS_MAJOR_LABELS	= 0x0020;
const unsigned long	AF_AXIS_MINOR_LABELS	= 0x0040;
const unsigned long	AF_AXIS_LOG_MODE		= 0x0080;
const unsigned long	AF_AXIS_AUTOSCALED		= 0x0100;
const unsigned long	AF_AXIS_INVISIBLE		= 0x0200;
const unsigned long	AF_AXIS_TIME_HISTORY	= 0x0400;

class XYPlot;
class Profile;

class Axis : public SavableItem
{
public:	
	
	Axis(XYPlot* parent);
	virtual ~Axis();

	BOOL Create(XYPlot* parentPlot, int id, unsigned long lStyles, const char* strTitle = NULL, double dMin = 0.0, double dMax = 1.0);

protected:

	//Размещение оси
	int id;

	// Подпись к оси
	Label*	pLabel;

   	// Тип линии оси
	LineData line;	

	// Тип основные и дополнительные отметки 
	Ticks*	 pTicks;

	// Надписи к отметкам 
	Digits*	 pDigits;
	GridLines* pGridLines;
	
	// 
	double rangeMin;
	double rangeMax;

	//
	BOOL autoScale;

	RECT rcSelf;

	DWORD dwStyle;

	//std::map< long , Profile*  > attachedProfiles;
	std::set< Profile* > attachedProfiles;
	
public :

	virtual BOOL Write(HANDLE hFile) const;
	virtual BOOL Read(HANDLE hFile);

	/***********************/
	/*		Profiles       */
	/***********************/
	BOOL AttachProfile(Profile* prof);
	BOOL DetachProfile(Profile* prof);
	
	//std::map< long , Profile*  > GetAttachedProfiles() const { return attachedProfiles; };	
	
	int GetID() const { return id; }

	XYPlot*	parentPlot;

	SIZE  GetSize() const; 
	int Length() const;

	//int GetOrientation() const;
	//const unsigned long GetAlign() const { return align; };
	
	BOOL IsTimeHistory() const { return (dwStyle & AF_AXIS_TIME_HISTORY) != 0; }

	BOOL PointInRect(const POINT& pt) {  return PtInRect(&rcSelf, pt); };

	// Тип линии оси
	LineData& GetLine() { return line; };
	void SetLine(const LineData& line) { this->line = line; };

	LineData& GetGridLine(BOOL bMajor = TRUE) { return pGridLines->GetLine(bMajor); };
	void SetGridLine(const LineData& line, BOOL bMajor = TRUE) { return pGridLines->SetLine(line, bMajor); };

	LineData& GetTicksLine(BOOL bMajor = TRUE){ return pTicks->GetLine(bMajor); };
	void SetTicksLine(const LineData& line, BOOL bMajor = TRUE){ return pTicks->SetLine(line, bMajor); };

	DWORD  SetStyle(DWORD dwStyle, BOOL bUpdate = TRUE);
	DWORD  ModifyStyle(DWORD dwStyle, BOOL bAdd = TRUE, BOOL bUpdate = TRUE);	
	DWORD  GetStyle() const { return dwStyle ; };

	BOOL IsAutoScaled() const  { return (dwStyle & AF_AXIS_AUTOSCALED) == AF_AXIS_AUTOSCALED; };
	BOOL IsLogarithmic() const  { return (dwStyle & AF_AXIS_LOG_MODE) == AF_AXIS_LOG_MODE; };
    
	/*Хранилище координат основных делений*/
	const std::vector<POINT>& GetMajorTicksPos() const { return pTicks->GetMajorTicksPos(); };	

	/*Хранилище координат дополнительных делений*/
	const std::vector<POINT>& GetMinorTicksPos() const { return pTicks->GetMinorTicksPos(); };	

	/*Хранилище физических координат основных делений*/
	const std::vector<double>& GetMajorTicksValues() const { return pTicks->GetMajorTicksValues(); };	

	/*Хранилище физических координат дополнительных делений*/
	const std::vector<double>& GetMinorTicksValues() const { return pTicks->GetMinorTicksValues(); };	

	BOOL GetNearestTick(double& val, POINT& pt) const { return pTicks->GetNearestTick(val, pt); };
	
	const int GetTicksOffset() const { return pTicks->GetMaxLength(); };
	
	void SetRect(HDC hDC, RECT rect);
	
	/* Функции для работы с подписью к оси */
	//EnhancedString& GetLabelEnhancedString() {return pLabel->GetEnhancedString();};
	
	void SetLabel(std::string text) { pLabel->SetText(text); }; 
	const std::string& GetLabel() const { return pLabel->GetText(); } ;

	void SetLabelFont(const LOGFONT* const plf) { pLabel->SetFont(plf); }
	void SetLabelColor(COLORREF clr) { pLabel->SetColor(clr); }

	//void SetLabelColor(COLORREF color) { pLabel->SetColor(color); };
	//COLORREF GetLabelColor() const { pLabel->GetColor(); };	

	//void SetLabelFontStyle(unsigned long style) { pLabel->SetFontAttributes(style); };
	//unsigned long  GetLabelFontStyle() const { return pLabel->GetFontAttributes(); };

	void EnableLabel(BOOL enable) { pLabel->Enable(enable); }
	BOOL IsLabelEnabled() const { return pLabel->IsEnabled(); }

	/* Функции для работы с линией оси */

	
	/* Функции для работы с метками оси */
	void SetMajorTicksLength(int length)    { pTicks->SetMajorLength( length ); };
	int  GetMajorTicksLength() const		{ return pTicks->GetMajorLength(); };

	void SetMinorTicksLength(int length)    { pTicks->SetMinorLength( length ); };
	int  GetMinorTicksLength() const		{ return pTicks->GetMinorLength(); };

	void SetMajorTicksCount(int count)  { pTicks->SetMajorCount( count ); };
	int  GetMajorTicksCount() const		{ return pTicks->GetMajorCount(); };

	void SetMinorTicksCount(int count)  { pTicks->SetMinorCount( count ); };
	int  GetMinorTicksCount() const		{ return pTicks->GetMinorCount(); };

	void EnableMajorTicks(BOOL enable)	{ pTicks->EnableMajor( enable) ; };
	BOOL IsMajorTicksEnabled() const	{ return pTicks->IsMajorEnabled(); };

	void EnableMinorTicks(BOOL enable)	{ pTicks->EnableMinor( enable) ; };
	BOOL IsMinorTicksEnabled() const	{ return pTicks->IsMinorEnabled(); };

	unsigned long GetTicksStyle() const		{ return pTicks->GetStyle(); };
	void SetTicksStyle(unsigned long style) { pTicks->SetStyle(style); };

	/* Функции для работы с линиями сетки */
	
	void EnableMajorGridLines(BOOL enable) { pGridLines->EnableMajor( enable); };
	BOOL IsMajorGridLinesEnabled() const { return pGridLines->IsMajorEnabled(); }
//
	void EnableMinorGridLines(BOOL enable) { pGridLines->EnableMinor( enable); };
	BOOL IsMinorGridLinesEnabled() const { return pGridLines->IsMinorEnabled(); }

	/* Функции для работы с подписями меток */

	void SetDigitsColor(COLORREF color, BOOL bMajor) { pDigits->SetColor(color, bMajor); };
	COLORREF GetDigitsColor(BOOL bMajor) const		{ return pDigits->GetColor(bMajor); };
	
	inline void SetDigitsFormat(std::string strTemplate, BOOL bMajor = TRUE) { pDigits->SetTemplate(strTemplate, bMajor); };
	inline const std::string& GetDigitsFormat(BOOL bMajor = TRUE) const { return pDigits->GetTemplate(bMajor); }

	//void SetDigitsFontStyle(unsigned long lFontStyle) { pDigits->SetFontAttributes(lFontStyle); };
	//unsigned long GetDigitsFontStyle() const		  { return pDigits->GetFontAttributes(); };
	void SetDigitsFont(const LOGFONT& lf, BOOL bMajor = TRUE) { pDigits->SetFont(lf, bMajor); }
	void GetDigitsFont(LOGFONT& lf, BOOL bMajor = TRUE) const { pDigits->GetFont(lf, bMajor); }

	inline void EnableMajorDigits(BOOL enable) { pDigits->EnableMajor( enable); };
	inline void EnableMinorDigits(BOOL enable) { pDigits->EnableMinor( enable); };

	inline BOOL IsMajorDigitsEnabled() const { return pDigits->IsEnabledMajor(); }
	inline BOOL IsMinorDigitsEnabled() const { return pDigits->IsEnabledMinor(); }

	/**/
	void SetRange(double dMin, double dMax);
	void GetRange(double &dMin, double &dMax) const { dMin = rangeMin; dMax = rangeMax; };	

	RECT PreDraw(HDC hDC, RECT rect);
	void OnDraw(HDC hDC);

	void GetClientRect(RECT &rect) const;
	void GetLimits(LPAXISLIMITS lpal) const;


private:

	void OptimizeScale();

};
