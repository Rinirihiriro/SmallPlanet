#include "BaseMenu.h"
#include "Scene_Game.h"
#include "JInput.h"

#include "TextureManager.h"
#include "SEManager.h"

BaseMenu::BaseMenu()
	:pSceneGame(NULL)
	, BASE_MENU_FUAL_POS(181, 238), BASE_MENU_MINERAL_POS(300, 238)
	, COLOR_NORMAL(0,0,0,0.75f), COLOR_ERROR(1,0,0,0.75f), COLOR_WHITE(1,1,1,1), COLOR_YELLOW(1,1,0,1)
{
	Reset();
}

void BaseMenu::Reset()
{
	ZeroMemory(fSelectItemAlpha, sizeof(fSelectItemAlpha));
	fBaseMenuScale = 0;
	iBaseMenuTab = iSelectedItem = iBaseMenuFuelVal = iBaseMenuMineralVal
		= iBaseMenuShowFuel = iBaseMenuShowMineral = 0;
	mFuelValColor = mFualValColorDest = mMineralValColor = mMineralValColorDest
		= COLOR_NORMAL;
	bActive = false;
}

void BaseMenu::SetSceneGame(Scene_Game* scene_game)
{
	pSceneGame = scene_game;
}

void BaseMenu::InputProc()
{
	if (!bActive) return;

	if (JInput::IsTriggered('Y') || JInput::IsTriggered('X') || JInput::controller.IsButtonTriggered(Controller::Y, 0) || JInput::controller.IsButtonTriggered(Controller::B, 0))
	{
		bActive = false;
	}

	if (JInput::IsTriggered(VK_LSHIFT) || JInput::controller.IsButtonTriggered(Controller::LTRIGGER, 0))
	{
		SEManager::GetInstance().PlaySE("tab");
		iBaseMenuTab = (iBaseMenuTab-1+3)%3;
		iSelectedItem = 0;
		ZeroMemory(fSelectItemAlpha, sizeof(fSelectItemAlpha));
		return;
	}
	else if (JInput::IsTriggered(VK_RSHIFT) || JInput::controller.IsButtonTriggered(Controller::RTRIGGER, 0))
	{
		SEManager::GetInstance().PlaySE("tab");
		iBaseMenuTab = (iBaseMenuTab+1)%3;
		iSelectedItem = 0;
		ZeroMemory(fSelectItemAlpha, sizeof(fSelectItemAlpha));
		return;
	}

	switch (iBaseMenuTab)
	{
	case 0:
		{
			if (JInput::IsTriggered(VK_LEFT) || JInput::controller.IsStickTriggered(Controller::LSTICK, Controller::LEFT, 0))
				iSelectedItem = (iSelectedItem/3)*3+((iSelectedItem%3)-1+3)%3;
			if (JInput::IsTriggered(VK_RIGHT) || JInput::controller.IsStickTriggered(Controller::LSTICK, Controller::RIGHT, 0))
				iSelectedItem = (iSelectedItem/3)*3+((iSelectedItem%3)+1)%3;
			if (JInput::IsTriggered(VK_UP) || JInput::controller.IsStickTriggered(Controller::LSTICK, Controller::UP, 0))
				iSelectedItem = (iSelectedItem/3-1+3)%3*3+(iSelectedItem%3);
			if (JInput::IsTriggered(VK_DOWN) || JInput::controller.IsStickTriggered(Controller::LSTICK, Controller::DOWN, 0))
				iSelectedItem = (iSelectedItem/3+1)%3*3+(iSelectedItem%3);
			if (JInput::IsTriggered('Z') || JInput::controller.IsButtonTriggered(Controller::A, 0))
			{
				const Upgrade& next_upgrade = gUpgrades[iSelectedItem][pSceneGame->iUpgrades[iSelectedItem]+1];
				if (next_upgrade.fuel > 0
					&& pSceneGame->mBaseCamp.iFuel >= next_upgrade.fuel
					&& pSceneGame->mBaseCamp.iMineral >= next_upgrade.mineral)
				{
					SEManager::GetInstance().PlaySE("upgrade");
					pSceneGame->mBaseCamp.iFuel -= next_upgrade.fuel;
					pSceneGame->mBaseCamp.iMineral -= next_upgrade.mineral;
					pSceneGame->iUpgrades[iSelectedItem] += 1;
					mFuelValColor = mMineralValColor = COLOR_WHITE;
					switch (iSelectedItem)
					{
					case UPGRADE_MAX_HEALTH:
						pSceneGame->pPlayer->iHealthMax = Player::BASIC_MAX_HEALTH*next_upgrade.effect/10;
						if (pSceneGame->mSpecial == Scene_Game::MODE_HARDCORE) pSceneGame->pPlayer->iHealthMax /= 2;
						break;
					case UPGRADE_MAX_OXYGEN:
						pSceneGame->pPlayer->iAirMax = Player::BASIC_MAX_AIR*next_upgrade.effect/10;
						if (pSceneGame->mSpecial == Scene_Game::MODE_HARDCORE) pSceneGame->pPlayer->iAirMax /= 2;
						break;
					case UPGRADE_CARGO_CAPACITY:
						pSceneGame->pPlayer->iCargoMax = Player::BASIC_MAX_CARGO*next_upgrade.effect/10;
						if (pSceneGame->mSpecial == Scene_Game::MODE_HARDCORE) pSceneGame->pPlayer->iCargoMax /= 2;
						break;
					case UPGRADE_HEALTH_RECOVERY:
						pSceneGame->pPlayer->iHealthRecovery = Player::BASIC_HEALTH_RECOVERY*next_upgrade.effect/10;
						break;
					case UPGRADE_OXYGEN_RECOVERY:
						pSceneGame->pPlayer->iAirRecover = Player::BASIC_AIR_RECOVER*next_upgrade.effect/10;
						break;
					case UPGRADE_JETPACK_FUEL_TANK:
						pSceneGame->pPlayer->iJetFuelMax = Player::BASIC_MAX_JETPACK*next_upgrade.effect/10;
						if (pSceneGame->mSpecial == Scene_Game::MODE_HARDCORE) pSceneGame->pPlayer->iJetFuelMax /= 2;
						break;
					case UPGRADE_MAGMA_PROOF:
						pSceneGame->iMagmaDamage = Scene_Game::MAGMA_DAMAGE*next_upgrade.effect/10;
						break;
					case UPGRADE_DIRLL_EFFICENCY:
						pSceneGame->pPlayer->iDrillPower = Player::BASIC_DRILL_POWER*next_upgrade.effect/10;
						pSceneGame->pPlayer->iDrillLevel = pSceneGame->iUpgrades[iSelectedItem];
						if (pSceneGame->iUpgrades[iSelectedItem] == 1)
							pSceneGame->bBreakable[BLOCK_OBSIDIAN] = true;
						break;
					}
				}
			}
		}
		break;
	case 1:
		{
			if (iSelectedItem >= 0 && (JInput::IsTriggered('Z') || JInput::controller.IsButtonTriggered(Controller::A, 0)))
			{
				const BlockData& data = gBlockData[gResourceBlock[iSelectedItem]];
				if (pSceneGame->mBaseCamp.iFuel >= data.research_fuel
					&& pSceneGame->mBaseCamp.iMineral >= data.research_mineral)
				{
					SEManager::GetInstance().PlaySE("upgrade");
					pSceneGame->mBaseCamp.iFuel -= data.research_fuel;
					pSceneGame->mBaseCamp.iMineral -= data.research_mineral;
					pSceneGame->bUsable[gResourceBlock[iSelectedItem]] = true;
					pSceneGame->bBreakable[gResourceBlock[iSelectedItem]] = true;
					mFuelValColor = mMineralValColor = COLOR_WHITE;
				}
			}
		}
		break;
	case 2:
		{
			if (iSelectedItem >= 0 && (JInput::IsTriggered('Z') || JInput::controller.IsButtonTriggered(Controller::A, 0)))
			{
				const BuildingData& data = gBuildingData[iSelectedItem+1];
				if (pSceneGame->mBaseCamp.iFuel >= data.research_fuel
					&& pSceneGame->mBaseCamp.iMineral >= data.research_mineral)
				{
					SEManager::GetInstance().PlaySE("upgrade");
					pSceneGame->mBaseCamp.iFuel -= data.research_fuel;
					pSceneGame->mBaseCamp.iMineral -= data.research_mineral;
					pSceneGame->bBuildable[iSelectedItem+1] = true;
					mFuelValColor = mMineralValColor = COLOR_WHITE;
				}
				if (pSceneGame->iHelpStep == 5) pSceneGame->iHelpStep++;
			}
		}
		break;
	}
}

