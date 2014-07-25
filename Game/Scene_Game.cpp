#include "Scene_Game.h"

#include "JInput.h"
#include "JMain.h"
#include "Random.h"

#include "Player.h"
#include "BlockParticle.h"
#include "Smoke.h"
#include "Bullet.h"
#include "Boss_Unobtainium.h"
#include "Boss_Cookie.h"

#include "TextureManager.h"
#include "PP_MonoColor.h"
#include "FTFontManager.h"
#include "SEManager.h"
#include "BGMManager.h"

#include <tchar.h>
#include <queue>

#include "Essential.h"

#include "GlobalDataManager.h"


const int Scene_Game::MAGMA_DAMAGE = 64;
const int Scene_Game::GOAL_FUEL = 30000;


Scene_Game::Scene_Game()
	:Scene(0xffffffff), pPlayer(NULL), iMapWidth(200), iMapHeight(150), bIgnoreLight(false)
	, txPaused(NULL), fTriFuncConst(0), iHelpStep(0), iRestartCount(0), mNextSpecialMode(MODE_NUM)
{
	mMapGenerator.SetSceneGame(this);
	mGameUI.SetSceneGame(this);
	mBaseCamp.SetSceneGame(this);
	mBaseMenu.SetSceneGame(this);
	ZeroMemory(bHelpKeyInput, sizeof(bHelpKeyInput));
	Reset();
}

Scene_Game::~Scene_Game()
{
	SAFE_DELETE(txPaused);
}


void Scene_Game::Reset()
{
	iRestartCount++;

	EraseAllObjects();

	mHeightMap.clear();
	mGameMap.clear();
	mLightMap.clear();
	mBuildings.clear();

	vCamera = vCameraDest = vCameraShake = vCameraDelta = D3DXVECTOR3(0,0,0);
	bGameOver = bEnding = bPause = bTheEnd = bDrawCracking = bBuild
		= bNearCamp = bUnobtainiumBoss = bCookieGrandma = bIgnoreLight = bBoss = bInputLock
		= bGrandma = false;
	iCutSceneTimer = iSelectedBuilding = iMapWidthPixel = iMapHeightPixel = iCutSceneTimer = 0;
	fFade = fWarningAlpha = fEndingAlpha = 0;
	mCutScene = CUTSCENE_NULL;
	mEndingString = NULL;

	ZeroMemory(bUsable, sizeof(bUsable));
	ZeroMemory(bBreakable, sizeof(bBreakable));
	ZeroMemory(bCollected, sizeof(bCollected));
	ZeroMemory(bBuildable, sizeof(bBuildable));
	ZeroMemory(iUpgrades, sizeof(iUpgrades));
	bUsable[BLOCK_MINERAL] = bUsable[BLOCK_COAL] = bUsable[BLOCK_OBSIDIAN] = true;
	bBreakable[BLOCK_DIRT] = bBreakable[BLOCK_ROCK] = bBreakable[BLOCK_MINERAL]
		= bBreakable[BLOCK_COAL] = bBreakable[BLOCK_URANIUM] = bBreakable[BLOCK_UNOBTAINIUM]
		= bBreakable[BLOCK_COOKIE] = true;

	mGameUI.Reset();
	mBaseCamp.Reset();
	mBaseMenu.Reset();

	TextureManager::GetInstance().GetBlank()->color = 0xffffffff;

	pBoss = NULL;

	mSpecial = MODE_NORMAL;
	char cleared = 0;
	GlobalDataManager::GetInstance().GetData("cleared", &cleared);
	bCleared = cleared > 0;
	if (bCleared) iHelpStep = 7;
	if (bCleared && iRestartCount > 0 && mNextSpecialMode == MODE_NUM)
	{
		long random = Random::rand.Next()%(MODE_NUM*2);
		mSpecial = (random<MODE_NUM)? (SpecialMode)random:MODE_NORMAL;
	}
	else if (mNextSpecialMode < MODE_NUM)
	{
		mSpecial = mNextSpecialMode;
		mNextSpecialMode = MODE_NUM;
	}

	iMagmaDamage = MAGMA_DAMAGE;
	pPlayer = new Player(100, 100);
	if (mSpecial == MODE_LESS_GRAVITY)
		pPlayer->fGravity *= 0.5f;
	else if (mSpecial == MODE_HARDCORE)
	{
		pPlayer->fGravity *= 1.5f;
		pPlayer->iHealthMax /= 2;	pPlayer->iHealth = pPlayer->iHealthMax;
		pPlayer->iAirMax /= 2;		pPlayer->iAir = pPlayer->iAirMax;
		pPlayer->iCargoMax /= 2;
		pPlayer->iJetFuelMax /= 2;	pPlayer->iJetFuel = pPlayer->iJetFuelMax;
	}
	AddObject(pPlayer);

	fLookAroundLimit = 150;
	if (mSpecial == MODE_FARSEER)
	{
		fLookAroundLimit *= 4;
		bIgnoreLight = true;
	}

	int w = 200, h = 150;
	float resource = 1, obstacle = 1;

	if (mSpecial==MODE_SO_MANY_RESOURCES) resource = 2;
	else if (mSpecial==MODE_FINDING_COOKIES)
	{
		resource = 0;
		obstacle = 8;
		mBaseCamp.iFuel = 5000;
		mBaseCamp.iMineral = 5000;
		bUsable[BLOCK_COOKIE] = true;
		bBreakable[BLOCK_OBSIDIAN] = true;
		iUpgrades[UPGRADE_DIRLL_EFFICENCY] = 3;
		bBuildable[BUILDING_LIGHT] = true;
		pPlayer->iDrillLevel = 3;
		pPlayer->iDrillPower = Player::BASIC_DRILL_POWER*gUpgrades[UPGRADE_DIRLL_EFFICENCY][3].effect;
	}
	else if (mSpecial==MODE_HARDCORE)
	{
		w = 150;
		h = 100;
		resource = 0.75f;
		obstacle = 1.25f;
	}
	else if (mSpecial == MODE_NOT_SMALL_PLANET)
	{
		w = 400;
		h = 250;
	}
	else if (mSpecial == MODE_GURREN_LAGANN)
	{
		iUpgrades[UPGRADE_DIRLL_EFFICENCY] = 5;
		bBreakable[BLOCK_OBSIDIAN] = true;
		pPlayer->iDrillLevel = 4;
		pPlayer->iDrillPower = Player::BASIC_DRILL_POWER*gUpgrades[UPGRADE_DIRLL_EFFICENCY][5].effect;
	}

	mMapGenerator.GenerateMap(w, h, resource, obstacle);

	BGMManager::GetInstance().StopAllBGMs();
	BGMManager::GetInstance().SetBGMsVolume(1.0f);
	BGMManager::GetInstance().PlayBGM("small_planet");
}


