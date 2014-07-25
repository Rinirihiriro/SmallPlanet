#include "MapGenerator.h"

#include "Scene_Game.h"
#include "GameData.h"

MapGenerator::MapGenerator()
	:pSceneGame(NULL)
{

}

void MapGenerator::SetSceneGame(Scene_Game* scene_game)
{
	pSceneGame = scene_game;
}

void MapGenerator::GenerateMap(const int width, const int height, const float resource, const float obstacle)
{
	std::vector<int>& mHeightMap			= pSceneGame->mHeightMap;
	std::vector<int>& mGameMap				= pSceneGame->mGameMap;
	std::vector<Scene_Game::LightMapData>& mLightMap	= pSceneGame->mLightMap;

	std::vector<double> heightMap(width);
	mHeightMap.resize(width);

	pSceneGame->iMapWidth = width;
	pSceneGame->iMapHeight = height;
	pSceneGame->iMapWidthPixel = width*BLOCK_SIZE;
	pSceneGame->iMapHeightPixel = height*BLOCK_SIZE;

	for (int i=0; i<width; i++)
	{
		heightMap[i] = (height*0.6+rand.NextDouble()*2);
	}

	// 당기기
	for (int i=0; i<width/20; i++)
	{
		int max_height = rand.Next()%41-20;
		double power = rand.NextDouble()*0.5+0.4;
		int seed_x = rand.Next()%width;
		while (max_height-->0)
		{
			double uplift = (max_height>0)? 1:-1;
			int offset = 0;
			while (abs(uplift)>0.1)
			{
				if (offset > 0)
					heightMap[(seed_x-offset+width)%width] += uplift;
				heightMap[(seed_x+offset)%width] += uplift;
				uplift *= power;
				offset++;
			}
		}
	}

	// 스무스
	for (int i=0; i<width; i++)
	{
		double h = 0;
		mHeightMap[i] = (int)((heightMap[(i-1+width)%width]
					+ heightMap[i]
					+ heightMap[(i+1)%width])/3);
	}

	// 생성
	mGameMap.resize(width*height);
	for (int i=0; i<width; i++)
	{
		int dirt = 6+rand.Next()%2;
		for (int j=0; j<height; j++)
		{
			if (j<height-mHeightMap[i])
				mGameMap[i+j*width] = BLOCK_AIR;
			else if (j >= (int)(height*0.7f)-1 && j<= (int)(height*0.7f)+1)
				mGameMap[i+j*width] = BLOCK_OBSIDIAN;
			else if (j >= height-2)
				mGameMap[i+j*width] = BLOCK_BEDROCK;
			else if(dirt)
			{
				mGameMap[i+j*width] = BLOCK_DIRT;
				dirt--;
			}
			else
				mGameMap[i+j*width] = BLOCK_ROCK;
		}
	}

	// 호수를 만든다.
	for (int i=0; i<width/40; i++) {
		int seed_x = rand.Next()%width;
		int seed_y = 0;
		bool retry = false;
		// 씨드를 뿌린다.
		while (true) {
			bool stop = true;
			while (mGameMap[seed_x+seed_y*width] == 0)
				seed_y++;
			if (mGameMap[seed_x+seed_y*width] == BLOCK_WATER) {
				retry = true;
				break;
			}
			for (int j=-5; j<=5; j++)
			{
				if (mGameMap[(seed_x+j+width)%width+seed_y*width] == 0) {
					stop = false;
					seed_x = (seed_x+j+width)%width;
					seed_y += 1;
					break;
				}
			}
			if (stop)
				break;
		}
		if (retry)
		{
			i--;
			continue;
		}
		// 그 자리에서부터 땅을 파고 들어간다.
		// 위에 있는 땅을 제거한다.
		double w = 3+rand.Next()%7;
		for (int k = seed_y; k>=seed_y-3; k--)
		{
			for (int j = (int)floor(-w/2); j < w/2; j++)
			{
				mGameMap[(seed_x+j+width)%width+k*width] = 0;
			}
		}
		while (w >= 1.5)
		{
			for (int j = (int)floor(-w/2); j < w/2; j++)
			{
				mGameMap[(seed_x+j+width)%width+seed_y*width] = BLOCK_WATER;
			}
			w *= 0.75;
			seed_y += 1;
			if (w >= 2)
				seed_x += rand.Next()%3-1;
		}
	}

	// 본부를 만든다.
	while (true)
	{
		int seed_x = rand.Next()%(width-30)+15;
		int seed_y = 0;
		bool retry = false;
		// 씨드를 뿌린다.
		while (true) {
			bool stop = true;
			while (mGameMap[seed_x+seed_y*width] == 0)
				seed_y++;
			if (mGameMap[seed_x+seed_y*width] == BLOCK_WATER) {
				retry = true;
				break;
			}
			for (int j=-10; j<=10; j++)
			{
				if (mGameMap[(seed_x+j+width)%width+seed_y*width] == 0) {
					stop = false;
					seed_x = (seed_x+j+width)%width;
					seed_y += 1;
					break;
				}
			}
			if (stop)
				break;
		}
		if (retry) continue;
		// 그 자리에 공간을 만든다.
		// 6X4
		for (int k = seed_y-1; k>=0; k--)
		{
			for (int j = -4; j <= 4; j++)
			{
				mGameMap[(seed_x+j+width)%width+k*width] = 0;
			}
		}
		for (int j = -4; j <= 4; j++)
		{
			mGameMap[(seed_x+j+width)%width+(seed_y+1)*width] = 
				mGameMap[(seed_x+j+width)%width+seed_y*width] = BLOCK_BEDROCK;
		}

		pSceneGame->mBaseCamp.SetBasePos(seed_x, seed_y-1);
		pSceneGame->SetPlayerPos(seed_x, seed_y-1);

		break;
	}

	int size = width*height;
	MakeMine(BLOCK_WATER, (int)(size*0.0013f*resource), 3, 0);	// 40
	MakeMine(BLOCK_MAGMA, (int)(size*0.0012f*obstacle), 3, (int)(height*0.6f));	// 35
	MakeMine(BLOCK_OBSIDIAN, (int)(size*0.001f*obstacle), 3, (int)(height*0.7f));				// 30
	MakeMine(BLOCK_MINERAL, (int)(size*0.0025f*resource), 5, 0);		// 75
	MakeMine(BLOCK_COAL, (int)(size*0.0017f*resource), 4, 0);		// 50
	MakeMine(BLOCK_OIL, (int)(size*0.001f*resource), 5, pSceneGame->mBaseCamp.iY+30);		// 30
	MakeMine(BLOCK_URANIUM, (int)(size*0.0007f*resource), 2, pSceneGame->mBaseCamp.iY+(height-pSceneGame->mBaseCamp.iY)/2);	// 20
	MakeMine(BLOCK_UNOBTAINIUM, (int)(size*0.0001f*resource), 5, height-20);	// 3
	MakeMine(BLOCK_COOKIE, 1, 2, height-4);	// 1

	// 라이트맵을 초기화한다.
	mLightMap.resize(width*height);
	for (int i=0; i<height; i++)
	{
		for (int j=0; j<width; j++)
		{
			if (mGameMap[j+i*width] == BLOCK_AIR)
			{
				mLightMap[j+i*width].light = mLightMap[j+i*width].base_light = 255;
				mLightMap[j+i*width].lighted = true;
// 				for (int k=0; k<5;k++)
// 				{
// 					if (i+k >= height) break;
// 					mLightMap[j+(i+k)*width].light += 10;
// 				}
			}
			else
			{
				mLightMap[j+i*width].light = mLightMap[j+i*width].base_light = min(max(255-(i-pSceneGame->mBaseCamp.iY)*10, 0), 255);
				mLightMap[j+i*width].lighted = mLightMap[j+i*width].base_light>64;
			}
		}
	}
}