void BaseMenu::Update()
{
	if (bActive)
	{
		switch (iBaseMenuTab)
		{
		case 1:
			iSelectedItem = -1;
			for (int i=0; i<RES_TYPE_NUM; i++)
			{
				if (!pSceneGame->bUsable[gResourceBlock[i]])
				{
					iSelectedItem = i;
					break;
				}
			}
			break;
		case 2:
			iSelectedItem = -1;
			for (int i=0; i<BUILDING_SMALL_CAMP; i++)
			{
				if (!pSceneGame->bBuildable[i+1])
				{
					iSelectedItem = i;
					break;
				}
			}
			break;
		}

		for (int i=0; i<UPGRADE_NUM; i++)
		{
			if (i == iSelectedItem)
				fSelectItemAlpha[i] += (1-fSelectItemAlpha[i])*0.25f;
			else
				fSelectItemAlpha[i] -= fSelectItemAlpha[i]*0.25f;
		}
		iBaseMenuShowFuel += (int)ceil(abs(iBaseMenuFuelVal-iBaseMenuShowFuel)*0.25f)
			*((iBaseMenuFuelVal>iBaseMenuShowFuel)?1:-1);
		iBaseMenuShowMineral += (int)ceil(abs(iBaseMenuMineralVal-iBaseMenuShowMineral)*0.25f)
			*((iBaseMenuMineralVal>iBaseMenuShowMineral)?1:-1);
		mFuelValColor += (mFualValColorDest-mFuelValColor)*0.125f;
		mMineralValColor += (mMineralValColorDest-mMineralValColor)*0.125f;
		fBaseMenuScale += (1-fBaseMenuScale)*0.25f;

	}
	else
		fBaseMenuScale -= fBaseMenuScale*0.25f;
}

