#pragma once

#include <d3dx9math.h>
#include "GameData.h"
class Scene_Game;

class BaseMenu
{
public:
	bool bActive;

private:
	const Position BASE_MENU_FUAL_POS, BASE_MENU_MINERAL_POS;
	const D3DXCOLOR COLOR_NORMAL, COLOR_ERROR, COLOR_WHITE, COLOR_YELLOW;

	Scene_Game* pSceneGame;

	float fSelectItemAlpha[UPGRADE_NUM];
	float fBaseMenuScale;
	int iBaseMenuTab, iSelectedItem;
	int iBaseMenuFuelVal, iBaseMenuMineralVal;
	int iBaseMenuShowFuel, iBaseMenuShowMineral;
	D3DXCOLOR mFuelValColor, mFualValColorDest, mMineralValColor, mMineralValColorDest;

public:
	BaseMenu();
	void Reset();
	void SetSceneGame(Scene_Game* scene_game);
	void InputProc();
	void Update();
	void Draw();

	void Open();

private:
	void DrawUpgradeMenu();
	void DrawResourceMenu();
	void DrawBuildMenu();
	void DrawBigNumber(const int x, const int y, const int value, const bool mlt, const D3DXCOLOR* color = NULL);

};