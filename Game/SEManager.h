/*
	SE를 관리하는 클래스
*/
#pragma once

#include "SE.h"
#include <map>
#include <string>

typedef std::pair<std::string, SE*> StrSEPair;

class SEManager
{
private:
	static SEManager* obj;
	std::map<std::string, SE*> SEMap;
	int map_num;

private:
	SEManager();
	
	void LoadSEMap(const int mapnum);

public:
	static SEManager& GetInstance();
	static void Release();
	~SEManager();
	void LoadSEs( const int mapnum );
	void UnloadSEs();

	void PlaySE( const std::string index, const bool play_if_stop = false );
	void StopSE( const std::string index );
	void PauseSE( const std::string index );
	void ResumeSE( const std::string index );

	int GetMapNum() const;
};