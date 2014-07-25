#include "GameHUD.h"
#include "Scene_Game.h"
#include "JInput.h"
#include "TextureManager.h"
#include "Boss.h"

GameHUD::GameHUD()
	:pSceneGame(NULL)
{
	Reset();
}


void GameHUD::SetSceneGame(Scene_Game* scene_game)
{
	pSceneGame = scene_game;
}

void GameHUD::Reset()
{
	ZeroMemory(fUi, sizeof(fUi));
	ZeroMemory(fUiDest, sizeof(fUiDest));
	ZeroMemory(fGauge, sizeof(fGauge));
	ZeroMemory(fGaugeDest, sizeof(fGaugeDest));
	ZeroMemory(iUIShowCounter, sizeof(iUIShowCounter));
	ZeroMemory(iNewResources, sizeof(iNewResources));
	fNewResourceBox = fResearchBox = fHelpAlpha = fHelpAlphaDest = fFullBox = fAirLeakBox = 0;
	iSelectedItem = -1;
	iNewResourceTimer = iProcessingFrame = 0;
	iHelpID[0] = iHelpID[1] = -1;
	bBuild = false;
	iHelpTimer = 0;
	pBoss = NULL;
}

void GameHUD::InputProc()
{
	if (JInput::IsPressed('Q') || JInput::controller.IsButtonPressed(Controller::LTRIGGER, 0))
	{
		ShowAll();
		if (pSceneGame->iHelpStep == 3) pSceneGame->bHelpKeyInput[5] = true;
	}

	if ((JInput::IsTriggered('C') || JInput::controller.IsButtonTriggered(Controller::X, 0)))
	{
		if (!bBuild)
		{
			ZeroMemory(fSelectItemAlpha, sizeof(fSelectItemAlpha));
			iSelectedItem = pSceneGame->iSelectedBuilding-1;
			if (!pSceneGame->bBuildable[BUILDING_LIGHT])
				iSelectedItem = -1;
			else if (iSelectedItem < 0)
				iSelectedItem = 0;
			pSceneGame->iSelectedBuilding = iSelectedItem+1;
			fUiDest[UI_BUILDING] = 1;
			bBuild = pSceneGame->bBuild = true;
		}
		else
		{
			fUiDest[UI_BUILDING] = 0;
			bBuild = false;
			if (!pSceneGame->bBuildable[BUILDING_LIGHT])
				pSceneGame->bBuild = false;
		}
		return;
	}

	if (!bBuild) return;

	if (pSceneGame->bBuildable[BUILDING_LIGHT])
	{
		if (JInput::IsTriggered(VK_LEFT) || JInput::controller.IsStickTriggered(Controller::LSTICK, Controller::LEFT, 0))
		{
			do 
			{
				iSelectedItem = (iSelectedItem-1+4)%4;
				pSceneGame->iSelectedBuilding = iSelectedItem+1;
			} while (!pSceneGame->bBuildable[iSelectedItem+1]);
		}
		if (JInput::IsTriggered(VK_RIGHT) || JInput::controller.IsStickTriggered(Controller::LSTICK, Controller::RIGHT, 0))
		{
			do 
			{
				iSelectedItem = (iSelectedItem+1)%4;
				pSceneGame->iSelectedBuilding = iSelectedItem+1;
			} while (!pSceneGame->bBuildable[iSelectedItem+1]);
		}
	}
	if (JInput::IsTriggered('Z') || JInput::controller.IsButtonTriggered(Controller::A, 0)
		|| JInput::IsTriggered('X') || JInput::controller.IsButtonTriggered(Controller::B, 0))
	{
		pSceneGame->iSelectedBuilding = -1;
		fUiDest[UI_BUILDING] = 0;
		bBuild = false;
	}
	return;
}

