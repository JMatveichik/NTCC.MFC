#include "stdafx.h"
#include "ah_users.h"
#include "ah_images.h"

#include "DLGs/UserLogonDlg.h"

AppUsersHelper::AppUsersHelper() : lpCurUser(NULL)
{

}

AppUsersHelper::~AppUsersHelper()
{

}

LPNTCC_APP_USER AppUsersHelper::GetCurrentUser( ) const 
{ 
	return lpCurUser; 
}


BOOL AppUsersHelper::Initialize( std::string fname )
{
	usersFileName = fname;

	HANDLE file = CreateFile( fname.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );	
	if (file == INVALID_HANDLE_VALUE)
		return FALSE;

	loadUsers(file);

	::CloseHandle(file);

	//LPNTCC_APP_USER lpUser = UserLogin("JohnnyM", "wis_Ntcc_09");
	return TRUE;
}

BOOL AppUsersHelper::saveUsers(HANDLE hFile)
{
	if ( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	if ( !WriteString(hFile, "USERS_SECTION_BEGIN") )
		return FALSE;

	DWORD dwCount = users.size();
	DWORD dwRes;

	if ( !WriteFile(hFile, &dwCount, sizeof(DWORD), &dwRes, NULL) || dwRes != sizeof(DWORD) )
		return FALSE;

	for (std::vector<NTCC_APP_USER>::const_iterator it = users.begin(); it != users.end(); ++it)
	{
		if ( !WriteString(hFile, "USER_SECTION_BEGIN") )
			return FALSE;

		if ( !WriteString(hFile, (*it).login) )
			return FALSE;

		if ( !WriteString(hFile, (*it).screenName) )
			return FALSE;

		if ( !WriteString(hFile, (*it).psw) )
			return FALSE;

		if ( !WriteString(hFile, (*it).phoneNum) )
			return FALSE;

		DWORD dwPrior = (DWORD)(*it).ug;
		if (!WriteFile(hFile, &dwPrior, sizeof(DWORD), &dwRes, NULL) || dwRes != sizeof(DWORD))
			return FALSE;

		if ( !WriteString(hFile, "USER_SECTION_END") )
			return FALSE;
	}

	if ( !WriteString(hFile, "USERS_SECTION_END") )
		return FALSE;

	return TRUE;
}

BOOL AppUsersHelper::loadUsers(HANDLE hFile)
{
	users.clear();
	std::string str;
	try
	{
		str = ReadString(hFile);

		if ( str != "USERS_SECTION_BEGIN" )
			return FALSE;

		DWORD dwCount;
		DWORD dwRes;

		if ( !ReadFile(hFile, &dwCount, sizeof(DWORD), &dwRes, NULL) || dwRes != sizeof(DWORD) )
			return FALSE;

		NTCC_APP_USER user;

		for (DWORD i = 0; i < dwCount; i++)
		{
			str = ReadString(hFile);
			if ( str != "USER_SECTION_BEGIN" )
				return FALSE;

			user.login		= ReadString(hFile);
			user.screenName = ReadString(hFile);
			user.psw		= ReadString(hFile);
			user.phoneNum   = ReadString(hFile);

			DWORD dwPrior;
			if ( !ReadFile(hFile, &dwPrior, sizeof(DWORD), &dwRes, NULL) || dwRes != sizeof(DWORD) )
				return FALSE;

			user.ug = (USERS_GROUPS)dwPrior;

			str = ReadString(hFile);
			if ( str != "USER_SECTION_END" )
				return FALSE;

			users.push_back(user);
		}

		str = ReadString(hFile);
		if ( str != "USERS_SECTION_END" )
			return FALSE;
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}

LPNTCC_APP_USER AppUsersHelper::UserLogin(std::string login, std::string psw )
{
	for (std::vector<NTCC_APP_USER>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if ( (*it).login == login )
		{
			if ( (*it).psw == psw )
			{
				lpCurUser = &(*it);

				evOnChangeUser(lpCurUser);

				return lpCurUser;
			}
		}
	}
	return NULL;
}

LPNTCC_APP_USER AppUsersHelper::UserLogin( )
{
	CUserLogonDlg dlg(AfxGetApp()->m_pMainWnd);
	dlg.DoModal();

	return GetCurrentUser();
}

const NTCC_APP_USER* AppUsersHelper::GetUser(std::string name ) const
{
	for (std::vector<NTCC_APP_USER>::const_iterator it = users.begin(); it != users.end(); ++it)
	{
		if ( (*it).screenName == name )
			return &(*it);

	}
	return NULL;
}

BOOL AppUsersHelper::AddUser(LPNTCC_APP_USER lpUser)
{
	for (std::vector<NTCC_APP_USER>::const_iterator it = users.begin(); it != users.end(); ++it)
	{
		if ( (*it).login == lpUser->login )
			return FALSE;

	}
	users.push_back(*lpUser);

	HANDLE file = CreateFile( usersFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );	
	if (file != INVALID_HANDLE_VALUE)
	{
		saveUsers(file);
		::CloseHandle(file);			
		return TRUE;
	}

	return FALSE;
}

Gdiplus::Image* AppUsersHelper::GetCurrentUserImage(UINT iconSize/* = 32*/)
{
	UINT iconID = IDI_GROUP_USERS; 

	if ( lpCurUser != NULL )
	{
		switch ( lpCurUser->ug )
		{
		case ADMINISTRATORS:
			iconID = IDI_GROUP_ADMINS; 
			break;

		case TECHNOLOGISTS:
			iconID = IDI_GROUP_TECHNOLOGISTS;
			break;

		case OPERATORS:
			iconID = IDI_GROUP_OPERATORS;
			break;

		case ONLOOKER:
			iconID = IDI_GROUP_USERS;
			break;

		default:
			iconID = IDI_GROUP_USERS;
			break;
		}
	}

	return AppImagesHelper::Instance().GetImageFromIcon(iconID, 32);
}

const std::vector<NTCC_APP_USER> & AppUsersHelper::GetAppicationUsers( )
{
	return users;
}


void AppUsersHelper::Crypt(const char* data, char* res, unsigned int len)
{
	std::string psw = "~q*f+1eqf!zG<";

	for (unsigned i = 0; i < len; i++)
	{
		char ch1 = data[i];
		char ch2 = psw[i % psw.length()];
		char ch = (char)(ch1 ^ ch2);
		res[i] = ch;
	}
}

BOOL AppUsersHelper::WriteString(HANDLE hFile, std::string theString)
{
	DWORD dwLen = theString.length();
	DWORD dwRes;

	if (!WriteFile(hFile, "{", 1, &dwRes, NULL) || dwRes != 1)
		return FALSE;

	if (!WriteFile(hFile, &dwLen, sizeof(DWORD), &dwRes, NULL) || dwRes != sizeof(DWORD))
		return FALSE;

	char* crt = new char[dwLen];
	Crypt(theString.c_str(), crt, dwLen);

	if (!WriteFile(hFile, crt, dwLen, &dwRes, NULL) || dwRes != dwLen)
	{
		delete [] crt;
		return FALSE;
	}

	delete [] crt;

	if (!WriteFile(hFile, "}", 1, &dwRes, NULL) || dwRes != 1)
		return FALSE;



	return TRUE;
}

std::string AppUsersHelper::ReadString(HANDLE hFile)
{
	char chMark;
	DWORD dwRes;

	if ( !ReadFile( hFile, &chMark, 1, &dwRes, NULL ) || dwRes != 1 )
		throw std::runtime_error("ReadString - I/O error");

	if (chMark !='{')
		throw std::runtime_error("ReadString - corrupted string");

	DWORD dwLen;
	if (!ReadFile( hFile, &dwLen, sizeof(DWORD), &dwRes, NULL ) || dwRes != sizeof(DWORD) )
		throw std::runtime_error("ReadString - I/O error");

	char* szBuffer = new char[dwLen];	
	char* szOut = new char[dwLen + 1];

	if (!ReadFile( hFile, szBuffer, dwLen, &dwRes, NULL ) || dwRes != dwLen)
	{
		delete [] szBuffer;
		delete [] szOut;
		throw std::runtime_error("ReadString - I/O error");
	}	

	Crypt(szBuffer, szOut, dwLen);
	szOut[dwLen] = 0;
	std::string theString(szOut);

	delete [] szBuffer;
	delete [] szOut;

	if ( !ReadFile( hFile, &chMark, 1, &dwRes, NULL ) || dwRes != 1 )
		throw std::runtime_error("ReadString - I/O error");

	if (chMark !='}')
		throw std::runtime_error("ReadString - corrupted string");

	return theString;
}