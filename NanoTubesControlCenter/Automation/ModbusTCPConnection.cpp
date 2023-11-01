#include "..\stdafx.h"
#include "modbustcpconnection.h"
#include <time.h>
#include <assert.h>
#include <Ws2tcpip.h>


#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

using namespace std;

tagMODBUS_HEADER::tagMODBUS_HEADER()
{
	reserved = 1;
	TxID = ProtID = length = 0;
}

tagMODBUS_REQUEST::tagMODBUS_REQUEST()
{
	memset(data, 0, MP_MAX_SIZE);
	command = LoopbackDiagnosis;
}


bool tagMODBUS_REQUEST::Send(SOCKET sock, MODBUS_COMMAND cmd, const char* data, unsigned data_size, std::ostream* pLog)
{
	command = cmd;

	unsigned short packet_size = short( sizeof(tagMODBUS_HEADER) + 1 + data_size );
	length = htons(data_size + 2);
	
	memcpy(this->data, data, data_size);

	if (pLog)
	{
		*pLog << "MODBUS_REQUEST::Send() : send: ";
		for (unsigned i=0; i<packet_size; i++)
			*pLog << setw(2) << uppercase << hex << setfill('0') << unsigned (((char*)this)[i] & 0xFF) << ' ';
		*pLog << endl;
	}

	unsigned nBytesSent = 0;
	while (nBytesSent != packet_size)
	{
		unsigned nCount = send(sock, &((const char*)this)[nBytesSent], int(packet_size - nBytesSent), 0);
		if (nCount == SOCKET_ERROR)
			return false;

		nBytesSent += nCount;
	}

	// Save request header
	unsigned long hdr;
	hdr = *(unsigned long*)this;
	
	unsigned nCount = recv(sock, (char*)this, sizeof(this->data), 0);
	if (nCount == SOCKET_ERROR || nCount == 0)
		return false;
	
	if ( nCount == sizeof(this->data) )
		throw exception("Too long packet size.");

	// Check request header
	if ( hdr != *(unsigned long*)this)
	{
		if (pLog)
			*pLog << "MODBUS_REQUEST::Send() : Invalid Modbus packet header received.";
		return false;
	}

	if (pLog)
	{
		*pLog << "MODBUS_REQUEST::Send() : recv: ";
		for (unsigned i=0; i<nCount; i++)
			*pLog << setw(2) << uppercase << hex << setfill('0') << unsigned (((char*)this)[i] & 0xFF) << ' ';
	}

	switch (cmd)
	{
	case ReadCoilStatus:			// Read Discrete Output Bit
		length = this->data[0];
		break;

	case ReadInputStatus:			// Read Discrete Input Bit
		ASSERT(FALSE);

	case ReadHoldingRegisters:		// Read 16-bit register. Used to read integer or floating point process data
		//ASSERT(FALSE);
		break;

	case ReadInputRegisters:		// --//--
		length = this->data[0];
		break;

	case ForceSingleCoil:			// Write data to force coil ON/OFF
		length = this->data[0];
		break;
		

	case PresetSingleRegister:		// Write data in 16-bit integer format
		//ASSERT(FALSE);
		break;

	case LoopbackDiagnosis:			// Diagnostic testing of the communication port
		ASSERT(FALSE);

	case ForceMultipleCoils:		// Write multiple data to force coil ON/OFF
		ASSERT(FALSE);

	case PresetMultipleRegisters:	// Write multiple data in 16-bit integer format 
		{
			if ( *(LPDWORD)data != *(LPDWORD)this->data )
				return false;

			length = 0;
		}		
		break;
		

	}
	

	return true;
}

std::string CModbusTCPConnection::strVirtualServer = "";
unsigned CModbusTCPConnection::nReadTimeout = 1000;			// 1 second
unsigned CModbusTCPConnection::nWriteTimeout = 1000;		// 1 second
unsigned CModbusTCPConnection::nMulfunctionTimeout = 60000;	// 1 minute


