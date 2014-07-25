#include "MyException.h"
#include "JXAudio.h"
#include "Essential.h"

JXAudio::JXAudio()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT result;
	result = XAudio2Create( &pXAudio );
	if ( FAILED( result ) ) throw ObjectWasNotCreated("XAudio2", result);
	result = pXAudio->CreateMasteringVoice( &pXAudioMastering );
	if ( FAILED( result ) )
	{
		result = pXAudio->CreateMasteringVoice( &pXAudioMastering, 2, 44100, 0, 0, NULL );
		if ( FAILED( result ) )
		{
			result = pXAudio->CreateMasteringVoice( &pXAudioMastering, 2, 48000, 0, 0, NULL );
			if ( FAILED( result ) ) throw ObjectWasNotCreated("XAudio2 Mastering Voice", result);
		}
	}
}

JXAudio::~JXAudio()
{
	pXAudioMastering->DestroyVoice(); pXAudioMastering = NULL;
	SAFE_RELEASE(pXAudio);
	CoUninitialize();
}