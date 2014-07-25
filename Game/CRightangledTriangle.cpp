#include <cmath>
#include "CRightangledTriangle.h"

#include "CRect.h"

CRightangledTriangle::CRightangledTriangle( )
	:center(0,0,0), width(0), height(0)
{

}

CRightangledTriangle::CRightangledTriangle( const float width, const float height )
	:center(0,0,0), width(width), height(height)
{

}

CRightangledTriangle::CRightangledTriangle( const D3DXVECTOR3 center, const float width, const float height )
	:center(center), width(width), height(height)
{

}


D3DXVECTOR3 CRightangledTriangle::realCenter()
{
	return center+D3DXVECTOR3(width, height, 0)/2;
}

void CRightangledTriangle::realCenter(D3DXVECTOR3 center)
{
	this->center = center-D3DXVECTOR3(width, height, 0)/2;
}

// 충돌은 &연산자로 검출한다.
// 삼각 대 삼각
// D3DXVECTOR3 CRightangledTriangle::operator&( const CRightangledTriangle& other ) const
// {
// }

// 삼각 대 사각
D3DXVECTOR3 CRightangledTriangle::operator&( const CRect& other ) const
{
	if ((*this) && other)
	{
		D3DXVECTOR3 rect_collision = CRect(center+D3DXVECTOR3(width*0.5f, height*0.5f, 0), abs(width), abs(height))&other;
		// 빗변에서 충돌하므로 삼각형을 사각형으로 보고 충돌검출한다.
		if (rect_collision==D3DXVECTOR3(0,0,0))
			return D3DXVECTOR3(0,0,0);

		/*
			1. 빗변의 법선 벡터를 구한다.
			2. 기준점을 삼각형의 직각 꼭짓점으로 바꾼다.
			3. 사각형과 삼각형을 투영해 길이를 구한다.
			4. 겹치는 길이를 구한다.
		*/
		// 1. 빗변의 법선 벡터를 구한다.
		D3DXVECTOR3 normal_vec = (center+D3DXVECTOR3(0,height,0))-(center+D3DXVECTOR3(width,0,0));
		
		D3DXVECTOR3 temp_vec(0,0,0);
		bool luslide = height*width>0;
		if (luslide)	// 빗면이 오른쪽 위 방향
		{
			temp_vec.x = normal_vec.y;
			temp_vec.y = -normal_vec.x;
		}
		else	// 왼쪽 위 방향
		{
			temp_vec.x = -normal_vec.y;
			temp_vec.y = normal_vec.x;
		}
		D3DXVec3Normalize(&normal_vec,&temp_vec);

		// 2. 기준점을 삼각형의 직각 꼭짓점으로 바꾼다.
		D3DXVECTOR3 tri_center(0,0,0), rect_center = other.center-this->center;

		// 3. 사각형과 삼각형을 투영해 길이를 구한다.
		// 삼각형 투영
		D3DXVECTOR3 tri_hypo(width,0,0), tri_projection_center(0,0,0);
		tri_hypo = D3DXVec3Dot(&tri_hypo, &normal_vec)*normal_vec;
		tri_projection_center = (tri_center+tri_hypo)*0.5f;
		
		// 사각형 투영
		D3DXVECTOR3 rect_upper(0,0,0), rect_lower(0,0,0), rect_projection_center;
		if (luslide)	// 빗면이 오른쪽 위 방향
		{
			rect_upper = rect_center+D3DXVECTOR3(-other.width*0.5f,-other.height*0.5f,0);
			rect_lower = rect_center+D3DXVECTOR3(other.width*0.5f,other.height*0.5f,0);
		}
		else	// 왼쪽 위 방향
		{
			rect_upper = rect_center+D3DXVECTOR3(other.width*0.5f,-other.height*0.5f,0);
			rect_lower = rect_center+D3DXVECTOR3(-other.width*0.5f,other.height*0.5f,0);
		}
		rect_upper = D3DXVec3Dot(&rect_upper, &normal_vec)*normal_vec;
		rect_lower = D3DXVec3Dot(&rect_lower, &normal_vec)*normal_vec;
		rect_projection_center = (rect_upper+rect_lower)*0.5f;

		// 삼각형과 사각형 사이 거리
		D3DXVECTOR3 hypo_collision(0,0,0);
		hypo_collision.x = (abs(tri_hypo.x)+abs(rect_upper.x-rect_lower.x))*0.5f
			-abs(rect_projection_center.x-tri_projection_center.x);
		hypo_collision.y = (abs(tri_hypo.y)+abs(rect_upper.y-rect_lower.y))*0.5f
			-abs(rect_projection_center.y-tri_projection_center.y);

		if (hypo_collision.x < 0 || hypo_collision.y < 0)
			return D3DXVECTOR3(0,0,0);

		if (rect_projection_center.x<tri_projection_center.x) hypo_collision.x *= -1;
		if (rect_projection_center.y<tri_projection_center.y) hypo_collision.y *= -1;

		// 4. 겹치는 길이를 구한다.
		float hypo_coli_len = D3DXVec3Length(&hypo_collision);
		if (hypo_coli_len<abs(rect_collision.x) || hypo_coli_len<abs(rect_collision.y))
			return hypo_collision;
		else
			return rect_collision;
	}
	else return D3DXVECTOR3(0,0,0);
}

// 삼각 대 점
bool CRightangledTriangle::operator&( const D3DXVECTOR3& other ) const
{
	if (*this)
	{
		// 빗변에서 충돌하므로 삼각형을 사각형으로 보고 충돌검출한다.
		if (!(CRect(center+D3DXVECTOR3(width*0.5f, height*0.5f, 0), abs(width), abs(height))&other))
			return false;

		// 1. 빗변의 법선 벡터를 구한다.
		D3DXVECTOR3 normal_vec = (center+D3DXVECTOR3(0,height,0))-(center+D3DXVECTOR3(width,0,0));
		
		D3DXVECTOR3 temp_vec(0,0,0);
		bool luslide = height*width>0;
		if (luslide)	// 빗면이 오른쪽 위 방향
		{
			temp_vec.x = normal_vec.y;
			temp_vec.y = -normal_vec.x;
		}
		else	// 왼쪽 위 방향
		{
			temp_vec.x = -normal_vec.y;
			temp_vec.y = normal_vec.x;
		}
		D3DXVec3Normalize(&normal_vec,&temp_vec);

		// 2. 기준점을 삼각형의 직각 꼭짓점으로 바꾼다.
		D3DXVECTOR3 tri_center(0,0,0), other_center = other-this->center;

		// 3. 점과 삼각형을 투영해 길이를 구한다.
		// 삼각형 투영
		D3DXVECTOR3 tri_hypo(width,0,0), tri_projection_center(0,0,0);
		tri_hypo = D3DXVec3Dot(&tri_hypo, &normal_vec)*normal_vec;
		tri_projection_center = (tri_center+tri_hypo)*0.5f;
		
		// 점 투영
		D3DXVECTOR3 other_projection = D3DXVec3Dot(&other_center, &normal_vec)*normal_vec;

		// 4. 겹치는지 확인한다.
		return fabs(other_projection.x) < fabs(tri_hypo.x);
	}
	else return false;
}


CRightangledTriangle::operator bool() const
{
	return width !=0 && height != 0;
}