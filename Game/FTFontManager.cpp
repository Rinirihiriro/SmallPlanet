#include "FTFontManager.h"

#include "MyException.h"
#include "Essential.h"

#include <tchar.h>
#include <algorithm>
#include <fstream>


FTFontManager* FTFontManager::obj = NULL;

const char* szFTFontMapFileName = "Manager/FTFontMap%02d.jfm";


FTFontManager::FTFontManager()
{

}

FTFontManager::~FTFontManager()
{
	UnLoadFonts();
}


void FTFontManager::LoadFontMap(const int mapnum)
{
	/*
		** Free Type 폰트 맵 파일 구조
		index_key font_path face_index size outlined
	*/
	
	std::string index_key;
	char font_path_buffer[256];
	int face_index, size, outlined;

	char filename[256];
	sprintf_s(filename, 256, szFTFontMapFileName, mapnum);
	std::ifstream ifs(filename);

	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			// index_key
			ifs>>index_key;
			// 주석행 무시
			if (index_key.substr(0, 2) != "//")
			{
				ZeroMemory(&font_path_buffer, sizeof(font_path_buffer));
				
				ifs>>font_path_buffer>>face_index>>size>>outlined;
				
				FontMap.insert(StrFTFontPair(index_key
					, new FTFont(font_path_buffer, face_index, size, outlined==1)));
			}
			else
			{
				char buf[256];
				ifs.getline(buf, 256);
			}
		}
		ifs.close();
	}
	else
		throw FileNotOpened(filename);
}


FTFontManager& FTFontManager::GetInstance()
{
	if (!obj) obj = new FTFontManager();
	return *obj;
}

void FTFontManager::Release()
{
	SAFE_DELETE(obj);
}


void FTFontManager::LoadFonts(const int mapnum)
{
	if (this->map_num == mapnum) return;
	UnLoadFonts();
	this->map_num = mapnum;
	LoadFontMap(mapnum);
}

void FTFontManager::UnLoadFonts()
{
	if (this->map_num == -1) return;
	this->map_num = -1;

	for (std::map<std::string, FTFont*>::iterator it=FontMap.begin(); it!=FontMap.end(); it++)
		SAFE_DELETE(it->second);
	FontMap.clear();
}


FTFont& FTFontManager::GetFont(const std::string key)
{
	return *FontMap[key];
}

int FTFontManager::GetMapNum() const
{
	return map_num;
}