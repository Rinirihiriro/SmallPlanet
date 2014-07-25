#pragma once

#include <Windows.h>

class CriticalSection
{
private:
	CRITICAL_SECTION *cs;

public:
	CriticalSection(CRITICAL_SECTION* cs);
	~CriticalSection();
};