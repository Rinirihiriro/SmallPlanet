#include "Scene_Logo.h"

#include "JMain.h"
#include "TextureManager.h"
#include "Scene_Game.h"
#include "JInput.h"

Scene_Logo::Scene_Logo()
	:Scene(0xff000000), iTimer(0), fAlpha(0)
{

}

void Scene_Logo::InputProc()
{
	if (iTimer < 120 && (JInput::IsTriggered(VK_ANYKEY) || JInput::controller.IsButtonTriggered(Controller::ALL, 0)))
	{
		iTimer = 120;
	}
}

void Scene_Logo::Update()
{
	if (iTimer == 0 && fAlpha < 1)
	{
		fAlpha += 0.0625f;
	}
	else if (iTimer >= 120 && fAlpha > 0)
	{
		fAlpha -= 0.0625f;
	}
	else if (iTimer < 180)
	{
		iTimer += 1;
	}
	else if (iTimer >= 180)
	{
		JMain::GetInstance().SetScene(new Scene_Game(), false);
	}
}

void Scene_Logo::Render()
{
	TextureManager::GetInstance().GetTexture("logo")->Draw(D3DXVECTOR3(0,0,0),0,0,0,fAlpha);
}