void Scene_Game::InputProc()
{
	// debug
	if (JInput::IsPressed(VK_SHIFT) && JInput::IsPressed('F'))
		mBaseCamp.iFuel += 256;
	if (JInput::IsPressed(VK_SHIFT) && JInput::IsPressed('M'))
		mBaseCamp.iMineral += 256;
	if (JInput::IsPressed(VK_SHIFT) && JInput::IsTriggered('L'))
		bIgnoreLight = !bIgnoreLight;
	if (!bBoss && JInput::IsPressed(VK_SHIFT) && JInput::IsTriggered('B'))
	{
		StartUnobtainiumBossBattle();
	}
	if (!bBoss && JInput::IsPressed(VK_SHIFT) && JInput::IsTriggered('C'))
	{
		StartCookieBossBattle();
	}


	if (JInput::IsTriggered(VK_ESCAPE) || JInput::controller.IsButtonTriggered(Controller::START, 0))
	{
		bPause = !bPause;
		if (bPause)
		{
			SEManager::GetInstance().PlaySE("pause");
			BGMManager::GetInstance().SetBGMsVolume(0.25f);
			MakePauseTexture();
		}
		else
		{
			SEManager::GetInstance().PlaySE("unpause");
			BGMManager::GetInstance().SetBGMsVolume(1.0f);
		}
	}

	if (bPause)
	{
		if (JInput::IsTriggered(VK_F2) || JInput::controller.IsButtonTriggered(Controller::BACK, 0))
		{
			Reset();
			return;
		}
		if (bCleared)
		{
			for (int i=0; i<MODE_NUM; i++)
			{
				if (JInput::IsTriggered('1'+i))
					mNextSpecialMode = (SpecialMode)i;
			}
			if (JInput::IsTriggered('0'))
				mNextSpecialMode = MODE_NUM;
		}
	}

	if (bPause || bInputLock) return;
	if (mBaseMenu.bActive)
	{
		mBaseMenu.InputProc();
		return;
	}

	mGameUI.InputProc();
	if (mGameUI.bBuild) return;

	float right_con_x, right_con_y;
	JInput::controller.GetRightAnalogStick(&right_con_x, &right_con_y, 0);

	if (bBuild)
	{
		if (JInput::IsTriggered('X') || JInput::controller.IsButtonTriggered(Controller::B, 0))
		{
			bBuild = false;
			Build();
		}
	}

	if (JInput::IsPressed(VK_CONTROL))
	{
		if (JInput::IsPressed(VK_LEFT))
		{
			vCameraDelta.x -= fLookAroundLimit*0.0625f;
			if (iHelpStep == 3) bHelpKeyInput[4] = true;
		}
		if (JInput::IsPressed(VK_RIGHT))
		{
			vCameraDelta.x += fLookAroundLimit*0.0625f;
			if (iHelpStep == 3) bHelpKeyInput[4] = true;
		}
		if (JInput::IsPressed(VK_UP))
		{
			vCameraDelta.y -= fLookAroundLimit*0.0625f;
			if (iHelpStep == 3) bHelpKeyInput[4] = true;
		}
		if (JInput::IsPressed(VK_DOWN))
		{
			vCameraDelta.y += fLookAroundLimit*0.0625f;
			if (iHelpStep == 3) bHelpKeyInput[4] = true;
		}
		if (D3DXVec3Length(&vCameraDelta) > fLookAroundLimit)
		{
			D3DXVECTOR3 v;
			D3DXVec3Normalize(&v, &vCameraDelta);
			vCameraDelta = v*fLookAroundLimit;
		}
	}
	else if (JInput::IsReleased(VK_CONTROL))
		vCameraDelta = D3DXVECTOR3(0,0,0);

	if (JInput::controller.IsConnected(0))
	{
		vCameraDelta.x = right_con_x*fLookAroundLimit;
		vCameraDelta.y = -right_con_y*fLookAroundLimit;
		if (iHelpStep == 3 && D3DXVec3Length(&vCameraDelta) > 1)
			bHelpKeyInput[4] = true;
	}

	if (!JInput::IsPressed(VK_CONTROL))
	{
		if (JInput::IsPressed(VK_LEFT) || JInput::controller.IsStickPressed(Controller::LSTICK, Controller::LEFT, 0))
		{
			pPlayer->Move(0);
			if (iHelpStep == 1) bHelpKeyInput[0] = true;
		}
		if (JInput::IsPressed(VK_RIGHT) || JInput::controller.IsStickPressed(Controller::LSTICK, Controller::RIGHT, 0))
		{
			pPlayer->Move(1);
			if (iHelpStep == 1) bHelpKeyInput[0] = true;
		}
	}

	if (JInput::IsPressed('Z') || JInput::controller.IsButtonPressed(Controller::A, 0))
	{
		pPlayer->Jump();
		if (iHelpStep == 1) bHelpKeyInput[1] = true;
	}
	if (JInput::IsPressed('X') || JInput::controller.IsButtonPressed(Controller::B, 0))
	{
		iDrillingX = mPlayerPos.x;
		iDrillingY = mPlayerPos.y;

		pPlayer->bDrilling = true;

		int dx = 1, dy = 1;
		
		if (JInput::IsPressed(VK_LEFT) || JInput::controller.IsStickPressed(Controller::LSTICK, Controller::LEFT, 0))
		{
			iDrillingX = (iDrillingX+iMapWidth-1)%iMapWidth;
			dx-=1;
		}
		if (JInput::IsPressed(VK_RIGHT) || JInput::controller.IsStickPressed(Controller::LSTICK, Controller::RIGHT, 0))
		{
			iDrillingX = (iDrillingX+1)%iMapWidth;
			dx+=1;
		}
		if (JInput::IsPressed(VK_UP) || JInput::controller.IsStickPressed(Controller::LSTICK, Controller::UP, 0))
		{
			if (JInput::IsPressed(VK_LEFT)
				||JInput::controller.IsStickPressed(Controller::LSTICK, Controller::LEFT, 0)
				||JInput::IsPressed(VK_RIGHT)
				||JInput::controller.IsStickPressed(Controller::LSTICK, Controller::RIGHT, 0))
				iDrillingY = max(iDrillingY-1, 0);
			else
				iDrillingY = max(iDrillingY-2, 0);
			dy-=1;
		}
		if (JInput::IsPressed(VK_DOWN) || JInput::controller.IsStickPressed(Controller::LSTICK, Controller::DOWN, 0))
		{
			iDrillingY = min(iMapHeight-1, iDrillingY+1);
			dy+=1;
		}

		pPlayer->iDir = dx+dy*3;
		bDrawCracking = false;
		Drill(iDrillingX, iDrillingY);
		if (iHelpStep == 1) bHelpKeyInput[2] = true;
	}
	else if (JInput::IsReleased('X') || JInput::controller.IsButtonReleased(Controller::B, 0))
	{
		pPlayer->bDrilling = false;
		pPlayer->iDrillDamage = 0;
		bDrawCracking = false;
	}


	if (mBaseCamp.bNearBase && (JInput::IsTriggered('Y') || JInput::controller.IsButtonTriggered(Controller::Y, 0)))
	{
		mBaseMenu.Open();
	}

	if (JInput::IsPressed(VK_LSHIFT) || JInput::controller.IsButtonPressed(Controller::RTRIGGER, 0))
	{
		pPlayer->Jet();
		mGameUI.Show(GameHUD::UI_JETPACK);
		if (pPlayer->iJetFuel > 0)
		{
			BGMManager::GetInstance().PlayBGM("jetpack");
			D3DXVECTOR3 pos = pPlayer->vPos;
			pos.x += (float)Random::rand.NextDouble()*10-5;
			pos.y += (float)Random::rand.NextDouble()*10-5;
			Smoke* smoke = new Smoke(0, pos.x, pos.y) ;
			smoke->z = 1;
			smoke->AddVelocity(0, 2);
			AddObject(smoke);
		}
		if (iHelpStep == 1) bHelpKeyInput[3] = true;
	}
	else
		BGMManager::GetInstance().FadeOutBGMAndStop("jetpack", 200);

}

