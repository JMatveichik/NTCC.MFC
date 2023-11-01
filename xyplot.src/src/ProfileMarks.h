#pragma once
#include "SavableItem.h"

class ProfileMarks : public SavableItem
{
public:
	

	ProfileMarks(void);
	~ProfileMarks(void);

	void OnDraw(HDC hdc, const POINT* pPoints, unsigned nCount);
	void PreDraw(HDC hdc);

	int GetFrequency() const { return m_nFrequency; };
	void SetFrequency(const int nFreq) { m_nFrequency = nFreq; };

	int GetSize() const { return m_nSize; };
	void SetSize(const int nSize)  { m_nSize = nSize; };
	
	COLORREF GetLineColor() const { return m_clrLine; };
	void SetLineColor(const COLORREF clrLine) { m_clrLine = clrLine; };

	COLORREF GetFillColor() const { return m_clrFill; };
	void SetFillColor(const COLORREF clrFill) { m_clrFill = clrFill; };	

	void SetMarkType(const int nType) { m_nType = nType; };
	int  GetMarkType() const { return m_nType; };

	void Enable(BOOL enable = TRUE) { m_bEnabled = enable; };
	BOOL IsEnabled() const { return m_bEnabled; };

	virtual BOOL Write(HANDLE hFile) const;
	virtual BOOL Read(HANDLE hFile);

private:
	BOOL		m_bEnabled;
	int			m_nType;
	int			m_nFrequency;
	int			m_nSize;
	COLORREF	m_clrLine;
	COLORREF	m_clrFill;
};
