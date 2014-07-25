#pragma once

#include "Texture.h"

class PixelProcessing
{
protected:
	virtual void ProcessPixel(DWORD* inPx, DWORD* outPx)=0;

public:
	virtual ~PixelProcessing();

	void ProcessTexture(Texture* in, Texture* out);
	void ProcessTexture(Texture* texture);
};
