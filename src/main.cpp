#include <cstdlib>
#include <iostream>
#include <glad/glad.h>
#include <GL/glu.h>
#include <debugbreak/debugbreak.h>
#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Noesis_pch.h>

#include "logger/gl-error-handler.hpp"
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
    /* Init SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[SDL2 Error] Unable to initialize SDL: " << SDL_GetError() << std::endl;
        debug_break();
        return EXIT_FAILURE;
    }
    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_GL_SetSwapInterval(1);

    /* Create Window */
    SDL_Window* window;
    window = SDL_CreateWindow(
        "IMAC Tower Defense",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_OPENGL
    );
    if (window == nullptr) {
        std::cerr << "[SDL2 Error] Window is null: " << SDL_GetError() << std::endl;
        debug_break();
        return EXIT_FAILURE;
    }

    /* Create OpenGl context */
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cerr << "[SDL2 Error] OpenGL context is null: " << SDL_GetError() << std::endl;
        debug_break();
        return EXIT_FAILURE;
    }
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "[Error] Glad not init" << std::endl;
        debug_break();
		return EXIT_FAILURE;
	}
    int glMajVersion, glMinVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajVersion);
    glGetIntegerv(GL_MINOR_VERSION, &glMinVersion);
    if (glMajVersion < 4 || glMajVersion < 4 && glMinVersion < 3) {
        std::cerr << "[Error] Your graphic card driver is not up to date, you must have at least OpenGL 4.4" << std::endl;
        std::cout << "OpenGL version " <<  glGetString(GL_VERSION) << std::endl;
        debug_break();
		return EXIT_FAILURE;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glr::messageCallback, 0);
    std::cout << "OpenGL version " <<  glGetString(GL_VERSION) << std::endl;
    std::cout << "Press 'P' to activate wireframe mode" << std::endl;
    std::cout << std::endl << "--------------------------------------------" << std::endl << std::endl;

    /* Enable blending & Z-Buffer */
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_DEPTH_TEST));

    /* Camera variables */
    glm::vec3 camPos = glm::vec3(0, 0, 3);
    glm::vec3 camTarget = glm::vec3(0, 0, 0);
    glm::vec3 camDirection = glm::vec3(0, 1, 0);

    /* Model and Projection matrices */
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(camPos, camTarget, camDirection);

    /* Create entities */
    entt::registry registry;
    auto hoursHandleEntity = registry.create();
    auto minutesHandleEntity = registry.create();
    auto clockEntity = registry.create();
    
    /* Assign components */
    SpriteFactory spriteFactory(registry);
    registry.assign<cmpt::Sprite>(hoursHandleEntity, spriteFactory.createAtlas("images/spritesheets/spaceman.jpg", glm::vec2(1.0f), GL_STATIC_DRAW, glm::vec2(196, 196)));
    registry.assign<cmpt::Transform>(hoursHandleEntity, glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, -2.0f), glm::quat());
    cmpt::SpriteAnimation myAnim;
    myAnim.activeTile = 0;
    myAnim.endTile = 25;
    myAnim.startTile = 0;
    registry.assign<cmpt::SpriteAnimation>(hoursHandleEntity, myAnim);
    registry.assign<tag::Hours>(hoursHandleEntity);

    /* Create systems */
    RenderSystem renderSystem;
    MovementSystem movementSystem;
    AnimationSystem animationSystem;

    /* TEST NOESIS */
    Noesis::GUI::Init(nullptr, noelog::logHandler, nullptr);

    /* TEST IMGUI */
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(window, &context);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

    /* Main loop */
    bool bWireframe = false;
    double deltatime = TARGET_DELTA_MS;
    Uint64 beginTicks = SDL_GetPerformanceCounter();
    bool bQuit = false;
    unsigned int tempFrameCount = 0;
    while (!bQuit) {
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        /* IMGUI Start */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
		{
			ImGui::Begin("Hello, world!");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		/* IMGUI Render */
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Render */
        view = glm::lookAt(camPos, camTarget, camDirection); // Update camera

        if (tempFrameCount >= 2) {
            animationSystem.update(registry, deltatime);
            tempFrameCount = 0;
        }
        tempFrameCount++;
        
        movementSystem.update(registry, deltatime);
        renderSystem.update(registry, view, proj);

        /* Handle events */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                bQuit = true;
                break;
            }
            
            switch (e.type) {
                case SDL_MOUSEBUTTONUP:
                    printf("clic en (%d, %d)\n", e.button.x, (SDL_GetWindowSurface(window)->h) - e.button.y);
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
        SDL_GL_SwapWindow(window);

        /* Check framerate */
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

    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
