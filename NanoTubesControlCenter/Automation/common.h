 #if !defined(_ADAM_COMMON_H_INCLUDED_)
#define _ADAM_COMMON_H_INCLUDED_

template <class T>
struct delete_object : public std::unary_function<T, void> 
{
	void operator() (T* pObj) const 
	{ 
		delete pObj; 
	}
};

template <typename ElemT>
struct hex_to {
	
	ElemT value;
	
	operator ElemT() const 
	{
		return value;
	}
	
	friend std::istream& operator>>(std::istream& in, hex_to& out) 
	{
		in >> std::hex >> out.value;
		return in;
	}
};

template <typename T>
void ExtractData(std::string str, const char* s, std::vector<T>& data)
{
	data.clear();
	boost::char_separator<char> sep(s);
	boost::tokenizer< boost::char_separator<char> > tokens(str, sep);

	for ( boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		try {
			data.push_back( boost::lexical_cast<T>(*it) );
		}
		catch(boost::bad_lexical_cast& e)
		{
			e;
			break;
		}		
	}	
};


template <typename T>
void ExtractData(std::string str, std::string seps, std::vector<T>& data)
{
	data.clear();
	
	size_t indFrom = 0;
	size_t indTo   = str.find_first_of(seps, indFrom + 1);

	while ( true )
	{
		if ( indTo == string::npos )
			indTo = str.length();

		std::string part = str.substr(indFrom, indTo - indFrom);
		if (part.empty())
			break;

		try {
			data.push_back( boost::lexical_cast<T>(part) );
		}
		catch(boost::bad_lexical_cast& e)
		{
			e;
			break;
		}

		indFrom = indTo;
		indTo = str.find_first_of(seps, indFrom + 1);
	}	
};

template <typename T>
void ExtractData(std::string str, int substrlen, std::vector<T>& data)
{
	data.clear();
	std::vector<string> tokens;
	
	int count = str.length() / substrlen;
	for (int i = 0; i < count; i++)
	{
		tokens.push_back( str.substr(i*substrlen, substrlen) );
	}

	for ( auto it = tokens.begin(); it != tokens.end(); ++it)
	{
		try {
			data.push_back( boost::lexical_cast<T>(*it) );
		}
		catch(boost::bad_lexical_cast& e)
		{
			e;
			break;
		}		
	}	
};

/*
template <typename T1, typename T2 = T1>
struct pair_to {

	std::pair<T1, T2> val;

	operator std::pair<T1,T2>() const 
	{
		return val;
	}

	friend std::istream& operator>>(std::istream& in, pair_to<T1, T2>& out) 
	{
		std::string s;		
		in >> s; //out.pt.x >> out.pt.y;

		std::vector<std::string> parts;
		boost::split(parts, s, boost::is_any_of(";"));

		if ( parts.size() != 2 )
		{
			out.val.first  = boost::lexical_cast<T1>("0");
			out.val.second = boost::lexical_cast<T2>("0");
		}
		else 
		{
			out.val.first  = boost::lexical_cast<T1>(parts[0]);
			out.val.second = boost::lexical_cast<T2>(parts[0]);
		}

		return in;
	}
};
*/
typedef struct tagMeassureRangeInfo
{
	tagMeassureRangeInfo() : rngid(0xff), desc("Unknown"), units(""), mins(0.0), maxs(100.0) 
	{		
	}

	tagMeassureRangeInfo(const tagMeassureRangeInfo& src)
	{
		*this = src;
	}
	const tagMeassureRangeInfo& operator = (const tagMeassureRangeInfo& src)
	{
		if (this == &src)
			return *this;

		this->rngid = src.rngid;	 //идентификатор измеряемого диапазона
		this->desc	= src.desc;	 //измеряемая величина
		this->units = src.units;	 //единицы	 измерения
		this->mins  = src.mins;	 //минимальное значение измеряемой величины
		this->maxs  = src.maxs;	 //максимальное значение измеряемой величины

		return *this;
	}

	tagMeassureRangeInfo(long rngid, std::string desc, std::string units, double mins, double maxs) 
	{
		this->rngid = rngid;	 //идентификатор измеряемого диапазона
		this->desc	= desc;	 //измеряемая величина
		this->units = units;	 //единицы	 измерения
		this->mins  = mins;	 //минимальное значение измеряемой величины
		this->maxs  = maxs;	 //максимальное значение измеряемой величины
	}


	long		rngid;	//идентификатор измеряемого диапазона
	std::string desc;	//измеряемая величина
	std::string units;	//единицы	 измерения
	double		mins;	//минимальное значение измеряемой величины
	double		maxs;	//максимальное значение измеряемой величины	

}MEASSURE_RANGE_INFO, *LPMEASSURE_RANGE_INFO;


