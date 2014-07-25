#include "CCircle.h"

CCircle::CCircle()
	:pos(0,0,0), radius(0)
{
}
CCircle::CCircle( const D3DXVECTOR3 pos, const float radius )
	:pos(pos), radius(radius)
{}

// 충돌은 &연산자로 검출한다.
// 원 대 원
bool CCircle::operator&( const CCircle& other ) const
{
	if (!(*this)) return false;
	D3DXVECTOR3 d = pos-other.pos;
	return d.x*d.x+d.y*d.y <= (radius + other.radius)*(radius + other.radius);
}

// 원 대 점
bool CCircle::operator&( const D3DXVECTOR3& other) const
{
	if (!(*this)) return false;
	D3DXVECTOR3 d = this->pos-other;
	return d.x*d.x + d.y*d.y <= radius*radius;
}

CCircle::operator bool() const
{
	return radius > 0;
}