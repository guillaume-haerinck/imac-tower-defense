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

#include "core/game.hpp"
#include "logger/gl-log-handler.hpp"
#include "logger/noesis-log-handler.hpp"
#include "core/tags.hpp"
#include "core/constants.hpp"
#include "factories/sprite-factory.hpp"
#include "components/transform.hpp"
#include "components/motion.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"
#include "systems/render-system.hpp"
#include "systems/movement-system.hpp"
#include "systems/animation-system.hpp"
#include "gui/start-menu.hpp"

static Noesis::IView* _noeView;

int main(int argc, char** argv) {
    Game game;
    if (game.init() == EXIT_FAILURE) {
        debug_break();
        return EXIT_FAILURE;
    }

    /* Model and Projection matrices */
	glm::mat4 projMat = glm::ortho(0.0f, 100.0f * WIN_RATIO, 0.0f, 100.0f, 0.0f, 100.0f);
	glm::mat4 viewMat = glm::mat4(1.0f);

    /* Camera variables */
    glm::vec3 camPos = glm::vec3(0, 0, 0);

    /* Create entities */
    entt::DefaultRegistry registry;
    SpriteFactory spriteFactory(registry);
    
    /* Assign components */
    auto myEntity =  registry.create();
    auto myEntity2 = registry.create();
    auto myEntity3 = registry.create();
    auto myEntity4 = registry.create();
    
    registry.assign<cmpt::Sprite>(myEntity, spriteFactory.createAtlas("res/images/spritesheets/test.jpg", glm::vec2(1.0f), GL_STATIC_DRAW, glm::vec2(50, 50)));
    registry.assign<cmpt::Transform>(myEntity, glm::vec3(20.0f), glm::vec3(90.0f * WIN_RATIO, 10.0f, 0.0f), glm::quat());
    cmpt::SpriteAnimation myAnim2(0, 5, 0);
    registry.assign<cmpt::SpriteAnimation>(myEntity, myAnim2);
    registry.assign<renderTag::Animated>(myEntity);

    registry.assign<cmpt::Sprite>(myEntity2, spriteFactory.create("res/images/textures/arrow.png", glm::vec2(1.0f), GL_STATIC_DRAW));
    registry.assign<cmpt::Transform>(myEntity2, glm::vec3(15.0f), glm::vec3(0.0f, 50.0f, 0.0f), glm::quat());
    registry.assign<renderTag::Still>(myEntity2);

    registry.assign<cmpt::Sprite>(myEntity3, spriteFactory.createAtlas("res/images/spritesheets/test.jpg", glm::vec2(1.0f), GL_STATIC_DRAW, glm::vec2(50, 50)));
    registry.assign<cmpt::Transform>(myEntity3, glm::vec3(25.0f), glm::vec3(50.0f * WIN_RATIO, 50.0f, 0.0f), glm::quat());
    cmpt::SpriteAnimation myAnim(6, 11, 6);
    registry.assign<cmpt::SpriteAnimation>(myEntity3, myAnim);
    registry.assign<renderTag::Animated>(myEntity3);
    
    registry.assign<cmpt::Sprite>(myEntity4, spriteFactory.create("res/images/textures/logo-imac.png", glm::vec2(1.0f), GL_STATIC_DRAW));
    registry.assign<cmpt::Transform>(myEntity4, glm::vec3(15.0f), glm::vec3(90.0f * WIN_RATIO, 90.0f, 0.0f), glm::quat());
    registry.assign<renderTag::Still>(myEntity4);
    
    /* Create systems */
    RenderSystem renderSystem;
    MovementSystem movementSystem;
    AnimationSystem animationSystem;

    /* Loop general variables */
    bool bWireframe = false;
    unsigned int tempFrameCount = 0;

    /* Noesis GUI */
    StartMenu startMenu;
    Noesis::Ptr<Noesis::FrameworkElement> xaml = startMenu;

    _noeView = Noesis::GUI::CreateView(xaml).GiveOwnership();
    _noeView->SetIsPPAAEnabled(true);
    _noeView->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
    _noeView->SetSize(WIN_WIDTH, WIN_HEIGHT);

    /* Main loop */
    bool bQuit = false;
    double deltatime = TARGET_DELTA_MS;
    Uint64 beginTicks = SDL_GetPerformanceCounter();
    while (!bQuit) {
        /* Imgui main debug window */
		{
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(game.getWindow());
            ImGui::NewFrame();
			ImGui::Begin("Main debug window");
			    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

        /* Noesis update */
        {
            // Noesis gui update
            _noeView->Update(SDL_GetTicks());
            _noeView->GetRenderer()->UpdateRenderTree();
            _noeView->GetRenderer()->RenderOffscreen();

            // Need to restore the GPU state because noesis changes it
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            GLCall(glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT));
            GLCall(glClearColor(0.8f, 1.0f, 1.0f, 0.0f));
            GLCall(glClearStencil(0));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        }

        /* Game updates */
        {
            // Update camera
            viewMat = glm::translate(glm::mat4(1.0f), camPos);

            // TODO use delatime or target framerate to have constant animation no matter the target
            if (tempFrameCount >= 10) {
                animationSystem.update(registry, deltatime);
                tempFrameCount = 0;
            }
            tempFrameCount++;
            movementSystem.update(registry, deltatime);

        }

        /* Render */
        {
            renderSystem.update(registry, viewMat, projMat);
            _noeView->GetRenderer()->Render();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        /* Handle events */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                bQuit = true;
                break;
            }
            
            switch (e.type) {
                case SDL_MOUSEBUTTONUP:
                    printf("clic en (%d, %d)\n", e.button.x, (SDL_GetWindowSurface(game.getWindow())->h) - e.button.y);
                    _noeView->MouseButtonUp(e.button.x, e.button.y, Noesis::MouseButton_Left);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    _noeView->MouseButtonDown(e.button.x, e.button.y, Noesis::MouseButton_Left);
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

        /* Update window */
        SDL_GL_SwapWindow(game.getWindow());

        /* Check framerate */
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

    // Cleanup
    _noeView->GetRenderer()->Shutdown();
    return EXIT_SUCCESS;
}
