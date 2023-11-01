#ifndef __GLOBAL_H_INCLUDED__
#define __GLOBAL_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>

namespace Global
{
	static const char* defaultRegLocation = "Software\\ARTech\\Components\\xyplot\\Defaults";
	static const char* currentRegLocation = "Software\\ARTech\\Components\\xyplot\\Preffered";

	bool SafeDouble2Ascii(char* pszBuffer, unsigned buf_size, const char* pszTemplate, double d);

	long Pointer2ID(const void* p);
}

#endif