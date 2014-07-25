#include "MyException.h"
#include "Texture.h"

void Texture::Init()
{
	color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
}

void Texture::LoadFile( LPCTSTR filename, const D3DCOLOR colorkey)
{
	LPDIRECT3DDEVICE9 device;
	sprite->GetDevice(&device);
	HRESULT hr = D3DXCreateTextureFromFileEx(
		device
		,filename
		,D3DX_DEFAULT
		,D3DX_DEFAULT
		,1
		,0
		,D3DFMT_UNKNOWN
		,D3DPOOL_MANAGED
		,0x00000001
		,0x00000001
		,colorkey
		,&info
		,NULL
		,&texture
		);
	if (FAILED(hr)) throw ObjectWasNotCreated(filename, hr);
	width = info.Width;
	height = info.Height;
	
	sizeW = width/vframe;
	sizeH = height/hframe;
	
	texture->GetLevelDesc(0, &srf_desc);
	LockRect();
	UnlockRect();
}

void Texture::CreateTexture(const UINT width, const UINT height, const D3DFORMAT format)
{
	LPDIRECT3DDEVICE9 device;
	sprite->GetDevice(&device);
	HRESULT hr = D3DXCreateTexture(
		device
		, width
		, height
		, 1
		, 0
		, format
		, D3DPOOL_MANAGED
		, &texture
		);
	if (FAILED(hr)) throw ObjectWasNotCreated(L"Texture", hr);

	sizeW = width/vframe;
	sizeH = height/hframe;

	texture->GetLevelDesc(0, &srf_desc);
	LockRect();
	UnlockRect();
}

Texture::Texture( LPCTSTR filename, const D3DXVECTOR3 *center
	, const UINT vframe, const UINT hframe, const D3DCOLOR colorkey)
	:vframe(vframe), hframe(hframe), frame(vframe*hframe)
{
	Init();
	LoadFile(filename, colorkey);
	if (center) this->center = *center;
	else Centering();
}

Texture::Texture( const UINT srfWidth, const UINT srfHeight, const D3DFORMAT format
	, const UINT *imgWidth, const UINT *imgHeight
	, const D3DXVECTOR3 *center, const UINT vframe, const UINT hframe)
	:vframe(vframe), hframe(hframe), frame(vframe*hframe)
{
	Init();

	if (imgWidth) width = *imgWidth;
	else width = srfWidth;
	if (imgHeight) height = *imgHeight;
	else height = srfHeight;

	CreateTexture(srfWidth, srfHeight, format);
	if (center) this->center = *center;
	else Centering();
}

Texture::Texture( LPDIRECT3DTEXTURE9 texture, const UINT width, const UINT height
	, const D3DXVECTOR3 *center, const UINT vframe, const UINT hframe)
	:width(width), height(height), vframe(vframe), hframe(hframe), frame(vframe*hframe)
{
	Init();
	this->texture = texture;

	sizeW = width/vframe;
	sizeH = height/hframe;

	texture->GetLevelDesc(0, &srf_desc);
	if (center) this->center = *center;
	else Centering();
}



Texture::~Texture()
{
	texture->Release();
	texture = NULL;
}

void Texture::Draw(const D3DXVECTOR3 pos
	, const D3DXVECTOR3 *scale
	, const float *rotation
	, const unsigned int frame
	, const float opacity) const
{
	if (!this) return;

	unsigned fx = frame%vframe, fy = frame/vframe;
	RECT rect = {fx*sizeW, fy*sizeH, (fx+1)*sizeW, (fy+1)*sizeH};

	D3DXCOLOR c = color;
	if (opacity <= 0.0f)
		return;
	else if ( opacity < 1.0f )
		c.a *= opacity;

	sprite->DrawTexture(
		texture
		, &rect
		, &center
		, &pos
		, scale
		, rotation
		, c);
}

