#pragma once

#include <d3dx9math.h>
#include "GameData.h"
class Scene_Game;

class BaseCamp
{
public:
	int iX, iY;
	D3DXVECTOR3 vBasePos;
	int iFuel, iMineral, iPower;
	int iShowFuel, iShowMineral, iShowPower;
	int iBaseCargo[BLOCK_TYPE_NUM];
	int iTimer;
	bool bNearBase, bFuelProcessing, bCargoEmpty;
	int iResearchNeedResource;
	float fEnterMessageScale;

private:
	Scene_Game* pSceneGame;

public:
	BaseCamp();
	void Reset();
	void SetSceneGame(Scene_Game* scene_game);

	void Update();
	void Draw(const float camera_x);

	void SetBasePos(const int x, const int y);

private:
	void MakeFuel();
};