#pragma once

enum AudioFiles {
	MUSIC_WAVE,
	EFFECT_CLICK
};

class IAudio {
public:
	virtual void playSound(int soundID) = 0;
	virtual void stopSound(int soundID) = 0;
	virtual void stopAllSounds() = 0;
};