bool CModbusTCPConnection::Establish(std::string strIP, unsigned nPort, std::ostream* pLog)
{
	assert(sock == INVALID_SOCKET);

	static char szPrefix[] = "CModbusTCPConnection::Establish() : ";

	// Create Internet socket
	if ((sock = CreateSocket()) == INVALID_SOCKET)
	{
		if (pLog)
			*pLog << szPrefix << "Unable to create socket" << endl;
		return false;
	}

	// Setup the sockaddr structure if it is not done yet
	if (!strIP.empty())
	{
		saModule.sin_family = AF_INET;

		//saModule.sin_addr.s_addr = inet_addr(strIP.c_str());
		
		InetPton(AF_INET, strIP.c_str(), &saModule.sin_addr.s_addr);

		if (saModule.sin_addr.s_addr == INADDR_NONE)
		{
			if (pLog)
				*pLog << szPrefix << "Invalid IP address specified for the module." << endl;
			return false;
		}
		moduleID = saModule.sin_addr.S_un.S_un_b.s_b4;
		
		if (!strVirtualServer.empty())
			//saModule.sin_addr.s_addr = inet_addr(strVirtualServer.c_str());
			InetPton(AF_INET, strVirtualServer.c_str(), &saModule.sin_addr.s_addr);

		if (saModule.sin_addr.s_addr == INADDR_NONE)
		{
			if (pLog)
				*pLog << szPrefix << "Invalid IP address specified for virtual server." << endl;
			return false;
		}

		saModule.sin_port = htons(nPort);
	}

	// Connect to remote module
	if (connect(sock, (sockaddr*)&saModule, sizeof(saModule)) == SOCKET_ERROR)
	{
		Finalize();
		if (pLog)
			*pLog << szPrefix << "Unable to connect to module." << endl;
		return false;
	}

	return true;
}

void CModbusTCPConnection::Finalize()
{
	shutdown(sock, SD_BOTH);
	closesocket(sock);
	sock = INVALID_SOCKET;
}

bool CModbusTCPConnection::Transact(MODBUS_COMMAND cmd, const char* data, unsigned data_size, std::vector<unsigned char>& response, std::ostream* pLog)
{
	assert(sock != INVALID_SOCKET);

	::EnterCriticalSection(&m_cs);

	response.clear();

	time_t start;
	time(&start);
	time_t current = start;

	if (moduleID)
		request.TxID = moduleID;

	static char szPrefix[] = "CModbusTCPConnection::Transact() : ";

	if (!request.Send(sock, cmd, data, data_size, pLog))
	{
		if (pLog)
			*pLog << szPrefix << "Unable to send data. ";
		
		if ( lpfnErrorCallback )
			if ( !lpfnErrorCallback(lpPar) )
			{
				if (pLog)
					*pLog  << endl << szPrefix << "Retry cancelled due to callback request." << endl;
				
				::LeaveCriticalSection(&m_cs);
				return false;
			}

		while (difftime(current, start)*1000 < nMulfunctionTimeout)
		{
			if (pLog)
				*pLog << szPrefix << "Trying to reconnect..." << endl;
		
			Finalize();

			if ( Establish("", 0, pLog) )
			{
				if (request.Send(sock, cmd, data, data_size, pLog))
				{
					::LeaveCriticalSection(&m_cs);
					return true;
				}
			}

			time(&current);
		}

		if (pLog)
			*pLog << szPrefix << "Module did not respond within " << difftime(current, start) << " seconds. Terminating session." << endl;

		::LeaveCriticalSection(&m_cs);
		return false;
	}

	response.assign(&request.data[1], &request.data[request.length+1]);

	::LeaveCriticalSection(&m_cs);
	return true;
}

SOCKET CModbusTCPConnection::CreateSocket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		return INVALID_SOCKET;

	bool bNoDelay = true;
	if (setsockopt(sock, SOL_SOCKET, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay)) == SOCKET_ERROR)
	{
		shutdown(sock, SD_BOTH);
		closesocket(sock);
		return INVALID_SOCKET;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&nWriteTimeout, sizeof(nWriteTimeout)) == SOCKET_ERROR)
	{
		shutdown(sock, SD_BOTH);
		closesocket(sock);
		return INVALID_SOCKET;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&nReadTimeout, sizeof(nReadTimeout)) == SOCKET_ERROR)
	{
		shutdown(sock, SD_BOTH);
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}