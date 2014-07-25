/*
	게임 내에 쓰이는 텍스처를 모아두는 클래스.
*/
#pragma once

#include "Texture.h"
#include <map>
#include <vector>
#include <string>

typedef std::pair<std::string, Texture*> StrTexPair;

class TextureManager
{
public:
	/*
	std::vector<int> vLoadingMap;
	std::vector<int> vUnloadingMap;
	*/
	std::vector<int> vMapNum;

private:
	struct TextureData
	{
		Texture* txTexture;
		int iMapNum;
		TextureData():txTexture(NULL), iMapNum(-1){}
		TextureData(Texture* texture, int mapnum): txTexture(texture), iMapNum(mapnum){}
	};

	static TextureManager* obj;
// 	static CRITICAL_SECTION cs;
// 	static bool bThreadEnd;
// 
// 	HANDLE LoadingThread;

	Texture *txBlank;
	std::map<std::string, TextureData> TextureMap;

private:
	TextureManager();
	// static DWORD WINAPI LoadingThreadProc( LPVOID lpParameter );


public:
	static TextureManager& GetInstance();
	static void Release();
	~TextureManager();

	void LoadTextureMap(const int mapnum);

	void LoadTextures( const int mapnum );
	void UnloadTextures(const int mapnum);
	void UnloadAllTextures();

	Texture* GetTexture( const std::string key );
	Texture* GetBlank() const;
	std::vector<int> GetTextureMapIds() const;

	bool IsLoadedMap(const int mapnum) const;
	// bool IsLoading() const;
};