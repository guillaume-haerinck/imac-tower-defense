#include "game.hpp"

#include <spdlog/spdlog.h>
#include <debugbreak/debugbreak.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <NsApp/LocalFontProvider.h>
#include <NsApp/LocalTextureProvider.h>
#include <NsApp/LocalXamlProvider.h>

#include "constants.hpp"
#include "logger/gl-log-handler.hpp"
#include "logger/noesis-log-handler.hpp"
#include "components/sprite.hpp"
#include "components/primitive.hpp"
#include "services/locator.hpp"
#include "services/debug-draw/debug-draw-service.hpp"
#include "services/random/random-service.hpp"
#include "services/audio/audio-service.hpp"
#include "events/change-game-state.hpp"

/* ------------------------ LIFETIME ------------------------ */

bool Game::m_bInstanciated = false;
bool Game::m_bInit = false;

Game::Game(EventEmitter& emitter)
:   m_window(nullptr), m_context(nullptr), emitter(emitter), m_state(GameState::LEVEL),
	m_projMat(glm::ortho(0.0f, PROJ_WIDTH_RAT, 0.0f, PROJ_HEIGHT, -50.0f, 50.0f)),
	m_viewMat(glm::mat4(1.0f)),
	m_viewTranslation(glm::vec2(0.0f)), m_viewScale(1.0f),
	renderSystem(nullptr),
	animationSystem(nullptr),
	movementSystem(nullptr),
	constructionSystem(nullptr),
	waveSystem(nullptr),
	attackSystem(nullptr),
	healthSystem(nullptr)
{
	assert(!m_bInstanciated);
	m_bInstanciated = true;

	emitter.on<evnt::ChangeGameState>([this](const evnt::ChangeGameState & event, EventEmitter & emitter) {
		// TODO use the interface to do this in 2 lines

		// Exit current state
		switch (this->m_state) {
		case TITLE_SCREEN:
			m_titleState->onExit();
			break;

		case LEVEL:
			m_levelState->onExit();
			break;

		case CINEMATIC:
			break;

		case GAME_OVER:
			m_gameOverState->onExit();
			break;

		default:
			break;
		}

		// Enter new state
		switch (event.state) {
		case TITLE_SCREEN:
			m_titleState->onEnter();
			break;

		case LEVEL:
			m_levelState->onEnter();
			break;

		case CINEMATIC:
			break;

		case GAME_OVER:
			m_gameOverState->onEnter();
			break;

		default:
			break;
		}

		// Update state
		this->m_state = event.state;
	});

	/* TODO listen to event here ? Or use a camera class, or even a camera service
		else if (e.key.keysym.scancode == SDL_SCANCODE_UP) {
			viewTranslation.y++;
			viewMat = glm::translate(viewMat, glm::vec3(0.0f, 1.0f, 0.0f));
		} else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
			viewTranslation.y--;
			viewMat = glm::translate(viewMat, glm::vec3(0.0f, -1.0f, 0.0f));
		} else if (e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
			viewTranslation.x--;
			viewMat = glm::translate(viewMat, glm::vec3(-1.0f, 0.0f, 0.0f));
		} else if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
			viewTranslation.x++;
			viewMat = glm::translate(viewMat, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		// TODO ameliorer translation quand proche du bord
		// FIXME
		viewScale += 0.1f;
		viewTranslation = glm::vec2(normMousePos.x * WIN_RATIO, normMousePos.y);
		viewMat = glm::translate(viewMat, glm::vec3(normMousePos.x * WIN_RATIO, normMousePos.y, 0.0f));
		viewMat = glm::scale(viewMat, glm::vec3(1.1f, 1.1f, 0.0f));
		viewMat = glm::translate(viewMat, glm::vec3(-normMousePos.x * WIN_RATIO, -normMousePos.y, 0.0f));
	*/
}

Game::~Game() {
	// Delete services
	locator::debugDraw::reset();
	locator::audio::reset();
	locator::random::reset();

	// Delete systems
	delete renderSystem;
	delete animationSystem;
	delete movementSystem;
	delete constructionSystem;
	delete waveSystem;
	delete attackSystem;
	delete healthSystem;

	// Delete level manager
	delete level;

	// Delete states
	delete m_levelState;
	delete m_titleState;
	delete m_gameOverState;

	// Shutdown 
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    spdlog::drop_all();
    spdlog::shutdown();

	m_bInit = false;
	m_bInstanciated = false;
}

int Game::init() {
    if (m_bInit) {
        spdlog::error("[Game] The init function as already been called !");
        return EXIT_FAILURE;
    }

    // Init logger
    spdlog::set_pattern("[%l] %^ %v %$");

    // Init SDL
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

    // Create Window
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

    // Create OpenGl context
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

    // Enable blending & Z-Buffer
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_DEPTH_TEST));

    // ImGui
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

    // Noesis GUI
    Noesis::GUI::Init(noelog::errorHandler, noelog::messageCallback, nullptr);
    Noesis::GUI::SetXamlProvider(Noesis::MakePtr<NoesisApp::LocalXamlProvider>("./res/gui"));
    Noesis::GUI::SetTextureProvider(Noesis::MakePtr<NoesisApp::LocalTextureProvider>("./res/images"));
    Noesis::GUI::SetFontProvider(Noesis::MakePtr<NoesisApp::LocalFontProvider>("./res/fonts"));

	// Services
	locator::debugDraw::set<DebugDrawService>();
	IDebugDraw& debugDraw = locator::debugDraw::ref();
	debugDraw.setProjMat(m_projMat);
	debugDraw.setViewMat(m_viewMat);
	locator::random::set<RandomService>();
	locator::audio::set<AudioService>();

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

	// Level
	level = new Level(registry, "res/levels/level-2.itd", m_viewTranslation, m_viewScale);

	// Systems
	renderSystem = new RenderSystem(registry, m_viewMat, m_projMat);
	animationSystem = new AnimationSystem(registry, emitter);
	movementSystem = new MovementSystem(registry, emitter);
	constructionSystem = new ConstructionSystem(registry, emitter, *level);
	waveSystem = new WaveSystem(registry, emitter, *level);
	attackSystem = new AttackSystem(registry);
	healthSystem = new HealthSystem(registry, emitter);

	// States
	m_levelState = new LevelState(emitter, *animationSystem, *movementSystem, *attackSystem, *renderSystem);
	m_titleState = new TitleScreenState(emitter, *animationSystem, *movementSystem, *attackSystem, *renderSystem);
	m_gameOverState = new GameOverState(emitter, *animationSystem, *movementSystem, *attackSystem, *renderSystem);

    m_bInit = true;
    return EXIT_SUCCESS;
}

void Game::update(float deltatime) {
	switch (m_state) {
	case TITLE_SCREEN:
		m_titleState->update(deltatime);
		break;

	case LEVEL:
		m_levelState->update(deltatime);
		level->drawGraph();
		level->drawGrid();
		break;

	case CINEMATIC:
		break;

	case GAME_OVER:
		m_gameOverState->update(deltatime);
		break;

	default:
		break;
	}
}

/* ----------------------- GETTERS ---------------------- */

SDL_Window* Game::getWindow() { return m_window; }
SDL_GLContext Game::getContext() { return m_context; }
GameState Game::getState() { return m_state; }

/* ----------------------- SETTERS --------------------- */

void Game::setState(GameState state) {
	// TODO handle level number
	// TODO empty registry ?
	// TODO use multiple functions ? (nextLevel, gameover, pause, options ...)
	m_state = state;
}
