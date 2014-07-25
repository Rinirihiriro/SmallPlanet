/*
	게임의 코어부분을 담당하는 클래스입니다.
*/
#pragma once

#include "JWindows.h"
#include "JDirectX.h"
#include "JXAudio.h"
#include "JFreeType.h"
#include "Font.h"

#define FPS 60
#define SPF ( 1000 / FPS )

class GameApp
{
public:
	float fRealFPS;

protected:
	JWindows* pWindow;
	JDirectX* pDirectx;
	JXAudio* pXaudio;
	JFreeType* pFreeType;

	LPCTSTR lpszClassName;

	DWORD dwOldTick;
	DWORD dwNowTick;

	bool bFrameBoost;

	virtual bool UpdateGame() = 0;

	LPDIRECT3DTEXTURE9 blackBlank;
	D3DXVECTOR3 blankPos[2];
	RECT blankRect;

public:
	GameApp( const HINSTANCE& hInstance, LPCTSTR caption, const int w, const int h
		, const bool fullscreen, const bool aspect_ratio = false );
	virtual ~GameApp();

	int Run();
	void CreateXAudio2SourceVoice( IXAudio2SourceVoice** source_voice, WAVEFORMATEX* wave_format );
	void SetXAudio2MasteringVoiceVolume( const float volume = 1.0f );
	void TogleFullscreen(const bool fullscreen);

	JWindows& GetJWindow();
	JDirectX& GetJDirectX();
	JXAudio& GetJXAudio();
	JFreeType& GetJFreeType();
};