#include <tchar.h>

#include "MyException.h"
#include "Essential.h"

#include "TextureManager.h"
#include "SEManager.h"
#include "BGMManager.h"
#include "FontManager.h"
#include "FTFontManager.h"
#include "GlobalDataManager.h"

#include "JInput.h"

#include "JMain.h"

#include "Scene_Game.h"
#include "Scene_Logo.h"

#ifdef _DEBUG
#define SHOWFPS true
#else
#define SHOWFPS false
#endif

JMain* JMain::gCMain = NULL;

JMain::JMain( const HINSTANCE& hInstance, LPCTSTR caption, const int w, const int h
	, const bool fullscreen )
	:GameApp( hInstance, caption, w, h, fullscreen )
{
	SetWindowScale(2);
	FontManager::SetCDirectX(pDirectx);
	LoadResource();
	JInput::RememberHWND(&pWindow->hWindow);
	FTFont::SetJFreeType(pFreeType);
	scene = NULL;
	recent_scene = NULL;
	next_scene = NULL;
	remain_recent_scene = true;
	show_fps = SHOWFPS;
}

JMain::~JMain()
{
	SAFE_DELETE(recent_scene);
	SAFE_DELETE(scene);
	SAFE_DELETE(next_scene);
	GlobalDataManager::Release();
	SEManager::Release();
	BGMManager::Release();
	TextureManager::Release();
	FontManager::Release();
	FTFontManager::Release();
}

void JMain::CreateCMain( const HINSTANCE& hInstance, LPCTSTR caption, const int w, const int h
	, const bool fullscreen )
{
	if (gCMain == NULL)
		gCMain = new JMain( hInstance, caption, w, h, fullscreen );
}

JMain& JMain::GetInstance()
{
	if (gCMain != NULL) return *gCMain;
	else throw MyException(L"Main Object Was Not Created.");
}

void JMain::Release()
{
	SAFE_DELETE(gCMain);
}

void JMain::LoadData()
{
	TextureManager::GetInstance().LoadTextures(1);

	SEManager::GetInstance().LoadSEs(1);
	BGMManager::GetInstance().LoadBGMs(1);
	FontManager::GetInstance().LoadFonts(1);
	FTFontManager::GetInstance().LoadFonts(1);

	int volume;
	GlobalDataManager::GetInstance().GetData("soundvolume", &volume);
	SetXAudio2MasteringVoiceVolume(volume*0.1f);
}

bool JMain::UpdateGame()
{
	if (scene)
	{
		scene->InputProc();
		if (JInput::IsTriggered(VK_F4)) show_fps = !show_fps;
// 		bFrameBoost = true;//JInput::IsPressed(VK_PAUSE);
// 		else if (JInput::IsPressed(VK_MENU) && JInput::IsTriggered(VK_RETURN))
// 			TogleFullscreen();
		if (!next_scene) scene->Update();
		if (!next_scene) Render();
	}

	if (next_scene) ChangeScene();

	return true;
}

// 시작하는 Scene 설정은 여기에서
void JMain::PrepareToStart()
{
	LoadData();
	SetScene(new Scene_Logo());
}

void JMain::Render()
{
	HRESULT hr;
	pDirectx->lpD3DDevice->Clear(
		0
		, NULL
		, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
		, scene->back_color
		, 1.0f
		, 0
		);
	hr = pDirectx->lpD3DDevice->BeginScene();
	if (FAILED(hr)) throw MyException(L"BeginScene Failed", hr);

	pDirectx->D3DSprite.Begin();
	// 안티앨리어싱 제거!
	pDirectx->lpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDirectx->lpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	scene->Render();
	pDirectx->D3DSprite.End();

	// 		if (fullscreen)
	// 		{
	// 			pDirectx->D3DSprite.DrawTexture(blackBlank, &blankRect, 0, &blankPos[0]);
	// 			pDirectx->D3DSprite.DrawTexture(blackBlank, &blankRect, 0, &blankPos[1]);
	// 		}

	//#ifdef _DEBUG
	if (show_fps)
		ShowGameDebugInfo( );
	//#endif

	pDirectx->lpD3DDevice->EndScene();
	hr = pDirectx->lpD3DDevice->Present(0, 0, 0, 0);
	if (FAILED(hr))
	{
		// 디바이스 소실
		if (hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET)
		{
			ResetDirectX();
		}
	}
}

