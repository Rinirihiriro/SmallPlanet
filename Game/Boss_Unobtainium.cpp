#include "Boss_Unobtainium.h"
#include "JMain.h"
#include "TextureManager.h"
#include "Bullet.h"
#include "Particle.h"
#include "Smoke.h"
#include "Random.h"
#include "Scene_Game.h"
#include "BGMManager.h"

Boss_Unobtainium::Boss_Unobtainium(const D3DXVECTOR3 pos, const int x, const int y)
	:Boss("uboss", pos, x, y-11, 2, 128), fShootDir(0)
{
	z = 5;
}


int Boss_Unobtainium::Update()
{
	static void(Boss_Unobtainium::*pattern_func[])(void) = {
		&Boss_Unobtainium::Phase1Pattern
		, &Boss_Unobtainium::Phase2Pattern
		, &Boss_Unobtainium::Phase3Pattern
		, &Boss_Unobtainium::Phase4Pattern
		, &Boss_Unobtainium::Phase5Pattern
		, &Boss_Unobtainium::Phase6Pattern
	};
	iTimer++;
	(this->*pattern_func[iPhase])();
	Scene_Game* scene = (Scene_Game*)JMain::GetInstance().GetScene();
	Smoke* smoke = new Smoke(0
		, vPos.x+(float)((Random::rand.NextDouble()-0.5)*16+32)*((Random::rand.Next()%2-0.5f)*2)
		, vPos.y-(float)(Random::rand.NextDouble()*32)-16);
	smoke->SetVelocity(0, -2);
	scene->AddObject(smoke);
	return Boss::Update();
}

void Boss_Unobtainium::Draw()
{
	TextureManager::GetInstance().GetTexture("uboss")->Draw(vPos);
	TextureManager::GetInstance().GetTexture("uboss_eye")->Draw(vPos-D3DXVECTOR3(25, 192, 0),0,0,(iHitCoolTime==0)?((iPhase < 3)?0:1):2);
	if (fFlash>0.125f)
		TextureManager::GetInstance().GetTexture("uboss")->Draw(vPos,0,0,1,fFlash);
}


void Boss_Unobtainium::Damage(const int dmg)
{
	if (iHitCoolTime > 0) return;
	Boss::Damage(dmg);
	iHitCoolTime = 200;
	Scene_Game* scene = (Scene_Game*)JMain::GetInstance().GetScene();
	for (int i=0; i<10; i++)
	{
		Particle* particle = new Particle("uboss_particle"
			, vPos.x+32*(float)(Random::rand.NextDouble()-0.5)
			, vPos.y-192+32*(float)(Random::rand.NextDouble()-0.5));
		particle->SetVelocity((float)(Random::rand.NextDouble()-0.5)*5, -(float)Random::rand.NextDouble()*5);
		if (scene->mSpecial == Scene_Game::MODE_LESS_GRAVITY)
			particle->fGravity *= 0.5f;
		else if (scene->mSpecial == Scene_Game::MODE_HARDCORE)
			particle->fGravity *= 1.5f;
		particle->fFrame = (float)(Random::rand.Next()%8)*2;
		scene->AddObject(particle);
	}
}


bool Boss_Unobtainium::IsDead()
{
	return iPhase == 5;
}


void Boss_Unobtainium::Phase1Pattern()
{
	iHitCoolTime = 2;

	Scene_Game* scene = (Scene_Game*)JMain::GetInstance().GetScene();
	Particle* particle = new Particle("uboss_particle"
		, vPos.x+(float)(Random::rand.NextDouble()-0.5)*64
		, vPos.y-64-(float)(Random::rand.NextDouble()*128));
	if (scene->mSpecial == Scene_Game::MODE_LESS_GRAVITY)
		particle->fGravity *= 0.5f;
	else if (scene->mSpecial == Scene_Game::MODE_HARDCORE)
		particle->fGravity *= 1.5f;
	particle->fFrame = (float)(Random::rand.Next()%8)*2;
	scene->AddObject(particle);

	if (iTimer >= 210)
	{
		CircularShot((float)Random::rand.NextDouble()*D3DX_PI, 16, 4.5f);
		iTimer = 0;
		fFlash = 1;
		iPhase++;
	}
}

void Boss_Unobtainium::Phase2Pattern()
{
	if (iTimer == 30)
	{
		CircularShot((float)Random::rand.NextDouble()*D3DX_PI, (iHealth>iMaxHealth*3/4)?8:12, 2);
	}
	else if (iTimer >= 60)
	{
		CircularShot((float)Random::rand.NextDouble()*D3DX_PI, (iHealth>iMaxHealth*3/4)?3:6, 5);
		if (iHealth<=iMaxHealth*3/4)
			MakeBottomBullet(vPos.x+(float)(Random::rand.NextDouble()*512)-256, false);
		iTimer = 0;
	}
	if (iHealth < iMaxHealth/2)
	{
		iPhase++;
		iTimer = 0;
	}
}

void Boss_Unobtainium::Phase3Pattern()
{
	if (iHealth > 0)
	{
		if (iTimer%((iHealth>iMaxHealth/4)?7:5) == 0)
		{
			CircularShot(fShootDir, 3, 4.5f);
			fShootDir -= 0.5f;
			if (fShootDir<0) fShootDir += D3DX_PI*2;
		}
		if (iTimer >= ((iHealth>iMaxHealth/4)?30:15))
		{
			MakeDropBullet(vPos.x+(float)(Random::rand.NextDouble()*512)-256);
			iTimer = 0;
		}
	}
	else
	{
		BGMManager::GetInstance().PlayBGM("trembling");
		BGMManager::GetInstance().FadeOutBGMAndStop("unobtainium", 1000);

		if (iTimer > 200)
		{
			BGMManager::GetInstance().FadeOutBGMAndStop("trembling", 500);
			BGMManager::GetInstance().PlayBGM("unobtainium_rage");
			iHealth = iMaxHealth;
			iTimer = 0;
			iPhase++;
			fFlash = 2;
			CircularShot((float)Random::rand.NextDouble()*D3DX_PI, 16, 4.5f);
			float start_x = vPos.x - 256 + (float)(Random::rand.NextDouble() - 0.5) * 512 / 8;
			for (int i = 0; i < 9; i++)
				MakeBottomBullet(start_x + (float)i * 512 / 8, true);
		}
	}
}

