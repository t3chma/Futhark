#pragma once
#include "Engine.h"
#include "Error.h"
//#include "SpriteBatch.h"
namespace fk {

Engine::Engine() {
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

Engine::~Engine() { stop(); }

void Engine::run() {
	while (m_gameState != GameState::EXIT) {
		m_gameState = m_ui.poll();
		m_ui.dispatch();
		for (int i = 0; i < m_currentScenePtrs.size(); ++i) {
			if (m_currentScenePtrs[i]->nextSceneName != "") {
				std::string nextSceneName = m_currentScenePtrs[i]->nextSceneName;
				m_currentScenePtrs[i]->nextSceneName = "";
				setWindowScene(i, nextSceneName);
			}
			m_currentScenePtrs[i]->update(m_gameState);
			m_windows[i].swapGLBuffer();
		}
	}
	stop();
}

void Engine::stop() {
	clearScenes();
	// Use this function to clean up all initialized subsystems.
	// ^ https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();
}

int Engine::makeWindow(const std::string& NAME, int width, int height, Window::Flag flags) {
	m_windows.emplace_back(NAME, width, height, flags);
	m_currentScenePtrs.emplace_back(nullptr);
	m_ui.windowPtrs.push_back(&m_windows.back());
	return m_windows.size() - 1;
}

void Engine::addWindowScene(int windowIndex, const std::string& name, Scene* const scenePtr) {
	scenePtr->p_windowPtr = &m_windows[windowIndex];
	//TODO: set cams to window dims
	m_scenePtrMap[name] = scenePtr;
}

void Engine::setWindowScene(int windowIndex, const std::string& sceneName) {
	auto mapSelection(m_scenePtrMap.find(sceneName));
	if (mapSelection == m_scenePtrMap.end()) {
		LOG_LINE("Scene \"" + sceneName + "\" doesn't exist");
	} else {
		if (m_currentScenePtrs[windowIndex] != nullptr) { m_currentScenePtrs[windowIndex]->close(); }
		mapSelection->second->open();
		m_currentScenePtrs[windowIndex] = mapSelection->second;
	}
}

fk::Scene* Engine::getWindowScenePtr(int windowIndex, const std::string& sceneName) const {
	if (sceneName == "") {
		return m_currentScenePtrs[windowIndex];
	} else {
		auto mapSelection(m_scenePtrMap.find(sceneName));
		if (mapSelection == m_scenePtrMap.end()) {
			return nullptr;
		} else { return mapSelection->second; }
	}
}

void Engine::clearScenes() {
	for (const auto& e : m_scenePtrMap) { delete e.second; }
	m_scenePtrMap.clear();
}
}