typedef struct tagRANGES {

private:

	std::map< long, MEASSURE_RANGE_INFO > rngs;

public:

	tagRANGES() 
	{
		rngs.insert(std::make_pair(0x00, MEASSURE_RANGE_INFO(0x00, "Напряжение ± 15 mV", "mV", -15.0, 15.0)));
		rngs.insert(std::make_pair(0x01, MEASSURE_RANGE_INFO(0x01, "Напряжение ± 50 mV", "mV", -50.0, 50.0)));
		rngs.insert(std::make_pair(0x02, MEASSURE_RANGE_INFO(0x02, "Напряжение ± 100 mV", "mV", -100.0, 100.0)));
		rngs.insert(std::make_pair(0x03, MEASSURE_RANGE_INFO(0x03, "Напряжение ± 500 mV", "mV", -500.0, 500.0)));
		rngs.insert(std::make_pair(0x04, MEASSURE_RANGE_INFO(0x04, "Напряжение ± 1 V", "V", -1.0, 1.0)));
		rngs.insert(std::make_pair(0x05, MEASSURE_RANGE_INFO(0x05, "Напряжение ± 2.5 V", "V", -2.5, 2.5)));
		rngs.insert(std::make_pair(0x06, MEASSURE_RANGE_INFO(0x06, "Напряжение ± 20 mV", "mV", -20.0, 20.0)));
		rngs.insert(std::make_pair(0x07, MEASSURE_RANGE_INFO(0x07, "Ток 4-20 mA", "mA", 4.0, 20.0)));
		rngs.insert(std::make_pair(0x08, MEASSURE_RANGE_INFO(0x08, "Напряжение ± 10 V", "V", -10.0, 10.0)));
		rngs.insert(std::make_pair(0x09, MEASSURE_RANGE_INFO(0x09, "Напряжение ± 5 V", "V", -5.0, 5.0)));
		rngs.insert(std::make_pair(0x0A, MEASSURE_RANGE_INFO(0x0A, "Напряжение ± 1 V", "V", -1.0, 1.0)));
		rngs.insert(std::make_pair(0x0B, MEASSURE_RANGE_INFO(0x0B, "Напряжение ± 500 mV", "mV", -500.0, 500.0)));
		rngs.insert(std::make_pair(0x0C, MEASSURE_RANGE_INFO(0x0C, "Напряжение ± 150 mV", "mV", -150.0, 150.0)));
		rngs.insert(std::make_pair(0x0D, MEASSURE_RANGE_INFO(0x0D, "Ток ± 20 mA", "mA", -20.0, 20.0)));
		rngs.insert(std::make_pair(0x0E, MEASSURE_RANGE_INFO(0x0E, "ТП типа J", "°C", 0.0, 760.0)));
		rngs.insert(std::make_pair(0x0F, MEASSURE_RANGE_INFO(0x0F, "ТП типа K", "°C", 0.0, 1370.0)));
		rngs.insert(std::make_pair(0x10, MEASSURE_RANGE_INFO(0x10, "ТП типа T", "°C", 100.0, 400.0)));
		rngs.insert(std::make_pair(0x11, MEASSURE_RANGE_INFO(0x11, "ТП типа E", "°C", 0.0, 1000.0)));
		rngs.insert(std::make_pair(0x12, MEASSURE_RANGE_INFO(0x12, "ТП типа R", "°C", 500.0, 1750.0)));
		rngs.insert(std::make_pair(0x13, MEASSURE_RANGE_INFO(0x13, "ТП типа S", "°C", 500.0, 1750)));
		rngs.insert(std::make_pair(0x14, MEASSURE_RANGE_INFO(0x14, "ТП типа B", "°C", 500.0, 1800.0))); 
		rngs.insert(std::make_pair(0x20, MEASSURE_RANGE_INFO(0x20, "Pt100 (а=0.00385)", "°C", -100.0, 100.0)));
		rngs.insert(std::make_pair(0x21, MEASSURE_RANGE_INFO(0x21, "Pt100 (а=0.00385)", "°C", 0.0, 100.0)));
		rngs.insert(std::make_pair(0x22, MEASSURE_RANGE_INFO(0x22, "Pt100 (а=0.00385)", "°C", 0.0, 200.0)));
		rngs.insert(std::make_pair(0x23, MEASSURE_RANGE_INFO(0x23, "Pt100 (а=0.00385)", "°C", 0.0, 600.0)));
		rngs.insert(std::make_pair(0x24, MEASSURE_RANGE_INFO(0x24, "Pt100 (а=0.00392)", "°C", -100.0, 100.0)));
		rngs.insert(std::make_pair(0x25, MEASSURE_RANGE_INFO(0x25, "Pt100 (а=0.00392)", "°C", 0.0, 100.0)));
		rngs.insert(std::make_pair(0x26, MEASSURE_RANGE_INFO(0x26, "Pt100 (а=0.00392)", "°C", 0.0, 200.0)));
		rngs.insert(std::make_pair(0x27, MEASSURE_RANGE_INFO(0x27, "Pt100 (а=0.00392)", "°C", 0.0, 600.0)));
		rngs.insert(std::make_pair(0x28, MEASSURE_RANGE_INFO(0x28, "Ni термослпротивление (а=0.00385)", "°C", -80.0, 100.0)));
		rngs.insert(std::make_pair(0x29, MEASSURE_RANGE_INFO(0x29, "Ni термослпротивление (а=0.00392)", "°C", -80.0, 100.0)));
		rngs.insert(std::make_pair(0x30, MEASSURE_RANGE_INFO(0x30, "Ток ± 20 mA", "mA", -20.0, 20.0)));
		rngs.insert(std::make_pair(0x31, MEASSURE_RANGE_INFO(0x31, "Ток 4-20 mA", "mA", 4.0, 20.0)));
		rngs.insert(std::make_pair(0x32, MEASSURE_RANGE_INFO(0x32, "Напряжение 0-10 V", "V", 0.0, 10.0)));		
	}

	const MEASSURE_RANGE_INFO& operator [](long rt)
	{
		std::map< long, MEASSURE_RANGE_INFO>::const_iterator itFind = rngs.find(rt);		

		if (itFind == rngs.end())
			return rngs[0xff];
		else
			return (*itFind).second;
	}

} ADAM_RANGES;


