#pragma once


// CAutoCalcSizeDialog dialog

class CAutoCalcSizeDialog : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CAutoCalcSizeDialog)

public:

	CAutoCalcSizeDialog(UINT ID, CWnd* pParent);   // standard constructor
	virtual ~CAutoCalcSizeDialog();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
	CSize m_sz;

public:

	const CSize& GetDlgMinSize() const { return m_sz; };

	virtual BOOL OnInitDialog();
};
