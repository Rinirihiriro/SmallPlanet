#pragma once

#include "PixelProcessing.h"

class PP_Gamma :public PixelProcessing
{
public:
	float invGamma;

protected:
	virtual void ProcessPixel(DWORD* inPx, DWORD* outPx) override;

public:
	PP_Gamma(const float gamma);

};