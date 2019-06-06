#include "progression.hpp"

#include "events/progression-updated.hpp"

Progression::Progression(EventEmitter& emitter)
	: m_emitter(emitter), m_introImgPath(""), m_introText(""), m_exitText(""),
	m_maxLife(0), m_life(0), m_maxMirrorNumber(0), m_mirrorNumber(0), m_maxSlowNumber(0), m_slowNumber(0)
{}

Progression::~Progression() {}

void Progression::setIntroImgPath(std::string imgPath) {
	m_introImgPath = imgPath;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
const char* Progression::getIntroImgPath() {
	return m_introImgPath.c_str();
}

void Progression::setIntroText(std::string text) {
	m_introText = text;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
const char* Progression::getIntroText() {
	return m_introText.c_str();
}

void Progression::setExitText(std::string text) {
	m_exitText = text;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
const char* Progression::getExitText() {
	return m_exitText.c_str();
}

void Progression::setMaxLife(int value) {
	m_maxLife = value;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
int Progression::getMaxLife() {
	return m_maxLife;
}

void Progression::setLife(int value) {
	m_life = value;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
void Progression::reduceLifeBy(int value) {
	m_life -= value;
	m_emitter.publish<evnt::ProgressionUpdated>();
	if (m_life < 0) {
		// TODO publish death event ? Or only say that progression is updated
	}
}
int Progression::getLife() {
	return m_life;
}

void Progression::setMaxMirrorNumber(int value) {
	m_maxMirrorNumber = value;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
int Progression::getMaxMirrorNumber() {
	return m_maxMirrorNumber;
}

void Progression::setMirrorNumber(int value) {
	m_mirrorNumber = value;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
void Progression::reduceMirrorNumberBy1() {
	if (m_mirrorNumber >= 0) {
		m_mirrorNumber--;
		m_emitter.publish<evnt::ProgressionUpdated>();
	}
}
void Progression::increaseMirrorNumberBy1() {
	if (m_mirrorNumber < m_maxMirrorNumber) {
		m_mirrorNumber++;
		m_emitter.publish<evnt::ProgressionUpdated>();
	}
}
int Progression::getMirrorNumbers() {
	return m_mirrorNumber;
}

void Progression::setMaxSlowNumber(int value) {
	m_maxSlowNumber = value;
	m_emitter.publish<evnt::ProgressionUpdated>();
}
int Progression::getMaxSlowNumber() {
	return m_maxSlowNumber;
}

void Progression::setSlowNumber(int value) {
	m_slowNumber = value;
}
void Progression::reduceSlowNumberBy1() {
	if (m_slowNumber >= 0) {
		m_slowNumber--;
		m_emitter.publish<evnt::ProgressionUpdated>();
	}
}
void Progression::increaseSlowNumberBy1() {
	if (m_slowNumber < m_maxSlowNumber) {
		m_slowNumber++;
		m_emitter.publish<evnt::ProgressionUpdated>();
	}
}
int Progression::getSlowNumbers() {
	return m_slowNumber;
}

void Progression::setLevelNumber(int value) {
	m_levelNumber = value;
}
int Progression::getLevelNumber() {
	return m_levelNumber;
}
