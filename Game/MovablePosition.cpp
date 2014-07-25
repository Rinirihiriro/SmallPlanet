#include "MovablePosition.h"

MovablePosition::MovablePosition()
	:vPos(0,0,0), vDest(vPos), vVelocity(0,0,0), vAcceleration(0,0,0), iCounter(0)
	, x(vPos.x), y(vPos.y), iCounterHalf(0), bSmooth(false)
{

}

MovablePosition::MovablePosition(const D3DXVECTOR3 pos)
	:vPos(pos), vDest(vPos), vVelocity(0,0,0), vAcceleration(0,0,0), iCounter(0)
	, x(vPos.x), y(vPos.y), iCounterHalf(0), bSmooth(false)
{

}


void MovablePosition::Update()
{
	if (iCounter > 0)
	{
		vPos += vVelocity;
		vVelocity += vAcceleration;
		iCounter--;
		if (iCounter == 0) vPos = vDest;
		if (bSmooth && iCounter==iCounterHalf)
		{
			vAcceleration = -vAcceleration;
			bSmooth = false;
		}
	}
	else
	{
		vPos += (vDest-vPos)*0.125f;
	}
}

void MovablePosition::Move(const D3DXVECTOR3 dest, const int frame)
{
	vDest = dest;
	vAcceleration = D3DXVECTOR3(0,0,0);
	vVelocity = (dest-vPos)/(float)frame;
	iCounter = frame;
}

void MovablePosition::MoveBouncily(const D3DXVECTOR3 dest, const int frame)
{
	vDest = dest;
	vAcceleration = -(dest-vPos)/(frame*(frame+1)*0.5f);
	vVelocity = -vAcceleration*(float)frame;
	iCounter = frame;
}

void MovablePosition::MoveSmoothly(const D3DXVECTOR3 dest, const int frame)
{
	bSmooth = true;
	vDest = dest;
	vVelocity = D3DXVECTOR3(0,0,0);
	vAcceleration = 4*(dest-vPos)/(float)(frame*frame);
	iCounter = frame;
	iCounterHalf = frame/2;
}


void MovablePosition::SetDestination(const D3DXVECTOR3 dest)
{
	vDest = dest;
}

void MovablePosition::Stop()
{
	vDest = vPos;
	vVelocity = D3DXVECTOR3(0,0,0);
	vAcceleration = D3DXVECTOR3(0,0,0);
	iCounter = 0;
}


bool MovablePosition::IsMoving() const
{
	D3DXVECTOR3 v=vPos-vDest;
	return D3DXVec3LengthSq(&v)>1;
}

MovablePosition::operator D3DXVECTOR3&(){return vPos;}
MovablePosition::operator const D3DXVECTOR3&() const {return vPos;}


D3DXVECTOR3 MovablePosition::operator +(D3DXVECTOR3 other)
{
	return vPos + other;
}

MovablePosition& MovablePosition::operator =(D3DXVECTOR3 other)
{
	vPos = other;
	vDest = vPos;
	return *this;
}

MovablePosition& MovablePosition::operator +=(D3DXVECTOR3 other)
{
	vPos += other;
	vDest = vPos;
	return *this;
}