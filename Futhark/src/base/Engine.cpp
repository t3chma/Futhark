#pragma once
#include "Engine.h"
#include "Error.h"
//#include "SpriteBatch.h"
namespace fk {

Engine::Engine() { m_initializeEngine(); }
Engine::~Engine() { stop(); }

void Engine::run() {
	m_gameLoop();
	stop();
}

void Engine::stop() {
	clearScenes();
	// Use this function to clean up all initialized subsystems.
	// ^ https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();
}

void Engine::initializeWindow(
	const std::string& NAME,
	const int flags,
	int width, int height
) { p_window.initialize(NAME, flags, width, height); }

void Engine::addScene(const std::string& NAME, Scene* const scenePTR) {
	scenePTR->p_windowPtr = &p_window;
	//TODO: set cams to window dims
	m_scenePtrMap[NAME] = scenePTR;
}

void Engine::setScene(const std::string& SCREEN_NAME) {
	auto mapSelection(m_scenePtrMap.find(SCREEN_NAME));
	if (mapSelection == m_scenePtrMap.end()) {
		LOG_LINE("Scene \"" + SCREEN_NAME + "\" doesn't exist");
	} else {
		if (m_currentScenePtr != nullptr) { m_currentScenePtr->close(); }
		mapSelection->second->open();
		m_currentScenePtr = mapSelection->second;
	}
}

Scene* Engine::getScenePtr(const std::string& SCREEN_NAME) const {
	if (SCREEN_NAME == "") {
		return m_currentScenePtr;
	} else {
		auto mapSelection(m_scenePtrMap.find(SCREEN_NAME));
		if (mapSelection == m_scenePtrMap.end()) {
			return nullptr;
		} else { return mapSelection->second; }
	}
}

void Engine::clearScenes() {
	for (const auto& e : m_scenePtrMap) { delete e.second; }
	m_scenePtrMap.clear();
}

void Engine::m_initializeEngine() {
	///Errors::initialize(&p_window);

	// Initialize SDL library. Must be called before using any other SDL function.
	// ^ https://wiki.libsdl.org/SDL_Init
	TRY_SDL(SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_JOYSTICK));

	// Set an OpenGL window attribute before window creation.
	// ^ https://wiki.libsdl.org/SDL_GL_SetAttribute
	TRY_SDL(SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1));
	TRY_SDL(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));

	///ResourceCache::initialize();
	///UIManager::initialize();
	///Futhark::SpriteBatch::initialize();
}

void Engine::m_gameLoop() {
	while (p_gameState != GameState::EXIT) {
		m_getInput();
		if (getScenePtr()->nextSceneName != "") {
			std::string nextSceneName = getScenePtr()->nextSceneName;
			getScenePtr()->nextSceneName = "";
			setScene(nextSceneName);
		}
		getScenePtr()->update(p_gameState);
		p_window.swapGLBuffer();
	}
}

void Engine::m_getInput() {
	///UIManager::update();

	SDL_Event sdlEvent;

	// Loops until there are no more events to process
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			p_gameState = GameState::EXIT;
		break;
		case SDL_KEYDOWN:
			///UIManager::setPressedKey(eventSDL.key.keysym.sym);
		break;
		case SDL_KEYUP:
			///UIManager::setUnpressedKey(eventSDL.key.keysym.sym);
		break;
		case SDL_MOUSEBUTTONDOWN:
			///UIManager::setPressedKey(eventSDL.button.button);
		break;
		case SDL_MOUSEBUTTONUP:
			///UIManager::setUnpressedKey(eventSDL.button.button);
		break;
		case SDL_JOYAXISMOTION:
			///UIManager::setJoystickAxis(eventSDL.jaxis.which, eventSDL.jaxis.axis, eventSDL.jaxis.value);
		break;
		case SDL_JOYHATMOTION:
			///UIManager::setHat(eventSDL.jaxis.which, eventSDL.jhat.value);
		break;
		case SDL_JOYBUTTONDOWN:
			///UIManager::setPressedButton(eventSDL.jaxis.which, eventSDL.jbutton.button);
		break;
		case SDL_JOYBUTTONUP:
			///UIManager::setUnpressedButton(eventSDL.jaxis.which, eventSDL.jbutton.button);
		break;
		case SDL_MOUSEMOTION:
			///UIManager::mouseMotion();
		//FALL
		default:
			p_window.handleEvents(sdlEvent);
		break;
		}
	}
}

}