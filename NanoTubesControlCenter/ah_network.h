#pragma once

#include "singletone.h"

#ifndef _APP_NETWORK_HELPER_H_INCLUDED_
#define _APP_NETWORK_HELPER_H_INCLUDED_

class AppNetworkHelper: public Singletone<AppNetworkHelper>
{
	friend class Singletone<AppNetworkHelper>;

public:

	virtual ~AppNetworkHelper();

protected:

	AppNetworkHelper(); 	
	AppNetworkHelper(const AppNetworkHelper& );
	const AppNetworkHelper& operator= (const AppNetworkHelper&);

public:	
	
	BOOL Connect(LPCTSTR lpResourceName, LPCTSTR lpUserName, LPCTSTR lpPassword) const;

protected:

	BOOL FindConnection(LPCTSTR lpResourceName) const;

	
};

#endif _APP_NETWORK_HELPER_H_INCLUDED_