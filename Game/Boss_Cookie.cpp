#include "Boss_Cookie.h"
#include "JMain.h"
#include "TextureManager.h"
#include "Bullet.h"
#include "Particle.h"
#include "Random.h"
#include "Scene_Game.h"
#include "Player.h"
#include "BGMManager.h"

Boss_Cookie::Boss_Cookie(const D3DXVECTOR3 pos, const int x, const int y)
	:Boss("cookie", pos, x, y, 7, 1024), fShootDir(0), fTriConst(0)
{
	ZeroMemory(mBulletArr, sizeof(mBulletArr));
	iArrInd = 0;
	z = 5;
}


int Boss_Cookie::Update()
{
	static void(Boss_Cookie::*pattern_func[])(void) = {
		&Boss_Cookie::Phase1Pattern
		, &Boss_Cookie::Phase2Pattern
		, &Boss_Cookie::Phase3Pattern
		, &Boss_Cookie::Phase4Pattern
		, &Boss_Cookie::Phase5Pattern
		, &Boss_Cookie::Phase6Pattern
		, &Boss_Cookie::Phase7Pattern
		, &Boss_Cookie::Phase8Pattern
		, &Boss_Cookie::Phase9Pattern
	};
	iTimer++;
	fTriConst += 0.0625f;
	if (fTriConst > D3DX_PI*2) fTriConst -= D3DX_PI*2;
	(this->*pattern_func[iPhase])();
	
	Scene_Game* scene = (Scene_Game*)JMain::GetInstance().GetScene();
	Player* player = scene->pPlayer;
	
	float distance = player->vPos.x-vPos.x;
	if (abs(distance) > scene->iMapWidthPixel/2)
		distance += (distance>0)? -scene->iMapWidthPixel:scene->iMapWidthPixel;
	D3DXVECTOR3 velocity = GetVelocity();
	if (abs(distance) > 128 && abs(velocity.x)<3.5) velocity.x += (distance>0)?0.0625f:-0.0625f;
	else if (abs(velocity.x) > 0.0625f) velocity.x -= (velocity.x>0)?0.0625f:-0.0625f;
	else velocity.x = 0;
	SetVelocity(velocity);
	iX = ((int)vPos.x/BLOCK_SIZE+scene->iMapWidth)%scene->iMapWidth;

	return Boss::Update();
}

void Boss_Cookie::Draw()
{
	D3DXVECTOR3 pos = vPos;
	pos.y += sin(fTriConst)*6;
	int frame = (iPhase < 5)?0:2;
	TextureManager::GetInstance().GetTexture("cookie")->Draw(pos,0,0,frame);
	if (fFlash>0.125f)
		TextureManager::GetInstance().GetTexture("cookie")->Draw(pos,0,0,frame+1,fFlash);
}


void Boss_Cookie::Damage(const int dmg)
{
	if (iHitCoolTime > 0) return;
	Boss::Damage(dmg);
	iHitCoolTime = 20;
	Scene_Game* scene = (Scene_Game*)JMain::GetInstance().GetScene();
	for (int i=0; i<10; i++)
	{
		Particle* particle = new Particle("cookie_particle"
			, vPos.x+100*(float)(Random::rand.NextDouble()-0.5)
			, vPos.y+100*(float)(Random::rand.NextDouble()-0.5));
		particle->SetVelocity((float)(Random::rand.NextDouble()-0.5)*5, -(float)Random::rand.NextDouble()*5);
		if (scene->mSpecial == Scene_Game::MODE_LESS_GRAVITY)
			particle->fGravity *= 0.5f;
		else if (scene->mSpecial == Scene_Game::MODE_HARDCORE)
			particle->fGravity *= 1.5f;
		particle->fFrame = (float)(Random::rand.Next()%8)*2;
		scene->AddObject(particle);
	}
}


bool Boss_Cookie::IsDead()
{
	return iHealth <= 0 && iPhase == 8;
}


void Boss_Cookie::Phase1Pattern()
{
	if (iTimer >= 210)
	{
		CircularShot(vPos.x, vPos.y, (float)Random::rand.NextDouble()*D3DX_PI, 16, 4.5f);
		iTimer = -1;
		fFlash = 1;
		iPhase++;
	}
}

