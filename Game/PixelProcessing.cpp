#include "PixelProcessing.h"

#include "Essential.h"

PixelProcessing::~PixelProcessing(){}

void PixelProcessing::ProcessTexture(Texture* in, Texture* out)
{
	const UINT width = in->GetSurfaceWidth();
	const UINT height = in->GetSurfaceHeight();

	DWORD *inSrf = (DWORD*)in->LockRect();
	DWORD *outSrf = (DWORD*)out->LockRect();

	for (int y=0; y<(int)height; y++)
	{
		for (int x=0; x<(int)width; x++)
		{
			ProcessPixel(&inSrf[y*width+x], &outSrf[y*width+x]);
		}
	}

	out->UnlockRect();
	in->UnlockRect();
}

void PixelProcessing::ProcessTexture(Texture* texture)
{
	const UINT width = texture->GetSurfaceWidth();
	const UINT height = texture->GetSurfaceHeight();

	DWORD *srf = (DWORD*)texture->LockRect();
	DWORD *tempSrf = new DWORD[texture->GetSurfaceWidth()*texture->GetSurfaceHeight()];

	for (int y=0; y<(int)height; y++)
	{
		for (int x=0; x<(int)width; x++)
		{
			ProcessPixel(&srf[y*width+x], &tempSrf[y*width+x]);
		}
	}

	for (int i=0; i<(int)(height*width); i++)
		srf[i] = tempSrf[i];

	texture->UnlockRect();
	SAFE_DELETE_ARR(tempSrf);
}
