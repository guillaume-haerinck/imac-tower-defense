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
#include <Noesis_pch.h>

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
    entt::registry registry;
    auto myEntity = registry.create();
    
    /* Assign components */
    SpriteFactory spriteFactory(registry);
    registry.assign<cmpt::Sprite>(myEntity, spriteFactory.createAtlas("images/spritesheets/spaceman.jpg", glm::vec2(1.0f), GL_STATIC_DRAW, glm::vec2(196, 196)));
    registry.assign<cmpt::Transform>(myEntity, glm::vec3(25.0f), glm::vec3(50.0f * WIN_RATIO, 50.0f, 0.0f), glm::quat());
    cmpt::SpriteAnimation myAnim;
    myAnim.activeTile = 0;
    myAnim.endTile = 25;
    myAnim.startTile = 0;
    registry.assign<cmpt::SpriteAnimation>(myEntity, myAnim);
    registry.assign<tag::Hours>(myEntity);

    /* Create systems */
    RenderSystem renderSystem;
    MovementSystem movementSystem;
    AnimationSystem animationSystem;

    /* Loop general variables */
    bool bWireframe = false;
    unsigned int tempFrameCount = 0;

    /* Main loop */
    bool bQuit = false;
    double deltatime = TARGET_DELTA_MS;
    Uint64 beginTicks = SDL_GetPerformanceCounter();
    while (!bQuit) {
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        /* Imgui main debug window */
		{
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(game.getWindow());
            ImGui::NewFrame();
			ImGui::Begin("Main debug window");
			    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

        /* Render */
        {
            // Update camera
            viewMat = glm::translate(glm::mat4(1.0f), camPos);

            // Update systems
            if (tempFrameCount >= 2) {
                animationSystem.update(registry, deltatime);
                tempFrameCount = 0;
            }
            tempFrameCount++;
            movementSystem.update(registry, deltatime);
            renderSystem.update(registry, viewMat, projMat);

            // Update gui
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

    return EXIT_SUCCESS;
}
