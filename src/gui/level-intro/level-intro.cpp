#include "level-intro.hpp"

#include <NsGui/Button.h>

#include "events/change-game-state.hpp"
#include "events/progression-updated.hpp"
#include "core/game-states/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(LevelIntro) {
	NsMeta<Noesis::TypeId>("LevelIntro");
}

LevelIntro::LevelIntro(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression) {
	m_bindings = *new LevelIntroBindings();
	Initialized() += MakeDelegate(this, &LevelIntro::OnInitialized);
	InitializeComponent();

	std::string title = "LEVEL " + decimalToRoman(m_progression.getLevelNumber());
	m_bindings->setTitle(title.c_str());
	m_bindings->setText(m_progression.getIntroText());
	m_bindings->setImagePath(m_progression.getIntroImgPath());
	m_emitter.on<evnt::ProgressionUpdated>([this](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {
		std::string title = "LEVEL " + decimalToRoman(this->m_progression.getLevelNumber());
		this->m_bindings->setTitle(title.c_str());
		this->m_bindings->setText(this->m_progression.getIntroText());
		this->m_bindings->setImagePath(this->m_progression.getIntroImgPath());
	});
}

void LevelIntro::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-intro.xaml");
}

bool LevelIntro::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onStartLevel);
	NS_CONNECT_EVENT(Noesis::Button, Click, onExit);
	return false;
}

void LevelIntro::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(m_bindings.GetPtr());
}

void LevelIntro::onStartLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(GameState::LEVEL, m_progression.getLevelNumber());
}

void LevelIntro::onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(GameState::TITLE_SCREEN);
}

std::string LevelIntro::decimalToRoman(int num) {
	int decimal[] = { 1000,900,500,400,100,90,50,40,10,9,5,4,1 }; //base values
	char* symbol[] = { "M","CM","D","CD","C","XC","L","XL","X","IX","V","IV","I" };  //roman symbols
	int i = 0;
	std::string result = "";

	while (num) { //repeat process until num is not 0
		while (num / decimal[i]) {  //first base value that divides num is largest base value
			result += symbol[i];    //print roman symbol equivalent to largest base value
			num -= decimal[i];  //subtract largest base value from num
		}
		i++;    //move to next base value to divide num
	}
	return result;
}