#ifdef _WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include <cstdlib>
#include <iostream>
#include <glad/glad.h>
#include <debugbreak/debugbreak.h>
#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "core/game.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "core/game-states/i-game-state.hpp"
#include "logger/gl-log-handler.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/inputs/left-click-up.hpp"
#include "events/inputs/left-click-down.hpp"
#include "events/inputs/right-click-up.hpp"
#include "events/inputs/right-click-down.hpp"
#include "events/inputs/mouse-scrolled.hpp"
#include "events/interactions/translate-view.hpp"
#include "events/interactions/scale-view.hpp"
#include "events/inputs/mouse-move.hpp"
#include "events/interactions/change-cursor.hpp"
#include "events/start-wave.hpp"
#include "events/change-game-state.hpp"

// DEBUG ONLY
#include "core/game-states/level-state.hpp"


int main(int argc, char** argv) {
#ifdef _WIN32 // Check memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Game init
	EventEmitter emitter{};
	Game game(emitter);
	if (game.init() == EXIT_FAILURE) {
		debug_break();
		return EXIT_FAILURE;
	}

	// Debug Window
	bool bDisplayDebugWindow = false;
	bool bDrawPhysic = false;
	bool bAllowClickEvent = true;
	bool bWireframe = false;

	// Game loop
	bool bQuit = false;
	double deltatime = TARGET_DELTA_MS;
	Uint64 beginTicks = SDL_GetPerformanceCounter();
	while (!bQuit) {

		// Imgui main debug window
		if(bDisplayDebugWindow){
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(game.getWindow());
			ImGui::NewFrame();
			ImGui::Begin("Main debug window");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			if (game.m_levelState != nullptr) {
				switch (game.m_levelState->getInteractionState()) {
				case LevelInteractionState::FREE:
					ImGui::Text("Current level state: FREE");
					break;

				case LevelInteractionState::BUILD:
					ImGui::Text("Current level state: BUILD");
					break;

				case LevelInteractionState::INVALID:
					ImGui::Text("Current level state: INVALID");
					break;

				case LevelInteractionState::OPTIONS:
					ImGui::Text("Current level state: OPTIONS");
					break;

				case LevelInteractionState::ROTATE:
					ImGui::Text("Current level state: ROTATE");
					break;

				default:
					break;
				}
				
			}
			
			if (ImGui::Button("Load level 1")) {
				emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_INTRO, 1);
			}

			if (ImGui::Button("Load level 2")) {
				emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_INTRO, 2);
			}

			if (ImGui::Button("Load level 3")) {
				emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_INTRO, 3);
			}

			if (ImGui::Button("Load level 4")) {
				emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_INTRO, 4);
			}

			if (ImGui::Button("Load level 5")) {
				emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_INTRO, 5);
			}

			// Check cursor position
			if (ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered()) {
				bAllowClickEvent = false;
			} else {
				bAllowClickEvent = true;
			}
			ImGui::End();
		}

		// Game update & render
		{
			game.update(deltatime/1000); //In seconds
			if (bDisplayDebugWindow) {
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
		}
		
		// Handle inputs
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			const glm::vec2 normMousePos = glm::vec2(
				imaths::rangeMapping(e.button.x, 0, WIN_WIDTH, 0, PROJ_WIDTH),
				imaths::rangeMapping(WIN_HEIGHT - e.button.y, 0, WIN_HEIGHT, 0, PROJ_HEIGHT)
			);

			if (e.type == SDL_QUIT) {
				bQuit = true;
				break;
			}

			switch (e.type) {
			case SDL_MOUSEBUTTONUP:
				if (bAllowClickEvent && e.button.button == SDL_BUTTON_LEFT)
					emitter.publish<evnt::LeftClickUp>(normMousePos, glm::vec2(e.button.x, e.button.y));
				else if (bAllowClickEvent && e.button.button == SDL_BUTTON_RIGHT)
					emitter.publish<evnt::RightClickUp>(normMousePos, glm::vec2(e.button.x, e.button.y));
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (bAllowClickEvent && e.button.button == SDL_BUTTON_LEFT)
					emitter.publish<evnt::LeftClickDown>(normMousePos, glm::vec2(e.button.x, e.button.y));
				else if (bAllowClickEvent && e.button.button == SDL_BUTTON_RIGHT)
					emitter.publish<evnt::RightClickDown>(normMousePos, glm::vec2(e.button.x, e.button.y));
				break;

			case SDL_MOUSEMOTION:
				emitter.publish<evnt::MouseMove>(normMousePos, glm::vec2(e.button.x, e.button.y));
				break;
			
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == 'p') {
					if (bWireframe) {
						GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
					} else {
						GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
					}
					bWireframe = !bWireframe;
				} else if (e.key.keysym.scancode == SDL_SCANCODE_UP) {
					emitter.publish<evnt::TranslateView>(0.0f, 1.0f);
				} else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
					emitter.publish<evnt::TranslateView>(0.0f, -1.0f);
				} else if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
					emitter.publish<evnt::TranslateView>(-1.0f, 0.0f);
				} else if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
					emitter.publish<evnt::TranslateView>(1.0f, 0.0f);
				}else if (e.key.keysym.sym == 'd') {
					bDisplayDebugWindow = !bDisplayDebugWindow;
				}
				break;

			case SDL_MOUSEWHEEL:
				emitter.publish<evnt::MouseScrolled>((int)e.motion.x);
				//if (e.motion.x > 0.0f) {
				//	emitter.publish<evnt::ScaleView>(1.0f);
				//} else {
				//	emitter.publish<evnt::ScaleView>(-1.0f);
				//}
				break;
			}
		}

		// Update window
		SDL_GL_SwapWindow(game.getWindow());

		// Check framerate
		// FIXME too rapid on portable computers
		{
			Uint64 endTicks = SDL_GetPerformanceCounter();
			deltatime = (double)(endTicks - beginTicks) * 1000 / (double)SDL_GetPerformanceFrequency();

			// If deltaTime too big, we must have resumed from a breakpoint
			// So frame lock on the target framerate
			if (deltatime > MAX_DELTA_MS) {
				deltatime = TARGET_DELTA_MS;
			}

			// If deltaTime too small, wait to get to the target framerate
			if (deltatime < TARGET_DELTA_MS) {
				SDL_Delay((Uint32)(TARGET_DELTA_MS - deltatime));
			}
			beginTicks = endTicks;
		}
	}
	return EXIT_SUCCESS;
}
