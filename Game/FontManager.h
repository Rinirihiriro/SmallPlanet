/*
	폰트를 관리하는 클래스
*/
#pragma once

#include "JDirectX.h"
#include "Font.h"
#include <map>
#include <string>
#include <vector>

typedef std::pair<std::string, Font*> StrFontPair;

class FontManager
{
private:
	static FontManager* obj;
	static const JDirectX* pDirectx;
	std::map<std::string, Font*> FontMap;
	std::vector<std::wstring> FontResVec;
	int map_num;

private:
	FontManager();
	
	void LoadFontResource();
	void LoadFontMap(const int mapnum);

public:
	static FontManager& GetInstance();
	static void Release();
	~FontManager();

	void LoadFonts(const int mapnum);
	void UnLoadFonts();

	Font& GetFont(const std::string key);
	int GetMapNum() const;

	void OnLostDevice();
	void OnResetDevice();

	static void SetCDirectX(const JDirectX* cdirectx);
};