/*
	ȿ���� Ŭ�����Դϴ�.
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

	void PlayIfStop();	// ������̸� �����Ѵ�.
	void Play();		// ������ ó������ ����Ѵ�.
	void Pause();
	void Resume();	// Pause�� ��� ��� ����� ��
	void Stop();
};