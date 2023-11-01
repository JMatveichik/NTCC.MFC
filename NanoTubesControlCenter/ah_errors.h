#pragma once

#include "singletone.h"
#include "Automation/errors.h"

#ifndef _APP_ERRORS_HELPER_H_INCLUDED_
#define _APP_ERRORS_HELPER_H_INCLUDED_

class AppErrorsHelper : public Singletone<AppErrorsHelper>
{
	friend class Singletone<AppErrorsHelper>;

protected:

	AppErrorsHelper() {};
	AppErrorsHelper(const AppErrorsHelper& );
	const AppErrorsHelper& operator= (const AppErrorsHelper&);

public:

	void SetLastError(UINT errorCode, std::string messsage);
	std::string GetLastError(UINT& code) const;

private:

	UINT lastError;
	std::string lastMessage; 
};

#endif _APP_ERRORS_HELPER_H_INCLUDED_
