#include "PP_MonoColor.h"

PP_MonoColor::PP_MonoColor()
	:mMonoColor(1,1,1,1)
{

}

PP_MonoColor::PP_MonoColor(const D3DXCOLOR monocolor)
	:mMonoColor(monocolor)
{

}

void PP_MonoColor::ProcessPixel(DWORD* inPx, DWORD* outPx)
{
	D3DXCOLOR in = *inPx;
	float c = in.r*0.3f + in.g*0.59f + in.b*0.11f;
	*outPx = D3DXCOLOR(c*mMonoColor.r, c*mMonoColor.g, c*mMonoColor.b, in.a);
}