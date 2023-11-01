#pragma once

class CUDPListener
{
public:
	
	CUDPListener();
	~CUDPListener(void);

	BOOL Init();
	BOOL ListenBroadcast(UINT portNo);

protected:
	
	SOCKET m_soc;

};
