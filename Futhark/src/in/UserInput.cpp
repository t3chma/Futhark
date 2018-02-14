#include "UserInput.h"
#include <iostream>

fk::UserInput::UserInput(int mouseHistory) {
	if (mouseHistory < 2) { mouseHistory = 2; }

	m_mouseHistory.resize(mouseHistory);

	m_keyStats.reserve(128);
	m_buttStats.reserve(128);
	m_boundStats.reserve(512);

	m_downMods.reserve(6);
	m_downPtrs.reserve(16);
	m_unpressedPtrs.reserve(16);
}

void fk::UserInput::bind(Trigger trigger, ModKey modKey, Key key, Action* actionPtr) {
	switch (trigger) {
		case Trigger::PRESS:
			m_boundStats[std::make_pair(
				modKey,
				&m_keyStats[static_cast<int>(key)]
			)].pressBinding = actionPtr;
		break;
		case Trigger::UNPRESS:
			m_boundStats[std::make_pair(
				modKey,
				&m_keyStats[static_cast<int>(key)]
			)].unpressBinding = actionPtr;
		break;
		case Trigger::HOLD:
			m_boundStats[std::make_pair(
				modKey,
				&m_keyStats[static_cast<int>(key)]
			)].holdBinding = actionPtr;
		break;
	}
}

void fk::UserInput::bind(Trigger trigger, Key key, Action* actionPtr) {
	if (m_isModKey(static_cast<int>(key))) { bind(trigger, static_cast<ModKey>(key), key, actionPtr); }
	else { bind(trigger, ModKey::NO_MOD, key, actionPtr); }
}

fk::GameState fk::UserInput::poll() {

	m_mouseHistory.push_front();

	GameState gs = m_pollSDL();

	MouseInfo& mouseInfo = m_mouseHistory.front();
	// Update down key/butt info.
	for (auto&& downPtr : m_downPtrs) {
		if (++(downPtr->downFrames) == 1) { downPtr->pressPos = m_mouseHistory.front().position; }
	}
	// Update unpressed key/butt info.
	for (auto&& unpressedPtr : m_downPtrs) {
		unpressedPtr->downFrames = 0;
		unpressedPtr->unpressPos = m_mouseHistory.front().position;
	}
	// Get last Mod key.
	ModKey mk = ModKey::NO_MOD;
	if (m_downMods.size() > 0) { mk = m_downMods.back(); }
	// Handle down keys/butts.
	for (auto&& downPtr : m_downPtrs) {
		KeyBinding& kb = m_boundStats[std::make_pair(mk, downPtr)];
		if (++(kb.downFrames) == 1) {
			kb.pressPos = m_mouseHistory.front().position;
			if (kb.pressBinding) { queuedActions.emplace_back(kb.pressBinding, Source::UI); }
			std::cout << "\npress:" << (int)mk << (char)downPtr;
		} else {
			if (kb.holdBinding) { queuedActions.emplace_back(kb.holdBinding, Source::UI); }
			std::cout << "\nhold:" << (int)mk << (char)downPtr;
		}
	}
	// Handle unpressed key/butts.
	for (auto&& unpressedPtr : m_unpressedPtrs) {
		KeyBinding& kb = m_boundStats[std::make_pair(mk, unpressedPtr)];
		kb.downFrames = 0;
		kb.unpressPos = m_mouseHistory.front().position;
		if (kb.unpressBinding) { queuedActions.emplace_back(kb.unpressBinding, Source::UI); }
		std::cout << "\nunpress:" << (int)mk << (char)unpressedPtr;
	}
	// Clear unpress list.
	m_unpressedPtrs.clear();

	return gs;
}

fk::UserInput::MouseInfo fk::UserInput::getMouseInfo(unsigned int framesAgo) {
	if (framesAgo < m_mouseHistory.size()) { return m_mouseHistory[framesAgo]; }
	else { return m_mouseHistory[m_mouseHistory.size() - 1]; }
}

fk::UserInput::KeyInfo fk::UserInput::getKeyInfo(Key key) {
	return m_keyStats[static_cast<int>(key)];
}

fk::UserInput::KeyInfo fk::UserInput::getButtInfo(Button butt) {
	return m_buttStats[static_cast<int>(butt)];
}

void fk::UserInput::dispatch() {
	for (auto&& queuedAction : queuedActions) { queuedAction.actionPtr->execute(); }
}

fk::GameState fk::UserInput::m_pollSDL() {
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
			m_keyEvent(true, sdlEvent.key.keysym.sym, false);
		break;
		case SDL_MOUSEBUTTONUP:
			m_keyEvent(false, sdlEvent.key.keysym.sym, false);
		case SDL_MOUSEMOTION:
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

void fk::UserInput::m_keyEvent(bool down, int keyID, bool notButt) {
	KeyInfo* kiPtr = &m_keyStats[keyID];
	// Setup down/up info.
	if (down) {
		// Add key/butt to down list.
		m_downPtrs.push_back(kiPtr);
		// Add key to down modkey list if modkey.
		if (notButt && m_isModKey(keyID)) { m_downMods.push_back(static_cast<ModKey>(keyID)); }
		// Note: m_unpressedKeyPtrs get cleared every frame so we don't have to remove from that.
	}
	else {
		// Find and remove key/butt from down list.
		for (auto&& downKeyPtr : m_downPtrs) {
			if (downKeyPtr == kiPtr) {
				downKeyPtr = m_downPtrs.back();
				m_downPtrs.pop_back();
				break;
			}
		}
		// Find and remove key from down modkey list if its a modkey.
		if (notButt && m_isModKey(keyID)) {
			for (auto&& downMod : m_downMods) {
				if (downMod == static_cast<ModKey>(keyID)) {
					downMod = m_downMods.back();
					m_downMods.pop_back();
					break;
				}
			}
		}
		// Add to the unpressed list. 
		m_unpressedPtrs.push_back(kiPtr);
	}
}

bool fk::UserInput::m_isModKey(int key) {
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

fk::QueuedAction::QueuedAction(Action* actionPtr, Source source) :
	actionPtr(actionPtr), source(source) {}
