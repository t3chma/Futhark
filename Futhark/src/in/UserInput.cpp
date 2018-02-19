#include "UserInput.h"
#include <iostream>

fk::UserInput::UserInput(int history) {
	if (history < 2) { history = 2; }

	m_actionHistory.resize(history);
	m_mouseHistory.resize(history);

	m_bindingStates.reserve(512);

	m_downMods.reserve(6);
	m_unpressedMods.reserve(6);
	m_downKeys.reserve(16);
	m_unpressedKeys.reserve(16);
}

void fk::UserInput::bind(Trigger trigger, ModKey modKey, Key key, Action* actionPtr, long holdTime) {
	auto& bind = m_bindingStates[std::make_pair(modKey, key)];
	bind.triggerFrames = holdTime;
	switch (trigger) {
		case Trigger::PRESS:
			bind.pressBinding = actionPtr;
		break;
		case Trigger::UNPRESS:
			bind.unpressBinding = actionPtr;
		break;
		case Trigger::HOLD:
			bind.holdBinding = actionPtr;
		break;
	}
}

void fk::UserInput::bind(Trigger trigger, Key key, Action* actionPtr, long holdTime) {
	if (m_isModKey(static_cast<int>(key))) { bind(trigger, static_cast<ModKey>(key), key, actionPtr, holdTime); }
	else { bind(trigger, ModKey::NO_MOD, key, actionPtr); }
}

fk::GameState fk::UserInput::poll() {

	m_mouseHistory.push_front(m_mouseHistory.front());
	GameState gs = m_pollSDL();
	MouseInfo& mouseInfo = m_mouseHistory.front();

	// Get last Mod key.
	ModKey mk = ModKey::NO_MOD;
	if (!m_downMods.empty()) { mk = m_downMods.back(); }

	// Handle down keys/butts.
	for (auto&& key : m_downKeys) {
		KeyBinding& kb = m_bindingStates[std::make_pair(mk, key)];
		if (++(kb.downFrames) == 1) {
			kb.pressPos = m_mouseHistory.front().position;
			if (kb.pressBinding) { queuedActions.emplace_back(kb.pressBinding, Source::UI); }
			///std::cout << "\npress:\t\t" << (char)mk << " " << (char)key;
		} else {
			if (kb.holdBinding && kb.downFrames > kb.triggerFrames) {
				queuedActions.emplace_back(kb.holdBinding, Source::UI);
			}
			///std::cout << "\nhold:\t\t" << (char)mk << " " << (char)key;
		}
	}

	// Handle unpressed key/butts.
	for (auto&& key : m_unpressedKeys) {
		KeyBinding& kb = m_bindingStates[std::make_pair(mk, key)];
		kb.downFrames = 0;
		kb.unpressPos = m_mouseHistory.front().position;
		if (kb.unpressBinding) { queuedActions.emplace_back(kb.unpressBinding, Source::UI); }
		///std::cout << "\nunpress:\t" << (char)mk << " " << (char)key;
	}

	// Handle unpressed mods.
	for (auto&& unpressedMod : m_unpressedMods) {
		for (auto&& downMod : m_downMods) {
			if (downMod == unpressedMod) {
				downMod = m_downMods.back();
				break;
			}
		}
		m_downMods.pop_back();
	}

	// Clear unpressed lists.
	m_unpressedKeys.clear();
	m_unpressedMods.clear();

	// Log the actions of this screen.
	m_actionHistory.push_front(queuedActions);

	return gs;
}

fk::UserInput::MouseInfo fk::UserInput::getMouseInfo(unsigned int framesAgo) {
	if (framesAgo < m_mouseHistory.size()) { return m_mouseHistory[framesAgo]; }
	else { return m_mouseHistory[m_mouseHistory.size() - 1]; }
}

fk::UserInput::KeyBinding fk::UserInput::getBindingInfo(ModKey modKey, Key key) {
	return m_bindingStates[std::make_pair(modKey, key)];
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
				m_keyEvent(true, sdlEvent.button.button, false);
			break;
			case SDL_MOUSEBUTTONUP:
				m_keyEvent(false, sdlEvent.button.button, false);
			case SDL_MOUSEMOTION:
				mouseInfo.position.x = sdlEvent.motion.x;
				mouseInfo.position.y = sdlEvent.motion.y;
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

void fk::UserInput::m_keyEvent(bool down, int keyID, bool notButt) {
	Key kiPtr = (Key)(notButt ? keyID : keyID);
	// Setup down/up info.
	if (down) {
		// Add key/butt to down list.
		m_downKeys.insert(kiPtr);
		// Add key to down modkey list if modkey.
		if (
			notButt
			&& m_isModKey(keyID)
			&& std::find(
				m_downMods.begin(),
				m_downMods.end(),
				static_cast<ModKey>(keyID)
			) == m_downMods.end()
		) {
			m_downMods.push_back(static_cast<ModKey>(keyID));
		}
		// Note: m_unpressedKeyPtrs get cleared every frame so we don't have to remove from that.
	} else {
		// Find and remove key/butt from down list.
		m_downKeys.erase(kiPtr);
		// Add key to unpressed modkey list if its a modkey.
		if (notButt && m_isModKey(keyID)) { m_unpressedMods.insert(static_cast<ModKey>(keyID)); }
		// Add to the unpressed list. 
		m_unpressedKeys.insert(kiPtr);
	}
}

bool fk::UserInput::m_isModKey(int keyID) {
	switch (keyID) {
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
