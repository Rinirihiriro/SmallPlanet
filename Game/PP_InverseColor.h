#pragma once

#include "PixelProcessing.h"

class PP_InverseColor :public PixelProcessing
{
protected:
	virtual void ProcessPixel(DWORD* inPx, DWORD* outPx);
};