void Scene_Game::Update()
{
	if (bPause) return;

	Scene::Update();

	if (bBoss)
	{
		D3DXVECTOR3 bosspos = D3DXVECTOR3((float)pBoss->iX*BLOCK_SIZE+BLOCK_SIZE/2, (float)pBoss->iY*BLOCK_SIZE+BLOCK_SIZE/2, 0);
		float distance = abs(pPlayer->vPos.x-bosspos.x);
		if (distance > iMapWidthPixel/2)
		{
			bosspos.x += (bosspos.x < iMapWidthPixel/2)? iMapWidthPixel:-iMapWidthPixel;
			distance = iMapWidthPixel-distance;
		}
		if (distance < 256.f && abs(pPlayer->vPos.y-bosspos.y) < 256.f)
		{
			vCameraDest = (pPlayer->vPos+bosspos)/2 - D3DXVECTOR3(200, 150, 0);
		}
		else
			vCameraDest = pPlayer->vPos-D3DXVECTOR3(200, 150, 0);
	}
	else
		vCameraDest = pPlayer->vPos-D3DXVECTOR3(200, 150, 0);

	if (vCameraDest.y+vCameraDelta.y < 0) vCameraDest.y = -vCameraDelta.y;
	else if (vCameraDest.y+vCameraDelta.y+300 > iMapHeightPixel) vCameraDest.y = (float)(iMapHeightPixel-vCameraDelta.y-300);

	vCamera += (vCameraDest+vCameraDelta-vCamera)*0.25f;
	if (fShake>0.1f)
	{
		fTriFuncConst -= 0.375f*fShake;
		while (fTriFuncConst < 0) fTriFuncConst += D3DX_PI*2;

		JInput::controller.SetVibration((unsigned)(fShake*32768), (unsigned)(fShake*32768), 0);
		vCameraShake.x = sin(fTriFuncConst)*fShake*2.5f;
		vCameraShake.y = sin(fTriFuncConst+0.5f)*fShake*2.5f;
		fShake = fShake*0.875f;
		if (fShake<0.125f) fShake = 0;
	}

	mPlayerPos.x = ((int)pPlayer->vPos.x)/BLOCK_SIZE;
	mPlayerPos.y = max(((int)pPlayer->vPos.y+8)/BLOCK_SIZE, 0);

	if (pBoss && pBoss->IsDead() && mCutScene == CUTSCENE_NULL)
	{
		bInputLock = true;
		bGameOver = true;
		bEnding = true;

		if (mLastCutScene == CUTSCENE_UNOBTAINIUM_BOSS)
		{
			mCutScene = CUTSCENE_UNOBTAINIUM_ENDING;
		}
		else if (mLastCutScene == CUTSCENE_COOKIE_BOSS)
		{
			mCutScene = CUTSCENE_COOKIE_ENDING;
		}
	}

	if (bTheEnd)
	{
		fEndingAlpha = min(fEndingAlpha+0.0625f, 1);
	}

	mBaseCamp.Update();
	mBaseMenu.Update();

	// 컷신
	switch (mCutScene)
	{
	case CUTSCENE_NORMAL_ENDING:
		NormalEndingCutScene();
		break;
	case CUTSCENE_UNOBTAINIUM_ENDING:
		UnobtainiumEndingCutScene();
		break;
	case CUTSCENE_COOKIE_ENDING:
		CookieEndingCutScene();
		break;
	case CUTSCENE_UNOBTAINIUM_BOSS:
		UnobtainiumBossCutScene();
		break;
	case CUTSCENE_COOKIE_BOSS:
		CookieBossCutScene();
		break;
	}

	fWarningAlpha = max(1-(float)pPlayer->iAir/pPlayer->iAirMax*5, 0);

	if (pPlayer->iHealth > 0 && pPlayer->iAir <= pPlayer->iAirMax*0.25) BGMManager::GetInstance().PlayBGM("air_warning");
	else BGMManager::GetInstance().FadeOutBGMAndStop("air_warning", 200);

	bNearCamp = false;
	if (!bGameOver)
	{
		pPlayer->iAir = max(pPlayer->iAir - (int)(max(0, mPlayerPos.y-mBaseCamp.iY)*0.25f), 0);
		for each (Building building in mBuildings)
		{
			if (building.type != BUILDING_SMALL_CAMP) continue;
			int cx = building.x+gBuildingData[building.type].width/2;
			int cy = building.y;
			bNearCamp = (abs(cx-mPlayerPos.x) < 2
				|| abs((cx+iMapWidth/2)%iMapWidth-(mPlayerPos.x+iMapWidth/2)%iMapWidth) < 2) // 건물이 경계에 걸쳤을 경우
				&& abs(cy-mPlayerPos.y) < 2 && cy >= mPlayerPos.y;
			if (bNearCamp) break;
		}
		if (gBlockData[mGameMap[mPlayerPos.x+mPlayerPos.y*iMapWidth]].fluid)
		{
			pPlayer->bInFluid = true;
			if (gBlockData[mGameMap[mPlayerPos.x+(mPlayerPos.y-1)*iMapWidth]].fluid)
				pPlayer->bTotalyInFluid = true;
		}
		if (mGameMap[mPlayerPos.x+mPlayerPos.y*iMapWidth] == BLOCK_MAGMA && iMagmaDamage>0)
		{
			pPlayer->iHitCooltime = 120;
			pPlayer->iHealth = max(pPlayer->iHealth-iMagmaDamage, 0);
		}
		if (pPlayer->vPos.y < -500)
			pPlayer->iHealth = 0;
		if (bBuild)
		{
			mBuildingPos.x = mPlayerPos.x-gBuildingData[iSelectedBuilding].width/2;
			mBuildingPos.y = mPlayerPos.y;
		}
		if (pPlayer->iAirpackBrokenTimer > 0)
		{
			D3DXVECTOR3 pos = pPlayer->vPos;
			pos.x += (float)Random::rand.NextDouble()*10-5;
			pos.y += (float)Random::rand.NextDouble()*10-5;
			Smoke* smoke = new Smoke(0, pos.x, pos.y) ;
			smoke->z = 1;
			smoke->AddVelocity(0, -3);
			AddObject(smoke);
		}
	}

	if (bNearCamp && !bGameOver)
	{
		pPlayer->iAir = min(pPlayer->iAir+64, pPlayer->iAirMax);
		pPlayer->iHealth = min(pPlayer->iHealthMax, pPlayer->iHealth+16);
		// 자동 충전
		{
			int charge_fuel = min(pPlayer->iJetFuelMax-pPlayer->iJetFuel, 16);
			if (charge_fuel > 0)
			{
				int need_fule = (charge_fuel+4)/8;
				if (mBaseCamp.iFuel >= need_fule)
				{
					pPlayer->iJetFuel += charge_fuel;
					mBaseCamp.iFuel -= need_fule;
				}
				else
				{
					pPlayer->iJetFuel += mBaseCamp.iFuel*8;
					mBaseCamp.iFuel = 0;
				}
			}
		}
	}

	GameObj other("null", 0, 0);
	other.mRect.width = other.mRect.height = BLOCK_SIZE;
	
	for each(GameObj* obj in objArr)
	{
		bool player = obj->GetObjectTag() == "player";
		if (player && pPlayer->iHealth<=0) continue;

		// 루프 보정
		if (obj->vPos.x < 0)
		{
			obj->vPos.x += iMapWidthPixel;
			if (player)
			{
				vCamera.x += iMapWidthPixel;
				vCameraDest.x += iMapWidthPixel;
			}
		}
		if (obj->vPos.x > iMapWidthPixel)
		{
			obj->vPos.x -= iMapWidthPixel;
			if (player)
			{
				vCamera.x -= iMapWidthPixel;
				vCameraDest.x -= iMapWidthPixel;
			}
		}

		if (obj->GetObjectTag() == "smoke") continue;

		int block_x = ((int)obj->vPos.x)/BLOCK_SIZE;
		int block_y = max(((int)obj->vPos.y)/BLOCK_SIZE, 0);

		/*
			충돌체크
			0 1 2 3 4
			5 6 7 8 9
			1011121314
			1516171819
			2021222324
		*/
		if (player || obj->GetObjectTag() == "bparticle")
		{
			static const int check[] = {12, 7,11,13,17, 6,8,16,18, 2,10,14,22, 1,3,5,9,15,19,21,23};
			for each (int i in check)
			{
				int x = (block_x+i%5-2+iMapWidth)%iMapWidth;
				int y = block_y+i/5-2;
				if (y<0 || y>=iMapHeight) continue;
				if (mGameMap[x+y*iMapWidth] == BLOCK_AIR || gBlockData[mGameMap[x+y*iMapWidth]].fluid)
					continue;
				other.mRect.center = D3DXVECTOR3((float)((block_x+i%5-2)*BLOCK_SIZE+BLOCK_SIZE/2)
					, (float)((block_y+i/5-2)*BLOCK_SIZE+BLOCK_SIZE/2)
					, 0);
				(*obj)&other;
			}
		}

		// 빨아들이기
		if (pPlayer->iHealth>0 && obj->GetObjectTag() == "bparticle")
		{
			BlockParticle* particle = (BlockParticle*)obj;
			if (!gBlockData[particle->iType].collectable) continue;

			D3DXVECTOR3 d = obj->vPos-pPlayer->vPos;
			float distance = D3DXVec3LengthSq(&d);

			if (particle->vFollow == NULL
				&& pPlayer->iCargo < pPlayer->iCargoMax
				&& distance < 512)
			{
				particle->vFollow = &pPlayer->vPos;
			}
			
			else if (particle->vFollow != NULL)
			{
				// 가방이 꽉 차면 버린다.
				if (pPlayer->iCargo >= pPlayer->iCargoMax)
				{
					particle->vFollow = NULL;
				}
				else if (distance < 81)
				{
					SEManager::GetInstance().PlaySE("stack", true);
					obj->erase();
					pPlayer->iCargo += 1;
					pPlayer->iCargoDetail[particle->iType] += 1;
					mGameUI.Show(GameHUD::UI_CARGO);
					if (!bCollected[particle->iType])
					{
						bCollected[particle->iType] = true;
						mGameUI.NewResource(particle->iType);
					}
					if (pPlayer->iCargo >= pPlayer->iCargoMax)
						SEManager::GetInstance().PlaySE("cargo_full");
				}
			}
		}
		else if (obj->GetObjectTag() == "bullet")
		{
			Bullet* bullet = (Bullet*)obj;
			if (mGameMap[block_x+block_y*iMapWidth] != BLOCK_AIR)
			{
				SEManager::GetInstance().PlaySE("missile_crash");
				bullet->iLife = 1;
				fShake = max(bullet->GetSpeed()/5, fShake);
			}
			if (pPlayer->iHealth>0 && pPlayer->iHitCooltime == 0)
			{
				D3DXVECTOR3 coli = pPlayer->mRect&obj->mRect;
				if (coli.x != 0 || coli.y != 0)
				{
// 					bullet->iLife = 1;
// 					fShake = max(bullet->GetSpeed()/3, fShake);
					if (bullet->fFlash<0.125f)
					{
						float dir = (coli.x > 0)? -1.f:((coli.x==0)? 0:1.f);
						SEManager::GetInstance().PlaySE("hurt");
						pPlayer->iHitCooltime = 100;
						pPlayer->iHealth = max(0, pPlayer->iHealth-bullet->iPower);
						pPlayer->SetVelocity(dir*5, -5);
						if (bullet->fAirLeakRate > 0
							&& (float)Random::rand.NextDouble() < bullet->fAirLeakRate)
							pPlayer->iAirpackBrokenTimer = 600;
					}
				}
			}
		}
		else if (obj->GetObjectTag() == "boss")
		{
			Boss* boss = (Boss*)obj;
			if (pPlayer->bDrilling && boss->iHitCoolTime == 0
				&& iDrillingX >= boss->iX-boss->iSize/2 && iDrillingX <= boss->iX+boss->iSize/2
				&& iDrillingY >= boss->iY-boss->iSize/2 && iDrillingY <= boss->iY+boss->iSize/2)
			{
				SEManager::GetInstance().PlaySE("hit");
				boss->Damage(pPlayer->iDrillPower);
				fShake = 4.5f;
			}
			if (boss->iHealth == 0 && !bEnding) fShake = 2;
		}
	}
	if (pPlayer->iCargo >= pPlayer->iCargoMax)
		mGameUI.NotiFull();

	std::vector<std::vector<Building>::iterator> building_erase_vec;
	for (auto it=mBuildings.begin(); it<mBuildings.end(); it++)
	{
		Building& building = *it;
		const BuildingData& data = gBuildingData[building.type];
		// 프레임 처리
		if (building.type == BUILDING_POWER_WIND)
		{
			building.frame = (building.frame+1)%40;
		}
		// 전기 생산소비 계산
// 		mBaseCamp.iPower += data.power_per_sec;
		int d = abs(mBaseCamp.iY-building.y);
		// 본진에서 멀 수록 전기를 사용한다.
		// 발전소는 발전 효율이 떨어진다.
		if (data.power_per_sec > 0)
		{
			int pow = data.power_per_sec;
			switch (building.type)
			{
			case BUILDING_POWER_WIND:
				pow += max(0, (mBaseCamp.iY-building.y)/2);
				break;
			case BUILDING_POWER_HEAT:
				pow += max(0, (building.y-mBaseCamp.iY)/10);
				break;
			}
			mBaseCamp.iPower += (int)(pow*min(32.f/d, 1));
		}
		else if (data.power_per_sec < 0)
		{
			mBaseCamp.iPower += data.power_per_sec*max(1, d/32);
		}
		// 파괴 계산
		if (!CheckBuildable(building.type, building.x, building.y, false))
		{
			// 건물을 부순다
			SEManager::GetInstance().PlaySE("build");
			for (int i=0; i<data.width*BLOCK_SIZE; i+=4)
			{
				int x = (building.x*BLOCK_SIZE+i+iMapWidthPixel)%(iMapWidthPixel);
				AddObject(new Smoke(0, (float)x, (float)(building.y+1)*BLOCK_SIZE));
			}
			building_erase_vec.insert(building_erase_vec.begin(),it);
			if (building.type == BUILDING_LIGHT)
			{
				int bx, by, lx, ly, w = 21, ww = w*w, hw = w/2, hwhw = hw*hw;
				for (int i=0; i<ww; i++)
				{
					bx = i%w-hw;
					by = i/w-hw;
					lx = (mBuildingPos.x+bx+iMapWidth)%iMapWidth;
					ly = mBuildingPos.y+by;
					if (ly < 0 || ly >= iMapHeight) continue;
					mLightMap[lx+ly*iMapWidth].light -= max(0, (hwhw-((bx*bx)+(by*by)))*2);
				}
			}
			break;
		}
	}
	for each (auto it in building_erase_vec)
	{
		mBuildings.erase(it);
	}

	// 유체
	int start_x = ((int)vCamera.x)/BLOCK_SIZE-2;
	int start_y = max(((int)vCamera.y)/BLOCK_SIZE-2, 0);
	int end_x = ((int)vCamera.x+400)/BLOCK_SIZE+3;
	int end_y = min(((int)vCamera.y+300)/BLOCK_SIZE+3, iMapHeight-1);

	UpdateFluid(start_x, end_x, start_y, end_y);

	// 사망
	if (!bGameOver && pPlayer->iHealth <= 0)
	{
		SEManager::GetInstance().PlaySE("boon");
		BGMManager::GetInstance().FadeOutBGMAndStop("air_leak", 500);
		bGameOver = true;
		bInputLock = true;
		pPlayer->SetVelocity(0,0);
		pPlayer->fGravity = 0;
		for (int i=0; i<32; i++)
		{
			D3DXVECTOR3 v = pPlayer->vPos;
			v.x += (float)Random::rand.NextDouble()*30-15;
			v.y += (float)Random::rand.NextDouble()*20-10;
			AddObject(new Smoke(0, v.x, v.y));
		}
		pPlayer->bDrilling = false;
		pPlayer->iDrillDamage = 0;
		bDrawCracking = false;
	}

	if (pPlayer->iStandCounter == 60 || mBaseCamp.bNearBase || bNearCamp)
		mGameUI.ShowAll();

	if (pPlayer->iJetFuel < pPlayer->iJetFuelMax/2)
		mGameUI.Show(GameHUD::UI_JETPACK);
	if (pPlayer->iHitCooltime > 0)
		mGameUI.Show(GameHUD::UI_HEALTH);
	if (pPlayer->iHealth < pPlayer->iHealthMax)
		mGameUI.Show(GameHUD::UI_HEALTH, true);
	if (pPlayer->iAir < pPlayer->iAirMax/2)
		mGameUI.Show(GameHUD::UI_OXYGEN, true);
	if (pPlayer->iCargo >= (int)(pPlayer->iCargoMax*0.75f))
		mGameUI.Show(GameHUD::UI_CARGO, true);

	if (!mGameUI.IsHelpOver())
	{
		if (iHelpStep == 1)
		{
			if (bHelpKeyInput[0] && bHelpKeyInput[1] && bHelpKeyInput[2] && bHelpKeyInput[3])
				mGameUI.HideHelp();
		}
		else if (iHelpStep == 3)
		{
			if (bHelpKeyInput[4] && bHelpKeyInput[5])
				mGameUI.HideHelp();
		}
	}
	else
	{
		if (iHelpStep == 1 || iHelpStep == 3)
			iHelpStep++;
	}

	switch (iHelpStep)
	{
	case 0:
		mGameUI.ShowHelp(0); iHelpStep++; break;
	case 2:
		mGameUI.ShowHelp(1); iHelpStep++; break;
	case 4:
		mGameUI.ShowHelp(2); mGameUI.Show(GameHUD::UI_GOAL, 720); iHelpStep++; break;
	case 6:
		mGameUI.ShowHelp(3); iHelpStep++; break;
	}

	mGameUI.Update();

}

