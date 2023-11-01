#ifndef __LEGEND_H_INCLUDED__
#define __LEGEND_H_INCLUDED__

#include "SavableItem.h"
#include "BackGroundFill.h"
#include "Border.h"

#include "profile.h"

class RTFString;
class XYPlot;

struct profile_less
{
	bool operator()(const Profile* lhs, const Profile* rhs) const
	{
		return strcmp(lhs->GetName().c_str(), rhs->GetName().c_str()) < 0;
	}
};

class Legend : public SavableItem
{
public:
	Legend(XYPlot* parent);
	virtual ~Legend();

	inline BOOL IsEnabled() const { return m_enabled; }
	inline void Enable(BOOL enable) { m_enabled = enable; }

	inline BOOL IsHidden() const { return m_hidden; }
	inline void Hide(BOOL hide) { m_hidden = hide;	}

	inline void SetAlign(int align) { m_align = align; }
	inline int GetAlign() const { return m_align; }

	inline void SetRect(const RECT& self) { m_self = self; }
	void SetRect(int left, int top, int right, int bottom);
	inline void GetRect(RECT& self) const { self = m_self; }

	inline void SetCellSize(int width, int height) { m_cellWidth = width; m_cellHeight = height; }
	inline void GetCellSize(int &width, int &height) const { width = m_cellWidth; height = m_cellHeight; }

	inline void SetLineLength(int length) { m_lineLength = length; }
	inline int GetLineLength() const { return m_lineLength; }

	void SetFont(const LOGFONT* plf);

	void PreDraw(HDC hdc, RECT *ptotal);
	void OnDraw(HDC hdc);

	PROFILE_KEY HitTest(const POINT& ptHit);
 
	BOOL PointInRect(const POINT& pt) { return PtInRect(&m_self, pt); };

	virtual BOOL Write(HANDLE hFile) const;
	virtual BOOL Read(HANDLE hFile);

protected:
	void Clear();
	
	int m_LegendOffset;

	RTFString* m_basicEstr;

	std::set<Profile*, profile_less> profiles;
	
	const XYPlot& plot;

	Border		border;
	BackGround  backGround;

	BOOL m_enabled;
	BOOL m_hidden;

	int m_align;
	RECT m_self;
	int m_nLinesCount;

	int m_cellWidth;
	int m_cellHeight;
	int m_lineLength;
};

#endif