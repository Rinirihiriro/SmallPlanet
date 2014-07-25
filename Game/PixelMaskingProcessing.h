#pragma once

#include "Texture.h"

class PixelMaskingProcessing
{
protected:
	float* mask;
	int maskW;

public:
	PixelMaskingProcessing();
	virtual ~PixelMaskingProcessing();

	void ProcessTexture(Texture* in, Texture* out);
	void ProcessTexture(Texture* texture);
};