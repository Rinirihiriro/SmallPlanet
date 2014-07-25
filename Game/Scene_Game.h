#pragma once

#include "Scene.h"
#include "MapGenerator.h"
#include "BaseCamp.h"
#include "GameHUD.h"
#include "BaseMenu.h"
#include "Player.h"
#include "Boss.h"
#include "GameData.h"
#include <vector>

class Texture;

class Scene_Game :public Scene
{
public:
	static const int MAGMA_DAMAGE;
	static const int GOAL_FUEL;

	struct Building {
		int type;
		int x;
		int y;
		int frame;
	};
	struct LightMapData {
		unsigned short light;
		unsigned char base_light;
		bool lighted;
	};

	enum SpecialMode {
		MODE_NORMAL
		, MODE_HARDCORE
		, MODE_FARSEER
		, MODE_LESS_GRAVITY
		, MODE_SO_MANY_RESOURCES
		, MODE_NOT_SMALL_PLANET
		, MODE_FINDING_COOKIES
		, MODE_GURREN_LAGANN
		, MODE_NUM
	} mSpecial, mNextSpecialMode;

	std::vector<int> mHeightMap;
	std::vector<int> mGameMap;
	std::vector<LightMapData> mLightMap;
	std::vector<Building> mBuildings;

	int iMapWidth, iMapHeight;
	int iMapWidthPixel, iMapHeightPixel;
	float fShake;

	BaseMenu mBaseMenu;
	GameHUD mGameUI;
	BaseCamp mBaseCamp;

	Player* pPlayer;
	Position mPlayerPos;
	int iMagmaDamage;

	Boss* pBoss;

	int iSelectedBuilding;
	bool bBuild;

	bool bUsable[BLOCK_TYPE_NUM], bBreakable[BLOCK_TYPE_NUM];
	bool bCollected[BLOCK_TYPE_NUM];
	bool bBuildable[BUILDING_SMALL_CAMP+1];
	int iUpgrades[UPGRADE_NUM];

	bool bGameOver;
	bool bEnding;
	bool bBoss;
	bool bInputLock;

	int iHelpStep;
	bool bHelpKeyInput[6];

private:

	int iRestartCount;

	MapGenerator mMapGenerator;
	float fTriFuncConst;

	D3DXVECTOR3 vCamera, vCameraDest, vCameraShake, vCameraDelta;

	Position mBuildingPos;

	bool bNearCamp;

	bool bUnobtainiumBoss;
	bool bCookieGrandma;

	bool bPause;
	bool bTheEnd;
	bool bCleared;

	int iCutSceneTimer;
	enum CutSceneType
	{
		CUTSCENE_NULL
		, CUTSCENE_NORMAL_ENDING
		, CUTSCENE_UNOBTAINIUM_ENDING
		, CUTSCENE_COOKIE_ENDING
		, CUTSCENE_UNOBTAINIUM_BOSS
		, CUTSCENE_COOKIE_BOSS
	} mCutScene, mLastCutScene;

	bool bIgnoreLight;
	bool bGrandma;

	float fFade;
	float fWarningAlpha;
	float fEndingAlpha;

	bool bDrawCracking;
	int iDrillingX, iDrillingY, iDrillDmgLev;
	float fLookAroundLimit;

	Texture* txPaused;
	char* mEndingString;

public:
	Scene_Game();
	virtual ~Scene_Game();

	virtual void InputProc() override;
	virtual void Update() override;
	virtual void Render() override;

	void SetPlayerPos(const int x, const int y);
	void MakeParticle(const int type, const float x, const float y);
	int GetTileType(const int x, const int y);

	void NormalEnding();
	void StartUnobtainiumBossBattle();
	void StartCookieBossBattle();

private:
	void Reset();

	void UpdateFluid(const int start_x, const int end_x, const int start_y, const int end_y);
	bool MoveFluid(const Position& now_fluid, const int dir);
	int PushedDir(const Position& now_fluid, const int dir);

	void Drill(const int block_x, const int block_y);
	bool CheckBuildable(const int type, const int x, const int y, const bool check_cost = true);
	void Build();
	void Light(const int x, const int y);

	void CutSceneEnd();
	void NormalEndingCutScene();
	void UnobtainiumEndingCutScene();
	void CookieEndingCutScene();
	void UnobtainiumBossCutScene();
	void CookieBossCutScene();

	void SetEndingString();

	void MakePauseTexture();
};