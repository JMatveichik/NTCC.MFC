// TimedControl.h: interface for the TimedControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEDCONTROL_H__F3FE3814_F5C2_4D12_886F_A89B055A5F5A__INCLUDED_)
#define AFX_TIMEDCONTROL_H__F3FE3814_F5C2_4D12_886F_A89B055A5F5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class TimedControl  
{
public:	

	enum DELAYTIME { BASEDELAY = 250, MINDELAY = 100 };

	TimedControl();
	virtual ~TimedControl();

	// Функция запуска процесса
	virtual bool Start(unsigned long nInterval = 0);

	//Функция остановки процесса
	virtual bool Stop();

	//Функция получения интервала ожидания
	unsigned long GetInterval() const; 

	//Установка нового интервала ожидания 
	unsigned long SetInterval(unsigned  long interval);

	//Запущен ли поток
	bool IsActive() const 
	{ 
		return (m_hThread != NULL); 
	};


protected:
	//
	void Lock() const;

	void Unlock() const;


	// Функция выполняющаяся при остановке процесса 
	virtual void OnExit();

	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc() = 0;

protected:

	// Интервал ожидания между двумя действиями процесса
	unsigned long m_nInterval;	

	// Событие остановки
	HANDLE	m_hStopEvent;

	// Поток
	HANDLE	m_hThread;			 

	mutable CRITICAL_SECTION m_cs; 

private:

	static	DWORD WINAPI ThreadFunction(LPVOID lpParameter);
};



#endif // !defined(AFX_TIMEDCONTROL_H__F3FE3814_F5C2_4D12_886F_A89B055A5F5A__INCLUDED_)