void GameHUD::Update()
{
	iUIShowCounter[UI_RESOURCES] = 2;
	if (bBuild)
	{
		for (int i=0; i<4; i++)
		{
			if (i == iSelectedItem)
				fSelectItemAlpha[i] += (1-fSelectItemAlpha[i])*0.25f;
			else
				fSelectItemAlpha[i] -= fSelectItemAlpha[i]*0.25f;
		}
		iUIShowCounter[UI_BUILDING] = 2;
		iUIShowCounter[UI_HEALTH] = iUIShowCounter[UI_OXYGEN] = 0;
	}
	if (pBoss)
	{
		iUIShowCounter[UI_BOSS_HP] = 2;
		iUIShowCounter[UI_GOAL] = iUIShowCounter[UI_RESOURCES] = 0;
	}
// 	else
// 	{
// 
// 	}

	for (int i=0; i<UI_NUM; i++)
	{
		iUIShowCounter[i] = max(0, iUIShowCounter[i]-1);
		fUiDest[i] = (iUIShowCounter[i]>0)? 1.f:0.f;
		fUi[i] += (fUiDest[i]-fUi[i])*0.25f;
	}

	fGaugeDest[UI_GOAL] = max(0, min(14, 14.f*pSceneGame->mBaseCamp.iFuel/Scene_Game::GOAL_FUEL));
	fGaugeDest[UI_CARGO] = -14.f*pSceneGame->pPlayer->iCargo/pSceneGame->pPlayer->iCargoMax;
	fGaugeDest[UI_JETPACK] = -14.f*pSceneGame->pPlayer->iJetFuel/pSceneGame->pPlayer->iJetFuelMax;
	fGaugeDest[UI_HEALTH] = 10.f*pSceneGame->pPlayer->iHealth/pSceneGame->pPlayer->iHealthMax;
	fGaugeDest[UI_OXYGEN] = -10.f*pSceneGame->pPlayer->iAir/pSceneGame->pPlayer->iAirMax;
	if (pBoss)
		fGaugeDest[UI_BOSS_HP] = 18.f*pBoss->iHealth/pBoss->iMaxHealth;
	for (int i=0; i<UI_BUILDING; i++)
		fGauge[i] += (fGaugeDest[i]-fGauge[i])*0.25f;

	if (pSceneGame->mBaseCamp.bFuelProcessing)
		iProcessingFrame = (iProcessingFrame+1)%(5*10);

	iNewResourceTimer = max(0, iNewResourceTimer-1);
	if (iNewResourceTimer > 0)
		fNewResourceBox += (1-fNewResourceBox)*0.25f;
	else
		fNewResourceBox = fNewResourceBox*0.75f;

	if (iNewResources[0] > 0 && fNewResourceBox < 0.125f && iNewResourceTimer == 0)
	{
		for (int i=0; i<3; i++)
		{
			iNewResources[i] = iNewResources[i+1];
			iNewResources[i+1] = 0;
			if (iNewResources[i] == 0) break;
		}
		if (iNewResources[0]>0) iNewResourceTimer = 5*60;
	}

	if (pSceneGame->mBaseCamp.iResearchNeedResource>0)
		fResearchBox += (1-fResearchBox)*0.25f;
	else
		fResearchBox = fResearchBox*0.75f;

	fHelpAlpha += (fHelpAlphaDest-fHelpAlpha)*0.25f;
	iHelpTimer = max(0, iHelpTimer-1);
	if (iHelpID[0] >= 0 && iHelpTimer == 0 && fHelpAlphaDest > 0) fHelpAlphaDest = 0;
	if (iHelpID[0] >= 0 && fHelpAlphaDest == 0 && fHelpAlpha<0.125f)
	{
		fHelpAlpha = 0;
		iHelpID[0] = iHelpID[1];
		iHelpID[1] = -1;
		if (iHelpID[0] >= 0)
		{
			fHelpAlphaDest = 1;
			iHelpTimer = 600;
		}
		else
			iHelpTimer = 0;
	}

	iFullBoxTimer = max(0, iFullBoxTimer-1);
	if (iFullBoxTimer > 0)
		fFullBox += (1-fFullBox)*0.25f;
	else
		fFullBox *= 0.75f;

	if (pSceneGame->pPlayer->iAirpackBrokenTimer > 0)
		fAirLeakBox += (1-fAirLeakBox)*0.25f;
	else
		fAirLeakBox *= 0.75f;
}

void GameHUD::Draw()
{
	static const char* const help_tx_id[] = {"control1", "control2", "control4", "control3"};
	DrawGauge();
	DrawResource();
	DrawBuildingUI();
	if (iHelpID[0]>=0)
		TextureManager::GetInstance().GetTexture(help_tx_id[iHelpID[0]])->Draw(D3DXVECTOR3(0,0,0),0,0,0,fHelpAlpha*0.75f);
}

