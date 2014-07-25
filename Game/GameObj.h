/*
	게임 오브젝트 슈퍼클래스입니다.
*/
#pragma once

#include <string>

#include "CRect.h"

class GameObj
{
public:
	enum ObjectUpdateState
	{
		STATE_OK = 0
		, STATE_DELETED = -1
	};

	struct CompairZ
	{
		bool operator()(const GameObj* a, const GameObj* b);
	};

	std::string sTextureId;

	int z;
	D3DXVECTOR3 vPos;

// 	float fFriction;

	float fAlpha;
	D3DXVECTOR3 vScale;
	float fRotation;	// Radian

	float fFrame;
	float fFrame_rate;	// frame rate

	CRect mRect;

	bool bTransform;

protected:
	std::string sObject_tag;

private:
	bool bErase;
	D3DXVECTOR3 vVelocity;
	float fSpeed;
	float fDirection; // radian

private:
	void init();

	void speed2velocity();
	void velocity2speed();

public:

	GameObj( const std::string texture, const D3DXVECTOR3 pos, const int z=0 );
	GameObj( const std::string texture, const float x, const float y, const int z=0 );
	virtual ~GameObj();

	virtual int Update();
	virtual void Draw();
	void UpdateFrame();
	void UpdateVelocity();

	std::string GetObjectTag() const;

	void SetVelocity( const D3DXVECTOR3 velocity );
	void SetVelocity( const float x, const float y);
	void SetSpeed( const float speed );
	void SetDirection( const float direction );

	void AddVelocity( const D3DXVECTOR3 velocity );
	void AddVelocity( const float x, const float y );
	void AddSpeed( const float speed );
	void AddDirection( const float direction );

	D3DXVECTOR3 GetVelocity() const;
	float GetSpeed() const;
	float GetDirection() const;

	void erase();
	virtual bool operator &(GameObj& other);
};