void MapGenerator::MakeMine(const int type, const int number, const int spread, const int deep)
{
	std::vector<int>& mGameMap = pSceneGame->mGameMap;
	int width = pSceneGame->iMapWidth;
	int height = pSceneGame->iMapHeight;

	std::vector<MineSeed> seed_stack;
	for (int i=0; i<number; i++)
	{
		int seed_x = rand.Next()%width;
		int seed_y = 0;

		do 
		{
			seed_y = rand.Next()%height;
		} while (seed_y < deep);

		if (mGameMap[seed_x+seed_y*width] != 3)
		{
			i--;
			continue;
		}

		// 위치에서부터 확산한다.
		MineSeed start_seed = {seed_x, seed_y, spread+rand.Next()%2}; 
		seed_stack.push_back(start_seed);
		while (!seed_stack.empty())
		{
			MineSeed now_seed = seed_stack.back();
			seed_stack.pop_back();
			// 타일을 두고
			mGameMap[now_seed.x+now_seed.y*width] = type;

			// 확산 방향을 정한다.
			bool dir[4] = {0,};
			for (int j=0; j<min(now_seed.power, 4);)
			{
				int d = rand.Next()%4;
				if (dir[d]) continue;
				dir[d] = true;
				j++;
				// 확산한다.
				MineSeed child = {now_seed.x, now_seed.y, now_seed.power-1-rand.Next()%2};
				switch (d) {
				case 0: child.x=(child.x+width-1)%width; break;
				case 1: child.x=(child.x+1)%width; break;
				case 2: child.y-=1; break;
				case 3: child.y+=1; break;
				}
				if (mGameMap[child.x+child.y*width] != BLOCK_ROCK) continue;
				seed_stack.push_back(child);
			}
		}
	}
}

