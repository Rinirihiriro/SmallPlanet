#include "BaseCamp.h"
#include "Scene_Game.h"
#include "TextureManager.h"
#include "BGMManager.h"

BaseCamp::BaseCamp()
	:pSceneGame(NULL)
{
	Reset();
}

void BaseCamp::Reset()
{
	iX = iY = iFuel = iMineral = iPower = iShowFuel = iShowMineral = iShowPower
		= iTimer = iResearchNeedResource = 0;
	vBasePos.x = vBasePos.y = vBasePos.z = 0;
	bNearBase = bFuelProcessing = bCargoEmpty = false;
	fEnterMessageScale = 0;
	ZeroMemory(iBaseCargo, sizeof(iBaseCargo));
}

void BaseCamp::SetSceneGame(Scene_Game* scene_game)
{
	pSceneGame = scene_game;
}

void BaseCamp::Update()
{
	bNearBase = !pSceneGame->bGameOver && !pSceneGame->pBoss && !pSceneGame->bEnding &&
		((abs(iX-pSceneGame->mPlayerPos.x) < 5
			|| abs((iX+pSceneGame->iMapWidth/2)%pSceneGame->iMapWidth-(pSceneGame->mPlayerPos.x+pSceneGame->iMapWidth/2)%pSceneGame->iMapWidth) < 5) // 건물이 경계에 걸쳤을 경우
			&& abs(iY-pSceneGame->mPlayerPos.y) < 4 && iY >= pSceneGame->mPlayerPos.y);

	if (iFuel != iShowFuel)
		iShowFuel += (int)ceil(abs(iFuel-iShowFuel)*0.25f)*((iFuel>iShowFuel)?1:-1);
	if (iMineral != iShowMineral)
		iShowMineral += (int)ceil(abs(iMineral-iShowMineral)*0.25f)*((iMineral>iShowMineral)?1:-1);
	if (iPower != iShowPower)
		iShowPower += (int)ceil(abs(iPower-iShowPower)*0.25f)*((iPower>iShowPower)?1:-1);

	if (bNearBase)
	{
		if (pSceneGame->iHelpStep >= 4)
			fEnterMessageScale += (1-fEnterMessageScale)*0.25f;

		pSceneGame->pPlayer->iAir = min(pSceneGame->pPlayer->iAir+64, pSceneGame->pPlayer->iAirMax);
		pSceneGame->pPlayer->iHealth = min(pSceneGame->pPlayer->iHealthMax, pSceneGame->pPlayer->iHealth+16);
		// 자동 충전
		{
			int charge_fuel = min(pSceneGame->pPlayer->iJetFuelMax-pSceneGame->pPlayer->iJetFuel, 16);
			if (charge_fuel > 0)
			{
				int need_fule = (charge_fuel+4)/8;
				if (iFuel >= need_fule)
				{
					pSceneGame->pPlayer->iJetFuel += charge_fuel;
					iFuel -= need_fule;
				}
				else
				{
					pSceneGame->pPlayer->iJetFuel += iFuel*8;
					iFuel = 0;
				}
			}
		}
		// 자동 자원 넣기
		if (pSceneGame->pPlayer->iCargo > 0)
		{
			int total = 0;
			for (int i=0; i<BLOCK_TYPE_NUM; i++)
			{
				if (pSceneGame->pPlayer->iCargoDetail[i] > 0)
				{
					int move_amount = min(pSceneGame->pPlayer->iCargoDetail[i], 4-total);
					pSceneGame->pPlayer->iCargoDetail[i] -= move_amount;
					pSceneGame->pPlayer->iCargo -= move_amount;
					total += move_amount;
					if (gBlockData[i].fuel_rate > 0)
						iBaseCargo[i] += move_amount;
					iMineral += move_amount*gBlockData[i].mineral_rate;
					if (i == BLOCK_UNOBTAINIUM)
					if (move_amount == 4) break;
				}
			}
		}
		if (iFuel >= gGoal)
		{
			pSceneGame->NormalEnding();
		}

		// 산소 누출 수리
		if (pSceneGame->pPlayer->iAirpackBrokenTimer > 0)
			pSceneGame->pPlayer->iAirpackBrokenTimer -= 1;
	}
	else
		fEnterMessageScale -= fEnterMessageScale*0.25f;

	iTimer -= 1;
	if (iTimer <= 0)
	{
		// make fuel
		if (bFuelProcessing) MakeFuel();
		// make power
		iFuel = max(0, iFuel+min(0,iPower));
		iTimer = 60;
	}

	iPower = 0;
	iResearchNeedResource = 0;
	bFuelProcessing = false;
	bCargoEmpty = true;
	for (int i=0; i<BLOCK_TYPE_NUM; i++)
	{
		if (iBaseCargo[i] > 0)
		{
			if (!pSceneGame->bBoss && !pSceneGame->bEnding && pSceneGame->bUsable[i])
			{
				bFuelProcessing = true;
				iPower -= gBlockData[i].processing_power;
			}
			else
			{
				if (iResearchNeedResource == 0)
					iResearchNeedResource = i;
				if (!pSceneGame->pBoss)
				{
					if (i == BLOCK_UNOBTAINIUM) pSceneGame->StartUnobtainiumBossBattle();
					else if (i == BLOCK_COOKIE) pSceneGame->StartCookieBossBattle();
				}
			}
			bCargoEmpty = false;
		}
	}

	if (bFuelProcessing) BGMManager::GetInstance().PlayBGM("machine"); 
	else BGMManager::GetInstance().FadeOutBGMAndStop("machine", 500);
}

void BaseCamp::Draw(const float camera_x)
{
	float oldx = vBasePos.x;
	if (camera_x < 0 && vBasePos.x >= pSceneGame->iMapWidthPixel+camera_x-128)
		vBasePos.x -= pSceneGame->iMapWidthPixel;
	else if (camera_x+400 > pSceneGame->iMapWidthPixel && vBasePos.x <= camera_x+400-pSceneGame->iMapWidthPixel+128)
		vBasePos.x += pSceneGame->iMapWidthPixel;
	TextureManager::GetInstance().GetTexture("base")->Draw(vBasePos);
	vBasePos.x = oldx;
}


void BaseCamp::SetBasePos(const int x, const int y)
{
	iX = x;
	iY = y;
	vBasePos = D3DXVECTOR3((float)(iX*BLOCK_SIZE+BLOCK_SIZE/2), (float)((iY+1)*BLOCK_SIZE), 0);
}


void BaseCamp::MakeFuel()
{
	for (int i=0; i<BLOCK_TYPE_NUM; i++)
	{
		if (iBaseCargo[i] > 0 && pSceneGame->bUsable[i])
		{
			int make_num = min(gBlockData[i].processing_speed, iBaseCargo[i]);
			iBaseCargo[i] -= make_num;
			iFuel += make_num * gBlockData[i].fuel_rate;
		}
	}
}
