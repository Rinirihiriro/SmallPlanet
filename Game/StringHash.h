#pragma once

#include <tchar.h>

class StringHash
{
public:
	static unsigned ToHash(char* const str);
	static unsigned ToHash(TCHAR* const str);
};