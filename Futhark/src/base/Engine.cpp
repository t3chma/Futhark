#pragma once
#include "Scene.h"
#include "Engine.h"
#include "Errors.h"
#include <SDL/SDL.h>
#include "Errors.h"
#include <iostream>
//#include "SpriteBatch.h"

namespace fk {

App::~App() { stop(); }

void App::run() {
	m_initializeEngine();
	p_customRun();
	m_gameLoop();
	stop();
}

void App::stop() {
	clearScenes();
	// Use this function to clean up all initialized subsystems.
	// https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();
}

void App::initializeWindow(
	const std::string& NAME,
	Window::Flags flags,
	int width, int height
) { p_window.initialize(NAME, flags, width, height); }

void App::addScene(const std::string& NAME, Scene* const scenePTR) {
	scenePTR->p_windowPtr = windowPtr;
	m_scenePtrMap[NAME] = scenePTR;
}

void App::setScene(const std::string& SCREEN_NAME) {
	auto mapSelection(m_scenePtrMap.find(SCREEN_NAME));
	if (mapSelection == m_scenePtrMap.end()) {
		//Errors::fatal("Scene \"" + SCREEN_NAME + "\" doesn't exist", __FILE__, (long)__LINE__);
	} else {
		if (m_currentScenePtr != nullptr) { m_currentScenePtr->close(); }
		mapSelection->second->open();
		m_currentScenePtr = mapSelection->second;
	}
}

Scene* App::getScenePtr(const std::string& SCREEN_NAME) const {
	if (SCREEN_NAME == "") {
		return m_currentScenePtr;
	} else {
		auto mapSelection(m_scenePtrMap.find(SCREEN_NAME));
		if (mapSelection == m_scenePtrMap.end()) {
			return nullptr;
		} else { return mapSelection->second; }
	}
}

void App::clearScenes() {
	for (const auto& e : m_scenePtrMap) { delete e.second; }
	m_scenePtrMap.clear();
}

void App::m_initializeEngine() {
	//Errors::initialize(&p_window);

	// Use this function to initialize the SDL library.
	// It must be called before using any other SDL function.
	//^ https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_JOYSTICK) < 0) {
		printf("\nFailed to initiate SDL: %s", SDL_GetError());
		//Errors::fatal("", __FILE__, (long)__LINE__);
	}

	// Use this function to set an OpenGL window attribute before window creation.
	//^ https://wiki.libsdl.org/SDL_GL_SetAttribute
	if (SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1) != 0) {
		printf("\nFailed to set attribute: %s", SDL_GetError());
		//Errors::fatal("", __FILE__, (long)__LINE__);
	}

	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
		printf("\nFailed to set attribute: %s", SDL_GetError());
		//Errors::fatal("", __FILE__, (long)__LINE__);
	}

	//ResourceCache::initialize();
	//UIManager::initialize();
	//Futhark::SpriteBatch::initialize();

	// Specify the clear value for the depth buffer
	//^ https://www.opengl.org/sdk/docs/man2/xhtml/glClearDepth.xml
	//glClearDepth(1.0f);
}

void App::m_gameLoop() {
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

void App::m_getInput() {
	//UIManager::update();

	SDL_Event eventSDL;

	// Loops until there are no more events to process
	while (SDL_PollEvent(&eventSDL)) {
		switch (eventSDL.type) {
		case SDL_QUIT:
			p_gameState = GameState::EXIT;
			break;
		case SDL_KEYDOWN:
			//UIManager::setPressedKey(eventSDL.key.keysym.sym);
			break;
		case SDL_KEYUP:
			//UIManager::setUnpressedKey(eventSDL.key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			///std::cout << "X" << eventSDL.motion.x << " Y" << eventSDL.motion.y << "\n";
			p_window.setMouseWindowCoordinates(
				static_cast<float>(eventSDL.motion.x), static_cast<float>(eventSDL.motion.y)
			);
			//UIManager::mouseMotion();
			break;
		case SDL_MOUSEBUTTONDOWN:
			//UIManager::setPressedKey(eventSDL.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			//UIManager::setUnpressedKey(eventSDL.button.button);
			break;
		case SDL_JOYAXISMOTION:
			///if (eventSDL.jaxis.value > 10000 || eventSDL.jaxis.value < -10000) printf("%i", eventSDL.jaxis.axis);
			//UIManager::setJoystickAxis(eventSDL.jaxis.which, eventSDL.jaxis.axis, eventSDL.jaxis.value);
			break;
		case SDL_JOYHATMOTION:
			///printf("B");
			//UIManager::setHat(eventSDL.jaxis.which, eventSDL.jhat.value);
			break;
		case SDL_JOYBUTTONDOWN:
			///printf("B");
			//UIManager::setPressedButton(eventSDL.jaxis.which, eventSDL.jbutton.button);
			break;
		case SDL_JOYBUTTONUP:
			//UIManager::setUnpressedButton(eventSDL.jaxis.which, eventSDL.jbutton.button);
			break;
		}
	}
}
}