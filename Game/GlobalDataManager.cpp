#include <fstream>

#include "GlobalDataManager.h"
#include "Essential.h"
#include "MyException.h"

#include "FileEncryptor.h"

// 데이터 저장 형식 파일
const char* szDataFormatFileName = "Manager/GameDataFormat.jdf";
// 게임 데이터 파일
const char* szSaveFileName = "Data/GameData.sav";


GlobalDataManager* GlobalDataManager::obj = NULL;

GlobalDataManager::GlobalDataManager()
{
	LoadGameDataFormat();
	LoadData();
}


void GlobalDataManager::LoadGameDataFormat()
{
	/*
		GameDataFormat파일 형식 :
		index_key 데이터사이즈(바이트 수) 초기값
		index_key 데이터사이즈(바이트 수) 초기값
		...
	*/
	std::ifstream ifs(szDataFormatFileName);
	std::string index_key;
	GameDataFormat gdf;
	unsigned int index=0;

	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			// index_key
			ifs>>index_key;
			if (ifs.eof()) break;
			// 주석행 무시
			if (index_key.substr(0, 2) != "//")
			{
				ifs>>gdf.size>>gdf.init_value;
				// 데이터 형식 맞추기
				if (gdf.size==DS_BYTE || gdf.size==DS_WORD || gdf.size==DS_DWORD || gdf.size==DS_QWORD)
				{
					gdf.index = index;
					index += gdf.size;
					gamedataFormat.insert(std::pair<std::string, GameDataFormat>(index_key, gdf));
				}
			}
			else
			{
				char buf[256];
				ifs.getline(buf, 256);
			}
		}
		data_size = index;
		gamedata = new char[data_size];
		InitData();
		ifs.close();
	}
	else
		throw FileNotOpened(szDataFormatFileName);
}


GlobalDataManager::~GlobalDataManager()
{
	SaveData();
	SAFE_DELETE_ARR(gamedata);
}

GlobalDataManager& GlobalDataManager::GetInstance()
{
	if (obj == NULL)
		obj = new GlobalDataManager();
	return *obj;
}

void GlobalDataManager::Release()
{
	SAFE_DELETE(obj);
}

void GlobalDataManager::InitData()
{
	for (auto it=gamedataFormat.begin(); it!=gamedataFormat.end(); it++)
	{
		switch (it->second.size)
		{
		case DS_BYTE:
			gamedata[it->second.index] = (char)it->second.init_value;
			break;
		case DS_WORD:
			*((short*)&gamedata[it->second.index]) = (short)it->second.init_value;
			break;
		case DS_DWORD:
			*((int*)&gamedata[it->second.index]) = (int)it->second.init_value;
			break;
		case DS_QWORD:
			*((long long int*)&gamedata[it->second.index]) = it->second.init_value;
			break;
		}
	}
}

void GlobalDataManager::LoadData()
{
	InitData();
	FileEncryptor decryptor;
	char* out=0;
	int size=0;
	decryptor.OpenAndDecrypt(szSaveFileName, &out, &size);
	if (out!=NULL)
	{
		memcpy_s(gamedata, data_size, out, size);
	}
}

void GlobalDataManager::SaveData()
{
	std::ofstream ofs(szSaveFileName, std::ios::binary);
	if (ofs.is_open())
	{
		FileEncryptor encryptor;
		ofs.write(gamedata, data_size);
		ofs.close();
		encryptor.EncryptAndOverwrite(szSaveFileName);
	}
}

// return : data size
unsigned int GlobalDataManager::GetData(std::string key, void* out)
{
	if (gamedataFormat.find(key)==gamedataFormat.end()) return 0;
	if (out==NULL) return gamedataFormat[key].size;

	switch (gamedataFormat[key].size)
	{
	case DS_BYTE:
		*((char*)out) = gamedata[gamedataFormat[key].index];
		break;
	case DS_WORD:
		*((short*)out) = *((short*)&gamedata[gamedataFormat[key].index]);
		break;
	case DS_DWORD:
		*((long*)out) = *((long*)&gamedata[gamedataFormat[key].index]);
		break;
	case DS_QWORD:
		*((long long int*)out) = *((long long int*)&gamedata[gamedataFormat[key].index]);
		break;
	}
	
	return gamedataFormat[key].size;
}

void GlobalDataManager::SetData(std::string key, void* data)
{
	if (gamedataFormat.find(key)==gamedataFormat.end()) return;

	switch (gamedataFormat[key].size)
	{
	case DS_BYTE:
		gamedata[gamedataFormat[key].index] = *((char*)data);
		break;
	case DS_WORD:
		*((short*)&gamedata[gamedataFormat[key].index]) = *((short*)data);
		break;
	case DS_DWORD:
		*((long*)&gamedata[gamedataFormat[key].index]) = *((long*)data);
		break;
	case DS_QWORD:
		*((long long int*)&gamedata[gamedataFormat[key].index]) = *((long long int*)data);
		break;
	}
}
