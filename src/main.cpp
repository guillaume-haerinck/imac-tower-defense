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
#include <Box2D/Box2D.h>

#include "core/game.hpp"
#include "core/maths.hpp"
#include "core/constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "events/handlers/event-emitter.hpp"
#include "events/left-click.hpp"
#include "events/mouse-move.hpp"
#include "events/start-wave.hpp"

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

	// Init Physics
	/*
	b2Vec2 gravity(0.0f, 0.0f);
	std::shared_ptr<b2World> physicWorld = std::make_shared<b2World>(gravity);

	// Link debug draw to physics
	IDebugDraw& debugDraw = locator::debugDraw::ref();
	debugDraw.setProjMat(projMat);
	debugDraw.setViewMat(viewMat);
	physicWorld->SetDebugDraw(&debugDraw);
	*/

	// Debug Window
	bool bDrawPhysic = false;
	bool bClickEvent = true;
	bool bStartWave = false;
	bool bWireframe = false;

	// Test audio service
	// IAudio& audioService = entt::ServiceLocator<IAudio>::ref();
	//audioService.playSound(audioFiles::CROWD_1);

	// Game loop
	glm::vec2 normMousePos = glm::vec2(0.0f);
	bool bQuit = false;
	double deltatime = TARGET_DELTA_MS;
	Uint64 beginTicks = SDL_GetPerformanceCounter();
	while (!bQuit) {
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		// Imgui main debug window
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(game.getWindow());
			ImGui::NewFrame();
			ImGui::Begin("Main debug window");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			// Physic
			/*
			ImGui::Checkbox("Draw physic", &bDrawPhysic);
			if (bDrawPhysic) {
				debugDraw.SetFlags(b2Draw::e_shapeBit + b2Draw::e_centerOfMassBit + b2Draw::e_aabbBit + b2Draw::e_jointBit + b2Draw::e_pairBit);
			} else {
				debugDraw.ClearFlags(b2Draw::e_shapeBit + b2Draw::e_centerOfMassBit + b2Draw::e_aabbBit + b2Draw::e_jointBit + b2Draw::e_pairBit);
			}
			*/

			if (ImGui::Button("Send wave event")) {
				emitter.publish<evnt::StartWave>(10);
			}

			// Check cursor position
			if (ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered()) {
				bClickEvent = false;
			} else {
				bClickEvent = true;
			}
			ImGui::End();
		}

		// Game update & render
		{
			game.update(deltatime);
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		
		/* Handle inputs */
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				bQuit = true;
				break;
			}

			switch (e.type) {
			case SDL_MOUSEBUTTONUP:
				// Send click event
				if (bClickEvent) {
					if (e.button.button == SDL_BUTTON_LEFT) {
						normMousePos = glm::vec2(
							imac::rangeMapping(e.button.x, 0, WIN_WIDTH, 0, PROJ_WIDTH),
							imac::rangeMapping(WIN_HEIGHT - e.button.y, 0, WIN_HEIGHT, 0, PROJ_HEIGHT)
						);
						emitter.publish<evnt::LeftClick>(normMousePos);
					}
				}
				//noeView->MouseButtonUp(e.button.x, e.button.y, Noesis::MouseButton_Left);
				break;

			case SDL_MOUSEBUTTONDOWN:
				//noeView->MouseButtonDown(e.button.x, e.button.y, Noesis::MouseButton_Left);
				break;

			case SDL_MOUSEMOTION:
				// Send move event
			{
				normMousePos = glm::vec2(
					imac::rangeMapping(e.button.x, 0, WIN_WIDTH, 0, PROJ_WIDTH),
					imac::rangeMapping(WIN_HEIGHT - e.button.y, 0, WIN_HEIGHT, 0, PROJ_HEIGHT)
				);
				emitter.publish<evnt::MouseMove>(normMousePos);
			}
			break;

			case SDL_KEYDOWN:
				if (e.key.keysym.sym == 'q') { // Quit
					bQuit = true;
				}
				else if (e.key.keysym.sym == 'p') {
					if (bWireframe) {
						GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
					}
					else {
						GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
					}
					bWireframe = !bWireframe;
				}
				/*
				else if (e.key.keysym.scancode == SDL_SCANCODE_UP) {
					viewTranslation.y++;
					viewMat = glm::translate(viewMat, glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
					viewTranslation.y--;
					viewMat = glm::translate(viewMat, glm::vec3(0.0f, -1.0f, 0.0f));
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
					viewTranslation.x--;
					viewMat = glm::translate(viewMat, glm::vec3(-1.0f, 0.0f, 0.0f));
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
					viewTranslation.x++;
					viewMat = glm::translate(viewMat, glm::vec3(1.0f, 0.0f, 0.0f));
				}
				*/
				break;

			case SDL_MOUSEWHEEL:
			{
				if (e.motion.x > 0.0f) {
					// TODO ameliorer translation quand proche du bord
					// FIXME liens avec grille pas bon
					/*
					viewScale += 0.1f;
					viewTranslation = glm::vec2(normMousePos.x * WIN_RATIO, normMousePos.y);
					viewMat = glm::translate(viewMat, glm::vec3(normMousePos.x * WIN_RATIO, normMousePos.y, 0.0f));
					viewMat = glm::scale(viewMat, glm::vec3(1.1f, 1.1f, 0.0f));
					viewMat = glm::translate(viewMat, glm::vec3(-normMousePos.x * WIN_RATIO, -normMousePos.y, 0.0f));
					*/
				}
				else if (e.motion.x < 0.0f) {
					/*
					const glm::vec2 invertNormMousePos = glm::vec2(
						PROJ_WIDTH - normMousePos.x,
						PROJ_HEIGHT - normMousePos.y
					);
					// TODO réduire translation quand proche du bord
					// FIXME liens avec grille pas bon
					
					viewScale -= 0.05f;
					viewTranslation = glm::vec2(invertNormMousePos.x * WIN_RATIO, invertNormMousePos.y);
					viewMat = glm::translate(viewMat, glm::vec3(invertNormMousePos.x * WIN_RATIO, invertNormMousePos.y, 0.0f));
					viewMat = glm::scale(viewMat, glm::vec3(0.95f, 0.95f, 0.0f));
					viewMat = glm::translate(viewMat, glm::vec3(-invertNormMousePos.x * WIN_RATIO, -invertNormMousePos.y, 0.0f));
					*/
				}
			}
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