void Scene_Game::Render()
{
	if (bPause)
	{
		static const char* const mode_names[] = {
			"Normal", "Hardcore", "Far-seer", "Less gravity", "So many resources"
			, "NOT Small Planet", "Finding Cookies", "Gurren Lagann"
		};
		D3DXVECTOR3 scale(2,2,1);
		txPaused->Draw(D3DXVECTOR3(0,0,0),&scale);
		TextureManager::GetInstance().GetTexture("pause")->Draw(D3DXVECTOR3(200,150,0));
		D3DXCOLOR white(1,1,1,1);
		if (mSpecial > MODE_NORMAL)
			DrawFont(5, 103, mode_names[mSpecial], &white);
		if (mNextSpecialMode < MODE_NUM)
			DrawFont(5, 112, mode_names[mNextSpecialMode], &white);
		return;
	}

	D3DXMATRIX mat, trans;
	float camera_x = floor(vCamera.x - vCameraShake.x);
	float camera_y = floor(vCamera.y - vCameraShake.y);
	Texture::GetSprite().GetTransform(&mat);
	D3DXMatrixTranslation(&trans, -camera_x, -camera_y, 0);

	int start_x = ((int)vCamera.x)/BLOCK_SIZE-2;
	int start_y = max(((int)vCamera.y)/BLOCK_SIZE-2, 0);
	int end_x = ((int)vCamera.x+400)/BLOCK_SIZE+2;
	int end_y = min(((int)vCamera.y+300)/BLOCK_SIZE+2, iMapHeight-1);

	TextureManager::GetInstance().GetTexture(bGrandma? "background_grandma":"background")->Draw(D3DXVECTOR3(0,0,0));

	if (camera_y <= mBaseCamp.vBasePos.y)
		TextureManager::GetInstance().GetTexture("gradient")->Draw(
		D3DXVECTOR3(0, mBaseCamp.vBasePos.y - 150 - camera_y, 0)
		);
	else if (camera_y+300 >= iMapHeightPixel)
		TextureManager::GetInstance().GetTexture("gradient")->Draw(D3DXVECTOR3(0, 0, 0));

	Texture::GetSprite().SetTransform(trans*mat);

	// 배경
	if (!pBoss)
	{
		Texture * const underground = TextureManager::GetInstance().GetTexture("underground");
		for (int i=start_x; i<=end_x; i++)
		{
			int x = i%iMapWidth;
			while (x<0) x+=iMapWidth;
			for (int j=start_y; j<=end_y; j++)
			{
				if (j<iMapHeight-mHeightMap[x]) continue;
				underground->Draw(D3DXVECTOR3((float)(i*BLOCK_SIZE), (float)(j*BLOCK_SIZE), 0));
			}
		}
	}
	// 건물들
	if (!pBoss)
		mBaseCamp.Draw(camera_x);

	static const char* const tx_ids[] = {"null", "light", "wpp", "gpp", "small_camp"};
	for each (Building building in mBuildings)
	{
		D3DXVECTOR3 pos((float)building.x*BLOCK_SIZE, (float)(building.y+1)*BLOCK_SIZE, 0);
		// 경계에서 반대쪽 경계 건물이 보이지 않는 현상 해결
		if (camera_x-32 < 0 && pos.x >= iMapWidthPixel+camera_x-64)
			pos.x -= iMapWidthPixel;
		else if (camera_x+400+32 > iMapWidthPixel && pos.x <= camera_x+400-iMapWidthPixel+64)
			pos.x += iMapWidthPixel;
		TextureManager::GetInstance().GetTexture(tx_ids[building.type])->Draw(pos,0,0,building.frame/10);
	}
	// 보스, 플레이어, 블럭 파티클
	if(pBoss) pBoss->Draw();
	pPlayer->Draw();
	for each (GameObj* obj in objArr)
	{
		if (obj->GetObjectTag() == "bparticle")
		obj->Draw();
		// 경계에서 반대쪽 경계 오브젝트가 보이지 않는 현상 해결
		if (camera_x < 0 && obj->vPos.x >= iMapWidthPixel+camera_x-64)
		{
			float oldx = obj->vPos.x;
			obj->vPos.x -= iMapWidthPixel;
			obj->Draw();
			obj->vPos.x = oldx;
		}
		else if (camera_x+400 > iMapWidthPixel && obj->vPos.x <= camera_x+400-iMapWidthPixel+64)
		{
			float oldx = obj->vPos.x;
			obj->vPos.x += iMapWidthPixel;
			obj->Draw();
			obj->vPos.x = oldx;
		}
	}
	// 지을 건물
	if (bBuild && CheckBuildable(iSelectedBuilding, mBuildingPos.x, mBuildingPos.y))
	{
		D3DXVECTOR3 pos((float)mBuildingPos.x*BLOCK_SIZE, (float)(mBuildingPos.y+1)*BLOCK_SIZE, 0);
		// 경계에서 반대쪽 경계 건물이 보이지 않는 현상 해결
		if (camera_x < 0 && pos.x >= iMapWidthPixel+camera_x-128)
			pos.x -= iMapWidthPixel;
		else if (camera_x+400 > iMapWidthPixel && pos.x <= camera_x+400-iMapWidthPixel+128)
			pos.x += iMapWidthPixel;
		TextureManager::GetInstance().GetTexture(tx_ids[iSelectedBuilding])->Draw(pos,0,0,0,0.5f);
	}
	// 블럭 타일
	Texture * const tile = TextureManager::GetInstance().GetTexture("tile");
	tile->color = 0xffffffff;
	for (int i=start_x; i<=end_x; i++)
	{
		int x = i%iMapWidth;
		while (x<0) x+=iMapWidth;
		for (int j=start_y; j<=end_y; j++)
		{
			D3DXVECTOR3 pos = D3DXVECTOR3((float)(i*BLOCK_SIZE), (float)(j*BLOCK_SIZE), 0);
			if (mGameMap[x+j*iMapWidth]!=BLOCK_AIR)
			{
				tile->Draw(pos,0,0,mGameMap[x+j*iMapWidth], gBlockData[mGameMap[x+j*iMapWidth]].fluid? 0.75f:1.f);
				if (bDrawCracking && x == iDrillingX && j == iDrillingY)
					TextureManager::GetInstance().GetTexture("cracking")->Draw(pos,0,0,iDrillDmgLev);
			}
		}
	}
	// 오브젝트들
	for each (GameObj* obj in objArr)
	{
		if (obj==pPlayer || obj->GetObjectTag() == "bparticle" || obj->GetObjectTag()=="boss") continue;
		obj->Draw();
		// 경계에서 반대쪽 경계 오브젝트가 보이지 않는 현상 해결
		if (camera_x < 0 && obj->vPos.x >= iMapWidthPixel+camera_x-64)
		{
			float oldx = obj->vPos.x;
			obj->vPos.x -= iMapWidthPixel;
			obj->Draw();
			obj->vPos.x = oldx;
		}
		else if (camera_x+400 > iMapWidthPixel && obj->vPos.x <= camera_x+400-iMapWidthPixel+64)
		{
			float oldx = obj->vPos.x;
			obj->vPos.x += iMapWidthPixel;
			obj->Draw();
			obj->vPos.x = oldx;
		}
	}
	// 라이트 맵
	for (int i=start_x; i<=end_x; i++)
	{
		int x = i%iMapWidth;
		while (x<0) x+=iMapWidth;
		for (int j=start_y; j<=end_y; j++)
		{
			D3DXVECTOR3 pos = D3DXVECTOR3((float)(i*BLOCK_SIZE), (float)(j*BLOCK_SIZE), 0);
			float light = bIgnoreLight? 1.f:mLightMap[x+j*iMapWidth].light/255.f;
			tile->color = D3DXCOLOR(0, 0, 0, 1-light);
			tile->Draw(pos,0,0,1);
		}
	}
	// enter message
	{
		D3DXVECTOR3 scale(mBaseCamp.fEnterMessageScale,mBaseCamp.fEnterMessageScale,1);
		TextureManager::GetInstance().GetTexture("enter_message")
			->Draw(mBaseCamp.vBasePos+D3DXVECTOR3(0,-75,0),&scale);
	}
	Texture::GetSprite().SetTransform(mat);
	// Transform END

	if (fWarningAlpha > 0)
		TextureManager::GetInstance().GetTexture("warning_gradient")->Draw(D3DXVECTOR3(0,0,0),0,0,0,fWarningAlpha);

// 	TextureManager::GetInstance().GetBlank()->color = 0xffffffff;
	TextureManager::GetInstance().GetBlank()->Draw(D3DXVECTOR3(0,0,0),0,0,0,fFade);

	if (!bEnding)
	{
		mGameUI.Draw();
		mBaseMenu.Draw();
	}

	if (bTheEnd)
	{
		D3DXCOLOR white = D3DXCOLOR(1,1,1,fEndingAlpha);
		TextureManager::GetInstance().GetTexture("the_end")->Draw(D3DXVECTOR3(200, 130, 0),0,0,0,fEndingAlpha);
		if (mEndingString)
			DrawFont(200.f-GetFontLength(mEndingString)/2, 200, mEndingString, &white);
// 		FTFontManager::GetInstance().GetFont("default").DrawText(200, 150, L"THE END", 0xffffffff, FTFont::FONT_CENTER);
	}

// 	{
// 		int d = abs(mBaseCamp.iY-mPlayerPos.y);
// 		TCHAR str[128]={0,};
// 		_stprintf_s(str, 128, L"X:%d Y:%d D:%d(%f, %d)", mPlayerPos.x, mPlayerPos.y, d, min(32.f/d, 1), max(d/32, 1));
// 		FTFontManager::GetInstance().GetFont("default").DrawText(3, 15, str, 0xffffffff, FTFont::FONT_LEFT);
// 	}
}


