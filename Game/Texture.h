/*
	�ؽ�ó�� �����ϴ� Ŭ����.
*/
#pragma once

#include "JSprite.h"

class Texture 
{
private:
	static JSprite* sprite;

	LPDIRECT3DTEXTURE9 texture;
	D3DXIMAGE_INFO info;
	D3DSURFACE_DESC srf_desc;
	D3DLOCKED_RECT locked_rect;

	unsigned sizeW, sizeH;

	UINT width;
	UINT height;

	const UINT vframe;
	const UINT hframe;
	const UINT frame;

	void Init();
	void LoadFile(const LPCTSTR filename, const D3DCOLOR colorkey);
	void CreateTexture(const UINT width, const UINT height, const D3DFORMAT dFormat);

public:
	D3DXVECTOR3 center;
	D3DXCOLOR color;

public:
	
	// ���� �ҷ��ͼ� �����. center NULL�� �ڵ� �߾�
	Texture( LPCTSTR filename, const D3DXVECTOR3 *center=NULL
		, const UINT vframe=1, const UINT hframe=1, const D3DCOLOR colorkey = 0x00000000);
	// �׳� �����. imgWidht, imgHeight NULL�� ���ǽ� ũ��� ����
	Texture( const UINT srfWidth, const UINT srfHeight, const D3DFORMAT format
		, const UINT *imgWidth=NULL, const UINT *imgHeight=NULL
		, const D3DXVECTOR3 *center=NULL, const UINT vframe=1, const UINT hframe=1);
	// �̸� ������� Texture�� �����. center NULL�� �ڵ� �߾�
	Texture( LPDIRECT3DTEXTURE9 texture, const UINT width, const UINT height
		, const D3DXVECTOR3 *center=NULL, const UINT vframe=1, const UINT hframe=1);
	~Texture();

	// scale, rotation�� NULL�� ���� ����
	void Draw(const D3DXVECTOR3 pos
		, const D3DXVECTOR3 *scale = NULL
		, const float *rotation = NULL
		, const unsigned int frame = 0
		, const float opacity = 1.f) const;
	void DrawRect(const D3DXVECTOR3 pos
		, const RECT rect
		, const D3DXVECTOR3 *scale = NULL
		, const float *rotation = NULL
		, const float opacity = 1.f) const;
	void DrawOnTexture(Texture *texture
		, const D3DXVECTOR3 pos
		, const unsigned int frame = 0);
	void ClearTexture(const D3DXCOLOR color);

	void Centering();
	void* LockRect();
	void UnlockRect();
	int GetPixelByte() const;

	D3DXVECTOR3 GetSize() const;
	
	UINT GetImageWidth() const;
	UINT GetImageHeight() const;

	UINT GetVFrame() const;
	UINT GetHFrame() const;
	UINT GetFrame() const;

	UINT GetSurfaceWidth() const;
	UINT GetSurfaceHeight() const;
	D3DFORMAT GetSurfaceFormat() const;

	static void SetSprite(JSprite* sprite);
	static JSprite& GetSprite();
};