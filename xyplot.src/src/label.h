#ifndef __LABEL_H_INCLUDED__
#define __LABEL_H_INCLUDED__

#include <string>
#include "SavableItem.h"

class RTFString;
class XYPlot;

class Label : public SavableItem
{
public:
	Label(XYPlot* parent);
	virtual ~Label();
	
	void SetText(std::string label);
	const std::string& GetText() const;

	void SetRect(int left, int top, int right, int bottom);
	inline void SetRect( RECT self ) { m_self = self; }
	inline void GetRect(RECT &self) const { self = m_self; }
	SIZE GetLabelSize() const;

	void SetStyle(DWORD align);

	void SetFont(const LOGFONT* const plf);
	void SetColor(COLORREF clr);

	inline BOOL IsEnabled() const { return m_enabled; }
	inline void Enable(BOOL enable) { m_enabled = enable; }

	void PreDraw(HDC hdc);
	void OnDraw(HDC hdc);

	virtual BOOL Write(HANDLE hFile) const;
	virtual BOOL Read(HANDLE hFile);

protected:
	DWORD m_align;
	RTFString* m_label;
	RECT m_self;
	BOOL m_enabled;
};

#endif