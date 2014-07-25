/*
	게임 장면 클래스. 오브젝트의 관리도 책임진다.
*/
#pragma once

#include <vector>
#include <map>
#include "GameObj.h"

class Scene
{
private:
	std::vector<std::vector<GameObj*>::iterator> objEraseArr_vec;

	unsigned int ctexture_id;
	unsigned int bgm_id;
	unsigned int se_id;
	unsigned int font_id;

protected:
	std::vector<GameObj*> objAddArr;
	std::map<std::vector<GameObj*>::iterator, int> objEraseArr;

public:
	std::vector<GameObj*> objArr;
	D3DCOLOR back_color;

protected:
// 	void AdjustAdding();
// 	void AdjustErasing();
	virtual void OnAddObject(GameObj* const obj){};
	virtual void OnEraseObject(GameObj* const obj){};

public:
	Scene(const D3DCOLOR back_color);
	virtual ~Scene();

	virtual void InputProc() = 0;
	virtual void Update();
	virtual void Render();

	void RenderAllObject();

	void AddObject( GameObj* object );
	void EraseObject( std::vector<GameObj*>::iterator object_iterator );
	void EraseObject( const GameObj* const object );
	void EraseAllObjects();

	void AdjustAdding();
	void AdjustErasing();
};