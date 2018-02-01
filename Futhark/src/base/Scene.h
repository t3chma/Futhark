#pragma once
#include <vector>
#include "Window.h"
#include <map>
#include <string>
namespace fk {

enum class GameState { PLAY, EXIT };

// A scene class for deriving your own scenes.
class Scene {
public:

	friend class Engine;

	std::string nextSceneName{ "" };

	Scene() = default;
	virtual ~Scene() = default;

	/* This is called when a scene is made the current active scene in a scene list.
	[T3chma : 2018/01/31] */
	virtual void open() = 0;
	
	/* This is called when a DIFFERENT scene is made the current active scene in a scene list
	[T3chma : 2018/01/31] */
	virtual void close() = 0;
	
	/* Updates scene elements before draw (called by App)
	[T3chma : 2018/01/31] */
	virtual void update(GameState& gameState) = 0;

	virtual int getSceneIndex() const final;

protected:

	// A handle to the game's window
	Window* p_windowPtr{ nullptr };

	// Used to keep track of the scenes by the app
	int p_sceneIndex{ -1 };
};

}