void GameHUD::Show(const int ui, const bool short_showing)
{
	iUIShowCounter[ui] = max((short_showing? 2:100), iUIShowCounter[ui]);
}

void GameHUD::Show(const int ui, const int frame)
{
	iUIShowCounter[ui] = max(frame, iUIShowCounter[ui]);
}


void GameHUD::ShowAll()
{
	for (int i=0; i<UI_BOSS_HP; i++)
	{
		iUIShowCounter[i] = max(2, iUIShowCounter[i]);
	}
}

void GameHUD::Hide(const int ui)
{
	iUIShowCounter[ui] = 0;
}

void GameHUD::HideAll()
{
	for (int i=0; i<UI_BOSS_HP; i++)
	{
		fUiDest[i] = 0;
	}
}


void GameHUD::ShowHelp(const int id)
{
	if (iHelpID[0] < 0)
	{
		iHelpID[0] = id;
		fHelpAlphaDest = 1;
		iHelpTimer = 720;
	}
	else
	{
		iHelpID[1] = id;
		HideHelp();
	}
}

void GameHUD::HideHelp()
{
	fHelpAlphaDest = 0;
	iHelpTimer = 0;
}

bool GameHUD::IsHelpOver()
{
	return fHelpAlphaDest == 0 && iHelpID[1] == -1;
}



void GameHUD::NotiFull()
{
	iFullBoxTimer = 10;
}


void GameHUD::NewResource(const int resource_id)
{
	int i=0;
	while (iNewResources[i] > 0) i++;
	iNewResources[i] = resource_id;
	if (i == 0)
		iNewResourceTimer = 5*60;
}