static ADAM_RANGES ranges;


#define MAX_CMD_LEN 256
#define MAX_RESPONSE_LEN 1024

//////////////////////////////////////////////////////////////////////////

class IdentifiedPhysDevice;

typedef struct tagASCIICMD
{
	char			cmd[MAX_CMD_LEN];
	char			response[MAX_RESPONSE_LEN];
	int length;	

	const IdentifiedPhysDevice* pSender;
	tagASCIICMD* pSrcCmd;

	HANDLE			hCommand;

} ASCII_COMMAND, *LPASCII_COMMAND;
typedef const ASCII_COMMAND* LPCASCII_COMMAND;


typedef struct tagModBusCommand
{
	unsigned char func;
	unsigned char slaveAddress;	
	int regAddress; 
	int regCount;

	unsigned short response[MAX_CMD_LEN];
	
	 
	const IdentifiedPhysDevice* pSender;
	tagModBusCommand* pSrcCmd;

	HANDLE			hCommand;

} MODBUS_SERIAL_COMMAND, *LPMODBUS_SERIAL_COMMAND;

typedef struct tagSerialCommand
{
	int size;
	unsigned int	cmdLife;

	union 
	{
		
		MODBUS_SERIAL_COMMAND modbus;
		ASCII_COMMAND   ascii;
	};

}SERIAL_COMMAND, *LPSERIAL_COMMAND;

typedef std::list<SERIAL_COMMAND> SERIAL_CMD_LIST;

//Интерфейс для устройств включающих в себя один или несколько 
//аналоговых входов с которых считываются данные
class IAnalogInput
{
public:
	virtual ~IAnalogInput() {}

	//Получение общего числа аналоговых входов
	virtual int GetChannelsCountAI() const = 0;

	//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
	virtual void	ReadChannels(double* data  = NULL) const = 0;

	//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
	virtual double  ReadChannel(unsigned char channel)  const = 0;