void Scene_Game::UpdateFluid(const int start_x, const int end_x, const int start_y, const int end_y)
{
	// 이동 가능한 블럭을 찾는다.
	for (int i=end_y-1; i>=start_y; i--)
	{
		for (int j=start_x; j<end_x; j++)
		{
			Position data((j+iMapWidth)%iMapWidth, i);
			if (!gBlockData[mGameMap[(j+iMapWidth)%iMapWidth+i*iMapWidth]].fluid) continue;

			// 아래로 이동시도
			if (MoveFluid(data, 1)) continue;
			// 밀리고 있으면
			int dir = PushedDir(data, 0);
			if (dir == 1)
			{
				// 위면 좌 우 한 번 씩 시도
				if (MoveFluid(data, 2)) continue;
				if (MoveFluid(data, 3)) continue;
			}
			// 옆이면 옆으로
			else if (dir) MoveFluid(data, dir);
		}
	}
	/*
	// 현재 보이는 유체들 중 상단에 유체가 없는 유체들부터 시작.
	for (int i=start_x; i<end_x; i++)
	{
		for (int j=start_y; j<end_y; j++)
		{
			int now_pos = (i+iMapWidth)%iMapWidth+j*iMapWidth;
			int upper_pos = (i+iMapWidth)%iMapWidth+(j-1)*iMapWidth;
			if (gBlockData[mGameMap[now_pos]].fluid && !gBlockData[mGameMap[upper_pos]].fluid)
			{
				FluidData data = {i, j};
				// 누른다.
				PushFluid(data, 0);
			}
		}
	}
	*/
}

