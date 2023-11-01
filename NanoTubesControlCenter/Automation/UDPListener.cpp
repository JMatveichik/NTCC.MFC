#include "StdAfx.h"
#include "UDPListener.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

CUDPListener::CUDPListener(void)
{
}

CUDPListener::~CUDPListener(void)
{
}


BOOL CUDPListener::Init()
{
	m_soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	bool bBC = true;
	if( setsockopt( m_soc, SOL_SOCKET, SO_BROADCAST, (char*)&bBC, sizeof(bBC)) == SOCKET_ERROR)
		return FALSE;

	if ( m_soc == INVALID_SOCKET )
		return FALSE;

	return TRUE;
}

BOOL CUDPListener::ListenBroadcast( UINT portNo )
{
	sockaddr_in local_addr;

	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = INADDR_ANY;
	local_addr.sin_port = htons(portNo);

	if ( ::bind(m_soc,(sockaddr *) &local_addr, sizeof(local_addr)))
	{		
		closesocket(m_soc);		
		return FALSE;
	}

	char buff[2048];

	while( 1 )
	{
		sockaddr_in client_addr;
		int client_addr_size = sizeof(client_addr);
		
		int size = recvfrom(m_soc, &buff[0], sizeof(buff)-1, 0, (sockaddr *) &client_addr, &client_addr_size);

	if ( size == SOCKET_ERROR)			
		return FALSE;
		//TRACE("recvfrom() error: %d\n",WSAGetLastError());
		
		// ���������� IP-����� ������� � ������ ��������
// 		HOSTENT *hst;
// 		hst = gethostbyaddr((char *) &client_addr.sin_addr, 4, AF_INET);
// 
// 		//TRACE("+%s [%s:%d] new DATAGRAM size : %d!\n", (hst) ? hst->h_name : "Unknown host", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), size);
		
		UINT pos = 13;
		//����� ����������
		DWORD dwTrNO;
		memcpy(&dwTrNO, &buff[pos], 4);	pos += 4;

		WORD  wSize;
		memcpy(&wSize, &buff[pos], 2);	pos += 2;

		WORD  wStatus;
		memcpy(&wStatus, &buff[pos], 2); pos += 2;
		
		struct {
			BYTE  YY;
			BYTE  MM;
			BYTE  DD;
			BYTE  hh;
			BYTE  mm;
			BYTE  ss;
		} dt;
		
		memcpy(&dt.YY, &buff[pos], 1); pos += 1;
		memcpy(&dt.MM, &buff[pos], 1); pos += 1;
		memcpy(&dt.DD, &buff[pos], 1); pos += 1;
		memcpy(&dt.hh, &buff[pos], 1); pos += 1;
		memcpy(&dt.mm, &buff[pos], 1); pos += 1;
		memcpy(&dt.ss, &buff[pos], 1); pos += 1;


		DWORD dwGrNO;
		memcpy(&dwGrNO, &buff[pos], 4); pos += 4;
		
		DWORD dwScalesNO;
		memcpy(&dwScalesNO, &buff[pos], 5); pos += 5;

		BYTE  opID;
		memcpy(&opID, &buff[pos], 1); pos += 1;

		DWORD productPLU;		
		memcpy(&productPLU, &buff[pos], 4); pos += 4;

		DWORD productPrice;
		memcpy(&productPrice, &buff[pos], 4); pos += 4;
		
		DWORD productMass;
		memcpy(&productMass, &buff[pos], 4); pos += 4;
		
		CString strOut;
		CStringArray lines;		
		
		strOut.Format("����� ���������� : %06d\n\r", dwTrNO); lines.Add(strOut);
		strOut.Format("����  : %02d.%02d.20%02d �.\n\r" , dt.DD, dt.MM, dt.YY); lines.Add(strOut);
		strOut.Format("����� : %02d:%02d:%02d\n\r" , dt.hh, dt.mm, dt.ss); lines.Add(strOut);
		strOut.Format("PLU ��������    : %06d\n\r" , productPLU); lines.Add(strOut);
		strOut.Format("��������� �������� : %g ���./��\n\r" , productPrice / 100.0); lines.Add(strOut);
		strOut.Format("����� ��������     : %d �\n\r" , productMass); lines.Add(strOut);

		strOut = "";
		for (int i = 0; i < lines.GetCount(); i++)
			strOut += lines[i];

		AfxMessageBox(strOut, MB_OK|MB_ICONINFORMATION);		

		//TRACE("\n-------------------------");
		//TRACE("\n����� ���������� : %d\n������ ������ : %d\n������ : 0x%02X" , dwTrNO, wSize, wStatus);
		//TRACE("\n����  : %02d.%02d.20%02d �." , dt.DD, dt.MM, dt.YY);
		//TRACE("\n����� : %02d:%02d:%02d" , dt.hh, dt.mm, dt.ss);
		//TRACE("\n��������� ����� : %06d" , dwGrNO);
		//TRACE("\n����� �����     : %02d" , dwScalesNO);
		//TRACE("\n����� ��������� : %02d" , opID);
		//TRACE("\nPLU ��������    : %06d" , productPLU);
		//TRACE("\n��������� �������� : %g ���./��" , productPrice / 100.0);
		//TRACE("\n����� ��������     : %d �" , productMass);
		//TRACE("\n-------------------------");	

	}

}

