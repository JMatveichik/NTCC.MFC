#pragma once

class CFSDoc;
class CDrawObj;
class DiscreteDataSrc;

typedef CTypedPtrList < CObList, CDrawObj* > CDrawObjList;

class FSLayer
{

public:

	FSLayer(CFSDoc* parent);
	virtual ~FSLayer(void);

	BOOL CreateFromXML(IXMLDOMNode* pNode);

public:

	//отрисовка слоя
	virtual void Draw(Gdiplus::Graphics& gr);

	//показать слой 
	void Show();

	//спрятать слой
	void Hide();

	BOOL IsVisible() const;

	BOOL IsActive() const;
	void SetActive(BOOL bActive = TRUE);

	CDrawObj* HitTest(CPoint& pt) const;
	 
	int GetItemCount() const;

	std::string GetDefault(const char* pszName) const;
	

	CFSDoc* const GetParent() const;
	CView* GetParentView() const;

	const Gdiplus::PointF& GetBase() const;
	
	std::string Name() const;

private:

	CFSDoc* m_pParentDoc;
	CDrawObjList items;

	const DiscreteDataSrc* pVisDS;
	BOOL visState;

	BOOL		 isVisible;
	BOOL		 isActive;

	std::string strName;

	Gdiplus::PointF basePos;
	Gdiplus::PointF maxPos;
	Gdiplus::PointF minPos;

	std::map<std::string, std::string> defaults;
};

