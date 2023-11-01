#include "stdafx.h"
#include "ah_network.h"

void DisplayStruct(int i, LPNETRESOURCE lpnrLocal)
{
	TRACE("NETRESOURCE[%d] Scope: ", i);
	switch (lpnrLocal->dwScope) {
	case (RESOURCE_CONNECTED):
		TRACE("connected\n");
		break;
	case (RESOURCE_GLOBALNET):
		TRACE("all resources\n");
		break;
	case (RESOURCE_REMEMBERED):
		TRACE("remembered\n");
		break;
	default:
		TRACE("unknown scope %d\n", lpnrLocal->dwScope);
		break;
	}

	TRACE("NETRESOURCE[%d] Type: ", i);
	switch (lpnrLocal->dwType) {
	case (RESOURCETYPE_ANY):
		TRACE("any\n");
		break;
	case (RESOURCETYPE_DISK):
		TRACE("disk\n");
		break;
	case (RESOURCETYPE_PRINT):
		TRACE("print\n");
		break;
	default:
		TRACE("unknown type %d\n", lpnrLocal->dwType);
		break;
	}

	TRACE("NETRESOURCE[%d] DisplayType: ", i);
	switch (lpnrLocal->dwDisplayType) {
	case (RESOURCEDISPLAYTYPE_GENERIC):
		TRACE("generic\n");
		break;
	case (RESOURCEDISPLAYTYPE_DOMAIN):
		TRACE("domain\n");
		break;
	case (RESOURCEDISPLAYTYPE_SERVER):
		TRACE("server\n");
		break;
	case (RESOURCEDISPLAYTYPE_SHARE):
		TRACE("share\n");
		break;
	case (RESOURCEDISPLAYTYPE_FILE):
		TRACE("file\n");
		break;
	case (RESOURCEDISPLAYTYPE_GROUP):
		TRACE("group\n");
		break;
	case (RESOURCEDISPLAYTYPE_NETWORK):
		TRACE("network\n");
		break;
	default:
		TRACE("unknown display type %d\n", lpnrLocal->dwDisplayType);
		break;
	}

	TRACE("NETRESOURCE[%d] Usage: 0x%x = ", i, lpnrLocal->dwUsage);
	if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONNECTABLE)
		TRACE("connectable ");
	if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONTAINER)
		TRACE("container ");
	TRACE("\n");

	TRACE("NETRESOURCE[%d] Localname: %s\n", i, lpnrLocal->lpLocalName);
	TRACE("NETRESOURCE[%d] Remotename: %s\n", i, lpnrLocal->lpRemoteName);
	TRACE("NETRESOURCE[%d] Comment: %s\n", i, lpnrLocal->lpComment);
	TRACE("NETRESOURCE[%d] Provider: %s\n", i, lpnrLocal->lpProvider);
	TRACE("\n");
}

