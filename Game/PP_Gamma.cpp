#include "PP_Gamma.h"

#include <cmath>

PP_Gamma::PP_Gamma(const float gamma)
{
	invGamma = (gamma!=0)? 1/gamma:(float)0/*HUGE_VAL*/;
}

void PP_Gamma::ProcessPixel(DWORD* inPx, DWORD* outPx)
{
	D3DXCOLOR in = *inPx, out;
	out.r = powf(in.r, invGamma);
	out.g = powf(in.g, invGamma);
	out.b = powf(in.b, invGamma);
	out.a = in.a;
	*outPx = out;
}
