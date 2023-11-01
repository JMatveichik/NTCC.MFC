#pragma once

#ifndef _MAYERS_SINGLETONE_H_INCLUDED_
#define _MAYERS_SINGLETONE_H_INCLUDED_

template<class T>
class Singletone
{
public:

	static T& Instance()
	{
		static T obj;
		return obj;
	}

	virtual ~Singletone() {};

protected:

	Singletone() { };
	Singletone(const Singletone& );
	const Singletone& operator= (const Singletone&);
};

#endif //_MAYERS_SINGLETONE_H_INCLUDED_