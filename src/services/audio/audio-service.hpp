#pragma once

#include "i-audio.hpp"

class AudioService : IAudio {
public:
	AudioService();
	~AudioService();

	void playSound(int soundID) override;
	void stopSound(int soundID) override;
	void stopAllSounds() override;

private:

};
