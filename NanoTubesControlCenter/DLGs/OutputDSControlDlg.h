#pragma once

#include "..\resource.h"
#include "..\Automation\DataSrc.h"
#include "ExtControls.h"
#include "AutoCalcSizeDialog.h"
#include "..\FSDoc.h"
#include "..\ah_users.h"


UINT GetFreeCtrlID (HWND hDlg);
class LinearConverter;

// COutputDSControlDlg dialog
class COutputDSControlDlg : public CAutoCalcSizeDialog
{
	DECLARE_DYNAMIC(COutputDSControlDlg)

public:
	COutputDSControlDlg(CONTROLBAR_INFO cbi, CWnd* pParent = NULL);   // standard constructor
	virtual ~COutputDSControlDlg();

// Dialog Data
	enum { IDD = IDD_DS_OUTPUT_DLG};

protected:
	
	typedef struct tagAnalogOutputDSDialogBlock
	{
		tagAnalogOutputDSDialogBlock () : 
			pAODS(NULL), pConvert(NULL), 
			pSlider(IDC_STATIC, (CExtSliderWndColoredSelection*)NULL), 
			pLblBack(IDC_STATIC, (CExtLabel*)NULL),
			pLblName(IDC_STATIC, (CExtLabel*)NULL), 
			pLblMinVal(IDC_STATIC, (CExtLabel*)NULL), 
			pLblMaxVal(IDC_STATIC, (CExtLabel*)NULL), 
			pLblCurVal(IDC_STATIC, (CExtLabel*)NULL), 
			pApplyBtn(IDC_STATIC, (CExtButton*)NULL)
		{
		}		

		AnalogOutputDataSrc*			pAODS;
		LinearConverter*				pConvert;

		std::pair<UINT, CExtLabel*>		pLblName;
		std::pair<UINT, CExtLabel*>		pLblBack;
		std::pair<UINT, CExtLabel*>		pLblMinVal;
		std::pair<UINT, CExtLabel*>		pLblCurVal;
		std::pair<UINT, CExtLabel*>		pLblMaxVal;
		std::pair<UINT, CExtSliderWndColoredSelection*>  pSlider;
		std::pair<UINT, CExtButton*>	pApplyBtn;

		bool autoApply;

	} AODSBLOCK, *LPAODSBLOCK;

	std::vector<AODSBLOCK> m_vdsBlocks;
	
	CExtButton btnPresets;
	CExtButton btnApplyAll;
	static UINT miID; //  = ID_PRESET_FIRST;

	UINT startID;

	CONTROLBAR_INFO cbInfo;

	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	BOOL AddOutputDSBlock(CONTROLBAR_ITEM_INFO& cbii);

	int GetBlockIndex(UINT ID) const;
	int GetBlockIndex(HWND hWnd) const;
	int GetBlockIndex(const AnalogOutputDataSrc* pChangedDS) const;
	int GetBlockIndex(const std::string id) const;
	
	static std::map<std::string, COutputDSControlDlg*> m_usedDSMap;

	void OnAnalogOutputValueChanged(const AnalogOutputDataSrc* pChangedDS);
	void OnSwitchAutoMode (const DiscreteDataSrc* pChangedDS);
	void OnChangeUser(const NTCC_APP_USER* pUser);

	afx_msg LRESULT OnAnalogValueChange( WPARAM wParam, LPARAM lParam);
	


	void OnApplyAll();

public:
	std::map <UINT, char> numPadToChar;
	std::string pressKeyBuffer;
	
	virtual BOOL OnInitDialog();	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	
	
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage( MSG * pMsg );
	
	afx_msg BOOL OnTtnNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
};

