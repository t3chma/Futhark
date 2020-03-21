#include "UserInput.h"
namespace fk {


UserInput::UserInput(int history) {
	if (history < 2) { history = 2; }
	m_mouseHistory.resize(history);
	m_keyStates.reserve(512);
	m_downKeys.reserve(16);
	m_unpressedKeys.reserve(16);
	m_joyStates.reserve(512);
	m_downJoys.reserve(16);
	m_unpressedJoys.reserve(16);
	for (int p = 0; p < 5; ++p) {
		m_axiStates[std::make_pair(Joy::LZ, p)] = (int)Joy::MINXI;
		m_axiStates[std::make_pair(Joy::RZ, p)] = (int)Joy::MINXI;
	}
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
	// Get last Mod key.
	// Handle down keys/butts.
	for (auto&& key : m_downJoys) {
		JoyInfo& ki = m_joyStates[key];
		++ki.downFrames;
	}
	// Handle unpressed key/butts.
	for (auto&& key : m_unpressedJoys) {
		JoyInfo& ki = m_joyStates[key];
		ki.downFrames = 0;
	}
	// Clear unpressed lists.
	m_unpressedJoys.clear();
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
UserInput::JoyInfo UserInput::getJoyInfo(Joy joy, int player) {
	return m_joyStates[std::make_pair(joy, player - 1)];
}
int UserInput::getAxiInfo(Joy joy, int player) {
	return m_axiStates[std::make_pair(joy, player - 1)];
}
GameState UserInput::m_pollSDL() {
	SDL_Event sdlEvent;
	GameState gs = GameState::PLAY;
	MouseInfo& mouseInfo = m_mouseHistory.front();
	Key k;
	// Loops until there are no more events to process
	while (SDL_PollEvent(&sdlEvent)) {
		auto j = std::make_pair((Joy)sdlEvent.jbutton.button, (int)sdlEvent.jbutton.which);
		auto a = std::make_pair((Joy)(-sdlEvent.jaxis.axis), (int)sdlEvent.jaxis.which);
		auto h = std::make_pair((Joy)(10), (int)sdlEvent.jaxis.which);
		switch (sdlEvent.type) {
		  case SDL_QUIT:
			gs = GameState::EXIT;
		  break;
		  case SDL_KEYDOWN:
			k = (Key)sdlEvent.key.keysym.sym;
			m_downKeys.insert(k);
		  break;
		  case SDL_KEYUP:
			k = (Key)sdlEvent.key.keysym.sym;
			m_downKeys.erase(k);
			m_unpressedKeys.insert(k);
		  break;
		  case SDL_MOUSEBUTTONDOWN:
			k = (Key)sdlEvent.button.button;
			m_downKeys.insert(k);
		  break;
		  case SDL_MOUSEBUTTONUP:
			k = (Key)sdlEvent.button.button;
			m_downKeys.erase(k);
			m_unpressedKeys.insert(k);
		  break;
		  case SDL_JOYBUTTONDOWN:
			m_downJoys.insert(j);
		  break;
		  case SDL_JOYBUTTONUP:
			m_downJoys.erase(j);
			m_unpressedJoys.insert(j);
		  break;
		  case SDL_JOYHATMOTION:
			if (sdlEvent.jhat.value & SDL_HAT_LEFT) { h.first = Joy::DL; }
			if (sdlEvent.jhat.value & SDL_HAT_RIGHT) { h.first = Joy::DR; }
			if (sdlEvent.jhat.value & SDL_HAT_DOWN) { h.first = Joy::DD; }
			if (sdlEvent.jhat.value & SDL_HAT_UP) { h.first = Joy::DU; }
			m_downJoys.insert(h);
		  break;
		  break;
		  case SDL_JOYAXISMOTION:
			m_axiStates[a] = sdlEvent.jaxis.value;
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

}