void JMain::ChangeScene()
{
	if (next_scene == NULL) return;
	
	if (recent_scene != next_scene)
		SAFE_DELETE(recent_scene);
	
	if (remain_recent_scene)
		recent_scene = scene;
	else
	{
		SAFE_DELETE(scene);
	}
	
	scene = next_scene;
	next_scene = NULL;

	if (scene == recent_scene)
		recent_scene = NULL;

}

void JMain::SetScene( Scene* scene, bool remain_recent_scene )
{
	if (next_scene != NULL)
		SAFE_DELETE(next_scene);
	next_scene = scene;
	this->remain_recent_scene = remain_recent_scene;
}

void JMain::SetSceneRecentScene( bool remain_recent_scene )
{
	if (recent_scene == NULL) return;
	next_scene = recent_scene;
	this->remain_recent_scene = remain_recent_scene;
}

void JMain::ReleaseRecentScene()
{
	SAFE_DELETE(recent_scene);
}

void JMain::ShowGameDebugInfo()
{
	const long x = 5;
	const long y = 5;
	const long h = 15;

	size_t len = 100;
	TCHAR* str = new TCHAR[len];
	Font& font = FontManager::GetInstance().GetFont("gulim");

	_stprintf_s(str, len, L"FPS : %.2f", fRealFPS);
	font.DrawText(x-1, y, str, -1, 0xff000000);
	font.DrawText(x, y-1, str, -1, 0xff000000);
	font.DrawText(x+1, y, str, -1, 0xff000000);
	font.DrawText(x, y+1, str, -1, 0xff000000);
	font.DrawText(x, y, str, -1, 0xffffffff);

/*
	_stprintf_s(str, len, L"Objects : %d", scene->objArr.size());
	font.DrawText(x-1, y+h, str, 0xff000000);
	font.DrawText(x, y+h-1, str, 0xff000000);
	font.DrawText(x+1, y+h, str, 0xff000000);
	font.DrawText(x, y+h+1, str, 0xff000000);
	font.DrawText(x, y+h, str, 0xffffffff);
*/

	delete[] str;

}

Scene* JMain::GetScene() { return scene; }
Scene* JMain::GetRecentScene() { return recent_scene; }
Scene* JMain::GetNextScene() {return next_scene; }

void JMain::GetStringResource(const int resource_id, TCHAR* out, const int size)
{
	LoadString(pWindow->hInstance, resource_id, out, size);
}

