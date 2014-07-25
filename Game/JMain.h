/*
	메인 클래스.
	게임 Scene와 폰트를 가진다.
	싱글톤이므로 CreateCMain으로 생성한다.
*/
#pragma once

#include <Windows.h>
#include "GameApp.h"
#include "Scene.h"

class Texture;

class JMain :public GameApp
{
private:
	static JMain* gCMain;
	Scene* scene;
	Scene* recent_scene;
	Scene* next_scene;

	bool remain_recent_scene;

	bool show_fps;

private:
	JMain( const HINSTANCE& hInstance, LPCTSTR caption, const int w, const int h
		, const bool fullscreen );
	virtual bool UpdateGame() override;
	void LoadData();
	void ChangeScene();

public:
	static void CreateCMain( const HINSTANCE& hInstance, LPCTSTR caption, const int w, const int h
		, const bool fullscreen );
	static JMain& GetInstance();
	static void Release();
	~JMain();

	void PrepareToStart();
	
	void Render();

	void ShowGameDebugInfo();

	void SetScene( Scene* scene, bool remain_recent_scene = true );
	void SetSceneRecentScene( bool remain_recent_scene = true );
	void ReleaseRecentScene();

	Scene* GetScene();
	Scene* GetRecentScene();
	Scene* GetNextScene();

	void GetStringResource(const int resource_id, TCHAR* out, const int size);

// 	Texture* RenderOnTexture();
// 	void SetRenderTarget();
	Texture* GetBackBufferTexture();

	void SetWindowSize(const int w, const int h);
	void SetWindowScale(const double scale);

	void ResetDirectX();

	void LoadResource();
	void ReleaseResource();

	void TogleFullscreen();

	void ExitGame();

	bool IsFullscreen();
};