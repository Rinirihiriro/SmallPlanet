/*
	����� Ŭ�����Դϴ�.
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
	
	long loop_start;	// ���� ���� ����
	int buffer_index;	// submit�� ������ index
	float delta_volume;	// fade�� ���ϴ� volume��
	bool fadeout_and_stop;	// fadeout �� ��������
	
	bool loop;			// ���� ����
	bool playing;		// ����� ����
	bool pausing;		// �Ͻ����� ����
	bool loopback;		// ������ ������ �о �ٽ� ������ ���ư��� ����

public:
	BGM( LPCSTR filename, const bool loop = true, const long loop_start = 0 );
	~BGM();
	
	void Streaming();
	void Update();	// Fade�� ���� Update �޼ҵ�

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