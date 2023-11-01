#if !defined(_WIN_MESSAGES_H_INCLUDED_)
#define _WIN_MESSAGES_H_INCLUDED_

const UINT RM_SHOW_USER_MESSAGE		= WM_USER + 1;
const UINT RM_DOCK_MSG_WND			= WM_USER + 2;

const UINT RM_LOADING_INIT			= WM_USER + 3;


//////////////////////////////////////////////////////////////
const UINT RM_ADD_LOADING_SECTION	= WM_USER + 4;

typedef struct tagDocumentSectionInfo{
	CString title;
	UINT subItemsCount;
}DOCUMENT_SECTION_INFO, *LPDOCUMENT_SECTION_INFO;

/////////////////////////////////////////////////////////////

const UINT RM_ADD_INFO_SECTION		= WM_USER + 5;
const UINT RM_STEP_SECTION			= WM_USER + 6;
const UINT RM_LOADING_COMPLETE		= WM_USER + 7;

const UINT RM_ADD_CHECKPOINT		= WM_USER + 8;
const UINT RM_ANALOG_VALUE_CHANGE	= WM_USER + 9;
const UINT RM_DISCRETE_VALUE_CHANGE	= WM_USER + 10;

const UINT RM_TIMER_STARTED		= WM_USER + 11;
const UINT RM_TIMER_STOPED		= WM_USER + 12;
const UINT RM_TIMER_OVERFLOWED	= WM_USER + 13;


#endif //_WIN_MESSAGES_H_INCLUDED_