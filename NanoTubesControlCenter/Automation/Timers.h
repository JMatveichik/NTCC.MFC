#if !defined(_TIMERS_H_INCLUDED_)
#define _TIMERS_H_INCLUDED_

#include "DataSrc.h"

class BaseTimer;
typedef boost::function< void (const BaseTimer*) > TIMERHANDLER;

class BaseTimer
{
public:
	
	BaseTimer(void);
	virtual ~BaseTimer(void);

	//�������� ������� �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ������� �� XML ����
	static BaseTimer* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	bool IsStarted() const;
	virtual bool IsOwerflowed() const;
	
	virtual bool Start();
	virtual bool Stop();

	enum { STARTHANDLER, STOPHANDLER, PAUSEHANDLER, OVERFLOWHANDLER};

	void AddTimerHandler(UINT handlerType, TIMERHANDLER handler); 

	long Duration() const;
	
	long StartTime() const;

	
public:


	static bool AddTimer(BaseTimer* pTimer);
	static BaseTimer* GetTimer(std::string id);
	static void ClearAll();

	virtual const char* ToString();

	std::string Name() const;
	

protected:

	virtual void OnStartDSChangeState(const DiscreteDataSrc* pDS );
	virtual void OnStopDSChangeState(const DiscreteDataSrc* pDS );

protected:

	CTime			m_stStart;
	std::string		m_strElapsedTime;

	std::string m_strOutFmt;
	std::string m_strName;

	bool		m_started;
	//bool		m_prevDSState;
	
	mutable long m_lDur;
	const AnalogDataSrc* m_pDurationDS;

	bool m_startState;
	bool m_stopState;

private:
	
	bool InitDSListen(bool bStart,  std::string& val);

	//��� ��������� ��������� �������
	static std::map<std::string, BaseTimer*> m_mapTimers;

	//��������� ������� ������� �������
	boost::signals2::signal< void (const BaseTimer*) > evOnTimerStart;

	//��������� ������� ��������� �������
	boost::signals2::signal< void (const BaseTimer*) > evOnTimerStop;
	
	//��������� ������� ������������ ������� 
	boost::signals2::signal< void (const BaseTimer*) > evOnTimerOverFlow;
};


class CountDownTimer : public BaseTimer
{
public:
	
	CountDownTimer();
	virtual ~CountDownTimer();

	//�������� ������� �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ������� �� XML ����
	static CountDownTimer* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	virtual const char* ToString();
	
};

#endif //_TIMERS_H_INCLUDED_