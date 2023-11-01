#pragma once

#include "singletone.h"

#ifndef _APP_SMS_HELPER_H_INCLUDED_
#define _APP_SMS_HELPER_H_INCLUDED_

class AppSMSHelper : public Singletone<AppSMSHelper>
{
	friend class Singletone<AppSMSHelper>;

public:

	virtual ~AppSMSHelper();

protected:

	AppSMSHelper();
	AppSMSHelper(const AppSMSHelper& );
	const AppSMSHelper& operator= (const AppSMSHelper&);

public:
	BOOL Initialize(IXMLDOMNode* pNode );
	BOOL Initialize(unsigned char portNo );


	//std::string phoneID
	//phoneID - 1). If found  group with id "phoneID" send SMS to all group members
	//			2). If "phoneID" not found  group id  send SMS to user with "phoneID" user id
	//			3). If "phoneID" not found  group id  AND user id send SMS to phone number with "phoneID"
	//std::string text - SMS message text
	BOOL SendSMS( std::string phoneID, std::string text );
	BOOL SendSMS( std::vector< std::string > phoneNums, std::string text );


protected:

	std::map<std::string, std::string> phoneBook;
	std::multimap<std::string, std::string> userGroups;

	unsigned char modemPort;
};

#endif //_APP_SMS_HELPER_H_INCLUDED_