void BaseMenu::Draw()
{
	static const char* const basemenu_back[] = {"basemenu01","basemenu02","basemenu03"};
	static void (BaseMenu::*basemenu_draw[])() = {
		&BaseMenu::DrawUpgradeMenu, &BaseMenu::DrawResourceMenu, &BaseMenu::DrawBuildMenu};

	D3DXMATRIX mat, trans, trans_inv, scale;
	Texture::GetSprite().GetTransform(&mat);
	D3DXMatrixTranslation(&trans_inv, -200, -150, 0);
	D3DXMatrixTranslation(&trans, 200, 150, 0);
	D3DXMatrixScaling(&scale, fBaseMenuScale, fBaseMenuScale, 1);

	Texture::GetSprite().SetTransform(trans_inv*scale*trans*mat);

	Texture* back = TextureManager::GetInstance().GetTexture(basemenu_back[iBaseMenuTab]);
	back->Draw(D3DXVECTOR3(200,150,0));
	TextureManager::GetInstance().GetTexture("basemenu_control")->Draw(D3DXVECTOR3(320, 55, 0));
	(this->*basemenu_draw[iBaseMenuTab])();

	DrawBigNumber(BASE_MENU_FUAL_POS.x, BASE_MENU_FUAL_POS.y, iBaseMenuShowFuel, false, &mFuelValColor);
	DrawBigNumber(BASE_MENU_MINERAL_POS.x, BASE_MENU_MINERAL_POS.y, iBaseMenuShowMineral, false, &mMineralValColor);

	Texture::GetSprite().SetTransform(mat);
}