bool Scene_Game::MoveFluid(const Position& now_fluid, const int dir)
{
	int now_pos = now_fluid.x+now_fluid.y*iMapWidth;
	Position new_fluid(now_fluid.x, now_fluid.y);
	int next_pos = 0;
	switch (dir)
	{
	case 1:
		new_fluid.y = new_fluid.y+1;
		break;
	case 2:
		new_fluid.x = (new_fluid.x-1+iMapWidth)%iMapWidth;
		break;
	case 3:
		new_fluid.x = (new_fluid.x+1)%iMapWidth;
		break;
	default:
		return false;
	}

	next_pos = new_fluid.x+new_fluid.y*iMapWidth;

	// 비어있다면 이동한다.
	if (mGameMap[next_pos] == BLOCK_AIR)
	{
		mGameMap[next_pos] = mGameMap[now_pos];
		mGameMap[now_pos] = BLOCK_AIR;
		return true;
	}
	// 다른 종류라면 적당히 처리한다.
	if (mGameMap[now_pos] != mGameMap[next_pos])
	{
		switch (mGameMap[now_pos])
		{
		case BLOCK_WATER:
			if (mGameMap[next_pos] == BLOCK_OIL)
			{
				mGameMap[next_pos] = mGameMap[now_pos];
				mGameMap[now_pos] = BLOCK_OIL;
				return true;
			}
			else if (mGameMap[next_pos] == BLOCK_MAGMA)
			{
				mGameMap[next_pos] = BLOCK_OBSIDIAN;
				// 연기
				for (int i=0; i<5; i++)
				{
					AddObject(new Smoke(0
						, new_fluid.x*BLOCK_SIZE+BLOCK_SIZE/2+(float)Random::rand.NextDouble()*10-5
						, new_fluid.y*BLOCK_SIZE+BLOCK_SIZE/2+(float)Random::rand.NextDouble()*10-5));
				}
				return true;
			}
			break;
		case BLOCK_OIL:
			if (mGameMap[next_pos] == BLOCK_MAGMA)
			{
				mGameMap[now_pos] = BLOCK_AIR;
				return true;
			}
			break;
		case BLOCK_MAGMA:
			if (mGameMap[next_pos] == BLOCK_OIL || mGameMap[next_pos] == BLOCK_COAL)
			{
				mGameMap[next_pos] = mGameMap[now_pos];
				mGameMap[now_pos] = BLOCK_AIR;
				return true;
			}
			else if (mGameMap[next_pos] == BLOCK_WATER)
			{
				mGameMap[now_pos] = BLOCK_OBSIDIAN;
				// 연기
				for (int i=0; i<5; i++)
				{
					AddObject(new Smoke(0
						, now_fluid.x*BLOCK_SIZE+BLOCK_SIZE/2+(float)Random::rand.NextDouble()*10-5
						, now_fluid.y*BLOCK_SIZE+BLOCK_SIZE/2+(float)Random::rand.NextDouble()*10-5));
				}
				return true;
			}
			break;
		}
	}
	return false;
}

int Scene_Game::PushedDir(const Position& now_fluid, const int dir)
{
	int now_pos = now_fluid.x+now_fluid.y*iMapWidth;
	if (mGameMap[now_pos-iMapWidth] == mGameMap[now_pos])
		return 1;

	Position data;
	int new_pos;
	data.y = now_fluid.y;
	if (dir == 0 || dir == 3)
	{
		data.x = (now_fluid.x-1+iMapWidth)%iMapWidth;
		new_pos = data.x+data.y*iMapWidth;
		if (mGameMap[new_pos] == mGameMap[now_pos])
		{
			int dir = PushedDir(data, 3);
			if (dir == 1 || dir == 3) return 3;
		}
	}
	if (dir == 0 || dir == 2)
	{
		data.x = (now_fluid.x+1)%iMapWidth;
		new_pos = data.x+data.y*iMapWidth;
		if (mGameMap[new_pos] == mGameMap[now_pos])
		{
			int dir = PushedDir(data, 2);
			if (dir == 1 || dir == 2) return 2;
		}
	}
	return 0;
}


void Scene_Game::Drill(const int block_x, const int block_y)
{
	int block_type = mGameMap[block_x+block_y*iMapWidth];
	if (bBreakable[block_type] && gBlockData[block_type].durability > 0)
	{
		int dmg = pPlayer->Drill();
		bDrawCracking = !gBlockData[block_type].fluid;
		iDrillDmgLev = dmg*4/gBlockData[block_type].durability;
		if (gBlockData[block_type].durability < dmg)
		{
			mGameMap[block_x+block_y*iMapWidth] = BLOCK_AIR;
			pPlayer->iDrillDamage = 0;
			if (!gBlockData[block_type].fluid)
			{
				fShake = 2;
				SEManager::GetInstance().PlaySE("crash");
				// 라이트맵 수정
				Light(block_x, block_y);
				// 연기
				if (!gBlockData[block_type].fluid)
				{
					for (int i=0; i<5; i++)
					{
						AddObject(new Smoke(0
							, block_x*BLOCK_SIZE+BLOCK_SIZE/2+(float)Random::rand.NextDouble()*10-5
							, block_y*BLOCK_SIZE+BLOCK_SIZE/2+(float)Random::rand.NextDouble()*10-5));
					}
				}
			}
			else
			{
				fShake = 0.5f;
				SEManager::GetInstance().PlaySE("fluid");
			}
			// 파티클
			for (int i=0; i<gBlockData[block_type].particle_num; i++)
				MakeParticle(block_type, (float)block_x*BLOCK_SIZE, (float)block_y*BLOCK_SIZE);
		}
	}
	else
		pPlayer->iDrillDamage = 0;
}

bool Scene_Game::CheckBuildable(const int type, const int x, const int y, const bool check_cost)
{
	if (type < 0) return false;
	const BuildingData& data = gBuildingData[type];
	// 자원부터 확인
	if (check_cost && (mBaseCamp.iFuel < data.fuel || mBaseCamp.iMineral < data.mineral))
		return false;

// 	bool ok = true;
	for (int i=0; i<data.width; i++)
	{
		int lx = (x+i+iMapWidth)%iMapWidth;
		// 공중 확인
		for (int j=0; j<data.height; j++)
		{
			int block = mGameMap[lx+(y-j)*iMapWidth];
			if (block != BLOCK_AIR
				&& !gBlockData[block].fluid
				|| block == BLOCK_MAGMA)
			{
				return false;
			}
		}
// 		if (!ok) break;
		if (data.hover) break;
		// hover가 아니라면 바닥 확인
		int floor = mGameMap[lx+(y+1)*iMapWidth];
		if (floor == BLOCK_AIR || floor == BLOCK_BEDROCK || gBlockData[floor].fluid)
		{
			return false;
		}
	}
	// 다른 건물들과 겹치는지 확인
	for each (Building building in mBuildings)
	{
		// 타입, 좌표가 똑같은 경우 자기 자신... 단 check_cost가 false일 경우만
		if (!check_cost && building.type == type && building.x == x && building.y == y) continue;
		BuildingData otherdata = gBuildingData[building.type];

		// 걸칠 일이 없는 y좌표부터 계산
		if (building.y<y-(data.height-1) || building.y-(otherdata.height-1)>y) continue;
		// x좌표... 걸치는 경우는 따로 빼서 처리해준다. 걸치지 않는 경우만 적당히
		if (x>(x+data.width-1)%iMapWidth || building.x>(building.x+otherdata.width-1)%iMapWidth)
		{
			int new_x = (x+iMapWidth/2)%iMapWidth;
			int new_other_x = (building.x+iMapWidth/2)%iMapWidth;
			// 옆으로 밀었는데 다른 하나가 겹치면 둘은 만날 리가 없을 정도로 멀리 떨어진 것
			if (new_x>(new_x+data.width)%iMapWidth || new_other_x>(new_other_x+otherdata.width)%iMapWidth) continue;
			// 겹치지 않으면 비교
			else if (new_other_x<=new_x+(data.width-1) && new_other_x-(otherdata.width-1)>=new_x)
			{
				return false;
			}
		}
		// 걸치지 않는 경우... 이땐 단순히 비교.
		else if (building.x<=x+(data.width-1) && building.x+(otherdata.width-1)>=x)
		{
			return false;
		}
	}
	return true;
}

