#include "stdafx.h"
#include "ah_sms.h"
#include "ah_xml.h"

#include "Automation/commandmanager.h"

AppSMSHelper::AppSMSHelper() 
{

}

AppSMSHelper::~AppSMSHelper()
{

}

BOOL AppSMSHelper::Initialize(IXMLDOMNode* pNode )
{
	std::string val;
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "PORT", val) )
		return FALSE;

	unsigned char portNo = (unsigned char)atoi(val.c_str() );

	if ( !Initialize(portNo) )
		return FALSE;

	phoneBook.clear();

	CComPtr<IXMLDOMNodeList> pUsersNodes;
	HRESULT hr = pNode->selectNodes(CComBSTR("USER"), &pUsersNodes);

	if ( FAILED(hr) || pUsersNodes == NULL )
		return FALSE;

	long userCount;
	pUsersNodes->get_length(&userCount);
	pUsersNodes->reset();

	if (userCount == 0)
		return FALSE;

	for (long i=0; i < userCount; i++)
	{
		CComPtr<IXMLDOMNode> pUserNode;
		pUsersNodes->get_item(i, &pUserNode);


		if ( !appXML.GetNodeAttributeByName(pUserNode, "NAME", val) )
			continue;

		std::string uid = val;

		if ( !appXML.GetNodeAttributeByName(pUserNode, "NUMBER", val) )
			continue;

		std::string pn = val;

		phoneBook.insert(std::make_pair(uid, pn));
	}


	CComPtr<IXMLDOMNodeList> pGroupNodes;
	hr = pNode->selectNodes(CComBSTR("GROUP"), &pGroupNodes);

	if ( FAILED(hr) || pGroupNodes == NULL )
		return FALSE;

	long groupCount;
	pGroupNodes->get_length(&groupCount);
	pGroupNodes->reset();

	if (groupCount == 0)
		return FALSE;

	for (long i=0; i < groupCount; i++)
	{
		CComPtr<IXMLDOMNode> pGroupNode;
		pGroupNodes->get_item(i, &pGroupNode);

		if ( !appXML.GetNodeAttributeByName(pGroupNode, "ID", val) )
			continue;

		std::string gid = val;


		CComPtr<IXMLDOMNodeList> pUsersNodes;
		HRESULT hr = pGroupNode->selectNodes(CComBSTR("USER"), &pUsersNodes);

		if ( FAILED(hr) || pUsersNodes == NULL )
			return FALSE;

		long userCount;
		pUsersNodes->get_length(&userCount);
		pUsersNodes->reset();

		if (userCount == 0)
			continue;

		for (long j=0; j < userCount; j++)
		{
			CComPtr<IXMLDOMNode> pUserNode;
			pUsersNodes->get_item(j, &pUserNode);

			std::string val;
			if ( !appXML.GetNodeAttributeByName(pUserNode, "NAME", val) )
				continue;

			std::string uid = val;

			if ( gid == uid )
				continue;

			userGroups.insert(std::make_pair(gid, uid));
		}
	}
	// 	SendSMS("Administrators", "Test message for NTCC Administrators group");
	//	SendSMS("Johnny A. Matveichik", "Test message for Johnny A. Matveichik");
	// 	SendSMS("WorkShift2", "Test message for WorkShift2");
	// 	SendSMS("+375290000009", "Test message for +375290000009");

	return TRUE;
}


BOOL AppSMSHelper::Initialize(unsigned char portNo )
{
	CommandManager* pMan = CommandManager::GetCommandManager(portNo);

	if ( pMan == NULL )
		return FALSE;

	string cmd = "ATZ\r\n";
	std::string res = pMan->DataPass(NULL, cmd, true);
	if  ( res.find("OK") == string::npos )
		return FALSE;

	cmd = "AT+CMGF=1\r\n";
	res = pMan->DataPass(NULL, cmd, true);
	if  ( res.find("OK") == string::npos )
		return FALSE;

	modemPort = portNo;

	return TRUE;
}

BOOL AppSMSHelper::SendSMS( std::vector< std::string > phoneNums, std::string text )
{
	for (std::vector< std::string >::const_iterator it = phoneNums.begin(); it != phoneNums.end(); ++it) 
	{
		SendSMS( (*it), text );
	}

	return TRUE;
}

BOOL AppSMSHelper::SendSMS( std::string phoneID, std::string text )
{
	CommandManager* pMan = CommandManager::GetCommandManager(modemPort);

	if ( pMan == NULL )
		return FALSE;

	if ( userGroups.count(phoneID) != 0 )
	{
		//TRACE( "\n--- Send to group <%s> ---", phoneID.c_str() );

		std::vector<std::string>  phones;
		std::multimap<std::string, std::string>::const_iterator pos;
		for (pos = userGroups.lower_bound(phoneID); pos != userGroups.upper_bound(phoneID); ++pos)
		{
			std::string uid = pos->second;
			std::map<std::string, std::string>::const_iterator  user = phoneBook.find(uid);
			if ( user != phoneBook.end() )
				phones.push_back(user->second);
		}
		return SendSMS(phones, text);
	}
	else if ( phoneBook.find(phoneID) != phoneBook.end() )
	{
		//TRACE( "\n--- Send to single user <%s> ---", phoneID.c_str() );

		std::map<std::string, std::string>::const_iterator  user = phoneBook.find( phoneID );
		return SendSMS(user->second, text);
	}
	else {
		//TRACE( "\n\tSend to <%s> : %s", phoneID.c_str(), text.c_str() );

		std::string cmd = "AT+CMGS=\"" + phoneID + "\"\r\n";
		std::string res = pMan->DataPass(NULL, cmd, true);

		if  ( res.find(">") == string::npos )
			return FALSE;

		cmd = text + "\r\n";
		res = pMan->DataPass(NULL, cmd, true);

		cmd = char(26);
		res = pMan->DataPass(NULL, cmd, true);

		return TRUE;
	}
	return FALSE;
}