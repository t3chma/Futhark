#pragma once
#include "Scene.h"
#include "../in/UserInput.h"
#include <memory>
#include <map>
namespace fk {

/* This is a virtual class intended to be inherited from when creating a game with Futhark.
[T3chma] */
class Engine {
public:

	Engine();
	virtual ~Engine();

	/* Starts the app.
	Make sure to add your screens before you call this.
	[T3chma : 2018/01/31] */
	virtual void run() final;

	/* Stops the app.
	Use this when you want to restart or something without stopping the program.
	[T3chma : 2018/01/31] */
	virtual void stop() final;

	/* Creates the window for the app.
	This is just a wrapper and can be called on p_window instead.
	(name) The name of the window.
	(flags) These ORable flags specify the type of window.
	(width) The width of the window in pixels, 0 = scene width, width < 0 = scene width + width.
	(height) The height of the window in pixels, 0 = scene height, height < 0 = scene width + height.
	< The index of the window.
	[T3chma : 2018/01/31] */
	virtual int makeWindow(
		const std::string& name = "Default Window Name",
		int width = 1000,
		int height = 500,
		Window::Flag flags = Window::RESIZABLE
	) final;

	/* Adds a scene to the internally managed scene list.
	(name) The name of the scene.
	(scenePtr) A pointer to the scene.
	[T3chma : 2018/01/31] */
	virtual void addWindowScene(int windowIndex, const std::string& name, Scene* const scenePtr) final;

	/* Sets the current active scene, closing the old one.
	(sceneName) The name of the scene to switch to.
	[T3chma : 2018/01/31] */
	virtual void setWindowScene(int windowIndex, const std::string& sceneName) final;

	/* Gets the pointer of the scene name given.
	The current scene pointer is returned by default.
	(sceneName) The name of the scene whos pointer will be returned.
	< The pointer to scene with the given name, or nullptr otherwise.
	[T3chma : 2018/01/31] */
	virtual Scene* getWindowScenePtr(int windowIndex, const std::string& sceneName = "") const final;

	/* Deletes all the scenes from the scene list.
	[T3chma : 2018/01/31] */
	virtual void clearScenes() final;

private:

	/* Used to switch between app modes.
	^ Utility.h: GameState
	(PLAY) Normal operation mode.
	(EXIT) Shuts down the app.
	[T3chma : 2018/01/31] */
	GameState m_gameState{ GameState::PLAY };

	/* Window handle.
	^ SDLWindow.h
	[T3chma : 2018/01/31] */
	std::vector<Window> m_windows;

	UserInput m_ui;

	// Hash table for looking up scene pointers by name.
	// ^ http://www.cplusplus.com/reference/map/map/
	std::map<std::string, Scene*> m_scenePtrMap;

	// A handle to the current scene
	std::vector<Scene*> m_currentScenePtrs;
};

}