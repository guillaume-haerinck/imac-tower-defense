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
#include "services/helper/helper-service.hpp"
#include "events/change-game-state.hpp"

/* ------------------------ LIFETIME ------------------------ */

bool Game::m_bInstanciated = false;
bool Game::m_bInit = false;

Game::Game(EventEmitter& emitter)
:   m_window(nullptr), m_context(nullptr), emitter(emitter),
	level(nullptr), m_state(GameState::LEVEL), progression(emitter),

	// Camera
	m_projMat(glm::ortho(0.0f, PROJ_WIDTH_RAT, 0.0f, PROJ_HEIGHT, -10.0f, 10.0f)),
	m_viewMat(glm::mat4(1.0f)),
	m_viewTranslation(glm::vec2(0.0f)), m_viewScale(1.0f),

	// Systems
	renderSystem(nullptr),
	animationSystem(nullptr),
	movementSystem(nullptr),
	waveSystem(nullptr),
	attackSystem(nullptr),
	lifeAndDeathSystem(nullptr),

	// States
	m_cinematicState(nullptr),
	m_titleState(nullptr),
	m_levelIntroState(nullptr),
	m_levelState(nullptr),
	m_levelExitState(nullptr),
	m_gameOverState(nullptr)
	
{
	assert(!m_bInstanciated);
	m_bInstanciated = true;

	// Handle state changes
	// TODO use an interfare vector to do this automatically in a loop ?
	emitter.on<evnt::ChangeGameState>([this](const evnt::ChangeGameState & event, EventEmitter & emitter) {
		// Exit current state
		switch (this->m_state) {
		case CINEMATIC:
			m_cinematicState->exit();
			break;

		case TITLE_SCREEN:
			m_titleState->exit();
			break;

		case LEVEL_INTRO:
			m_levelIntroState->exit();
			break;

		case LEVEL:
			m_levelState->exit();
			break;

		case LEVEL_EXIT:
			m_levelExitState->exit();
			break;

		case GAME_OVER:
			m_gameOverState->exit();
			break;

		default:
			break;
		}

		// Enter new state
		switch (event.state) {
		case CINEMATIC:
			if (m_cinematicState == nullptr) {
				m_cinematicState = new CinematicState(*this);
			}
			m_cinematicState->enter();
			break;

		case TITLE_SCREEN:
			if (m_titleState == nullptr) {
				m_titleState = new TitleScreenState(*this);
			}
			m_titleState->enter();
			break;

		case LEVEL_INTRO:
			if (m_levelIntroState == nullptr) {
				m_levelIntroState = new LevelIntroState(*this);
			}
			m_levelIntroState->enter();
			break;

		case LEVEL:
			if (m_levelState == nullptr) {
				m_levelState = new LevelState(*this);
			}
			level->setLevel(event.subState);
			m_levelState->enter();
			break;

		case LEVEL_EXIT:
			if (m_levelExitState == nullptr) {
				m_levelExitState = new LevelExitState(*this);
			}
			m_levelExitState->enter();
			break;

		case GAME_OVER:
			if (m_gameOverState == nullptr) {
				m_gameOverState = new GameOverState(*this);
			}
			m_gameOverState->enter();
			break;

		default:
			break;
		}

		// Update state
		this->m_state = event.state;
	});
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
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);

    // Enable blending & Z-Buffer
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthMask(GL_TRUE));
	GLCall(glDepthFunc(GL_LESS));
	GLCall(glClearColor(1, 1, 1, 1));

    // ImGui
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

    // Noesis GUI
    Noesis::GUI::Init(noelog::errorHandler, noelog::messageCallback, nullptr);
    Noesis::GUI::SetXamlProvider(Noesis::MakePtr<NoesisApp::LocalXamlProvider>("./res/gui"));
    Noesis::GUI::SetTextureProvider(Noesis::MakePtr<NoesisApp::LocalTextureProvider>("./res/images"));
    Noesis::GUI::SetFontProvider(Noesis::MakePtr<NoesisApp::LocalFontProvider>("./res/fonts"));

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

	// Services
	locator::debugDraw::set<DebugDrawService>();
	IDebugDraw& debugDraw = locator::debugDraw::ref();
	debugDraw.setProjMat(m_projMat);
	debugDraw.setViewMat(m_viewMat);
	locator::random::set<RandomService>();
	locator::audio::set<AudioService>();
	locator::helper::set<HelperService>();

	// Level
	level = new Level(registry, 1, m_viewTranslation, m_viewScale);

	// Special service helper
	IHelper& helper = locator::helper::ref();
	helper.setRegistry(&registry);
	helper.setEmitter(&emitter);
	helper.setLevel(level);

	// Systems
	renderSystem = new RenderSystem(registry, emitter, m_viewMat, m_projMat);
	animationSystem = new AnimationSystem(registry, emitter);
	movementSystem = new MovementSystem(registry, emitter);
	waveSystem = new WaveSystem(registry, emitter, *level);
	attackSystem = new AttackSystem(registry, emitter);
	lifeAndDeathSystem = new LifeAndDeathSystem(registry, emitter, progression);

	// Init current state
	switch (this->m_state) {
	case CINEMATIC:
		m_cinematicState = new CinematicState(*this);
		m_cinematicState->enter();
		break;

	case TITLE_SCREEN:
		m_titleState = new TitleScreenState(*this);
		m_titleState->enter();
		break;

	case LEVEL_INTRO:
		m_levelIntroState = new LevelIntroState(*this);
		m_levelIntroState->enter();
		break;

	case LEVEL:
		m_levelState = new LevelState(*this);
		m_levelState->enter();
		break;

	case LEVEL_EXIT:
		m_levelExitState = new LevelExitState(*this);
		m_levelExitState->enter();
		break;

	case GAME_OVER:
		m_gameOverState = new GameOverState(*this);
		m_gameOverState->enter();
		break;
	}

    m_bInit = true;
    return EXIT_SUCCESS;
}

void Game::update(float deltatime) {
	switch (m_state) {
	case CINEMATIC:
		break;

	case TITLE_SCREEN:
		m_titleState->update(deltatime);
		break;

	case LEVEL_INTRO:
		m_levelIntroState->update(deltatime);
		break;

	case LEVEL:
		m_levelState->update(deltatime);
		//level->drawGraph();
		//level->drawGrid();
		break;

	case LEVEL_EXIT:
		m_levelExitState->update(deltatime);
		break;

	case GAME_OVER:
		m_gameOverState->update(deltatime);
		break;

	default:
		break;
	}
}

Game::~Game() {
	// Delete services
	locator::debugDraw::reset();
	locator::audio::reset();
	locator::random::reset();
	locator::helper::reset();

	// Delete systems
	delete renderSystem;
	delete animationSystem;
	delete movementSystem;
	delete waveSystem;
	delete attackSystem;
	delete lifeAndDeathSystem;

	// Delete level manager
	delete level;

	// Delete states
	delete m_cinematicState;
	delete m_titleState;
	delete m_levelIntroState;
	delete m_levelState;
	delete m_levelExitState;
	delete m_gameOverState;

	// Shutdown 
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	Noesis::GUI::Shutdown();
	SDL_DestroyWindow(m_window);
	SDL_Quit();
	spdlog::drop_all();
	spdlog::shutdown();

	m_bInit = false;
	m_bInstanciated = false;
}

/* ----------------------- GETTERS ---------------------- */

SDL_Window* Game::getWindow() { return m_window; }
SDL_GLContext Game::getContext() { return m_context; }
GameState Game::getState() { return m_state; }

