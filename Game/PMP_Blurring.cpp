#include "PMP_Blurring.h"
#include "Essential.h"

PMP_Blurring::PMP_Blurring(){}

PMP_Blurring::PMP_Blurring(const UINT power)
{
	SetPower(power);
}

void PMP_Blurring::SetPower(const UINT power)
{
	maskW = power*2+1;
	int halfMaskW = maskW/2;
	int maskWsq = maskW*maskW;
	SAFE_DELETE_ARR(mask);
	mask = new float[maskWsq];
	int r = halfMaskW*halfMaskW;
	int count=0;
	for (int y=0; y<(int)maskW; y++)
	{
		for (int x=0; x<(int)maskW; x++)
		{
			if ((int)((x-halfMaskW)*(x-halfMaskW)+(y-halfMaskW)*(y-halfMaskW))<=r)
			{
				count++;
				mask[y*maskW+x] = 1.f;
			}
			else
				mask[y*maskW+x] = 0.f;
		}
	}
	float invCount = 1.f/count;
	for (int i=0; i<maskWsq; i++)
		mask[i]*=invCount;
}

void SetMotionPower(const UINT xPower, const UINT yPower)
{
// 	maskW = (xPower>yPower)? xPower*2+1:yPower*2+1;
// 	SAFE_DELETE_ARR(mask);
// 	mask = new float[maskW*maskW];
// 	mask[maskW*maskW/2] = 1;
// 	int count=0;
// 	int center = mask[maskW*maskW/2];
// 	for (int x=0; x<(int)maskW; x++)
// 	{
// 		mask[(maskW/2)*maskW+x]=center*;
// 	}
// 	for (int i=0; i<(int)(maskW*maskW); i++)
// 		mask[i]/=count;
}