void GameHUD::DrawGauge()
{
	// 좌표들
	float health_y = 300-floor(fUi[UI_HEALTH]*16+0.5f);
	float oxygen_y = 300-floor(fUi[UI_OXYGEN]*16+0.5f);
	float goal_y = -16+floor(fUi[UI_GOAL]*16+0.5f);
	float bosshp_y = -16+floor(fUi[UI_BOSS_HP]*16+0.5f);
	float cargo_x = -16+floor(fUi[UI_CARGO]*16+0.5f);
	float jetpack_x = 400-floor(fUi[UI_JETPACK]*16+0.5f);

	// 배경
	D3DXVECTOR3 scale(1,1,1);
	scale.x = 10;
	TextureManager::GetInstance().GetTexture("gaugeback")->Draw(D3DXVECTOR3(16*2,health_y,0), &scale);
	scale.x = -10;
	TextureManager::GetInstance().GetTexture("gaugeback")->Draw(D3DXVECTOR3(16*23,oxygen_y,0), &scale);
	scale.x = 14;
	TextureManager::GetInstance().GetTexture("gaugeback")->Draw(D3DXVECTOR3(16*6,goal_y,0), &scale);
	scale.x = 18;
	TextureManager::GetInstance().GetTexture("gaugeback")->Draw(D3DXVECTOR3(16*5,bosshp_y,0), &scale);
	scale.x = 1;
	scale.y = -14;
	TextureManager::GetInstance().GetTexture("gaugeback")->Draw(D3DXVECTOR3(cargo_x,16*17,0), &scale);
	TextureManager::GetInstance().GetTexture("gaugeback")->Draw(D3DXVECTOR3(jetpack_x,16*17,0), &scale);
	scale.y = 1;

	// 체력
	scale.x = fGauge[UI_HEALTH];
	TextureManager::GetInstance().GetTexture("gauge2")->Draw(D3DXVECTOR3(16*2,health_y,0), &scale);

	// 산소
	scale.x = fGauge[UI_OXYGEN];
	TextureManager::GetInstance().GetTexture("gauge2")->Draw(D3DXVECTOR3(16*23,oxygen_y,0), &scale,0,1);
	TextureManager::GetInstance().GetTexture("airleakbox")->Draw(
		D3DXVECTOR3(352, 300-18*fAirLeakBox*fUi[UI_OXYGEN], 0),0,0,0,fAirLeakBox);

	// 목표
	scale.x = fGauge[UI_GOAL];
	TextureManager::GetInstance().GetTexture("gauge2")->Draw(D3DXVECTOR3(16*6,goal_y,0), &scale,0,2);

	// 보스 HP
	scale.x = fGauge[UI_BOSS_HP];
	TextureManager::GetInstance().GetTexture("gauge2")->Draw(D3DXVECTOR3(16*5,bosshp_y,0), &scale);
	
	scale.x = 1;
	// 보관함
	int stacked_amount = pSceneGame->pPlayer->iCargo;
	for (int i = BLOCK_TYPE_NUM-1; i >= 0; i--)
	{
		if (pSceneGame->pPlayer->iCargoDetail[i] == 0) continue;
		scale.y = fGauge[UI_CARGO]*stacked_amount/pSceneGame->pPlayer->iCargo;
		TextureManager::GetInstance().GetTexture("gauge")->Draw(D3DXVECTOR3(cargo_x,16*17,0), &scale,0,i);
		stacked_amount -= pSceneGame->pPlayer->iCargoDetail[i];

		if (fNewResourceBox > 0.125f && i==iNewResources[0])
		{
			float delta = fNewResourceBox*fUi[UI_CARGO];
			char name[32]={0,};
			D3DXVECTOR3 pos(18*delta, 271+scale.y*16, 0);
			D3DXCOLOR color(0,0,0,fNewResourceBox);
			TextureManager::GetInstance().GetTexture("newbox")->Draw(pos,0,0,0,fNewResourceBox);
			sprintf_s(name, 32, "<%s>", gBlockData[i].name);
			DrawFont(pos.x+30+25-GetFontLength(name)/2, pos.y-4, name, &color);
		}

		if (stacked_amount == 0) break;
	}
	TextureManager::GetInstance().GetTexture("fullbox")->Draw(D3DXVECTOR3(18*fFullBox*fUi[UI_CARGO], 41, 0),0,0,0,fFullBox);

	// 제트팩
	scale.y = fGauge[UI_JETPACK];
	TextureManager::GetInstance().GetTexture("gauge3")->Draw(D3DXVECTOR3(jetpack_x,16*17,0), &scale);

	TextureManager::GetInstance().GetTexture("ui_health")->Draw(D3DXVECTOR3(16,health_y,0));
	TextureManager::GetInstance().GetTexture("ui_oxygen")->Draw(D3DXVECTOR3(16*13-1,oxygen_y,0));
	TextureManager::GetInstance().GetTexture("ui_goal")->Draw(D3DXVECTOR3(16*5,goal_y,0));
	TextureManager::GetInstance().GetTexture("ui_bosshp")->Draw(D3DXVECTOR3(16*2,bosshp_y,0));
	TextureManager::GetInstance().GetTexture("ui_cargo")->Draw(D3DXVECTOR3(cargo_x,16*2,0));
	TextureManager::GetInstance().GetTexture("ui_jetpack")->Draw(D3DXVECTOR3(jetpack_x,16*2,0));	
}

void GameHUD::DrawResource()
{
	D3DXCOLOR COLOR_RED(1,0,0,1);
	float delta = -27+fUi[UI_RESOURCES]*27;
	TextureManager::GetInstance().GetTexture("ui_resources")->Draw(D3DXVECTOR3(332,delta,0));
	if (!pSceneGame->mBaseCamp.bCargoEmpty)
		TextureManager::GetInstance().GetTexture("processing")->Draw(D3DXVECTOR3(337,1+delta,0)
		,0,0,iProcessingFrame/10,0.5f);
	DrawNumber(400-3, 2+(int)delta, pSceneGame->mBaseCamp.iShowFuel, false);
	DrawNumber(400-3, 2+8+(int)delta, pSceneGame->mBaseCamp.iShowMineral, false);
	DrawNumber(400-3, 2+16+(int)delta, pSceneGame->mBaseCamp.iShowPower, true
		, (pSceneGame->mBaseCamp.iShowPower<0)?&COLOR_RED:NULL);

	if (fResearchBox>0.125f && fUi[UI_RESOURCES]>0.125f)
	{
		float delta = fResearchBox*fUi[UI_RESOURCES];
		char name[32]={0,};
		D3DXCOLOR researchbox_color(0,0,0,delta);
		D3DXVECTOR3 pos(339,28*delta,0);
		TextureManager::GetInstance().GetTexture("researchbox")->Draw(pos,0,0,0,delta);
		sprintf_s(name, 32, "<%s>", gBlockData[pSceneGame->mBaseCamp.iResearchNeedResource].name);
		DrawFont(pos.x-34+35-GetFontLength(name)/2, pos.y+17, name, &researchbox_color);
	}
}