void Boss_Cookie::Phase2Pattern()
{
	for (int i=0; i<10; i++)
	{
		if (!mBulletArr[i]) continue;
		float dir = mBulletArr[i]->GetDirection()+D3DX_PI;
		mBulletArr[i]->vPos = D3DXVECTOR3(vPos.x+75*cos(dir), vPos.y+75*sin(dir), 0);
	}

	if (iTimer == 0)
	{
		fShootDir = (float)Random::rand.NextDouble()*D3DX_PI;
	}
	if (iTimer < 60 && iTimer % 6 == 0)
	{
		int ind = iTimer/6;
		mBulletArr[ind] = MakeBullet(vPos.x+75*cos(fShootDir), vPos.y+75*sin(fShootDir), fShootDir+D3DX_PI, 0);
		fShootDir += D3DXToRadian(36);
	}
	if (iTimer >= 60 && iTimer < 120 && iTimer % 6 == 0)
	{
		int ind = (iTimer-60)/6;
		mBulletArr[ind]->fFlash = 1;
		mBulletArr[ind]->SetSpeed(4);
		mBulletArr[ind] = NULL;
	}
	if (iTimer >= 120)
	{
		iTimer = -1;
	}

	if (iHealth <= iMaxHealth*3/4)
	{
		for (int i=0; i<10; i++)
		{
			if (mBulletArr[i])
			{
				mBulletArr[i]->iLife = 1;
				mBulletArr[i] = NULL;
			}
		}
		iTimer = -1;
		iPhase++;
	}
}

void Boss_Cookie::Phase3Pattern()
{
	for (int i=0; i<10; i++)
	{
		if (!mBulletArr[i]) continue;
		float dir = mBulletArr[i]->GetDirection()+D3DX_PI;
		mBulletArr[i]->vPos = D3DXVECTOR3(vPos.x+75*cos(dir), vPos.y+75*sin(dir), 0);
	}

	if (iTimer < 60 && iTimer % 6 == 0)
	{
		int ind = iTimer/6;
		if (mBulletArr[ind])
		{
			mBulletArr[ind]->fFlash = 1;
			mBulletArr[ind]->SetSpeed(4);
		}
		mBulletArr[ind] = MakeBullet(vPos.x+75*cos(fShootDir), vPos.y+75*sin(fShootDir), fShootDir+D3DX_PI, 0);
		fShootDir += D3DXToRadian(39);
	}
	if (iTimer >= 60)
	{
		iTimer = -1;
	}

	if (iHealth <= iMaxHealth/2)
	{
		for (int i=0; i<10; i++)
		{
			if (mBulletArr[i])
			{
				mBulletArr[i]->iLife = 1;
				mBulletArr[i] = NULL;
			}
		}
		iTimer = -1;
		iPhase++;
	}
}

void Boss_Cookie::Phase4Pattern()
{
	if (iTimer == 2)
	{
		iTimer = -1;
		MakeBullet(vPos.x+512*(float)(Random::rand.NextDouble()-0.5), max(0, vPos.y-200), D3DXToRadian(90), 4, 8192);
	}

	if (iHealth <= iMaxHealth/4)
	{
		iTimer = -1;
		iPhase++;
		fShootDir = (float)Random::rand.NextDouble()*D3DX_PI;
	}
}

void Boss_Cookie::Phase5Pattern()
{
	if (iHealth > 0)
	{
		if (iTimer == 10)
		{
			iTimer = -1;
			for (int i=0; i<3; i++)
				CircularShot(vPos.x+90*cos(fShootDir+D3DX_PI*i*2/3)
				, vPos.y+90*sin(fShootDir+D3DX_PI*i*2/3), fShootDir*2, 3, 4);
			fShootDir -= D3DXToRadian(7.f);
			if (fShootDir<0) fShootDir+=D3DX_PI*2;
		}
	}
	else
	{
		BGMManager::GetInstance().PlayBGM("trembling");
		BGMManager::GetInstance().FadeOutBGMAndStop("cookieclicker", 1000);
		if (iTimer >= 200)
		{
			BGMManager::GetInstance().FadeOutBGMAndStop("trembling", 500);
			BGMManager::GetInstance().PlayBGM("cookieclicker_rage");
			iHealth = iMaxHealth;
			fFlash = 2;
			iTimer = -1;
			iPhase++;
		}
	}
}

