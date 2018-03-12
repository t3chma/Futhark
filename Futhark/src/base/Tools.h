#pragma once
#include "../in/UserInput.h"
#include "../in/FileCache.h"
#include "Utility.h"
#include "../in/ActionQueue.h"
namespace fk {


struct Tools {
	/* Used to switch between app modes.
	^ Utility.h: GameState
	(PLAY) Normal operation mode.
	(EXIT) Shuts down the app.
	[T3chma : 2018/01/31] */
	GameState gameState{ GameState::PLAY };
	// If this is not "" then the engine will try to switch to the screen associated to the given name.
	std::string nextSceneName{ "" };
	// For logging player input.
	UserInput ui;
	// A handle to this screens window.
	Window* windowPtr{ nullptr };
	// A handle to the game's cached shaders.
	ShadersCache shaders;
	// A handle to the game's cached textures.
	TextureCache textures;
	// Collects and executes actions
	ActionQueue actionQueue;
};

}