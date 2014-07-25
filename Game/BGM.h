/*
	배경음 클래스입니다.
*/
#pragma once

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <XAudio2.h>

#define MAX_BUFFER_SIZE 102400
#define BUFFER_NUM 3

class BGM
{
private:
	OggVorbis_File ovf;
	IXAudio2SourceVoice* source_voice;
	XAUDIO2_BUFFER buffer[BUFFER_NUM];
	BYTE audio_data[BUFFER_NUM][MAX_BUFFER_SIZE];
	
	long loop_start;	// 루프 시작 지점
	int buffer_index;	// submit된 버퍼의 index
	float delta_volume;	// fade시 변하는 volume량
	bool fadeout_and_stop;	// fadeout 후 정지여부
	
	bool loop;			// 루프 여부
	bool playing;		// 재생중 여부
	bool pausing;		// 일시정지 여부
	bool loopback;		// 파일을 끝까지 읽어서 다시 앞으로 돌아가는 여부

public:
	BGM( LPCSTR filename, const bool loop = true, const long loop_start = 0 );
	~BGM();
	
	void Streaming();
	void Update();	// Fade를 위한 Update 메소드

	void Play();
	void Stop();
	void Pause();
	void Resume();

	void FadeIn( const int msec );
	void FadeOut( const int msec );
	void FadeOutAndStop( const int msec );

	void SetLoop( const bool loop );
	void SetVolume( const float volume );	// 0.0 to 1.0

	float GetVolume() const;
	bool IsLoop() const;
	bool IsPlaying() const;
	bool IsPausing() const;

};