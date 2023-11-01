#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__

#include <string>

class BaseMessageException
{
public:
	BaseMessageException( std::string message )
	{
		this->message = message;
	}
	
	~BaseMessageException()
	{
	}

inline const std::string& Message() { return message; }

protected:
	std::string message;
};

class XYPlotException : public BaseMessageException
{
public:
	XYPlotException( std::string message ) : BaseMessageException( message )
	{
	}
};


#endif // __EXCEPTIONS_H__