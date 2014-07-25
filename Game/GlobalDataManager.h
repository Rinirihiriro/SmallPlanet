#pragma once

#include <map>
#include <string>

class GlobalDataManager
{
public:
	enum GameDataSize
	{
		DS_BYTE = 1
		, DS_WORD = 2
		, DS_DWORD = 4
		, DS_QWORD = 8
	};

private:
	struct GameDataFormat
	{
		unsigned int index;
		unsigned int size;
		long long int init_value;
	};

	static GlobalDataManager* obj;
	char* gamedata;
	unsigned int data_size;

	std::map<std::string, GameDataFormat> gamedataFormat;

private:
	GlobalDataManager();
	
	void LoadGameDataFormat();

public:
	static GlobalDataManager& GetInstance();
	static void Release();
	~GlobalDataManager();

	void InitData();
	void LoadData();
	void SaveData();

	unsigned int GetData(std::string key, void* out);
	void SetData(std::string key, void* data);
};