void GameHUD::DrawBuildingUI()
{
	Texture* building_tx = TextureManager::GetInstance().GetTexture("buildings_small");
	float y_delta = 42-floor(fUi[UI_BUILDING]*42+0.5f);
	TextureManager::GetInstance().GetTexture("ui_building")->Draw(
		D3DXVECTOR3(114,258+y_delta,0));
	for (int i=0; i<4; i++)
	{
		D3DXVECTOR3 pos(120+(float)i*40, 264+y_delta, 0);
		if (pSceneGame->bBuildable[i+1])
		{
			building_tx->Draw(pos, 0, 0, (i+1)*2);
			building_tx->Draw(pos, 0, 0, (i+1)*2+1, fSelectItemAlpha[i]);
		}
		else
			building_tx->Draw(pos);
	}
	if (pSceneGame->bBuildable[iSelectedItem+1])
	{
		BuildingData building_data = gBuildingData[iSelectedItem+1];
		DrawNumber(165, 288+(int)y_delta, building_data.fuel, false);
		DrawNumber(221, 288+(int)y_delta, building_data.mineral, false);
		DrawNumber(277, 288+(int)y_delta, building_data.power_per_sec, true);
	}
	else
	{
		DrawNumber(165, 288+(int)y_delta, 0, false);
		DrawNumber(221, 288+(int)y_delta, 0, false);
		DrawNumber(277, 288+(int)y_delta, 0, true);
	}
}

void GameHUD::DrawNumber(const int x, const int y, const int value, const bool power, const D3DXCOLOR* color)
{
	D3DXVECTOR3 pen = D3DXVECTOR3((float)x, (float)y, 0);
	int number = abs(value);
	int big_number = 0, decimal = 0;
	Texture* tx = TextureManager::GetInstance().GetTexture("numbers");
	tx->color = color? *color:D3DXCOLOR(0,0,0,0.75f);

	// Power인 경우 /s를 써준다.
	if (power)
	{
		pen.x -= gNumberFontWidth[NUMBER_S];
		tx->Draw(pen,0,0,NUMBER_S);
		pen.x -= gNumberFontWidth[NUMBER_SLASH];
		tx->Draw(pen,0,0,NUMBER_SLASH);
		pen.x -= 1;
	}

	// 큰 수는 작게 만든다.
	if (number >= 1000000)
	{
		big_number = 1;
		number /= 100000;
		decimal = number%10;
		number /= 10;
		while (number >= 1000)
		{
			big_number += 1;
			number /= 100;
			decimal = number%10;
			number /= 10;
		}
	}

	if (big_number)
	{
		// 영문자
		pen.x -= gNumberFontWidth[NUMBER_M+big_number-1];
		tx->Draw(pen,0,0,NUMBER_M+big_number-1);
		// 소숫점 아래 한 글자
		pen.x -= gNumberFontWidth[decimal]+1;
		tx->Draw(pen,0,0,decimal);
		// 소숫점
		pen.x -= gNumberFontWidth[NUMBER_DOT]+1;
		tx->Draw(pen,0,0,NUMBER_DOT);
	}

	// 나머지 숫자
	int comma_count = 0;
	pen.x += 1;
	do {
		if (comma_count == 3) {
			pen.x -= gNumberFontWidth[NUMBER_COMMA]+1;
			tx->Draw(pen,0,0,NUMBER_COMMA);
			comma_count = 0;
		}
		else
		{
			int n = number%10;
			pen.x -= gNumberFontWidth[n]+1;
			tx->Draw(pen,0,0,n);
			number /= 10;
			comma_count++;
		}
	} while (number > 0);

	if (power)
	{
		int mark = (value > 0)? NUMBER_PLUS:((value<0)?NUMBER_MINUS:NUMBER_PLUSMINUS);
		pen.x -= gNumberFontWidth[mark]+1;
		tx->Draw(pen,0,0,mark);
	}
}