// TODO : surface 크기 줄이는 비율을 인수로 넣을 수 있게
Texture* JMain::GetBackBufferTexture()
{
	/*
	LPDIRECT3DTEXTURE9 render_target = NULL;
	LPDIRECT3DSURFACE9 render_target_srf = NULL;

	LPDIRECT3DSURFACE9 orig_render_target = NULL, orig_depth_stencil = NULL;

	HRESULT hr = D3DXCreateTexture(
		pDirectx->lpD3DDevice
		, pDirectx->GetWidth()
		, pDirectx->GetHeight()
		, 1
		, D3DUSAGE_RENDERTARGET
		, D3DFMT_A8R8G8B8
		, D3DPOOL_DEFAULT
		, &render_target
		);
	if (FAILED(hr)) throw MyException(L"CreateTexture", hr);

	render_target->GetSurfaceLevel(0, &render_target_srf);

	pDirectx->lpD3DDevice->GetRenderTarget(0, &orig_render_target);
	pDirectx->lpD3DDevice->GetDepthStencilSurface(&orig_depth_stencil);
	pDirectx->lpD3DDevice->SetRenderTarget(0, render_target_srf);
	Render();
	pDirectx->lpD3DDevice->SetRenderTarget(0, orig_render_target);
	pDirectx->lpD3DDevice->SetDepthStencilSurface(orig_depth_stencil);
	SAFE_RELEASE(orig_render_target);
	SAFE_RELEASE(orig_depth_stencil);
	SAFE_RELEASE(render_target_srf);

	D3DXVECTOR3 c(0,0,0);
	return new Texture(render_target, pDirectx->GetWidth(), pDirectx->GetHeight(), &c);
*/

	HRESULT hr;
	LPDIRECT3DTEXTURE9 out_texture = NULL;
	LPDIRECT3DSURFACE9 backbuffer_surface = NULL, resized_surface = NULL, out_surface = NULL;
// 	D3DSURFACE_DESC surface_desc;

	hr = pDirectx->lpD3DDevice->GetBackBuffer(
		0
		, 0
		, D3DBACKBUFFER_TYPE_MONO
		, &backbuffer_surface
		);
	if (FAILED(hr))throw MyException(L"GetBackBuffer", hr);

// 	backbuffer_surface->GetDesc(&surface_desc);

	// 백버퍼 가져온 surface 크기 줄이기용 RT
	hr = pDirectx->lpD3DDevice->CreateRenderTarget(
		pDirectx->GetWidth()>>1
		, pDirectx->GetHeight()>>1
		, D3DFMT_A8R8G8B8
		, D3DMULTISAMPLE_NONE
		, 0
		, FALSE
		, &resized_surface
		, NULL);
	if (FAILED(hr))throw MyException(L"CreateRenderTarget", hr);

	// 크기 줄이기
	hr = pDirectx->lpD3DDevice->StretchRect(
		backbuffer_surface, NULL
		, resized_surface, NULL
		, D3DTEXF_LINEAR /*D3DTEXF_POINT*/);

	if (FAILED(hr))throw MyException(L"StretchRect", hr);

	hr = D3DXCreateTexture(pDirectx->lpD3DDevice
		, pDirectx->GetWidth()>>1
		, pDirectx->GetHeight()>>1
		, 1
		, 0
		, D3DFMT_A8R8G8B8
		, D3DPOOL_MANAGED
		, &out_texture);
	if (FAILED(hr)) throw ObjectWasNotCreated(L"Texture", hr);

	out_texture->GetSurfaceLevel(0, &out_surface);

	// 크기가 줄어서 오버헤드가 줄어든다.
	// 가로세로 1/2로 줄여서 총 75%의 오버헤드 감소효과를 얻는다.
	// 주의 : 그릴 때는 2배 확대해서 그려야한다.
	// 그 결과 자동으로 흐림처리까지 된다(...)
	D3DXLoadSurfaceFromSurface(out_surface, 0, 0, resized_surface, 0, 0, D3DX_FILTER_BOX, 0);
//	D3DXLoadSurfaceFromSurface(out_surface, 0, 0, backbuffer_surface, 0, 0, D3DX_FILTER_BOX, 0);

 	SAFE_RELEASE(out_surface);
	SAFE_RELEASE(resized_surface);
	SAFE_RELEASE(backbuffer_surface);

	D3DXVECTOR3 center(0,0,0);
	return new Texture(out_texture, pDirectx->GetWidth()>>1, pDirectx->GetHeight()>>1, &center);
}


void JMain::SetWindowSize(const int w, const int h)
{
	pWindow->SetWindowSize(w, h);
}

void JMain::SetWindowScale(const double scale)
{
	pWindow->SetWindowSize((int)(pDirectx->GetWidth()*scale), (int)(pDirectx->GetHeight()*scale));
}

void JMain::ResetDirectX()
{
	ReleaseResource();
	while (!pDirectx->ResetDevice());
	LoadResource();
}

void JMain::LoadResource()
{
	Texture::SetSprite(&pDirectx->D3DSprite);
	FontManager::GetInstance().OnResetDevice();
}

void JMain::ReleaseResource()
{
	FontManager::GetInstance().OnLostDevice();
}

void JMain::TogleFullscreen()
{
	bool fullscreen = !pDirectx->IsFullScreen();
	ReleaseResource();
	pDirectx->SetFullScreenAndReset(fullscreen);
	LoadResource();
// 	pWindow->SetFullScreenAndReset(fullscreen);
}

void JMain::ExitGame()
{
	PostQuitMessage(0);
}

bool JMain::IsFullscreen()
{
	return pDirectx->IsFullScreen();
}