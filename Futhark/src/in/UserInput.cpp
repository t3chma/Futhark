#include "UserInput.h"

fk::UserInput::UserInput(int mouseHistory) {
	if (mouseHistory < 2) { mouseHistory = 2; }

	m_keyHistory.resize(2);
	for (auto&& frameMap : m_keyHistory) { frameMap.reserve(512); }

	m_buttHistory.resize(2);
	for (auto&& frameMap : m_buttHistory) { frameMap.reserve(512); }

	m_mouseHistory.resize(mouseHistory);

	m_downKeys.reserve(16);
	m_upKeys.reserve(16);
	m_downButts.reserve(5);
	m_upButts.reserve(5);
}

fk::GameState fk::UserInput::update() {

	m_keyHistory.rotate(m_keyHistory.end());
	m_buttHistory.rotate(m_buttHistory.end());
	m_mouseHistory.rotate(m_mouseHistory.end());

	GameState gs = m_poll();

	for (auto&& modIt = m_ModHistory.front().begin(); modIt != m_ModHistory.front().end(); ++modIt) {
		if (modIt->second.down) {
			auto pressedMod = modIt->first;
			MouseInfo& mouseInfo = m_mouseHistory.front();
			for (auto&& pressedButt : m_downButts) {
				auto pair = std::make_pair(pressedButt, pressedMod);
				KeyInfo& ki = m_buttHistory.front()[pair];
				ki.down = true;
				ki.mousePos = m_mouseHistory.front().position;
				if (m_buttHistory[1][pair].down) {
					queuedActions.emplace_back(ki.holdBinding, ki.holdUndo);
				}
				else {
					queuedActions.emplace_back(ki.downBinding, ki.downUndo);
				}
			}
			for (auto&& unpressedButt : m_upButts) {
				KeyInfo& ki = m_buttHistory.front()[std::make_pair(unpressedButt, pressedMod)];
				ki.down = false;
				queuedActions.emplace_back(ki.upBinding, ki.upUndo);
			}
			for (auto&& pressedKey : m_downKeys) {

				auto pair = std::make_pair(pressedKey, pressedMod);
				KeyInfo& ki = m_keyHistory.front()[pair];
				ki.down = true;
				ki.mousePos = m_mouseHistory.front().position;
				if (m_keyHistory[1][pair].down) {
					queuedActions.emplace_back(ki.holdBinding, ki.holdUndo);
				}
				else {
					queuedActions.emplace_back(ki.downBinding, ki.downUndo);
				}
			}
			for (auto&& unpressedKey : m_upKeys) {
				KeyInfo& ki = m_keyHistory.front()[std::make_pair(unpressedKey, pressedMod)];
				ki.down = false;
				queuedActions.emplace_back(ki.upBinding, ki.upUndo);
			}

		}
	}

	m_downKeys.clear();
	m_upKeys.clear();
	m_downButts.clear();
	m_upButts.clear();

	return gs;
}

bool fk::UserInput::isModKey(int key) {
	switch (key) {
	case (int)ModKey::SHIFT_L:
	case (int)ModKey::SHIFT_R:
	case (int)ModKey::ALT_L:
	case (int)ModKey::ALT_R:
	case (int)ModKey::CTRL_L:
	case (int)ModKey::CTRL_R:
		return true;
	break;
	default:
		return false;
	break;
	}
}

fk::GameState fk::UserInput::m_poll() {
	SDL_Event sdlEvent;
	GameState gs = GameState::PLAY;
	m_ModHistory.front()[0].down = true;

	// Loops until there are no more events to process
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			gs = GameState::EXIT;
		break;
		case SDL_KEYDOWN:
			if (isModKey(sdlEvent.key.keysym.sym)) {
				m_ModHistory.front()[sdlEvent.key.keysym.sym].down = true;
				m_ModHistory.front()[0].down = false;
			}
			m_downKeys.push_back(sdlEvent.key.keysym.sym);
		break;
		case SDL_KEYUP:
			m_upKeys.push_back(sdlEvent.key.keysym.sym);
		break;
		case SDL_MOUSEBUTTONDOWN:
			m_downButts.push_back(sdlEvent.key.keysym.sym);
		break;
		case SDL_MOUSEBUTTONUP:
			m_upButts.push_back(sdlEvent.key.keysym.sym);
		case SDL_MOUSEMOTION:
			MouseInfo& mouseInfo = m_mouseHistory.front();
			mouseInfo.position.x = sdlEvent.motion.x;
			mouseInfo.position.y = sdlEvent.motion.y;
			mouseInfo.windowID = sdlEvent.motion.windowID;
		break;
		default:
			for (auto&& windowPtr : windowPtrs) { windowPtr->handleEvents(sdlEvent); }
		break;
		}
	}

	return gs;
}

fk::QueuedAction::QueuedAction(Action& action, bool undo) :
	undo(undo), actionPtr(&action) {};