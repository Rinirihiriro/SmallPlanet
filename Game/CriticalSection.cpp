#include "CriticalSection.h"

CriticalSection::CriticalSection(CRITICAL_SECTION* cs)
{
	this->cs = cs;
	EnterCriticalSection(cs);
}

CriticalSection::~CriticalSection()
{
	LeaveCriticalSection(cs);
}