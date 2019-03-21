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

static Noesis::IView* _view;

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
    
    /* --------------------------------- PLAYGROUND TO CHECK FOR BUGS ----------------- */
    auto myEntity =  registry.create();
    auto myEntity2 = registry.create();
    auto myEntity3 = registry.create();
    auto myEntity4 = registry.create();
    
    registry.assign<cmpt::Sprite>(myEntity, spriteFactory.createAtlas("images/spritesheets/test.jpg", glm::vec2(1.0f), GL_STATIC_DRAW, glm::vec2(50, 50)));
    registry.assign<cmpt::Transform>(myEntity, glm::vec3(20.0f), glm::vec3(90.0f * WIN_RATIO, 10.0f, 0.0f), glm::quat());
    cmpt::SpriteAnimation myAnim2(0, 5, 0);
    registry.assign<cmpt::SpriteAnimation>(myEntity, myAnim2);
    registry.assign<renderTag::Animated>(myEntity);

    registry.assign<cmpt::Sprite>(myEntity2, spriteFactory.create("images/textures/arrow.png", glm::vec2(1.0f), GL_STATIC_DRAW));
    registry.assign<cmpt::Transform>(myEntity2, glm::vec3(15.0f), glm::vec3(0.0f, 50.0f, 0.0f), glm::quat());
    registry.assign<renderTag::Still>(myEntity2);

    registry.assign<cmpt::Sprite>(myEntity3, spriteFactory.createAtlas("images/spritesheets/test.jpg", glm::vec2(1.0f), GL_STATIC_DRAW, glm::vec2(50, 50)));
    registry.assign<cmpt::Transform>(myEntity3, glm::vec3(25.0f), glm::vec3(50.0f * WIN_RATIO, 50.0f, 0.0f), glm::quat());
    cmpt::SpriteAnimation myAnim(6, 11, 6);
    registry.assign<cmpt::SpriteAnimation>(myEntity3, myAnim);
    registry.assign<renderTag::Animated>(myEntity3);
    
    registry.assign<cmpt::Sprite>(myEntity4, spriteFactory.create("images/textures/logo-imac.png", glm::vec2(1.0f), GL_STATIC_DRAW));
    registry.assign<cmpt::Transform>(myEntity4, glm::vec3(15.0f), glm::vec3(90.0f * WIN_RATIO, 90.0f, 0.0f), glm::quat());
    registry.assign<renderTag::Still>(myEntity4);
    
    /* Create systems */
    RenderSystem renderSystem;
    MovementSystem movementSystem;
    AnimationSystem animationSystem;

    /* Loop general variables */
    bool bWireframe = false;
    unsigned int tempFrameCount = 0;

    /* -------------------------------- NOESIS TEST --------------------------- */
    // For simplicity purposes we are not using resource providers in this sample. ParseXaml() is
    // enough if there is no extra XAML dependencies
    Noesis::Ptr<Noesis::Grid> xaml(Noesis::GUI::ParseXaml<Noesis::Grid>(R"(
        <Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation">
            <Grid.Background>
            </Grid.Background>
            <Viewbox>
                <StackPanel Margin="50">
                    <Button Content="Hello World!" Margin="0,30,0,0"/>
                </StackPanel>
            </Viewbox>
        </Grid>
    )"));

    // View creation to render and interact with the user interface
    // We transfer the ownership to a global pointer instead of a Ptr<> because there is no way
    // in GLUT to do shutdown and we don't want the Ptr<> to be released at global time
    _view = Noesis::GUI::CreateView(xaml).GiveOwnership();
    _view->SetIsPPAAEnabled(true);

    // Renderer initialization with an OpenGL device
    _view->GetRenderer()->Init(NoesisApp::GLFactory::CreateDevice());
    _view->SetSize(WIN_WIDTH, WIN_HEIGHT);

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

        /* Render */
        {
            // Update camera
            viewMat = glm::translate(glm::mat4(1.0f), camPos);

            // Update systems
            if (tempFrameCount >= 10) {
                animationSystem.update(registry, deltatime);
                tempFrameCount = 0;
            }
            tempFrameCount++;
            movementSystem.update(registry, deltatime);
            
            // Update view (layout, animations, ...)
            _view->Update(SDL_GetTicks());

            // Offscreen rendering phase populates textures needed by the on-screen rendering
            _view->GetRenderer()->UpdateRenderTree();
            _view->GetRenderer()->RenderOffscreen();

            // If you are going to render here with your own engine you need to restore the GPU state
            // because noesis changes it. In this case only framebuffer and viewport need to be restored
            GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            GLCall(glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT));

            GLCall(glClearColor(0.8f, 1.0f, 1.0f, 0.0f));
            GLCall(glClearStencil(0));
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            renderSystem.update(registry, viewMat, projMat);

            // Rendering is done in the active framebuffer
            _view->GetRenderer()->Render();

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
                    _view->MouseButtonUp(e.button.x, e.button.y, Noesis::MouseButton_Left);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    _view->MouseButtonDown(e.button.x, e.button.y, Noesis::MouseButton_Left);
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
