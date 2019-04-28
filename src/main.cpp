#ifdef _WIN32
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include <cstdlib>
#include <iostream>
#include <glad/glad.h>
#include <GL/glu.h>
#include <debugbreak/debugbreak.h>
#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <NsRender/GLFactory.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <NsGui/IView.h>
#include <NsGui/Grid.h>
#include <Box2D/Box2D.h>

#include "core/game.hpp"
#include "core/maths.hpp"
#include "core/tags.hpp"
#include "core/constants.hpp"
#include "events/event-emitter.hpp"
#include "services/locator.hpp"
#include "services/debug-draw/i-debug-draw.hpp"
#include "logger/gl-log-handler.hpp"
#include "logger/noesis-log-handler.hpp"
#include "map/map.hpp"
#include "entity-factories/tower-factory.hpp"
#include "entity-factories/enemy-factory.hpp"
#include "systems/render-system.hpp"
#include "systems/physic-system.hpp"
#include "systems/animation-system.hpp"
#include "systems/construction-system.hpp"
#include "systems/follow-system.hpp"
#include "gui/start-menu.hpp"
#include "events/left-click.hpp"
#include "events/mouse-move.hpp"

#pragma warning (disable : 26495) // Initialisation of a member missing in constructor of box2d and imgui

static Noesis::IView* noeView;

int main(int argc, char** argv) {
    #ifdef _WIN32 // Check memory leaks
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

	// Reference to these instances are passed around a lot
	entt::DefaultRegistry registry;
	EventEmitter emitter{};

	// Game start
    Game game(registry);
    if (game.init() == EXIT_FAILURE) {
        debug_break();
        return EXIT_FAILURE;
    }

    // View matrices, they are passed around a lot as well
	glm::mat4 projMat = glm::ortho(0.0f, PROJ_WIDTH_RAT, 0.0f, PROJ_HEIGHT, 0.0f, 100.0f);
	glm::mat4 viewMat = glm::mat4(1.0f);
	glm::vec2 viewTranslation = glm::vec2(0.0f);
	float viewScale = 1.0f;

    // Init Physics
	b2Vec2 gravity(0.0f, -10.0f);	
    auto physicWorld = std::make_shared<b2World>(gravity);

	// Link debug draw to physics
	IDebugDraw& debugDraw = locator::debugDraw::ref();
	debugDraw.setProjMat(projMat);
	debugDraw.setViewMat(viewMat);
    physicWorld->SetDebugDraw(&debugDraw);
    //debugDraw.SetFlags(b2Draw::e_shapeBit + b2Draw::e_centerOfMassBit + b2Draw::e_aabbBit + b2Draw::e_jointBit + b2Draw::e_pairBit);
    
	// Noesis GUI
	/*
	StartMenu startMenu;
	Noesis::Ptr<Noesis::FrameworkElement> xaml = startMenu;
	noeView = Noesis::GUI::CreateView(xaml).GiveOwnership();
	noeView->SetIsPPAAEnabled(true);
	noeView->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	noeView->SetSize(WIN_WIDTH, WIN_HEIGHT);
	*/
    
	// Map
	Map map1(registry, "res/maps/map-2.itd", viewTranslation, viewScale);

    // Systems
    RenderSystem renderSystem(registry, viewMat, projMat);
    AnimationSystem animationSystem(registry);
    PhysicSystem physicSystem(registry);
	ConstructionSystem constructionSystem(registry, emitter, map1, *physicWorld.get());
	FollowSystem followSystem(registry, emitter);

    // Game loop
	glm::vec2 normMousePos = glm::vec2(0.0f);
    bool bWireframe = false;
    unsigned int tempFrameCount = 0;
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
			ImGui::End();
		}

        // Noesis update
		/*
        {
            // Noesis gui update
            noeView->Update(SDL_GetTicks());
            noeView->GetRenderer()->UpdateRenderTree();
            noeView->GetRenderer()->RenderOffscreen();

            // Need to restore the GPU state because noesis changes it
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            GLCall(glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT));
            GLCall(glClearStencil(0));
        }
		*/

        // Game updates
        {
			// Update view
			viewMat = glm::mat4(1.0f);
			viewMat = glm::translate(viewMat, glm::vec3(viewTranslation, 0.0f));
			//viewMat = glm::translate(viewMat, glm::vec3(normMousePos.x * WIN_RATIO, normMousePos.y, 0.0f));
			viewMat = glm::scale(viewMat, glm::vec3(viewScale, viewScale, 0.0f));
			//viewMat = glm::translate(viewMat, glm::vec3(-normMousePos.x * WIN_RATIO, -normMousePos.y, 0.0f));

            // Update animation
            if (tempFrameCount >= 5) { // TODO use delatime or target framerate to have constant animation no matter the target
                animationSystem.update(deltatime);
                tempFrameCount = 0;
            }
            tempFrameCount++;
			followSystem.update(deltatime);
            
            // Update physics
            physicWorld->Step(1.0f / 60.0f, 6, 2);
            physicSystem.update(deltatime, physicWorld.get());
        }

		// Render Debug
		{
			// TODO render debugdraw here and not directly
			debugDraw.setProjMat(projMat);
			debugDraw.setViewMat(viewMat);
			map1.drawGraph();
			map1.drawGrid();
			physicWorld->DrawDebugData();
		}

        // Render
        {
            renderSystem.update();
            //noeView->GetRenderer()->Render();
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
					if (e.button.button == SDL_BUTTON_LEFT) {
						normMousePos = glm::vec2(
							imac::rangeMapping(e.button.x, 0, WIN_WIDTH, 0, PROJ_WIDTH),
							imac::rangeMapping(WIN_HEIGHT - e.button.y, 0, WIN_HEIGHT, 0, PROJ_HEIGHT)
						);
						emitter.publish<evnt::LeftClick>(normMousePos);
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
                    } else if (e.key.keysym.sym == 'p') {
                        if (bWireframe) {
                            GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
                        } else {
                            GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
                        }
                        bWireframe = !bWireframe;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_UP) {
                        viewTranslation.y++;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
						viewTranslation.y--;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
						viewTranslation.x--;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
						viewTranslation.x++;
                    }
                    break;

				case SDL_MOUSEWHEEL:
					{
						if (e.motion.x > 0.0f) {
							// TODO ameliorer translation quand proche du bord
							viewScale += 0.1f;
							
						}
						else if (e.motion.x < 0.0f) {
							/*
							const glm::vec2 invertNormMousePos = glm::vec2(
								PROJ_WIDTH - normMousePos.x,
								PROJ_HEIGHT - normMousePos.y
							);
							*/
							// TODO réduire translation quand proche du bord
							viewScale -= 0.05f;
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
            deltatime = (double) (endTicks - beginTicks) * 1000 / (double) SDL_GetPerformanceFrequency();

            // If deltaTime too big, we must have resumed from a breakpoint
            // So frame lock on the target framerate
            if (deltatime > MAX_DELTA_MS) {
                deltatime = TARGET_DELTA_MS; 
            }

            // If deltaTime too small, wait to get to the target framerate
            if (deltatime < TARGET_DELTA_MS) { 
                SDL_Delay((Uint32) (TARGET_DELTA_MS - deltatime)); 
            }
            beginTicks = endTicks;
        }
    }

    //noeView->GetRenderer()->Shutdown();
    return EXIT_SUCCESS;
}
