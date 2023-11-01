#ifndef __LOCK_H__
#define __LOCK_H__

class MainCriticalSectionHandler
{
public:
	LPCRITICAL_SECTION lpcs;
	
	MainCriticalSectionHandler( LPCRITICAL_SECTION lpcs )
	{
		this->lpcs = lpcs;
		EnterCriticalSection( lpcs );
	}

	~MainCriticalSectionHandler()
	{
		LeaveCriticalSection( lpcs );
	}

	void Release()
	{
		LeaveCriticalSection( lpcs );
	}

};

#endif