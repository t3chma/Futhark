#include "UserInput.h"
namespace fk {


UserInput::UserInput(int history) {
	if (history < 2) { history = 2; }
	m_mouseHistory.resize(history);
	m_keyStates.reserve(512);
	m_downKeys.reserve(16);
	m_unpressedKeys.reserve(16);
}
GameState UserInput::poll() {
	m_mouseHistory.push_front(m_mouseHistory.front());
	GameState gs = m_pollSDL();
	MouseInfo& mouseInfo = m_mouseHistory.front();
	// Get last Mod key.
	// Handle down keys/butts.
	for (auto&& key : m_downKeys) {
		KeyInfo& ki = m_keyStates[key];
		if (++ki.downFrames == 1) {
			ki.pressPos = m_mouseHistory.front().windowPosition;
		}
	}
	// Handle unpressed key/butts.
	for (auto&& key : m_unpressedKeys) {
		KeyInfo& ki = m_keyStates[key];
		ki.downFrames = 0;
		ki.unpressPos = m_mouseHistory.front().windowPosition;
	}
	// Clear unpressed lists.
	m_unpressedKeys.clear();
	return gs;
}
UserInput::MouseInfo UserInput::getMouseInfo(unsigned int framesAgo) const {
	if (framesAgo < m_mouseHistory.size()) { return m_mouseHistory[framesAgo]; }
	else { return m_mouseHistory[m_mouseHistory.size() - 1]; }
}
void UserInput::setShowCursor(bool show) {
	if (show) { SDL_ShowCursor(SDL_ENABLE); }
	else { SDL_ShowCursor(SDL_DISABLE); }
}
UserInput::KeyInfo UserInput::getKeyInfo(Key key) {
	return m_keyStates[key];
}
GameState UserInput::m_pollSDL() {
	SDL_Event sdlEvent;
	GameState gs = GameState::PLAY;
	MouseInfo& mouseInfo = m_mouseHistory.front();
	// Loops until there are no more events to process
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
		  case SDL_QUIT:
			gs = GameState::EXIT;
		  break;
		  case SDL_KEYDOWN:
			m_keyEvent(true, sdlEvent.key.keysym.sym, true);
		  break;
		  case SDL_KEYUP:
			m_keyEvent(false, sdlEvent.key.keysym.sym, true);
		  break;
		  case SDL_MOUSEBUTTONDOWN:
			m_keyEvent(true, sdlEvent.button.button, false);
		  break;
		  case SDL_MOUSEBUTTONUP:
			m_keyEvent(false, sdlEvent.button.button, false);
		  break;
		  case SDL_MOUSEMOTION:
			mouseInfo.windowPosition.x = sdlEvent.motion.x;
			mouseInfo.windowPosition.y = sdlEvent.motion.y;
			mouseInfo.windowID = sdlEvent.motion.windowID;
		  break;
		  case SDL_MOUSEWHEEL:
			mouseInfo.wheel = sdlEvent.wheel.y;
		  break;
		  default:
			for (auto&& windowPtr : windowPtrs) { windowPtr->handleEvents(sdlEvent); }
		  break;
		}
	}
	return gs;
}
void UserInput::m_keyEvent(bool down, int keyID, bool notButt) {
	Key kiPtr = (Key)(notButt ? keyID : keyID);
	// Setup down/up info.
	if (down) {
		// Add key/butt to down list.
		m_downKeys.insert(kiPtr);
		// Note: m_unpressedKeyPtrs get cleared every frame so we don't have to remove from that.
	}
	else {
		// Find and remove key/butt from down list.
		m_downKeys.erase(kiPtr);
		// Add to the unpressed list. 
		m_unpressedKeys.insert(kiPtr);
	}
}

}
