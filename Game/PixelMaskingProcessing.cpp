#include "PixelMaskingProcessing.h"

#include "Essential.h"

#include <ppl.h>

PixelMaskingProcessing::PixelMaskingProcessing()
{
	mask = NULL;
	maskW = 0;
}

PixelMaskingProcessing::~PixelMaskingProcessing()
{
	SAFE_DELETE_ARR(mask);
}

void PixelMaskingProcessing::ProcessTexture(Texture* in, Texture* out)
{
	const UINT srfW = in->GetSurfaceWidth();
	const UINT srfH = in->GetSurfaceHeight();

	const int imgW = (int)in->GetImageWidth();
	const int imgH = (int)in->GetImageHeight();

	int halfMaskW = maskW/2;
	int maskStartX = -halfMaskW, maskEndX = halfMaskW;
	int maskStartY = maskStartX, maskEndY = maskEndX;
	float fMask;
	D3DXCOLOR pxColor=D3DXCOLOR(0,0,0,0);
	D3DXCOLOR inColor;

	DWORD *inSrf = (DWORD*)in->LockRect();
	DWORD *outSrf = (DWORD*)out->LockRect();

// 	Concurrency::parallel_for(0, (int)(imgW*imgH), [&](int i)
	for (int y=0, i=0; y<imgH; y++)
	{
		for (int x=0; x<imgW; x++, i++)
		{
			pxColor=D3DXCOLOR(0,0,0,0);

			for (int maskY=maskStartY, j=0; maskY<=maskEndY; maskY++)
			{
				for (int maskX=maskStartX; maskX<=maskEndX; maskX++, j++)
				{
					int xx=x+maskX, yy=y+maskY;
					// int xx=(i%imgW)+maskX, yy=(i/imgW)+maskY;
					if (xx<0 || yy<0 || xx>=imgW || yy>=imgH)
						inColor = inSrf[i];
					else
						inColor = inSrf[yy*srfW+xx];

					// fMask = mask[(maskY+halfMaskW)*maskW+maskX+halfMaskW];
					fMask = mask[j];

					pxColor += inColor*fMask;
				}
			}

			if (pxColor.r>1.f) pxColor.r=1.f;
			if (pxColor.g>1.f) pxColor.g=1.f;
			if (pxColor.b>1.f) pxColor.b=1.f;
			if (pxColor.a>1.f) pxColor.a=1.f;

			outSrf[i] = pxColor;
// 			outSrf[y*srfW+x] = pxColor;
		}
	}
// 	);
	out->UnlockRect();
	in->UnlockRect();
}

void PixelMaskingProcessing::ProcessTexture(Texture* texture)
{
	const UINT srfW = texture->GetSurfaceWidth();
	const UINT srfH = texture->GetSurfaceHeight();

	const UINT imgW = texture->GetImageWidth();
	const UINT imgH = texture->GetImageHeight();

	DWORD *srf = (DWORD*)texture->LockRect();
	DWORD *tempSrf = new DWORD[texture->GetSurfaceWidth()*texture->GetSurfaceHeight()];

	for (int y=0; y<(int)imgH; y++)
	{
		for (int x=0; x<(int)imgW; x++)
		{
			int maskStartX = -maskW/2, maskEndX = maskW/2;
			int maskStartY = maskStartX, maskEndY = maskEndX;

			float fMask;
			D3DXCOLOR pxColor=D3DXCOLOR(0,0,0,0);
			D3DXCOLOR inColor;

			for (int maskY=maskStartY; maskY<=maskEndY; maskY++)
			{
				for (int maskX=maskStartX; maskX<=maskEndX; maskX++)
				{
					int xx=x+maskX, yy=y+maskY;
					if (xx<0) xx=0; else if(xx>=(int)imgW) xx=imgW-1;
					if (yy<0) yy=0; else if(yy>=(int)imgH) yy=imgH-1;

					inColor = srf[yy*srfW+xx];

					fMask = mask[(maskY+maskW/2)*maskW+maskX+maskW/2];

					pxColor.r += inColor.r*fMask;
					pxColor.g += inColor.g*fMask;
					pxColor.b += inColor.b*fMask;
					pxColor.a += inColor.a*fMask;
				}
			}

			if (pxColor.r>1.f) pxColor.r=1.f;
			if (pxColor.g>1.f) pxColor.g=1.f;
			if (pxColor.b>1.f) pxColor.b=1.f;
			if (pxColor.a>1.f) pxColor.a=1.f;

			tempSrf[y*srfW+x] = pxColor;
		}
	}

	for (int i=0; i<(int)(srfH*srfW); i++)
		srf[i] = tempSrf[i];

	texture->UnlockRect();
	SAFE_DELETE_ARR(tempSrf);
}
