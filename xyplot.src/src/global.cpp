#include "stdafx.h"
#include "global.h"

using namespace std;

bool Global::SafeDouble2Ascii(char* pszBuffer, unsigned buf_size, const char* pszTemplate, double d)
{
	static const int CRITICAL_STRING_LENGTH = 32;
	static const char SAFE_DOUBLE_FORMAT[] = "%g";

	const unsigned BUFSIZE = 1024;
	char szBuffer[BUFSIZE];
	int decimal, sign;
	char *p = szBuffer;
	_fcvt_s(szBuffer, BUFSIZE, d, 0, &decimal, &sign);
	if (strlen(p) > CRITICAL_STRING_LENGTH)
		p = (char*)SAFE_DOUBLE_FORMAT;
	else
		p = (char*)pszTemplate;

	sprintf_s(pszBuffer, buf_size, p, d);
	return p == pszTemplate;
}

long Global::Pointer2ID(const void* p)
{
	static map<long, const void*> g_id_map;
	map<long, const void*>::iterator it;

	if (g_id_map.empty())
		srand( (unsigned)time( NULL ) );

	for (it = g_id_map.begin(); it != g_id_map.end(); ++it)
	{
		if (it->second == p)
			return it->first;
	}

	long res;
	for(;;)
	{
		res = rand() + RAND_MAX + 2;
		
		it = g_id_map.find(res);
		
		if (it == g_id_map.end())
			break;
	}

	g_id_map[res] = p;

	return res;
}

