#include <cmath>
#include "CRect.h"

#include "CRightangledTriangle.h"

// #define IGNORE_AMOUNT 5

CRect::CRect( )
	:center(0,0,0), width(0), height(0)
{
}

CRect::CRect( const float width, const float height )
	:center(0,0,0), width(width), height(height)
{
}

CRect::CRect( const D3DXVECTOR3 center, const float width, const float height )
	:center(center), width(width), height(height)
{
}

// 충돌은 &연산자로 검출한다.
// 사각 대 사각
// return : 얼마나 밀고 들어갔는가
D3DXVECTOR3 CRect::operator&( const CRect& other ) const
{
	if ((bool)(*this) && (bool)other)
	{
		D3DXVECTOR3 collision_vec(0,0,0);
		collision_vec.x = (this->width+other.width)/2-abs(other.center.x-this->center.x);
		collision_vec.y = (this->height+other.height)/2-abs(other.center.y-this->center.y);

		if (collision_vec.x<0 || collision_vec.y<0)
			return D3DXVECTOR3(0,0,0);

		if (collision_vec.x<collision_vec.y)
		{
// 			if (abs(collision_vec.y)>IGNORE_AMOUNT)
				collision_vec.y = 0;
// 			else
// 				collision_vec.x = 0;
		}
		else
		{
// 			if (abs(collision_vec.x)>IGNORE_AMOUNT)
				collision_vec.x = 0;
// 			else
// 				collision_vec.y = 0;
		}

		if (other.center.x<this->center.x)collision_vec.x *= -1;
		if (other.center.y<this->center.y)collision_vec.y *= -1;

		return collision_vec;
	}
	else return D3DXVECTOR3(0,0,0);
}

// 사각 대 삼각
D3DXVECTOR3 CRect::operator&( const CRightangledTriangle& other ) const
{
	return -(other&(*this));
}

// 사각 대 점
bool CRect::operator&( const D3DXVECTOR3& other ) const
{
	if ((bool)(*this))
	{
		return abs(center.x-other.x) < width/2
			&& abs(center.y-other.y) < height/2;
	}
	else return false;
}

CRect::operator bool() const
{
	return width != 0 && height != 0;
}
