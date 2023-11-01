// ContainersExt.h: interface for the CMultiQueueBuffer class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(_CONTAINERS_EXT_H_INCLUDED_)
#define _CONTAINERS_EXT_H_INCLUDED_

#pragma warning(disable:4786)
#include <map>
#include <deque>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <class DATA>
class CMultiQueueBuffer  
{
	typedef std::vector<DATA*> SINGLEDATA;
	
public:
	CMultiQueueBuffer();
	virtual ~CMultiQueueBuffer();

	bool Initialize(std::size_t nSubBufferCount, std::size_t nSubBufferSize);
	void Clear();
	bool AppendData(DATA* pdData);

	const DATA* GetSubBufferData(std::size_t nIndex) const;

	std::size_t GetSubBufferCount() const {return m_nSubBufferCount;};	
	std::size_t GetSubBufferSize() const;
		
private:
	
	SINGLEDATA m_DataBuffer;
	
	std::size_t m_nBufferSize;
	std::size_t m_nSubBufferCount;
	std::size_t m_nAppendCount;

	CRITICAL_SECTION m_cs;

};

template <class DATA>
CMultiQueueBuffer<DATA>::CMultiQueueBuffer() :
m_nBufferSize(0),  m_nSubBufferCount(0), m_nAppendCount(0)
{	
	::InitializeCriticalSection(&m_cs);
}

template <class DATA>
CMultiQueueBuffer<DATA>::~CMultiQueueBuffer()
{
	Clear();
	::DeleteCriticalSection(&m_cs);
}

template <class DATA>
void CMultiQueueBuffer<DATA>::Clear()
{
	::EnterCriticalSection(&m_cs);

	for(SINGLEDATA::iterator it = m_DataBuffer.begin(); it != m_DataBuffer.end(); ++it)	
		delete [] (*it);

	m_nAppendCount = 0;
	m_DataBuffer.clear();

	::LeaveCriticalSection(&m_cs);
}

template <class DATA>
bool CMultiQueueBuffer<DATA>::Initialize( std::size_t nSubBufferCount, std::size_t nBufferSize)
{
	::EnterCriticalSection(&m_cs);

	// Clear old data
	Clear();

	m_nBufferSize		= nBufferSize;		//Set new buffer size
	m_nSubBufferCount	= nSubBufferCount;	//Set new subbuffer count	
	
	for(std::size_t i=0; i < nSubBufferCount; i++) //Initialize data map
		m_DataBuffer.push_back( new DATA[nBufferSize + 1] );
				
	
	::LeaveCriticalSection(&m_cs);
	return true;
}

template <class DATA>
std::size_t CMultiQueueBuffer<DATA>::GetSubBufferSize() const 
{
	return (m_nAppendCount <= m_nBufferSize) ? m_nAppendCount : m_nBufferSize;
}

template <class DATA>
bool CMultiQueueBuffer<DATA>::AppendData(DATA* pNewData)
{
	::EnterCriticalSection(&m_cs);	
	
	for(std::size_t i = 0; i < m_nSubBufferCount; i++) 
	{		
		DATA* pData = m_DataBuffer.at(i);
		
		if (m_nAppendCount <= m_nBufferSize) 
			pData[m_nAppendCount] = pNewData[i];
		else 
		{
			memcpy(pData, &pData[1],  sizeof(DATA)*(m_nBufferSize - 1));
			pData[m_nBufferSize - 1]	= pNewData[i];	
		}
	}
	
	m_nAppendCount++;
	::LeaveCriticalSection(&m_cs);	
	
	return true;
}

template <class DATA>
const DATA* CMultiQueueBuffer<DATA>::GetSubBufferData(std::size_t nIndex) const
{
	return m_DataBuffer.at(nIndex);	
}

template <class T>
class rotated_queue
{
protected:
	std::deque<T> cont;

public:
	
	//число элементов
	typename std::deque<T>::size_type size() const
	{
		return cont.size();
	}

	//проверка пустой очереди
	bool empty() const
	{
		return cont.empty();
	}

	//добавление елемента в очередь
	void push(const T& elem)
	{
		cont.push_back(elem);
	}

	//извлечение элемента из очереди с возвращением его значения
	T pop()
	{
		if ( cont.empty() )
			throw std::length_error("Queue is empty");

		T elem( cont.front() );
		cont.pop_front();
		return elem;
	}

	//перемещение элемента из начала очереди в конец с возвращением его значения
	T rotate()
	{
		if ( cont.empty() )
			throw std::length_error("Queue is empty");

		T elem( cont.front() );
		
		cont.pop_front();
		cont.push_back(elem);

		return elem;
	}
	
	//очистка очереди
	void clear()
	{
		cont.clear();
	}

	//получение значения следующего элемента
	T& front() 
	{
		if ( cont.empty() )
			throw std::length_error("Queue is empty");
		
		return cont.front();
	}

	const T& front() const
	{
		if ( cont.empty() )
			throw std::length_error("Queue is empty");

		return cont.front();
	}

	//получение значения последнего элемента
	T& back()
	{
		if ( cont.empty() )
			throw std::length_error("Queue is empty");

		return cont.back();
	}

	const T& back() const
	{
		if ( cont.empty() )
			throw std::length_error("Queue is empty");

		return cont.back();
	}

};

#endif // !defined(_CONTAINERS_EXT_H_INCLUDED_)