void Scene_Game::Build()
{
	if (!CheckBuildable(iSelectedBuilding, mBuildingPos.x, mBuildingPos.y)) return;
	const BuildingData& data = gBuildingData[iSelectedBuilding];

	Building building = {iSelectedBuilding, mBuildingPos.x, mBuildingPos.y, 0};
	mBuildings.push_back(building);

	mBaseCamp.iFuel -= data.fuel;
	mBaseCamp.iMineral -= data.mineral;

	SEManager::GetInstance().PlaySE("build");

	// 연기 효과
	for (int i=0; i<data.width*BLOCK_SIZE; i+=4)
	{
		int x = (mBuildingPos.x*BLOCK_SIZE+i+iMapWidthPixel)%(iMapWidthPixel);
		AddObject(new Smoke(0, (float)x, (float)(mBuildingPos.y+1)*BLOCK_SIZE));
	}

	if (iSelectedBuilding == BUILDING_LIGHT)
	{
		int bx, by, lx, ly, w = 21, ww = w*w, hw = w/2, hwhw = hw*hw;
		for (int i=0; i<ww; i++)
		{
			bx = i%w-hw;
			by = i/w-hw;
			lx = (mBuildingPos.x+bx+iMapWidth)%iMapWidth;
			ly = mBuildingPos.y+by;
			if (ly < 0 || ly >= iMapHeight) continue;
			mLightMap[lx+ly*iMapWidth].light += max(0, (hwhw-((bx*bx)+(by*by)))*2);
		}
	}

	fShake = 1;
	iSelectedBuilding = -1;
	if (iHelpStep == 7) mGameUI.HideHelp();
}

void Scene_Game::Light(const int x, const int y)
{
	std::queue<Position> q;
	Position start(x, y);
	q.push(start);
	while (!q.empty())
	{
		Position now_pos = q.front(); q.pop();
		// light 처리
		int lx, ly;
		for (int i=0; i<3*3; i++)
		{
			lx = (now_pos.x+i%3-1+iMapWidth)%iMapWidth;
			ly = now_pos.y+i/3-1;
			if (ly < 0 || ly >= iMapHeight) continue;
			if (mLightMap[lx+ly*iMapWidth].lighted
				|| mLightMap[lx+ly*iMapWidth].base_light>=64) continue;
			int add_light = min(64, mLightMap[lx+ly*iMapWidth].base_light+((4-abs(i%3-1))+(4-abs(i%3-1)))*8)
				- mLightMap[lx+ly*iMapWidth].base_light;
			mLightMap[lx+ly*iMapWidth].base_light += add_light;
			mLightMap[lx+ly*iMapWidth].light += add_light;
		}
		// lighted 마크
		mLightMap[now_pos.x+now_pos.y*iMapWidth].lighted = true;
		// 주변으로 이동
		static const Position delta[4] = {Position(0,-1), Position(0,1), Position(-1,0), Position(1,0)};
		Position child;
		for (int i=0; i<4; i++)
		{
			child.x = (now_pos.x+delta[i].x+iMapWidth)%iMapWidth;
			child.y = now_pos.y+delta[i].y;
			const int pos = child.x+child.y*iMapWidth;
			if ((gBlockData[mGameMap[pos]].fluid || mGameMap[pos] == BLOCK_AIR)
				&& !mLightMap[pos].lighted)
				q.push(child);
		}
	}
}


void Scene_Game::SetPlayerPos(const int x, const int y)
{
	pPlayer->vPos = D3DXVECTOR3((float)(x*BLOCK_SIZE+BLOCK_SIZE/2), (float)(y*BLOCK_SIZE), 0);
	vCamera = vCameraDest = pPlayer->vPos-D3DXVECTOR3(200, 150, 0);
}

void Scene_Game::MakeParticle(const int type, const float x, const float y)
{
	BlockParticle *particle = new BlockParticle(
		type
		, (float)(x+(Random::rand.Next()%BLOCK_SIZE))
		, (float)(y+(Random::rand.Next()%BLOCK_SIZE))
		);
	particle->SetVelocity((float)(Random::rand.NextDouble()-0.5)*3, (float)-Random::rand.NextDouble()*2);
	if (mSpecial == MODE_LESS_GRAVITY)
		particle->fGravity *= 0.5f;
	else if (mSpecial == MODE_HARDCORE)
		particle->fGravity *= 1.5f;
	AddObject(particle);
}

int Scene_Game::GetTileType(const int x, const int y)
{
	if (y < 0 || y >= iMapHeight) return BLOCK_AIR;
	return mGameMap[(x+iMapWidth)%iMapWidth+y*iMapWidth];
}


void Scene_Game::NormalEnding()
{
	if (mCutScene != CUTSCENE_NULL) return;
	mCutScene = CUTSCENE_NORMAL_ENDING;
	bInputLock = true;
	bGameOver = true;
	bEnding = true;
}

void Scene_Game::StartUnobtainiumBossBattle()
{
	if (mCutScene != CUTSCENE_NULL) return;
	mCutScene = CUTSCENE_UNOBTAINIUM_BOSS;
	bInputLock = true;
}

void Scene_Game::StartCookieBossBattle()
{
	if (mCutScene != CUTSCENE_NULL) return;
	mCutScene = CUTSCENE_COOKIE_BOSS;
	bInputLock = true;
}


void Scene_Game::MakePauseTexture()
{
	PP_MonoColor monotone(0xffffffff);
	SAFE_DELETE(txPaused);
	txPaused = JMain::GetInstance().GetBackBufferTexture();
	monotone.ProcessTexture(txPaused);
}


void Scene_Game::CutSceneEnd()
{
	mLastCutScene = mCutScene;
	mCutScene = CUTSCENE_NULL;
	iCutSceneTimer = 0;
}

void Scene_Game::NormalEndingCutScene()
{
	iCutSceneTimer++;

	if (iCutSceneTimer == 1)
	{
		BGMManager::GetInstance().FadeOutAllBGMsAndStop(2000);
	}
	if (iCutSceneTimer < 60)
		fFade = iCutSceneTimer/60.f;
	else if (iCutSceneTimer == 60)
	{
		fFade = 1;
		pPlayer->vPos = mBaseCamp.vBasePos;
		pPlayer->vPos.y -= BLOCK_SIZE;
		pPlayer->sTextureId = "ship";
		pPlayer->fFrame = 0;
		pPlayer->fFrame_rate = 0;
		pPlayer->fGravity = 0;
		pPlayer->SetVelocity(0, 0);
		pPlayer->bDrilling = false;
		pPlayer->iHealth = pPlayer->iHealthMax;
		pPlayer->iAir = pPlayer->iAirMax;
		pPlayer->iAirpackBrokenTimer = 0;
	}
	else if (iCutSceneTimer > 180 && iCutSceneTimer <= 240)
		fFade = (240 - iCutSceneTimer) / 60.f;
	else if (iCutSceneTimer == 480)
		BGMManager::GetInstance().PlayBGM("rocket");
	else if (iCutSceneTimer > 480 && iCutSceneTimer < 500)
		pPlayer->AddVelocity(0, -0.5f);
	else if (iCutSceneTimer == 600)
	{
		BGMManager::GetInstance().FadeOutBGMAndStop("rocket", 1000);
		pPlayer->SetVelocity(0, 0);
		TextureManager::GetInstance().GetBlank()->color = 0xff000000;
	}
	else if (iCutSceneTimer > 660 && iCutSceneTimer <= 720)
	{
		fFade = (iCutSceneTimer-660)/60.f;
	}
	if (iCutSceneTimer >= 720)
	{
		bTheEnd = true;
		int data = 1;
		GlobalDataManager::GetInstance().SetData("cleared", &data);
		CutSceneEnd();
		SetEndingString();
	}

	if (iCutSceneTimer > 300 && iCutSceneTimer < 600)
	{
		D3DXVECTOR3 v = pPlayer->vPos;
		v.x += (float)Random::rand.NextDouble()*20-10;
		v.y += 16;
		AddObject(new Smoke(0, v.x, v.y));
		fShake = (float)((iCutSceneTimer < 480)? 2:1);
		if (iCutSceneTimer < 480 && iCutSceneTimer % 15 == 1) SEManager::GetInstance().PlaySE("rocket_ready");
	}
}

