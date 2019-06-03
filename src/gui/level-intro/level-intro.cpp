#include "level-intro.hpp"

#include <NsGui/Button.h>

NS_IMPLEMENT_REFLECTION(LevelIntro) {
	NsMeta<Noesis::TypeId>("LevelIntro");
}

LevelIntro::LevelIntro(EventEmitter& emitter) : m_emitter(emitter) {
	InitializeComponent();
}

void LevelIntro::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-intro.xaml");
}

bool LevelIntro::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	return false;
}

