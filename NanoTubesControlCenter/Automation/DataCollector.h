#pragma once

#include "TimedControl.h"
#include "ContainersExt.h"
#include "DataSrc.h"
#include "Timers.h"

const double TIME_COEFFS[3] = { 1.0, 0.016667, 2.7777778E-4 };
enum { SECONDS, MINUTES, HOURS };


class DataCollector : public TimedControl
{
	
public:
	
	virtual ~DataCollector(void);
	static	DataCollector& Instance();

	//������������� �������� ������
	//double liveTime - ������������ ����� ����� ������, ��� 
	//double updateInterval - �������� ���������� ������, ��� 
	bool Initialize(double liveTime, double updateInterval);

public:

	//�������� ����� ������ ��� ��
	bool GetDataSourceBuffer(std::string name, std::vector<double>& data) const; 

	//�������� ����� ������ ��� �� �� ��������� N ���
	bool GetDataSourceBuffer(std::string name, std::vector<double>& data, int sec) const; 

	//�������� ��������� �������� ���������� � �����
	bool GetLast(std::string name, double& val) const; 

	//�������� ����� ������ �������
	bool GetTimeBuffer(std::vector<double>& data) const; 

	//�������� ������������ ��������� �������� 
	//������� ����� ��������� � ������, � ��������
	double GetTimeInterval() const; 

	//�������� ������������ ������ ������
	std::size_t GetSubBufferCapacity() const
	{ 
		return m_nCapacity; 
	}

private:

	void AppendData(std::string name, double data);

protected:

	DataCollector();
	DataCollector(const DataCollector& );
	const DataCollector& operator= (const DataCollector&);

	virtual void ControlProc();

public:
	//
	void LockData() const;

	void UnlockData() const;

private:

	void TicTime();

private:

	typedef std::deque< double >  SUBBUFFER;
	typedef std::map<std::string, SUBBUFFER> NAMED_MULTI_DEQUE;

	NAMED_MULTI_DEQUE m_mainDataBuffer;
	std::vector<const DataSrc*> m_ds;
	std::size_t m_nCapacity;
	
};


///
//////////////////////////////////////////////////////////////////////////


class DataLogger: public TimedControl
{
	
public:
	
	DataLogger();
	virtual ~DataLogger();


	static DataLogger* CreateFromXML(IXMLDOMNode* pNode, UINT& nError);
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	bool Create(std::string name, std::vector< std::pair<std::string, bool> > DSIDs, 
				std::string pszLogFileDir = NULL, 
				std::string pszFileExt="txt", 
				bool bClearOldData = true, 
				char cSep = '\t', 
				std::string pszTimeFormat = "MINUTES" );

public:

	bool MakeOutFile(std::string dir, bool bClearOldData);

	void WriteData(std::string comment = "");

	virtual bool Start(unsigned long nInterval = 0); 

	static DataLogger* GetDataLogger(std::string name);
	static void ClearAll();
	static bool AddDataLogger(DataLogger* pDL);

public:

	void OnAnalogDSChanged(const AnalogOutputDataSrc* pADS);
	void OnDiscreteDSChanged(const DiscreteDataSrc* pDDS);

protected:

	virtual void ControlProc();

public:
	
	std::string GetOutFilePath() const 
	{ 
		return m_strLogFilePath; 
	}

// 	std::string GetOutDir() const 
// 	{ 
// 		return m_strLogFileDir; 
// 	}

	int GetItemCount() const
	{
		return (int)m_ds.size();
	}

	const DataSrc* GetItem(int i) const
	{
		return m_ds.at(i);
	}
	std::string Name() const
	{
		return m_strID;
	}

private:

	std::string		m_strLogFilePath;	//���� � ����� ������
	std::string		m_strBaseDir;		//���� � ����� ������
	std::string     m_strID;
	std::string     m_strFileExt;
	int m_nLatsDay;
	char			m_cSep;				//����������� ������
	std::ofstream	m_streamData;		//�������� �������� ����� ������


	unsigned long m_lStartTime;			//����� �������

	double m_dTimeCoef;

	std::vector<const DataSrc* > m_ds;
	static std::map<std::string, DataLogger*> m_loggers;
	
	const DiscreteDataSrc* pStartStopDS;  
};