void Scene_Game::UnobtainiumEndingCutScene()
{
	iCutSceneTimer++;

	if (iCutSceneTimer == 1)
	{
		BGMManager::GetInstance().FadeOutAllBGMsAndStop(2000);
	}
	if (iCutSceneTimer <= 100)
		fFade = iCutSceneTimer/100.f;
	else if (iCutSceneTimer == 220)
	{
		pPlayer->vPos = mBaseCamp.vBasePos;
		pPlayer->vPos.y -= BLOCK_SIZE;
		pPlayer->sTextureId = "ship";
		pPlayer->fFrame = 1;
		pPlayer->fFrame_rate = 0;
		pPlayer->fGravity = 0;
		pPlayer->SetVelocity(0, 0);
		pPlayer->bDrilling = false;
		pPlayer->iHealth = pPlayer->iHealthMax;
		pPlayer->iAir = pPlayer->iAirMax;
		pPlayer->iAirpackBrokenTimer = 0;
	}
	else if (iCutSceneTimer > 220 && iCutSceneTimer <= 280)
		fFade = (280-iCutSceneTimer)/60.f;
	else if (iCutSceneTimer == 520)
		BGMManager::GetInstance().PlayBGM("rocket");
	else if (iCutSceneTimer > 520 && iCutSceneTimer < 540)
		pPlayer->AddVelocity(0, -0.5f);
	else if (iCutSceneTimer == 640)
	{
		BGMManager::GetInstance().FadeOutBGMAndStop("rocket", 1000);
		pPlayer->SetVelocity(0, 0);
		TextureManager::GetInstance().GetBlank()->color = 0xff000000;
	}
	else if (iCutSceneTimer > 700 && iCutSceneTimer <= 760)
	{
		fFade = (iCutSceneTimer-700)/60.f;
	}
	if (iCutSceneTimer >= 760)
	{
		bTheEnd = true;
		int data = 1;
		GlobalDataManager::GetInstance().SetData("cleared", &data);
		CutSceneEnd();
		SetEndingString();
	}

	if (iCutSceneTimer > 340 && iCutSceneTimer < 640)
	{
		D3DXVECTOR3 v = pPlayer->vPos;
		v.x += (float)Random::rand.NextDouble()*20-10;
		v.y += 16;
		AddObject(new Smoke(0, v.x, v.y));
		fShake = (float)((iCutSceneTimer < 520)? 2:1);
		if (iCutSceneTimer < 520 && iCutSceneTimer % 15 == 1) SEManager::GetInstance().PlaySE("rocket_ready");
	}
}

void Scene_Game::CookieEndingCutScene()
{
	iCutSceneTimer++;

	if (iCutSceneTimer == 1)
	{
		BGMManager::GetInstance().FadeOutAllBGMsAndStop(2000);
		SEManager::GetInstance().PlaySE("noise");
	}
	if (iCutSceneTimer <= 100)
		fFade = iCutSceneTimer/100.f;
	else if (iCutSceneTimer == 220)
	{
		pPlayer->fGravity = 0;
		pPlayer->SetVelocity(0, 0);
		pPlayer->bDrilling = false;
		pPlayer->iHealth = pPlayer->iHealthMax;
		pPlayer->iAir = pPlayer->iAirMax;
		pPlayer->iAirpackBrokenTimer = 0;
	}
	else if (iCutSceneTimer > 220 && iCutSceneTimer <= 320)
	{
		float c = (320-iCutSceneTimer)/100.f;
		TextureManager::GetInstance().GetBlank()->color = D3DXCOLOR(c,c,c,1);
	}
	if (iCutSceneTimer >= 440)
	{
		bTheEnd = true;
		int data = 1;
		GlobalDataManager::GetInstance().SetData("cleared", &data);
		CutSceneEnd();
		SetEndingString();
	}
}

void Scene_Game::UnobtainiumBossCutScene()
{
	iCutSceneTimer++;

	if (iCutSceneTimer == 1)
	{
		BGMManager::GetInstance().FadeOutBGMAndStop("small_planet", 2000);
		BGMManager::GetInstance().FadeInBGM("trembling", 1000);
	}
	else if (iCutSceneTimer == 100)
	{
		SEManager::GetInstance().PlaySE("noise");
	}		

	if (iCutSceneTimer < 130)
		vCameraDelta = D3DXVECTOR3(0, -iCutSceneTimer*0.75f, 0);
	if (iCutSceneTimer <= 200)
		fShake = 2;
	if (iCutSceneTimer >= 100 && iCutSceneTimer < 130 )
		fFade = (iCutSceneTimer-100)/30.f;
	else if (iCutSceneTimer == 130)
	{
		fFade = 1;
		pBoss = new Boss_Unobtainium(mBaseCamp.vBasePos, mBaseCamp.iX, mBaseCamp.iY);
		bBoss = true;
		vCameraDelta = D3DXVECTOR3(0,0,0);
		// 주변 땅을 변경시킨다.
		for (int i = 0; i<iMapWidth; i++)
		{
			for (int j=0; j<iMapHeight; j++)
			{
				if (j <= mBaseCamp.iY)
				{
					mGameMap[i+j*iMapWidth] = BLOCK_AIR;
				}
				else
				{
					if (mGameMap[i+j*iMapWidth] != BLOCK_BEDROCK)
						mGameMap[i+j*iMapWidth] = BLOCK_UNOBTAINIUM_PLATFORM;
				}
			}
		}
		AddObject(pBoss);
	}
	else if (iCutSceneTimer > 160 && iCutSceneTimer <= 220)
		fFade = (220-iCutSceneTimer)/60.f;
	else if (iCutSceneTimer > 220 && iCutSceneTimer < 280)
		fShake = (260-iCutSceneTimer)/30.f;
	else if (iCutSceneTimer == 280)
	{
		BGMManager::GetInstance().FadeOutBGMAndStop("trembling", 2000);
		bInputLock = false;
	}
	else if (iCutSceneTimer == 340)
	{
		BGMManager::GetInstance().PlayBGM("unobtainium");
		mGameUI.pBoss = pBoss;
		fShake = 4;
		CutSceneEnd();
	}
}

void Scene_Game::CookieBossCutScene()
{
	iCutSceneTimer++;

	if (iCutSceneTimer == 1)
	{
		BGMManager::GetInstance().FadeOutBGMAndStop("small_planet", 2000);
		BGMManager::GetInstance().FadeInBGM("trembling", 1000);
	}
	else if (iCutSceneTimer == 100)
	{
		SEManager::GetInstance().PlaySE("noise");
	}
	else if (iCutSceneTimer == 160)
		BGMManager::GetInstance().FadeOutBGMAndStop("trembling", 1000);

	if (iCutSceneTimer < 130)
		vCameraDelta = D3DXVECTOR3(0, -iCutSceneTimer*0.75f, 0);
	if (iCutSceneTimer <= 200)
		fShake = 2;
	if (iCutSceneTimer >= 100 && iCutSceneTimer < 130 )
		fFade = (iCutSceneTimer-100)/30.f;
	else if (iCutSceneTimer == 130)
	{
		fFade = 1;
		pBoss = new Boss_Cookie(mBaseCamp.vBasePos+D3DXVECTOR3(0, -12.f*BLOCK_SIZE, 0), mBaseCamp.iX, mBaseCamp.iY-12);
		bBoss = true;
		bGrandma = true;
		vCameraDelta = D3DXVECTOR3(0,0,0);
		// 땅을 변경시킨다.
		for (int i = 0; i<iMapWidth; i++)
		{
			for (int j=0; j<iMapHeight; j++)
			{
				if (j <= mBaseCamp.iY)
				{
					mGameMap[i+j*iMapWidth] = BLOCK_AIR;
				}
				else
				{
					mGameMap[i+j*iMapWidth] = BLOCK_COOKIE_PLATFORM;
				}
			}
		}
		AddObject(pBoss);
	}
	else if (iCutSceneTimer > 160 && iCutSceneTimer <= 220)
		fFade = (220-iCutSceneTimer)/60.f;
	else if (iCutSceneTimer > 220 && iCutSceneTimer < 280)
		fShake = (260-iCutSceneTimer)/30.f;
	else if (iCutSceneTimer == 280)
		bInputLock = false;
	else if (iCutSceneTimer == 340)
	{
		BGMManager::GetInstance().PlayBGM("cookieclicker");
		mGameUI.pBoss = pBoss;
		fShake = 4;
		CutSceneEnd();
	}
}


void Scene_Game::SetEndingString()
{
	/*
		Mode\Ending	NORMAL	UNOBTAINIUM	COOKIE
		NORMAL
		HARDCORE
	*/

	static char* ending_strings[] = {
		"Thank you for playing!", "<TIP> You can see current game mode in pause menu.", "You are a Cookie Clicker!"
		, "<TIP> Press number keys to select special modes in pause menu.", "<TIP> Shift+F, Shift+M, Shift+L, Shift+B, Shift+C", "YOU ARE THE COOKIE SLAYER!!"};
	int ind = 0;

	switch (mLastCutScene)
	{
	case CUTSCENE_NORMAL_ENDING:
		ind += 0;
		break;
	case CUTSCENE_UNOBTAINIUM_ENDING:
		ind += 1;
		break;
	case CUTSCENE_COOKIE_ENDING:
		ind += 2;
		break;
	}

	if (mSpecial == MODE_NORMAL) ind += 0;
	else if (mSpecial == MODE_HARDCORE) ind += 3;
	else ind = 0;

	mEndingString = ending_strings[ind];
}
