#pragma once

#include <NsGui/IView.h>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <memory>

#include "systems/render-system.hpp"
#include "systems/movement-system.hpp"
#include "systems/animation-system.hpp"
#include "systems/construction-system.hpp"
#include "systems/wave-system.hpp"
#include "systems/attack-system.hpp"
#include "systems/health-system.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/left-click-down.hpp"
#include "events/left-click-up.hpp"
#include "core/progression.hpp"
#include "i-game-state.hpp"
#include "gui/level-hud.hpp"

class LevelState : public IGameState {
public:
	LevelState(Progression& progression,
		EventEmitter& emitter,
		AnimationSystem& animationSystem,
		AttackSystem& attackSystem,
		ConstructionSystem& constructionSystem,
		HealthSystem& healthSystem,
		MovementSystem& movementSystem,
		RenderSystem& renderSystem,
		WaveSystem& waveSystem);

	~LevelState();

	void onEnter() override;
	void update(float deltatime) override;
	void onExit() override;

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	LevelHud m_levelHud;

	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickDown>> m_clickDownCon;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>> m_clickUpCon;
};
