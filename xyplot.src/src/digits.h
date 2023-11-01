#ifndef __DIGITS_H_INCLUDED__
#define __DIGITS_H_INCLUDED__

#include <string>
#include <vector>
#include "SavableItem.h"

class Axis; 
class XYPlot;

class Digits : public SavableItem
{
public:
	
	Digits(XYPlot* parent, Axis* parentAxis);
	virtual ~Digits();
	
	void SetColor(COLORREF color, BOOL bMajor = TRUE);
	COLORREF GetColor(BOOL bMajor = TRUE) const;

	void SetRect(int left, int top, int right, int bottom);
	inline void SetRect(RECT self) { m_self = self; }
	inline void GetRect(RECT &self) const { self = m_self; }

	void SetTemplate(std::string strTemplate, BOOL bMajor = TRUE);
	const std::string& GetTemplate(BOOL bMajor = TRUE) const;

	void SetFont(const LOGFONT& lf, BOOL bMajor = TRUE);
	void GetFont(LOGFONT& lf, BOOL bMajor = TRUE) const;

	int CalcRect(HDC hDC, RECT& rc) const;

	inline void EnableMajor(BOOL enable) { m_enabledMajor = enable; }
	inline void EnableMinor(BOOL enable) { m_enabledMinor = enable; }

	inline BOOL IsEnabledMajor() const { return m_enabledMajor; }
	inline BOOL IsEnabledMinor() const { return m_enabledMinor; }
	
	int MaxItemLength(HDC hdc, double fmin, double fmax) const;

	inline void SetRange(double fLower, double fUpper) { m_fLower = fLower; m_fUpper = fUpper; }
	inline void GetRange(double& fLower, double& fUpper) const { fLower = m_fLower; fUpper = m_fUpper; }
	
	void PreDraw( HDC hdc );
	void OnDraw( HDC hdc );

	void OptimizeTicks(double &fmin, double &fmax, int &majorCount, int &minorCount);

	virtual BOOL Write(HANDLE hFile) const;
	virtual BOOL Read(HANDLE hFile);

protected:

	void PushBackMajor(std::string s);
	void PushBackMinor(std::string s);

	void Beautify(std::string& s);
	void Clear();

	BOOL	m_enabledMajor;
	BOOL	m_enabledMinor;

	int  offsetFromTicks;

	RECT m_self;
	
	std::string majorTemplate;
	std::string minorTemplate;
	
	std::vector< std::string > m_majorDigitsVector;
	std::vector< std::string > m_minorDigitsVector;

	double m_fLower;
	double m_fUpper;

	unsigned nFontMinor;
	unsigned nFontMajor;

	COLORREF crMinor;
	COLORREF crMajor;

	Axis* parentAxis;	//Ось которой принадлежат подписи к делениям
};

#endif