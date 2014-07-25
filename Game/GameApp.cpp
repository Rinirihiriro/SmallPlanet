#include "GameApp.h"

// #include <locale>
#include <mmsystem.h>
#include <tchar.h>

#include "MyException.h"
#include "JInput.h"

#include <physfs.h>

#define SAFE_DELETE(obj) {if(obj!=NULL){delete obj; obj=NULL;}}


GameApp::GameApp( const HINSTANCE& hInstance, LPCTSTR caption, const int w, const int h
	, const bool fullscreen, const bool aspect_ratio)
	:lpszClassName( caption ), blackBlank(NULL), dwNowTick(0), dwOldTick(0)
{
// 	dwNowTick = timeGetTime();
	// 로케일 설정
	// std::locale::global(std::locale(""));

	// 상대경로를 exe파일 경로로 고정
// #ifndef NORELEASE
// 	TCHAR current_folder[256]={0};
// 	GetModuleFileName(NULL,current_folder,256);
// 	for (int i=_tcslen(current_folder)-1; i>=0; i--)
// 	{
// 		if (current_folder[i]=='\\')
// 		{
// 			current_folder[i] = 0;
// 			break;
// 		}
// 	}
// 	SetCurrentDirectory(current_folder);
// #endif
	WSADATA wsadata;
	PHYSFS_init(NULL);
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	/*
		** 풀 스크린 종횡비 고정
		1. 화면 크기를 구한다.
		2. 게임화면 버퍼를 화면 크기로 한다.
		3. Draw가 끝날 때 마다 그려줄 검은 공백을 만든다.
		4. sprite의 transform matrix를 설정한다.
	*/
/*	if (fullscreen && aspect_ratio)
	{
		int screen_w, screen_h, game_w, game_h;
		double screen_ratio, game_ratio;	// w/h
		float scale;

		screen_w = GetSystemMetrics(SM_CXSCREEN);
		screen_h = GetSystemMetrics(SM_CYSCREEN);

		screen_ratio = (double)screen_w/(double)screen_h;
		game_ratio = (double)w/(double)h;

		if (screen_ratio>game_ratio)
		{
			scale = (float)screen_h/(float)h;
			game_w = (h*screen_w/screen_h)/2*2;
			game_h = h;
		}
		else if (screen_ratio<game_ratio)
		{
			scale = (float)screen_w/(float)w;
			game_h = (w*screen_h/screen_w)/2*2;
			game_w = w;
		}
		blankRect.left = blankRect.top = 0;
		blankRect.right = (game_w-w==0)? game_w:(game_w-w)/2;
		blankRect.bottom = (game_h-h==0)? game_h:(game_h-h)/2;

		if (blankRect.right<blankRect.bottom)
		{
			blankPos[0] = D3DXVECTOR3((float)(-blankRect.right), 0, 0);
			blankPos[1] = D3DXVECTOR3((float)(game_w-blankRect.right*2), 0, 0);
		}
		else
		{
			blankPos[0] = D3DXVECTOR3((float)(-blankRect.bottom), 0, 0);
			blankPos[1] = D3DXVECTOR3(0, (float)(game_h-blankRect.bottom*2), 0);
		}

		pWindow = new JWindows(hInstance, lpszClassName, screen_w, screen_h, true);
		pDirectx = new JDirectX(pWindow->hWindow, screen_w, screen_h, true);

		D3DXMATRIX translateMat, scaleMat;
		if (blankRect.right<blankRect.bottom)
			D3DXMatrixTranslation(&translateMat, (float)blankRect.right, 0, 0);
		else
			D3DXMatrixTranslation(&translateMat, 0, (float)blankRect.bottom, 0);
		D3DXMatrixScaling(&scaleMat, scale, scale, 1);
		pDirectx->D3DSprite.SetTransform(translateMat);
		pDirectx->D3DSprite.SetFinalScale(scaleMat);
		
		HRESULT hr = D3DXCreateTexture(
			pDirectx->lpD3DDevice
			, blankRect.right, blankRect.bottom
			, 1
			, 0
			, D3DFMT_A8R8G8B8
			, D3DPOOL_MANAGED
			, &blackBlank
			);
		if (FAILED(hr)) throw ObjectWasNotCreated(L"Texture", hr);
		
		D3DLOCKED_RECT lockrect;
		blackBlank->LockRect(0, &lockrect, 0, 0);
		DWORD* surface = (DWORD*)lockrect.pBits;
		for (int y=0; y<blankRect.bottom; y++)
		{
			for (int x=0; x<blankRect.right; x++)
			{
				surface[y*blankRect.right+x] = 0xff000000;
			}
		}
		blackBlank->UnlockRect(0);
	}
	else*/
	{
		pWindow = new JWindows( hInstance, lpszClassName, w, h, fullscreen);
		pDirectx = new JDirectX( pWindow->hWindow, w, h, fullscreen);
	}
	
	pXaudio = new JXAudio();
	pFreeType = new JFreeType();
}

GameApp::~GameApp()
{
	SAFE_DELETE(pXaudio);
	SAFE_DELETE(pDirectx);
	SAFE_DELETE(pWindow);
	SAFE_DELETE(pFreeType);
	WSACleanup();
	PHYSFS_deinit();
}

int GameApp::Run()
{
	MSG message;
	ZeroMemory(&message, sizeof(message));

	static int fps_calculating_counter = 0;
	static DWORD fps_calculating_tick = timeGetTime();

	while (message.message != WM_QUIT)
	{
		dwNowTick = timeGetTime();
		if (dwOldTick==0) dwOldTick = dwNowTick;

		if ( PeekMessage(&message, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else 
		{
			if ( dwNowTick >= dwOldTick + SPF )
			{
				fps_calculating_counter++;
				JInput::Update();
				if( !UpdateGame() ) break;
				dwOldTick += SPF;
				if (fps_calculating_counter >= 10)
				{
					fRealFPS = 1000.0f * fps_calculating_counter / ( dwNowTick - fps_calculating_tick );
					fps_calculating_tick = dwNowTick;
					fps_calculating_counter = 0;
				}
			}
		}
	}
	return (int)message.wParam;
}

void GameApp::CreateXAudio2SourceVoice( IXAudio2SourceVoice** source_voice, WAVEFORMATEX* wave_format )
{
	HRESULT result = pXaudio->pXAudio->CreateSourceVoice( source_voice, wave_format );
	if(FAILED( result )) throw ObjectWasNotCreated("SourceVoice", result);
}

void GameApp::SetXAudio2MasteringVoiceVolume(const float volume)
{
	pXaudio->pXAudioMastering->SetVolume(volume);
}


JWindows& GameApp::GetJWindow() {return *pWindow;}
JDirectX& GameApp::GetJDirectX() {return *pDirectx;}
JXAudio& GameApp::GetJXAudio() {return *pXaudio;}
JFreeType& GameApp::GetJFreeType() {return *pFreeType;}
