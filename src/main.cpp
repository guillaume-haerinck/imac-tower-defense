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
#include "core/map.hpp"
#include "services/debug-draw.hpp"
#include "logger/gl-log-handler.hpp"
#include "logger/noesis-log-handler.hpp"
#include "core/tags.hpp"
#include "core/constants.hpp"
#include "entity-factories/tower-factory.hpp"
#include "entity-factories/enemy-factory.hpp"
#include "systems/render-system.hpp"
#include "systems/physic-system.hpp"
#include "systems/animation-system.hpp"
#include "services/audio-service.hpp"
#include "gui/start-menu.hpp"

#include "components/transform.hpp"
#include "events/click.hpp"

// #pragma warning (disable : 26495) // Initialisation of a member missing in constructor

static Noesis::IView* noeView;

struct MyEmitter : entt::Emitter<MyEmitter> {};

int main(int argc, char** argv) {
    #ifdef _WIN32 // Check memory leaks
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

	// Game start
	entt::DefaultRegistry registry;
    Game game(registry);
    if (game.init() == EXIT_FAILURE) {
        debug_break();
        return EXIT_FAILURE;
    }

    // View position
	glm::mat4 projMat = glm::ortho(0.0f, PROJ_WIDTH * WIN_RATIO, 0.0f, PROJ_HEIGHT, 0.0f, 100.0f);
	glm::mat4 viewMat = glm::mat4(1.0f);
    glm::vec3 camPos = glm::vec3(0, 0, 0);

    // Init Physics
	b2Vec2 gravity(0.0f, -10.0f);	
    auto physicWorld = std::make_shared<b2World>(gravity);
    auto debugDraw = std::make_shared<DebugDraw>(viewMat, projMat);
    physicWorld->SetDebugDraw(debugDraw.get());
    debugDraw->SetFlags(b2Draw::e_shapeBit + b2Draw::e_centerOfMassBit + b2Draw::e_aabbBit + b2Draw::e_jointBit + b2Draw::e_pairBit);
    
	// Noesis GUI
	/*
	StartMenu startMenu;
	Noesis::Ptr<Noesis::FrameworkElement> xaml = startMenu;
	noeView = Noesis::GUI::CreateView(xaml).GiveOwnership();
	noeView->SetIsPPAAEnabled(true);
	noeView->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
	noeView->SetSize(WIN_WIDTH, WIN_HEIGHT);
	*/

	// Entity factories
	TowerFactory towerFactory(registry);
	EnemyFactory enemyFactory(registry);

    /* ----------------------- TESTING PLAYGROUND ------------------ */
    {
		//towerFactory.create(1, 9);
		//enemyFactory.create(50.0f, 50.0f);
    }

	// Event
	MyEmitter emitter{};

	emitter.on<evnt::Click>([](const evnt::Click &event, MyEmitter &emitter) {
		spdlog::info("Asynchronious event recieved with value {}", event.mousePos.x);
	});

	
    
	// Map
	// TODO use a service to pass the registry around ?
	Map map1(registry, "res/maps/map-1.itd");

    // Systems
    RenderSystem renderSystem;
    AnimationSystem animationSystem;
    PhysicSystem physicSystem;

    // Game loop
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
            // Update camera
            viewMat = glm::translate(glm::mat4(1.0f), camPos);
            debugDraw->setViewMat(viewMat);

            // Update animation
            if (tempFrameCount >= 5) { // TODO use delatime or target framerate to have constant animation no matter the target
                animationSystem.update(registry, deltatime);
                tempFrameCount = 0;
            }
            tempFrameCount++;
            
            // Update physics
            physicWorld->Step(1.0f / 60.0f, 6, 2);
            physicSystem.update(registry, deltatime, physicWorld.get());
        }

        // Render
        {
			// POUR JULES <3
			// 0,0 en bas a gauche
			// 100, 100 en haut a droite
			// Suffit de mettre ca dans le update() de n'importe quel systeme
			/*
			{
				debugDraw->setColor(255, 0, 0, 1);
				debugDraw->line(0., 50., 50., 50.);
				GLCall(glPointSize(13));
				debugDraw->point(10., 10.);
			}
			*/

            renderSystem.update(registry, viewMat, projMat);
            physicWorld->DrawDebugData();
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
					emitter.publish<evnt::Click>(glm::vec2(e.button.x, (SDL_GetWindowSurface(game.getWindow())->h) - e.button.y));
                    //printf("clic en (%d, %d)\n", e.button.x, (SDL_GetWindowSurface(game.getWindow())->h) - e.button.y);
                    //noeView->MouseButtonUp(e.button.x, e.button.y, Noesis::MouseButton_Left);
					/*
					{
						glm::vec2 tilePosition = map1.windowToGrid(e.button.x, (SDL_GetWindowSurface(game.getWindow())->h) - e.button.y);
						unsigned int entityId = map1.getTile(tilePosition.x, tilePosition.y);
						cmpt::Transform trans = registry.get<cmpt::Transform>(entityId);
						enemyFactory.create(trans.position.x, trans.position.y);
					}
					*/
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    //noeView->MouseButtonDown(e.button.x, e.button.y, Noesis::MouseButton_Left);
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
                        camPos.y++;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                        camPos.y--;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                        camPos.x--;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                        camPos.x++;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_PAGEUP) {
                        camPos.z--;
                    } else if (e.key.keysym.scancode == SDL_SCANCODE_PAGEDOWN) {
                        camPos.z++;
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
