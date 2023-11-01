#ifndef __BORDER_H_INCLUDED__
#define __BORDER_H_INCLUDED__

#include "linedata.h"
#include "SavableItem.h"

class Border : public SavableItem
{
public:
	Border();
	virtual ~Border();
	
	LineData& GetLineData() { return m_line; };
	void SetLineData(const LineData& line) { m_line = line;};
	void SetLineData(COLORREF clr, int nWidth, int nType, std::string  templ);	

	void SetRect( const RECT& self ) { m_self = self; }
	void GetRect( RECT &self ) const { self = m_self; }

	void Enable( BOOL enable ) { m_enabled = enable; }
	BOOL IsEnabled() const { return m_enabled; }

	void OnDraw( HDC hdc );

	virtual BOOL Write(HANDLE hFile) const;
	virtual BOOL Read(HANDLE hFile);

protected:
	int m_width;
	BOOL m_enabled;
	RECT m_self;
	LineData m_line;
};

#endif