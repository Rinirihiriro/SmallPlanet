#pragma once

#include "PixelProcessing.h"

class PP_MonoColor :public PixelProcessing
{
private:
	D3DXCOLOR mMonoColor;

public:
	PP_MonoColor();
	PP_MonoColor(const D3DXCOLOR monocolor);

protected:
	virtual void ProcessPixel(DWORD* inPx, DWORD* outPx) override;
};