#include "UserInput.h"

fk::GameState fk::UserInput::fetch() {
	GameState gs = m_poll();

	return gs;
}

fk::GameState fk::UserInput::m_poll() {
	SDL_Event sdlEvent;
	GameState gs = GameState::PLAY;

	// Loops until there are no more events to process
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			gs = GameState::EXIT;
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
			for (auto&& windowPtr : windowPtrs) { windowPtr->handleEvents(sdlEvent); }
		break;
		}
	}

	return gs;
}

