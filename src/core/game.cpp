#include "game.hpp"

#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <NsApp/LocalFontProvider.h>
#include <NsApp/LocalTextureProvider.h>
#include <NsApp/LocalXamlProvider.h>

#include "constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "logger/noesis-log-handler.hpp"
#include "components/sprite.hpp"
#include "components/primitive.hpp"

/* ------------------------ LIFETIME ------------------------ */

bool Game::m_bInstanciated = false;

Game::Game(entt::DefaultRegistry& registry)
: m_registry(registry), m_bInit(false), m_window(nullptr), m_context(nullptr) 
{
	assert(!m_bInstanciated);
	m_bInstanciated = true;
}

Game::~Game() {
	// Delete OpenGL data
	m_registry.view<cmpt::Sprite>().each([](auto entity, cmpt::Sprite& sprite) {
		GLCall(glDeleteTextures(1, &sprite.textureID));
		GLCall(glDeleteVertexArrays(1, &sprite.vaID));
	});

	m_registry.view<cmpt::Primitive>().each([](auto entity, cmpt::Primitive& primitive) {
		GLCall(glDeleteVertexArrays(1, &primitive.vaID));
	});

	// Shutdown 
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    spdlog::drop_all();
    spdlog::shutdown();

	m_bInstanciated = false;
}

int Game::init() {
    if (m_bInit) {
        spdlog::error("[Game] The init function as already been called !");
        return EXIT_FAILURE;
    }

    /* Init logger */
    spdlog::set_pattern("[%l] %^ %v %$");

    /* Init SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        spdlog::critical("[SDL2] Unable to initialize SDL: {}", SDL_GetError());
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
    m_window = SDL_CreateWindow(
        "IMAC Tower Defense",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_OPENGL
    );
    if (m_window == nullptr) {
        spdlog::critical("[SDL2] Window is null: {}", SDL_GetError());
        debug_break();
        return EXIT_FAILURE;
    }

    /* Create OpenGl context */
    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == nullptr) {
        spdlog::critical("[SDL2] OpenGL context is null: {}",  SDL_GetError());
        debug_break();
        return EXIT_FAILURE;
    }
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        spdlog::critical("[Glad] Glad not init");
        debug_break();
		return EXIT_FAILURE;
	}
    int glMajVersion, glMinVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajVersion);
    glGetIntegerv(GL_MINOR_VERSION, &glMinVersion);
    if (glMajVersion < 4 || glMajVersion < 4 && glMinVersion < 3) {
        spdlog::critical("[OpenGl] Your graphic card driver is not up to date, you must have at least OpenGL 4.4");
        spdlog::info("OpenGL version ", glGetString(GL_VERSION));
        debug_break();
		return EXIT_FAILURE;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gllog::messageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

    /* Enable blending & Z-Buffer */
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_DEPTH_TEST));

    /* Init ImGui */
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

    /* Init Noesis GUI */
    Noesis::GUI::Init(noelog::errorHandler, noelog::messageCallback, nullptr);
    Noesis::GUI::SetXamlProvider(Noesis::MakePtr<NoesisApp::LocalXamlProvider>("./res/gui"));
    Noesis::GUI::SetTextureProvider(Noesis::MakePtr<NoesisApp::LocalTextureProvider>("./res/images"));
    Noesis::GUI::SetFontProvider(Noesis::MakePtr<NoesisApp::LocalFontProvider>("./res/fonts"));

    m_bInit = true;
    return EXIT_SUCCESS;
}

/* ----------------------- GETTERS AND SETTERS ---------------------- */

SDL_Window* Game::getWindow() { return m_window; }
SDL_GLContext Game::getContext() { return m_context; }
