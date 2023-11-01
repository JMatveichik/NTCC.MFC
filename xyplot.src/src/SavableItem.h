#pragma once
#include <stdexcept>
#include <string>

class SavableItem
{
public:
	SavableItem() {};
	virtual ~SavableItem() {};

	virtual BOOL Write(HANDLE hFile) const = 0;
	virtual BOOL Read(HANDLE hFile) = 0;

protected:
	static std::string ReadString(HANDLE hFile);
	static BOOL WriteString(HANDLE hFile, std::string theString);
};
