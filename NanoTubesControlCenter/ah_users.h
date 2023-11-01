#pragma once

#include "singletone.h"

#ifndef _APP_USERS_HELPER_H_INCLUDED_
#define _APP_USERS_HELPER_H_INCLUDED_

//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]
//////////////////////////////////////////////////////////////////////////

typedef enum tagUserGroupID : unsigned { 
	ADMINISTRATORS	= 0, 
	TECHNOLOGISTS	= 1, 
	OPERATORS		= 2,
	ONLOOKER		= 100
} USERS_GROUPS;

typedef struct tagNTCCAppUser {
	USERS_GROUPS ug;
	std::string login;
	std::string psw;
	std::string screenName;
	std::string phoneNum;
}
NTCC_APP_USER, *LPNTCC_APP_USER;


class AppUsersHelper : public Singletone<AppUsersHelper>
{
	friend class Singletone<AppUsersHelper>;

public: 

	virtual ~AppUsersHelper();

protected:

	AppUsersHelper();
	AppUsersHelper(const AppUsersHelper& );
	const AppUsersHelper& operator= (const AppUsersHelper&);

public:	

	BOOL Initialize( std::string fname );
	LPNTCC_APP_USER GetCurrentUser( ) const;
	const NTCC_APP_USER* GetUser(std::string name ) const;
	LPNTCC_APP_USER UserLogin(std::string login, std::string psw );
	LPNTCC_APP_USER UserLogin();

	const std::vector<NTCC_APP_USER>& AppUsersHelper::GetAppicationUsers( );
	Gdiplus::Image* GetCurrentUserImage(UINT iconSize/* = 32*/);

	BOOL AddUser(LPNTCC_APP_USER lpUser);

	void AddOnUserChanged(boost::function< void (const NTCC_APP_USER*) > handler )
	{
		evOnChangeUser.connect(handler);
	}

protected:

	boost::signals2::signal< void (const NTCC_APP_USER*) > evOnChangeUser;

	BOOL saveGroups(HANDLE hFile);
	BOOL saveUsers(HANDLE hFile);

	BOOL loadGroups(HANDLE hFile);
	BOOL loadUsers(HANDLE hFile);

	void Crypt(const char* data, char* res, unsigned int len);

	BOOL WriteString(HANDLE hFile, std::string theString);

	std::string ReadString(HANDLE file);


	std::vector<NTCC_APP_USER> users;
	std::string usersFileName;
	LPNTCC_APP_USER lpCurUser;
};


#endif //_APP_USERS_HELPER_H_INCLUDED_