	// Получение данных из буфера для одного из аналоговых входов 
	// (запрос на чтение данных не выполняется)
	virtual double  GetChannelData(unsigned char channel)  const = 0;

	//Получить  информацию об измеряемом диапазоне канала
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const = 0;

	//Установить  диапазон  аналогового входа
	virtual bool SetInputRange(unsigned char ch, unsigned char range)  = 0;

	//Получить все возможные  диапазоны  измерения
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const = 0;

	
};

//Интерфейс для устройств включающих в себя один или несколько 
//аналоговых выходов в которые записываются данные
class IAnalogOutput
{
public:
	virtual ~IAnalogOutput() {}

	//Получение общего числа аналоговых выходов 
	virtual int GetChannelsCountAO() const = 0;

	//Записать данные в аналоговый выход 
	virtual double  WriteToChannel(unsigned char channel, double val) = 0;

	//Записать данные во все каналы
	virtual void WriteToAllChannels(const std::vector<double>& vals)  = 0; 

	//Получить последнее записанное в аналоговый выход значение 
	virtual double GetChannelStateAO (unsigned char channel) const = 0;  

	//Получить выходной диапазон канала 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const = 0; 

	//Получить  информацию об диапазоне канала
	virtual void GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const = 0;

	//Установить  диапазон  аналогового выхода
	virtual bool SetOutputRange(unsigned char ch, unsigned char range)  = 0;

	//Получить все возможные  диапазоны  
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const = 0;

};

//Интерфейс для устройств включающих в себя один или несколько 
//дискретных входов с которых считываются данные
class IDigitalInput
{
public:
	virtual ~IDigitalInput() {}

	//Получение общего числа дискретных входов
	virtual int GetChannelsCountDI() const = 0;

	//Обновить состояния дискретных входов	
	virtual bool UpdateChannelsStateDI() const = 0;

	//Получение состояния одного из дискретных входов	
	virtual bool  GetChannelStateDI(unsigned char channel, bool update)  const = 0;

};

//Интерфейс для устройств включающих в себя один или несколько 
//цифровых выходов в которые записываются данные
class IDigitalOutput
{
public:
	virtual ~IDigitalOutput() {}

	//Получение общего числа цифровых выходов 
	virtual int GetChannelsCountDO() const = 0;

	//Обновить текущее состояния дискретных выходов	
	virtual bool UpdateChannelsStateDO() const = 0;

	//Получить состояние цифрового выхода
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const  = 0;

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable) = 0;	
	
	//Записать данные во все каналы
	template <class T>
	void SetChannelsStateDO(const T& val)
	{
		const int size = min(sizeof(T)*8, GetChannelsCountDO());
		static bitmask<T> mask;

		for(int i = 0; i < size; i++)
			SetChannelStateDO(i, mask.check_bit(val, i));
	}

};

//Интерфейс для устройств передающих данные 
class IDataProvider
{
public:
	virtual ~IDataProvider() {}

	virtual std::string  DataPass(const IdentifiedPhysDevice* pSender, std::string data, bool bHiPriority) const = 0;

};



const int TC_BURN_VALUE = 888888;

const double UNKNONW_VALUE = 88888.99999;

inline std::string PrintTime(const SYSTEMTIME& st)
{
	char szBuffer[16];
	sprintf_s(szBuffer, 16, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
	return std::string(szBuffer);
}

inline std::string PrintTime()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	return PrintTime(st);
}

inline unsigned long Milliseconds(const SYSTEMTIME& st)
{
	return 24*3600*1000*st.wDay + 3600*1000*st.wHour + 60*1000*st.wMinute + 1000*st.wSecond + st.wMilliseconds;
}

inline unsigned long Milliseconds()
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	return 24*3600*1000*st.wDay + 3600*1000*st.wHour + 60*1000*st.wMinute + 1000*st.wSecond + st.wMilliseconds;
}

inline unsigned long Milliseconds(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
	return Milliseconds(st2) - Milliseconds(st1);
}

inline bool IsIPString(const std::string& input)
{
	boost::regex pat("^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$");
	boost::match_results<std::string::const_iterator> what;
   
	if( 0 == boost::regex_match(input, what, pat, boost::match_default | boost::match_partial) )
		return false;
	
	if(what[0].matched)
		return true;
	
	return false;
}



#endif //_ADAM_COMMON_H_INCLUDED_