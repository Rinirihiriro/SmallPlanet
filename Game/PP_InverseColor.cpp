#include "PP_InverseColor.h"

void PP_InverseColor::ProcessPixel(DWORD* inPx, DWORD* outPx)
{
	D3DXCOLOR in = *inPx;
	*outPx = D3DXCOLOR(1-in.r, 1-in.g, 1-in.b, in.a);
}