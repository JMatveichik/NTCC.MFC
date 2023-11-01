#include "stdafx.h"
#include "savableitem.h"

const unsigned MAXLENGTH = 9999;

BOOL SavableItem::WriteString(HANDLE hFile, std::string theString)
{
	const unsigned HEADERSIZE = 6;
	char szHeader[HEADERSIZE]; // Example: [0006|string]
	DWORD dwRes;

	if (theString.length() > MAXLENGTH)
		return FALSE;

	_snprintf_s(szHeader, HEADERSIZE, _TRUNCATE, "[%04d|", theString.length());

	if (!WriteFile(hFile, szHeader, HEADERSIZE, &dwRes, NULL) || dwRes != HEADERSIZE)
		return FALSE;

	if (!WriteFile(hFile, theString.c_str(), DWORD(theString.length()), &dwRes, NULL) || dwRes != theString.length())
		return FALSE;

	if (!WriteFile(hFile, "]", 1, &dwRes, NULL) || dwRes != 1)
		return FALSE;

	return TRUE;
}

std::string SavableItem::ReadString(HANDLE file)
{
	const unsigned HEADERSIZE = 6;
	char szSize[HEADERSIZE]; // Example: [006|string]
	DWORD dwRes;

	if (!ReadFile( file, szSize, HEADERSIZE, &dwRes, NULL ) || dwRes != HEADERSIZE)
		throw std::runtime_error("SavableItem::ReadString - I/O error");

	if (szSize[0] != '[' || szSize[HEADERSIZE-1] != '|')
		throw std::runtime_error("SavableItem::ReadString - corrupted string");

	szSize[HEADERSIZE-1] = 0;
	DWORD nSize = atoi(&szSize[1]);

	if (nSize < 0 || nSize > MAXLENGTH)
		throw std::runtime_error("SavableItem::ReadString - corrupted string");

	char* pStr = new char[nSize+1];
		
	if (!ReadFile( file, pStr, nSize+1, &dwRes, NULL ) || dwRes != nSize+1)
	{
		delete [] pStr ;
		throw std::runtime_error("SavableItem::ReadString - I/O error");
	}
	
	if (pStr[nSize] != ']')
		throw std::runtime_error("SavableItem::ReadString - corrupted string");

	pStr[nSize] = 0;
	std::string theString = pStr;
	delete [] pStr ;

	return theString;
}
