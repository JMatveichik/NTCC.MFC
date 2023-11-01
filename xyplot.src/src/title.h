#ifndef __TITLE_H_INCLUDED__
#define __TITLE_H_INCLUDED__

#include <windef.h>
#include "SavableItem.h"

class RTFString;
class XYPlot;

class Title : public SavableItem
{
public:
	Title(XYPlot* parent);
	virtual ~Title();
	
	void SetText( std::string title );
	const std::string& GetText() const;

	inline BOOL IsEnabled() const { return m_enabled; }
	inline void Enable(BOOL enable) { m_enabled = enable; }

	void SetRect(int left, int top, int right, int bottom);
	inline void SetRect(const RECT& self) { m_self = self; }
	inline void GetRect(RECT &self) const { self = m_self; }

	void SetColor(COLORREF clr);
	void SetFont(const LOGFONT* const plf);

	void CalcRect(RECT& rect) const;
	void PreDraw(HDC hdc) const;
	void OnDraw(HDC hdc) const;

	virtual BOOL Write(HANDLE hFile) const;
	virtual BOOL Read(HANDLE hFile);

	inline BOOL PointInRect(const POINT& pt) { return PtInRect(&m_self, pt); };

protected:
	RTFString *m_text;

	RECT m_self;
	BOOL m_enabled;
};

#endif