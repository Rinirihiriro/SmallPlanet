#include "Scene.h"
#include "Essential.h"

#include <algorithm>

Scene::Scene(const D3DCOLOR back_color)
	:back_color(back_color)
{

}

Scene::~Scene()
{
	EraseAllObjects();
}

void Scene::AdjustAdding()
{
	for each (auto obj in objAddArr)
	{
		objArr.push_back(obj);
		OnAddObject(obj);
	}
	objAddArr.clear();
}

void Scene::AdjustErasing()
{
	while (!objEraseArr.empty())
	{
		objEraseArr_vec.push_back(objEraseArr.begin()->first);
		objEraseArr.erase(objEraseArr.begin());
	}
	while (!objEraseArr_vec.empty())
	{
		OnEraseObject(*objEraseArr_vec.back());
		SAFE_DELETE(*objEraseArr_vec.back());
		objArr.erase(objEraseArr_vec.back());
		objEraseArr_vec.pop_back();
	}
}

void Scene::AddObject( GameObj* object )
{
	objAddArr.push_back(object);
}

void Scene::EraseObject( std::vector<GameObj*>::iterator object_iterator )
{
	objEraseArr[object_iterator] = 1;
}

void Scene::EraseObject( const GameObj* const object )
{
	auto ind = find(objArr.begin(), objArr.end(), object);
	if (ind != objArr.end())
		objEraseArr[ind] = 1;
}

void Scene::Update()
{
	AdjustAdding();
	for ( auto it = objArr.begin(); it < objArr.end(); ++it)
	{
		switch ((*it)->Update())
		{
		case GameObj::STATE_DELETED:
			EraseObject(it);
		}
	}
	AdjustErasing();
	std::stable_sort(objArr.begin(), objArr.end(), GameObj::CompairZ());
}

void Scene::Render()
{
	RenderAllObject();
}


void Scene::RenderAllObject()
{
	for each (auto obj in objArr)
		obj->Draw();
}

void Scene::EraseAllObjects()
{
	AdjustAdding();
	for each (auto obj in objArr)
		SAFE_DELETE(obj);
	objArr.clear();
	objEraseArr.clear();
}