#include "audio-service.hpp"

#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>

AudioService::AudioService() : m_fmodSystem(nullptr) 
{
	// Init Fmod
	FMOD_RESULT fmodResult;
	fmodResult = FMOD::System_Create(&m_fmodSystem);
	if (fmodResult != FMOD_OK) {
		spdlog::error("[FMOD] {} {}", fmodResult, FMOD_ErrorString(fmodResult));
		debug_break();
	}
	fmodResult = m_fmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (fmodResult != FMOD_OK) {
		spdlog::error("[FMOD] {} {}", fmodResult, FMOD_ErrorString(fmodResult));
		debug_break();
	}

	// Load sounds 
	// TODO Check ressource manager design for better efficiency + Find a more robust design
	// Load all sounds ? Or only repetitive ones ?
	
	FMOD::Sound* sound;
	m_fmodSystem->createSound("res/audio/crowd.mp3", FMOD_DEFAULT, 0, &sound);
	m_sounds.push_back(sound);
}

AudioService::~AudioService() {
	for (std::size_t i = 0; i < m_sounds.size(); ++i) {
		FMOD::Sound* sound = m_sounds.at(i);
		sound->release();
	}
}

void AudioService::playSound(int soundID) {
	FMOD::Channel* channel = 0;
	m_fmodSystem->playSound(m_sounds.at(soundID), 0, false, &channel);
}

void AudioService::stopSound(int soundID) {
}

void AudioService::stopAllSounds() {

}
