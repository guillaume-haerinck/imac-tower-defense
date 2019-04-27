#include "audio-service.hpp"

#include <fmod.hpp>
#include <fmod_errors.h>
#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>

AudioService::AudioService() {
	FMOD_RESULT fmodResult;
	FMOD::System* fmodSystem = nullptr;
	FMOD::Channel* channel = 0;

	fmodResult = FMOD::System_Create(&fmodSystem);
	if (fmodResult != FMOD_OK) {
		spdlog::error("[FMOD] {} {}", fmodResult, FMOD_ErrorString(fmodResult));
		debug_break();
	}

	fmodResult = fmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (fmodResult != FMOD_OK) {
		spdlog::error("[FMOD] {} {}", fmodResult, FMOD_ErrorString(fmodResult));
		debug_break();
	}

	//FMOD::Sound* mySound;
	//fmodSystem->createSound("res/audio/crowd.mp3", FMOD_DEFAULT, 0, &mySound);
	//fmodSystem->playSound(mySound, 0, false, &channel);
	//mySound->release();
}

AudioService::~AudioService() {
}

void AudioService::playSound(int soundID) {
}

void AudioService::stopSound(int soundID) {
}

void AudioService::stopAllSounds() {
}
