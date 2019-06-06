#pragma once

#include <string>
#include "events/handlers/event-emitter.hpp"

// TODO find a way to limit the set values of the itd only by the map ?

class Progression {
public:
	Progression(EventEmitter& emitter);
	~Progression();

	void resetValues();

	void setIntroImgPath(std::string imgPath);
	const char* getIntroImgPath();

	void setIntroText(std::string text);
	const char* getIntroText();

	void setExitText(std::string text);
	const char* getExitText();

	void setMaxLife(int value);
	int getMaxLife();

	void setLife(int value);
	void reduceLifeBy(int value);
	int getLife();

	void setMaxMirrorNumber(int value);
	int getMaxMirrorNumber();

	void setMirrorNumber(int value);
	void reduceMirrorNumberBy1();
	void increaseMirrorNumberBy1();
	int getMirrorNumbers();

	void setMaxSlowNumber(int value);
	int getMaxSlowNumber();

	void setSlowNumber(int value);
	void reduceSlowNumberBy1();
	void increaseSlowNumberBy1();
	int getSlowNumbers();

	int getMaxRobotNumber();
	int getRobotNumber();
	void setMaxRobotNumber(int value);
	void setRobotNumber(int value);
	void decreaseRobotNumber();

	int getMaxKamikazeNumber();
	int getKamikazeNumber();
	void setMaxKamikazeNumber(int value);
	void setKamikazeNumber(int value);
	void decreaseKamikazeNumber();

	void setLevelNumber(int value);
	int getLevelNumber();

private:
	EventEmitter& m_emitter;

	std::string m_introImgPath;
	std::string m_introText;
	std::string m_exitText;
	int m_maxLife;
	int m_life;
	int m_maxSlowNumber;
	int m_slowNumber;
	int m_maxMirrorNumber;
	int m_mirrorNumber;
	int m_maxRobotNumber;
	int m_robotNumber;
	int m_maxKamikazeNumber;
	int m_kamikazeNumber;
	int m_levelNumber;
};
