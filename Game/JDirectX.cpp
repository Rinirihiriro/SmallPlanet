#include "MyException.h"
#include "JDirectX.h"
#include "Essential.h"

JDirectX::JDirectX( const HWND& hWindow, const UINT width, const UINT height, const bool fullscreen)
	:hWindow(hWindow), bFullScreen(fullscreen), iWidth(width), iHeight(height)
{
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (lpD3D == NULL) throw ObjectWasNotCreated("Direct3D");

	CreateDevice();
	CreateSprite();
}

JDirectX::~JDirectX()
{
	D3DSprite.ResetSprite(NULL);
	SAFE_RELEASE(lpD3DDevice);
	SAFE_RELEASE(lpD3D);
}

LPD3DXFONT JDirectX::CreateFont( LPCTSTR fontname, UINT height, int weight, bool italic ) const
{
	LPD3DXFONT font;
	HRESULT hr = D3DXCreateFont(
		lpD3DDevice
		, (int)height
		, 0
		, weight
		, 1
		, italic
		, DEFAULT_CHARSET
		, OUT_DEFAULT_PRECIS
		, /*ANTIALIASED_QUALITY*/ PROOF_QUALITY
		, DEFAULT_PITCH | FF_DONTCARE
		, fontname
		, &font
		);
	if (FAILED(hr)) throw ObjectWasNotCreated("ID3DXFont", hr);
	return font;
}

bool JDirectX::ResetDevice()
{
	HRESULT hr = lpD3DDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICENOTRESET)
	{
		D3DSprite.ResetSprite(NULL);
		D3DPRESENT_PARAMETERS present_param;
		GetD3DPresentParam(&present_param);
		hr = lpD3DDevice->Reset(&present_param);
		if (FAILED(hr))
			return false;
		CreateSprite();
	}
	else if (hr == D3DERR_DEVICELOST)
	{
		Sleep(50);
	}
	return SUCCEEDED(lpD3DDevice->TestCooperativeLevel());
}


void JDirectX::SetFullScreenAndReset(const bool fullscreen)
{
	if (bFullScreen == fullscreen) return;
	bFullScreen = fullscreen;
	while (true)
	{
		D3DPRESENT_PARAMETERS present_param;
		D3DSprite.ResetSprite(NULL);
		GetD3DPresentParam(&present_param);
		if (FAILED(lpD3DDevice->Reset(&present_param))) continue;
		CreateSprite();
		break;
	}
}

bool JDirectX::IsFullScreen() const
{
	return bFullScreen;
}

UINT JDirectX::GetWidth() const
{
	return iWidth;
}

UINT JDirectX::GetHeight() const
{
	return iHeight;
}


void JDirectX::GetD3DPresentParam(D3DPRESENT_PARAMETERS *out) const
{
	ZeroMemory(out, sizeof(*out));
	out->Windowed = (bFullScreen)? FALSE:TRUE;
	out->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	out->SwapEffect = D3DSWAPEFFECT_DISCARD;
	out->EnableAutoDepthStencil = TRUE;
	out->AutoDepthStencilFormat = D3DFMT_D16;
	out->BackBufferWidth = iWidth;
	out->BackBufferHeight = iHeight;
	if (bFullScreen)
	{
		out->BackBufferFormat = D3DFMT_X8R8G8B8;
		out->hDeviceWindow = hWindow;
	}
}

void JDirectX::CreateDevice()
{
	HRESULT hr;
	D3DPRESENT_PARAMETERS present_param;
	GetD3DPresentParam(&present_param);
	hr = lpD3D->CreateDevice(
		D3DADAPTER_DEFAULT
		, D3DDEVTYPE_HAL
		, hWindow
		, D3DCREATE_MIXED_VERTEXPROCESSING
		, &present_param
		, &lpD3DDevice
		);
	if (FAILED(hr))
	{
		hr = lpD3D->CreateDevice(
			D3DADAPTER_DEFAULT
			, D3DDEVTYPE_HAL
			, hWindow
			, D3DCREATE_SOFTWARE_VERTEXPROCESSING
			, &present_param
			, &lpD3DDevice
			);
		if (FAILED(hr)) throw ObjectWasNotCreated("Direct3D Device", hr);
	}
}

void JDirectX::CreateSprite()
{
	HRESULT hr;
	LPD3DXSPRITE lpSprite=NULL;
	hr = D3DXCreateSprite(lpD3DDevice, &lpSprite);
	if (FAILED(hr)) throw ObjectWasNotCreated("Direct3D Sprite", hr);
	D3DSprite.ResetSprite(lpSprite);
}