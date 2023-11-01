// stdafx.cpp : source file that includes just the standard includes
// NanoTubesControlCenter.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"



namespace Globals
{
	
	std::string CheckError(bool bShowMessageDlg/* = false*/)
	{
		TCHAR szBuf[80];
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
			dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		wsprintf(szBuf, "Failed with error %d: %s", dw, lpMsgBuf);

		if (bShowMessageDlg)
			MessageBox(NULL, szBuf, "Error", MB_OK);

		LocalFree(lpMsgBuf);
		return szBuf;
	}

}