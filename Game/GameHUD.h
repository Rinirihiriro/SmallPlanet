#pragma once

#include <d3dx9math.h>
#include "GameData.h"

class Scene_Game;
class Boss;

class GameHUD
{
public:
	enum UI_ID
	{
		UI_GOAL
		, UI_CARGO
		, UI_JETPACK
		, UI_HEALTH
		, UI_OXYGEN
		, UI_BOSS_HP
		, UI_BUILDING
		, UI_RESOURCES
		, UI_NUM
	};
	int iSelectedItem;
	bool bBuild;
	Boss* pBoss;

private:
	Scene_Game* pSceneGame;
	float fUi[UI_NUM], fUiDest[UI_NUM];
	float fGauge[UI_BUILDING], fGaugeDest[UI_BUILDING];
	int iUIShowCounter[UI_NUM];
	float fSelectItemAlpha[BUILDING_SMALL_CAMP];
	float fNewResourceBox, fResearchBox, fFullBox, fAirLeakBox;
	int iNewResources[4];
	int iNewResourceTimer, iFullBoxTimer;
	int iProcessingFrame;

	int iHelpID[2];
	float fHelpAlpha, fHelpAlphaDest;
	int iHelpTimer;

public:
	GameHUD();

	void SetSceneGame(Scene_Game* scene_game);
	void Reset();

	void InputProc();
	void Update();
	void Draw();

	void Show(const int ui, const bool short_showing = false);
	void Show(const int ui, const int frame);
	void ShowAll();
	void Hide(const int ui);
	void HideAll();

	void ShowHelp(const int id);
	void HideHelp();
	bool IsHelpOver();

	void NotiFull();

	void NewResource(const int resource_id);

private:
	void DrawGauge();
	void DrawResource();
	void DrawBuildingUI();
	void DrawNumber(const int x, const int y, const int value, const bool power, const D3DXCOLOR* color=NULL);
};