#pragma once

#include "PixelMaskingProcessing.h"

class PMP_Blurring :public PixelMaskingProcessing
{
public:
	PMP_Blurring();
	PMP_Blurring(const UINT power);

	void SetPower(const UINT power);
	void SetMotionPower(const UINT xPower, const UINT yPower);
};