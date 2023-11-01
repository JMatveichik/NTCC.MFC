// PlotTesterDoc.h : interface of the CPlotTesterDoc class
//


#pragma once

class CPlotTesterDoc : public CDocument
{
protected: // create from serialization only
	CPlotTesterDoc();
	DECLARE_DYNCREATE(CPlotTesterDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CPlotTesterDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


