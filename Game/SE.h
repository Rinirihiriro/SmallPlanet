/*
	효과음 클래스입니다.
*/
#pragma once

#include <XAudio2.h>

#define SE_MAX_BUFFER_SIZE 1024*1024
#define SE_MAX_SOURCE_VOICE 5

class SE
{
private:
	IXAudio2SourceVoice* source_voice[SE_MAX_SOURCE_VOICE];
	XAUDIO2_BUFFER buffer;
	BYTE audio_data[SE_MAX_BUFFER_SIZE];

public:
	SE( LPCSTR filename );
	~SE();

	void PlayIfStop();	// 재생중이면 무시한다.
	void Play();		// 완전히 처음부터 재생한다.
	void Pause();
	void Resume();	// Pause한 경우 계속 재생할 때
	void Stop();
};