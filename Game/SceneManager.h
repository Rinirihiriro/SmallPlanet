#pragma once

#include <string>
#include <map>
#include "Scene.h"

class SceneManager
{
public:

private:
	static SceneManager* obj;
	std::map<std::string, Scene*> sceneMap;
	unsigned int map_num;

public:
	static SceneManager& GetInstance();
	static void Release();
	~SceneManager();

	void LoadScene(const int mapnum);
	void UnLoad();
	Scene& GetSceneStruct(const std::string key);

private:
	SceneManager();
};