void BaseMenu::Open()
{
	bActive = true;
	ZeroMemory(fSelectItemAlpha, sizeof(fSelectItemAlpha));
	iSelectedItem = iBaseMenuTab = iBaseMenuShowFuel = iBaseMenuShowMineral
		= iBaseMenuFuelVal = iBaseMenuMineralVal = 0;
	mFuelValColor = mFualValColorDest = mMineralValColor = mMineralValColorDest = COLOR_NORMAL;
}


void BaseMenu::DrawUpgradeMenu()
{
	Texture* tx_upgrades = TextureManager::GetInstance().GetTexture("upgrades");
	for (int i=0; i<UPGRADE_NUM; i++)
	{
		D3DXVECTOR3 pos = D3DXVECTOR3(96+72.f*(i%3), 64+48.f*(i/3), 0);
		D3DXCOLOR textcolor =
			COLOR_NORMAL*(1-mFuelValColor.b)*(1-fSelectItemAlpha[i])
			+ COLOR_YELLOW*(1-mFuelValColor.b)*fSelectItemAlpha[i]
		+ COLOR_WHITE*mFuelValColor.b*fSelectItemAlpha[i];
		tx_upgrades->Draw(pos,0,0,i);
		tx_upgrades->color = COLOR_YELLOW*(1-mFuelValColor.b)+COLOR_WHITE*mFuelValColor.b;
		tx_upgrades->Draw(pos,0,0,i+9,fSelectItemAlpha[i]);
		tx_upgrades->color = COLOR_WHITE;
		DrawBigNumber(158+72*(i%3), 83+48*(i/3), gUpgrades[i][pSceneGame->iUpgrades[i]].effect, true, &textcolor);
	}
	DrawBigNumber(187, 213, gUpgrades[iSelectedItem][pSceneGame->iUpgrades[iSelectedItem]].effect, true);
	const Upgrade& next_upgrade = gUpgrades[iSelectedItem][pSceneGame->iUpgrades[iSelectedItem]+1];
	if (next_upgrade.fuel > 0)
	{
		DrawBigNumber(240, 213, next_upgrade.effect, true);
		iBaseMenuFuelVal = next_upgrade.fuel;
		iBaseMenuMineralVal = next_upgrade.mineral;
		mFualValColorDest = (next_upgrade.fuel > pSceneGame->mBaseCamp.iFuel)? COLOR_ERROR:COLOR_NORMAL;
		mMineralValColorDest = (next_upgrade.mineral > pSceneGame->mBaseCamp.iMineral)? COLOR_ERROR:COLOR_NORMAL;
	}
	else
	{
		DrawBigNumber(240, 213, gUpgrades[iSelectedItem][pSceneGame->iUpgrades[iSelectedItem]].effect, true);
		iBaseMenuFuelVal = 0;
		iBaseMenuMineralVal = 0;
		mFualValColorDest = mMineralValColorDest = COLOR_NORMAL;
	}
}

void BaseMenu::DrawResourceMenu()
{
	Texture* tx_resources = TextureManager::GetInstance().GetTexture("resources");
	for (int i=0; i<RES_TYPE_NUM; i++)
	{
		D3DXVECTOR3 pos = D3DXVECTOR3(96, 62+28.f*i, 0);
		if (pSceneGame->bUsable[gResourceBlock[i]])
		{
			tx_resources->Draw(pos,0,0,(i+1)*2);
			tx_resources->Draw(pos,0,0,(i+1)*2+1,fSelectItemAlpha[i]);
		}
		else
		{
			tx_resources->Draw(pos);
		}
	}
	if (iSelectedItem>=0)
	{
		const BlockData& research_data = gBlockData[gResourceBlock[iSelectedItem]];
		TextureManager::GetInstance().GetTexture("buttons")->Draw(D3DXVECTOR3(272, 62+28.f*iSelectedItem, 0));
		iBaseMenuFuelVal = research_data.research_fuel;
		iBaseMenuMineralVal = research_data.research_mineral;
		mFualValColorDest = (research_data.research_fuel > pSceneGame->mBaseCamp.iFuel)? COLOR_ERROR:COLOR_NORMAL;
		mMineralValColorDest = (research_data.research_mineral > pSceneGame->mBaseCamp.iMineral)? COLOR_ERROR:COLOR_NORMAL;
	}
	else
	{
		iBaseMenuFuelVal = 0;
		iBaseMenuMineralVal = 0;
		mFualValColorDest = mMineralValColorDest = COLOR_NORMAL;
	}
}

