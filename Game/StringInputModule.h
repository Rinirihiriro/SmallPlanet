#pragma once

#include <tchar.h>

class StringInputModule
{
public:
	enum OPTION
	{
		NO_OPTION = 0x0
		, NUMBER_ONLY = 0x1
		, MULTI_LINE = 0x2
	};

	static void StringInputProc(TCHAR* str, const int str_len, OPTION options=NO_OPTION);
};