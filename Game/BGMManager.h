/*
	BGM을 관리하는 클래스
*/
#pragma once

#include "BGM.h"
#include <map>
#include <vector>
#include <string>

typedef std::pair<std::string, BGM*> StrBGMPair;

class BGMManager
{
private:
	static BGMManager* obj;
	std::map<std::string, BGM*> BGMMap;
	int map_num;
	HANDLE StreamingThread;
	static CRITICAL_SECTION cs;
	static bool bThreadEnd;

private:
	BGMManager();
	static DWORD WINAPI StreamingThreadProc( LPVOID lpParameter );

	void LoadBGMMap(const int mapnum);

public:
	static BGMManager& GetInstance();
	static void Release();
	~BGMManager();

	void LoadBGMs( const int mapnum );
	void UnloadBGMs();

// 	void UpdateBGMs();

	void PlayBGM( const std::string index );
	void StopBGM( const std::string index );
	void PauseBGM( const std::string index );
	void ResumeBGM( const std::string index );

	void PlaySingleBGM( const std::string index );
	void StopAllBGMs();
	void ResumePausingBGMs();

	void FadeInBGM( const std::string index, const int frame );
	void FadeInAllBGMs( const int frame );
	void FadeOutBGM( const std::string index, const int frame );
	void FadeOutAllBGMs( const int frame );
	void FadeOutBGMAndStop( const std::string index, const int frame );
	void FadeOutAllBGMsAndStop( const int frame );

	void SetSingleBGMLoop( const std::string index, const bool loop );
	void SetBGMsLoop( const bool loop );
	void SetSingleBGMVolume( const std::string index, const float volume );
	void SetBGMsVolume( const float volume );

	float GetBGMVolume( const std::string index );
	bool IsBGMLoop( const std::string index );
	bool IsSingleBGMPlaying( const std::string index );
	bool IsBGMPlaying();
	bool IsBGMPausing( const std::string index );

	std::vector<std::string> GetPlayingBGMs();

	int GetMapNum() const;

};