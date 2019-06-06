#pragma once

#include "i-audio.hpp"

#include <vector>
#include <fmod.hpp>
#include <fmod_errors.h>

class AudioService : public IAudio {
public:
	AudioService();
	virtual ~AudioService();

	void playSound(int soundID) override;
	void stopSound(int soundID) override;
	void stopAllSounds() override;

private:
	FMOD::System* m_fmodSystem;
	std::vector<FMOD::Sound*> m_sounds;
	FMOD::Channel* m_channel;
};
