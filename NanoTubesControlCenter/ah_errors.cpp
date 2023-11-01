#include "stdafx.h"
#include "ah_errors.h"

void AppErrorsHelper::SetLastError(UINT errorCode, std::string messsage)
{
	lastError = errorCode;
	lastMessage = messsage;
}

std::string AppErrorsHelper::GetLastError(UINT& code) const
{
	code = lastError;
	return lastMessage;
}