void Boss_Unobtainium::Phase4Pattern()
{
	if (iTimer%5 == 0)
	{
		CircularShot((float)Random::rand.NextDouble()*D3DX_PI, 4, 5.f);
	}
	if (iTimer >= 300)
	{
		for (int i=0; i<3; i++)
			MakeBottomBullet(vPos.x+(float)(Random::rand.NextDouble()-0.5)*512, true);
		iTimer = 0;
	}
	if (iHealth < iMaxHealth/2)
	{
		iPhase++;
		iTimer = 0;
	}
}

void Boss_Unobtainium::Phase5Pattern()
{
	if (iTimer%10 == 0)
	{
		CircularShot(fShootDir, 3, 4.5f);
		fShootDir -= 0.75f;
		if (fShootDir<0) fShootDir += D3DX_PI*2;
	}
	if (iTimer%15 == 0)
	{
		MakeBottomBullet(vPos.x+(float)(Random::rand.NextDouble()-0.5)*512, false);
	}
	if (iTimer%30 == 0)
	{
		if (iHealth <= iMaxHealth/4)
			MakeBottomBullet(vPos.x+(float)iTimer/200.f*512-256+(float)(Random::rand.NextDouble()-0.5)*32, true);
		MakeDropBullet(vPos.x+(float)(Random::rand.NextDouble()*512)-256);
	}
	if (iTimer >= 200)
	{
		iTimer = 0;
	}
	if (iHealth <= 0)
	{
		iPhase++;
		iTimer = 0;
	}
}

void Boss_Unobtainium::Phase6Pattern()
{
	iTimer = 0;
	iHitCoolTime = 2;

	Scene_Game* scene = (Scene_Game*)JMain::GetInstance().GetScene();
	Particle* particle = new Particle("uboss_particle"
		, vPos.x+(float)(Random::rand.NextDouble()-0.5)*64
		, vPos.y-64-(float)(Random::rand.NextDouble()*128));
	if (scene->mSpecial == Scene_Game::MODE_LESS_GRAVITY)
		particle->fGravity *= 0.5f;
	else if (scene->mSpecial == Scene_Game::MODE_HARDCORE)
		particle->fGravity *= 1.5f;
	particle->fFrame = (float)(Random::rand.Next()%8)*2;
	scene->AddObject(particle);
}



void Boss_Unobtainium::CircularShot(const float start_angle, const int num, const float spd)
{
	for (int i=0; i<num; i++)
		MakeBullet(vPos.x, vPos.y-175, start_angle+D3DX_PI*2/num*i, spd);
}


void Boss_Unobtainium::MakeBullet(const float x, const float y, const float dir, const float spd)
{
	Bullet* bullet = new Bullet("uboss_bullet", x, y, 4, 2048, 210);
	bullet->SetDirection(dir);
	bullet->SetSpeed(spd);
	float d = dir+D3DXToRadian(22.5f);
	while (d < 0) d += D3DX_PI*2;
	while (d > D3DX_PI*2) d -= D3DX_PI*2;
	bullet->fFrame = floor(d/D3DXToRadian(45.f))*2;
	bullet->sParticle_tx_id = "uboss_particle";
	JMain::GetInstance().GetScene()->AddObject(bullet);
}

void Boss_Unobtainium::MakeDropBullet(const float x)
{
	Bullet* bullet = new Bullet("uboss_drop", x, vPos.y-512, 8.5f, 4096, 300);
	bullet->fFrame = (float)(Random::rand.Next()%2)*2;
	bullet->SetVelocity(0, 10);
	JMain::GetInstance().GetScene()->AddObject(bullet);
	bullet->fAirLeakRate = 0.4f;
	bullet->sParticle_tx_id = "uboss_particle";
}

void Boss_Unobtainium::MakeBottomBullet(const float x, const bool big)
{
	Scene_Game* scene_game = (Scene_Game*)JMain::GetInstance().GetScene();
	int block_x = (int)x/BLOCK_SIZE;
	int block_y = 0;
	while (scene_game->GetTileType(block_x, block_y) == BLOCK_AIR
		|| gBlockData[scene_game->GetTileType(block_x, block_y)].fluid)
		block_y++;
	
	if (!big)
	{
		Bullet* bullet = new Bullet("uboss_bottom", x, (block_y-0.5f)*BLOCK_SIZE, 5, 2048, 210);
		bullet->fFrame = (float)(Random::rand.Next()%4)*2;
		bullet->fAirLeakRate = 0;
		bullet->sParticle_tx_id = "uboss_particle";
		JMain::GetInstance().GetScene()->AddObject(bullet);
		scene_game->fShake = max(1, scene_game->fShake);
	}
	else
	{
		Bullet* bullet = new Bullet("uboss_bottom_big", x, (float)block_y*BLOCK_SIZE-38, 30, 8192, 300);
		bullet->mRect.height = 60;
		bullet->fFrame = (float)(Random::rand.Next()%3)*2;
		bullet->fAirLeakRate = 0.2f;
		bullet->sParticle_tx_id = "uboss_particle";
		JMain::GetInstance().GetScene()->AddObject(bullet);
		scene_game->fShake = max(5, scene_game->fShake);
	}
}

