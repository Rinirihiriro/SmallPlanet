#pragma once

#include "Random.h"

class Scene_Game;

class MapGenerator
{
private:
	struct MineSeed {
		int x;
		int y;
		int power;
	};

	Scene_Game* pSceneGame;
	Random rand;

public:
	MapGenerator();
	void SetSceneGame(Scene_Game* scene_game);
	void GenerateMap(const int width, const int height, const float resource = 1.f, const float obstacle = 1.f);
	void MakeMine(const int type, const int number, const int spread, const int deep);
};