void Texture::DrawRect(const D3DXVECTOR3 pos
	, const RECT rect
	, const D3DXVECTOR3 *scale
	, const float *rotation
	, const float opacity) const
{
	if (!this) return;

	D3DXCOLOR c = color;
	if (opacity < 0.0f)
		return;
	else if ( opacity < 1.0f )
		c.a *= opacity;

	sprite->DrawTexture(
		texture
		, &rect
		, &center
		, &pos
		, scale
		, rotation
		, c);
}

void Texture::DrawOnTexture(Texture *dst_texture
	, const D3DXVECTOR3 pos
	, const unsigned int frame
	)
{
	unsigned fx = frame%vframe, fy = frame/vframe;
	RECT rect = {fx*sizeW, fy*sizeH, (fx+1)*sizeW, (fy+1)*sizeH};

	UINT src_width = srf_desc.Width;
	UINT dst_width = (UINT)dst_texture->GetSurfaceWidth();
	UINT dst_height = (UINT)dst_texture->GetSurfaceHeight();

	DWORD* src = (DWORD*)this->LockRect();
	DWORD* dst = (DWORD*)dst_texture->LockRect();

	for (int y=rect.top; y<rect.bottom; y++)
	{
		UINT dst_y = (UINT)(pos.y-center.y+y);
		if (dst_y>=dst_height) break;
		for (int x=rect.left; x<rect.right; x++)
		{
			UINT dst_x = (UINT)(pos.x-center.x+x);
			if (dst_x>=dst_width) break;
			D3DXCOLOR dst_c=dst[dst_y*dst_width+dst_x],src_c=src[y*src_width+x], blnd_c;
			float alpha=src_c.a*color.a, InvAlpha=1-alpha;
			blnd_c.a = min(alpha+dst_c.a*InvAlpha, 1);
			blnd_c.r = min(dst_c.r*InvAlpha+src_c.r*color.r*alpha, 1);
			blnd_c.g = min(dst_c.g*InvAlpha+src_c.g*color.g*alpha, 1);
			blnd_c.b = min(dst_c.b*InvAlpha+src_c.b*color.b*alpha, 1);
			dst[dst_y*dst_width+dst_x] = blnd_c;
		}
	}

	dst_texture->UnlockRect();
	UnlockRect();
}

void Texture::ClearTexture(const D3DXCOLOR color)
{
	DWORD* srf = (DWORD*)LockRect();
	for (int i=0; i<(int)(srf_desc.Width*srf_desc.Height); i++)
		srf[i] = (DWORD)color;
	UnlockRect();
}


void Texture::Centering()
{
	center.x = sizeW*0.5f;
	center.y = sizeH*0.5f;
	center.z = 0;
}

void* Texture::LockRect()
{
	HRESULT hr = texture->LockRect(0, &locked_rect, NULL, 0);
	if (FAILED(hr)) throw MyException(L"LockRect", hr);
	return locked_rect.pBits;
}

void Texture::UnlockRect()
{
	texture->UnlockRect(0);
}

int Texture::GetPixelByte() const
{
	return locked_rect.Pitch/srf_desc.Width;
}


D3DXVECTOR3 Texture::GetSize() const
{
	return D3DXVECTOR3((float)sizeW, (float)sizeH, 0);
}

UINT Texture::GetImageWidth() const
{
	return width;
}

UINT Texture::GetImageHeight() const
{
	return height;
}


UINT Texture::GetVFrame() const
{
	return vframe;
}

UINT Texture::GetHFrame() const
{
	return hframe;
}

UINT Texture::GetFrame() const
{
	return frame;
}

UINT Texture::GetSurfaceWidth() const
{
	return srf_desc.Width;
}

UINT Texture::GetSurfaceHeight() const
{
	return srf_desc.Height;
}

D3DFORMAT Texture::GetSurfaceFormat() const
{
	return srf_desc.Format;
}

// void Texture::SetDevice(LPDIRECT3DDEVICE9 device){Texture::device = device;}
void Texture::SetSprite(JSprite* sprite) {Texture::sprite = sprite;}
JSprite& Texture::GetSprite() {return *sprite;}

// LPDIRECT3DDEVICE9 Texture::device = NULL;
JSprite* Texture::sprite = NULL;
