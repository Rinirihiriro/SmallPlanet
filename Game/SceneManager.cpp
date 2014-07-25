#include "SceneManager.h"
#include "Essential.h"

SceneManager* SceneManager::obj = NULL;

SceneManager& SceneManager::GetInstance()
{
	if (obj==NULL)
		obj = new SceneManager();
	return *obj;
}

void SceneManager::Release()
{
	SAFE_DELETE(obj);
}

SceneManager::~SceneManager()
{
	UnLoad();
}


void SceneManager::LoadScene(const int mapnum)
{
	UnLoad();
}

void SceneManager::UnLoad()
{

}

Scene& SceneManager::GetSceneStruct(const std::string key)
{
	return *sceneMap[key];
}

SceneManager::SceneManager()
{

}