void Boss_Cookie::Phase6Pattern()
{
	if (iTimer == 0)
	{
		fShootDir = (float)Random::rand.NextDouble()*D3DX_PI;
		CircularShot(vPos.x+10, vPos.y, fShootDir, 20, 3, 8192);
		CircularShot(vPos.x-10, vPos.y, -fShootDir, 20, 3, 8192);
	}
	if (iTimer >= 100)
		iTimer = -1;

	if (iHealth <= iMaxHealth*3/4)
	{
		iTimer = -1;
		iPhase++;
	}
}

void Boss_Cookie::Phase7Pattern()
{
	static float len = 0;
	if (iTimer == 0)
	{
		len = 32;
		fShootDir = (float)Random::rand.NextDouble()*D3DX_PI;
	}

	if (iTimer > 100 && iTimer%5 == 0)
	{
		for (int i=0; i<3; i++)
		{
			float dir = fShootDir+D3DX_PI*i*2/3;
			MakeBullet(vPos.x+len*cos(dir), vPos.y+len*sin(dir), dir-D3DX_PI/2, 1.f, 4096);
			MakeBullet(vPos.x+len*cos(-dir), vPos.y+len*sin(-dir), -dir+D3DX_PI/2, 1.f, 4096);
		}
		fShootDir -= D3DXToRadian(29.f);
		if (fShootDir<0) fShootDir+=D3DX_PI*2;
		len += 16;
	}

	if (iTimer >= 180)
	{
		iTimer = -1;
	}

	if (iHealth <= iMaxHealth/2)
	{
		fShootDir = (float)Random::rand.NextDouble()*D3DX_PI;
		iTimer = -1;
		iPhase++;
	}
}

void Boss_Cookie::Phase8Pattern()
{
	static float shoot_pos = 0;
	static float last_move = 0;
	if (iTimer == 4)
	{
		for (int i=(int)shoot_pos; i<512; i+=60)
		{
			MakeBullet(vPos.x+i-256, max(0, vPos.y-200), D3DX_PI/2, 4.f, 8152);
		}
		shoot_pos = sin(fShootDir)*30;
		fShootDir -= D3DXToRadian(19.f);
		if (fShootDir<0) fShootDir+=D3DX_PI*2;
		if (last_move < 1 && abs(fShootDir-D3DX_PI/2)<0.125f && Random::rand.Next()%2==0)
		{
			fShootDir-=D3DX_PI;
			last_move += 1;
		}
		else if (last_move > -1 && abs(fShootDir-D3DX_PI*3/2)<0.125f && Random::rand.Next()%2==0)
		{
			fShootDir-=D3DX_PI;
			last_move -= 1;
		}
		iTimer = -1;
	}
	if (iHealth <= iMaxHealth/4)
	{
		iTimer = -1;
		iPhase++;
	}
}

void Boss_Cookie::Phase9Pattern()
{
	if (iHealth > 0)
	{
		if (iTimer == 3)
		{
			CircularShot(vPos.x, vPos.y, fShootDir, 3, 2.5f);
			CircularShot(vPos.x, vPos.y, -fShootDir, 3, 2.5f);
			fShootDir -= D3DXToRadian(17.f);
			if (fShootDir<0) fShootDir+=D3DX_PI*2;
			iTimer = -1;
		}
	}
}


void Boss_Cookie::CircularShot(const float x, const float y, const float start_angle, const int num, const float spd, const int power)
{
	for (int i=0; i<num; i++)
		MakeBullet(x, y, start_angle+D3DX_PI*2/num*i, spd, power);
}


Bullet* Boss_Cookie::MakeBullet(const float x, const float y, const float dir, const float spd, const int power)
{
	Bullet* bullet = new Bullet("cursor_bullet", x, y, 4, (power>0)? power:3000, 300);
	bullet->SetDirection(dir);
	bullet->SetSpeed(spd);
	float d = dir+D3DXToRadian(22.5f);
	while (d < 0) d += D3DX_PI*2;
	while (d > D3DX_PI*2) d -= D3DX_PI*2;
	bullet->fFrame = floor(d/D3DXToRadian(45.f))*2;
	bullet->sParticle_tx_id = "cursor_particle";
	JMain::GetInstance().GetScene()->AddObject(bullet);
	return bullet;
}