void BaseMenu::DrawBuildMenu()
{
	Texture* tx_buildings = TextureManager::GetInstance().GetTexture("buildings");
	for (int i=0; i<BUILDING_SMALL_CAMP; i++)
	{
		D3DXVECTOR3 pos = D3DXVECTOR3(92, 64+41.f*i, 0);
		if (pSceneGame->bBuildable[i+1])
		{
			tx_buildings->Draw(pos,0,0,(i+1)*2);
			tx_buildings->Draw(pos,0,0,(i+1)*2+1,fSelectItemAlpha[i]);
		}
		else
		{
			tx_buildings->Draw(pos);
		}
	}
	if (iSelectedItem>=0)
	{
		const BuildingData& building_data = gBuildingData[iSelectedItem+1];
		TextureManager::GetInstance().GetTexture("buttons")->Draw(D3DXVECTOR3(272, 72+41.f*iSelectedItem, 0));
		iBaseMenuFuelVal = building_data.research_fuel;
		iBaseMenuMineralVal = building_data.research_mineral;
		mFualValColorDest = (building_data.research_fuel > pSceneGame->mBaseCamp.iFuel)? COLOR_ERROR:COLOR_NORMAL;
		mMineralValColorDest = (building_data.research_mineral > pSceneGame->mBaseCamp.iMineral)? COLOR_ERROR:COLOR_NORMAL;
	}
	else
	{
		iBaseMenuFuelVal = 0;
		iBaseMenuMineralVal = 0;
		mFualValColorDest = mMineralValColorDest = COLOR_NORMAL;
	}
}

void BaseMenu::DrawBigNumber(const int x, const int y, const int value, const bool mlt, const D3DXCOLOR* color)
{
	D3DXVECTOR3 pen = D3DXVECTOR3((float)x, (float)y, 0);
	int number = abs(value);
	Texture* tx = TextureManager::GetInstance().GetTexture("big_numbers");

	if (color)
		tx->color = *color;
	else
		tx->color = D3DXCOLOR(0,0,0,0.75f);

	if (mlt)
	{
		if (value < 100)
		{
			int decimal = number%10;
			number/=10;
			pen.x -= gBigNumberFontWidth[decimal];
			tx->Draw(pen,0,0,decimal);
			pen.x -= gBigNumberFontWidth[BIG_NUMBER_DOT]+1;
			tx->Draw(pen,0,0,BIG_NUMBER_DOT);
		}
		else
		{
			number/=10;
		}
	}
	else
		pen.x += 1;

	int comma_count = 0;
	do {
		if (comma_count == 3) {
			pen.x -= gBigNumberFontWidth[BIG_NUMBER_COMMA]+1;
			tx->Draw(pen,0,0,BIG_NUMBER_COMMA);
			comma_count = 0;
		}
		else
		{
			int n = number%10;
			pen.x -= gBigNumberFontWidth[n]+1;
			tx->Draw(pen,0,0,n);
			number /= 10;
			comma_count++;
		}
	} while (number > 0);

	if (mlt)
	{
		pen.x -= gBigNumberFontWidth[BIG_NUMBER_X]+1;
		tx->Draw(pen,0,0,BIG_NUMBER_X);
	}
}
