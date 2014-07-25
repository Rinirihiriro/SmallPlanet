/*
	DirectX의 초기화 및 관리를 위한 클래스
*/
#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "JSprite.h"

class JDirectX
{
public:
	LPDIRECT3D9 lpD3D;
	LPDIRECT3DDEVICE9 lpD3DDevice;
	JSprite D3DSprite;
	
private:
	const HWND& hWindow;
	bool bFullScreen;

	unsigned int iWidth;
	unsigned int iHeight;

public:

	JDirectX( const HWND& hWindow, const UINT width, const UINT height, const bool fullscreen = false );
	~JDirectX();

	LPD3DXFONT CreateFont( LPCTSTR fontname, UINT height
		, int weight = FW_NORMAL, bool italic = false ) const;
	bool ResetDevice();

	void SetFullScreenAndReset(const bool fullscreen = true);

	bool IsFullScreen() const;
	UINT GetWidth() const;
	UINT GetHeight() const;

private:
	void GetD3DPresentParam(D3DPRESENT_PARAMETERS *out) const;
	void CreateDevice();
	void CreateSprite();

};