#pragma once

#include <d3dx9math.h>

struct MovablePosition
{
public:
	D3DXVECTOR3 vPos;
	float& x;
	float& y;

private:
	D3DXVECTOR3 vDest;
	D3DXVECTOR3 vVelocity;
	D3DXVECTOR3 vAcceleration;
	int iCounter, iCounterHalf;
	bool bSmooth;

public:
	MovablePosition();
	MovablePosition(const D3DXVECTOR3 pos);

	void Update();
	void Move(const D3DXVECTOR3 dest, const int frame);
	void MoveBouncily(const D3DXVECTOR3 dest, const int frame);
	void MoveSmoothly(const D3DXVECTOR3 dest, const int frame);
	void SetDestination(const D3DXVECTOR3 dest);
	void Stop();

	bool IsMoving() const;

	operator D3DXVECTOR3&();
	operator const D3DXVECTOR3&() const;
	D3DXVECTOR3 operator +(D3DXVECTOR3);
	MovablePosition& operator =(D3DXVECTOR3);
	MovablePosition& operator +=(D3DXVECTOR3);
};