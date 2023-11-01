// PlotTesterDoc.cpp : implementation of the CPlotTesterDoc class
//

#include "stdafx.h"
#include "PlotTester.h"

#include "PlotTesterDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPlotTesterDoc

IMPLEMENT_DYNCREATE(CPlotTesterDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlotTesterDoc, CDocument)
END_MESSAGE_MAP()


// CPlotTesterDoc construction/destruction

CPlotTesterDoc::CPlotTesterDoc()
{
	// TODO: add one-time construction code here

}

CPlotTesterDoc::~CPlotTesterDoc()
{
}

BOOL CPlotTesterDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CPlotTesterDoc serialization

void CPlotTesterDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CPlotTesterDoc diagnostics

#ifdef _DEBUG
void CPlotTesterDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPlotTesterDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPlotTesterDoc commands
