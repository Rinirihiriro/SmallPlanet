#include "StringHash.h"

#include <string>

unsigned StringHash::ToHash(char* const str)
{
	unsigned hash = 0;
	size_t len = strlen(str);
	for (size_t i=0; i<len; i++)
	{
		hash = 65599*hash + str[i];
	}
	return hash^(hash>>16);
}

unsigned StringHash::ToHash(TCHAR* const str)
{
	unsigned hash = 0;
	size_t len = _tcslen(str);
	for (size_t i=0; i<len; i++)
	{
		hash = 65599*hash + str[i];
	}
	return hash^(hash>>16);
}