BOOL WINAPI EnumerateFunc(LPNETRESOURCE lpnr, LPCSTR lpResourceName)
{
	DWORD dwResult, dwResultEnum;
	HANDLE hEnum;
	DWORD cbBuffer = 16384;     // 16K is a good size
	DWORD cEntries = -1;        // enumerate all possible entries
	LPNETRESOURCE lpnrLocal;    // pointer to enumerated structures
	DWORD i;
	
	// Call the WNetOpenEnum function to begin the enumeration.		
	dwResult = WNetOpenEnum(RESOURCE_CONNECTED, // all network resources
		RESOURCETYPE_ANY,   // all resources
		0,  // enumerate all resources
		lpnr,       // NULL first time the function is called
		&hEnum);    // handle to the resource

	if (dwResult != NO_ERROR) 
	{
		TRACE("WnetOpenEnum failed with error %d\n", dwResult);
		return FALSE;
	}	
	
	// Call the GlobalAlloc function to allocate resources.
	lpnrLocal = (LPNETRESOURCE) GlobalAlloc(GPTR, cbBuffer);
	if (lpnrLocal == NULL) 
	{
		TRACE("WnetOpenEnum failed with error %d\n", dwResult);
		//NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetOpenEnum");
		return FALSE;
	}

	do {
		
		// Initialize the buffer.		
		ZeroMemory(lpnrLocal, cbBuffer);
		
		// Call the WNetEnumResource function to continue the enumeration.		
		dwResultEnum = WNetEnumResource(hEnum,  // resource handle
			&cEntries,      // defined locally as -1
			lpnrLocal,      // LPNETRESOURCE
			&cbBuffer);     // buffer size
		
		// If the call succeeds, loop through the structures.
		if (dwResultEnum == NO_ERROR) {

			for (i = 0; i < cEntries; i++) 
			{
				// Call an application-defined function to
				// display the contents of the NETRESOURCE structures.
				
				DisplayStruct(i, &lpnrLocal[i]);
				CString strRemote =  lpnrLocal[i].lpRemoteName;
				if ( strRemote.Find(lpResourceName) != -1)
					return TRUE;

				// If the NETRESOURCE structure represents a container resource, 
				//  call the EnumerateFunc function recursively.

				if (RESOURCEUSAGE_CONTAINER == (lpnrLocal[i].dwUsage  & RESOURCEUSAGE_CONTAINER))
					//          if(!EnumerateFunc(hwnd, hdc, &lpnrLocal[i]))
					if (!EnumerateFunc(&lpnrLocal[i], lpResourceName))
						TRACE("EnumerateFunc returned FALSE\n");
				//            TextOut(hdc, 10, 10, "EnumerateFunc returned FALSE.", 29);
			}
		}
		// Process errors.
		//
		else if (dwResultEnum != ERROR_NO_MORE_ITEMS) {
			TRACE("WNetEnumResource failed with error %d\n", dwResultEnum);

			//      NetErrorHandler(hwnd, dwResultEnum, (LPSTR)"WNetEnumResource");
			break;
		}
	}
	while (dwResultEnum != ERROR_NO_MORE_ITEMS); // End do.	

	
	// Call the GlobalFree function to free the memory.
	GlobalFree((HGLOBAL) lpnrLocal);
	
	// Call WNetCloseEnum to end the enumeration.	
	dwResult = WNetCloseEnum(hEnum);

	if (dwResult != NO_ERROR) {
		
		// Process errors.		
		TRACE("WNetCloseEnum failed with error %d\n", dwResult);
		//    NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetCloseEnum");
		return FALSE;
	}

	return FALSE;
}



AppNetworkHelper::AppNetworkHelper() 
{
	
}

AppNetworkHelper::~AppNetworkHelper()
{

}

BOOL AppNetworkHelper::FindConnection(LPCTSTR lpResourceName) const
{

	LPNETRESOURCE lpnr = NULL;

	if ( !EnumerateFunc(lpnr, lpResourceName)) 
	{
		TRACE("Call to EnumerateFunc failed\n");
		return FALSE;
	}

	return TRUE;
}

BOOL AppNetworkHelper::Connect(LPCTSTR lpResourceName, LPCTSTR lpUserName, LPCTSTR lpPassword) const
{
	CString netpath;
	netpath.Format("\\\\%s", (LPCTSTR)lpResourceName);	

	NETRESOURCE nr;
	nr.lpLocalName = NULL;
	nr.dwDisplayType = RESOURCEDISPLAYTYPE_GENERIC;
	nr.dwScope = RESOURCE_GLOBALNET;
	nr.dwType = RESOURCETYPE_ANY;
	nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;
	nr.lpRemoteName = (LPSTR)netpath.GetBuffer(netpath.GetLength());
	nr.lpComment = NULL;
	nr.lpProvider = NULL;


	if ( !FindConnection ((LPCTSTR)netpath) ) 
	{
		if ( WNetAddConnection2(&nr, lpPassword, lpUserName, CONNECT_TEMPORARY) != NO_ERROR )
			return FALSE;
	}	

	return TRUE; 
}




