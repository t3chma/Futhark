#pragma once
#include <string>
#include <GLM/vec2.hpp>
class SDL_Window;
union SDL_Event;
namespace fk {

/* Window class using SDL
[t3chma] */
class Window {
public:
	
	static const int BORDERED = 1 << 0;
	static const int FULLSCREEN = 1 << 1;
	static const int BORDERLESS = 1 << 2;
	static const int HIGH_DPI = 1 << 3;
	static const int MINIMIZED = 1 << 4;
	static const int INVISIBLE = 1 << 5;
	static const int RESIZABLE = 1 << 6;

	/* Launch the SDL window.
	(WINDOW_NAME) The name of the window.
	(WINDOW_FLAGS) ORable flags: INVISIBLE, FULLSCREEN, BORDERLESS.
	(WINDOW_WIDTH) The width of the window.
	(WINDOW_HEIGHT) The height of the window.
	[T3chma : 2018/01/31] */
	void initialize(
		const std::string& WINDOW_NAME = "Default Window Name",
		const int& WINDOW_FLAGS = BORDERED,
		const int& WINDOW_WIDTH = 1000,
		const int& WINDOW_HEIGHT = 500
	);

	int getWidth() const;
	int getHeight() const;
	///void setDimentions(const int& WINDOW_WIDTH, const int& WINDOW_HEIGHT);
	///void setMouseWindowCoordinates(const float& X, const float& Y);
	glm::vec2 getMouseWindowCoordinates() const;
	void minimize();
	void restore();

	/* Swap the second OpenGL screen buffer to the screen.
	[T3chma : 2018/01/31] */
	void swapGLBuffer();

	// TODO: doc
	void handleEvents(const SDL_Event& sdlEvent);

private:

	// Current width of the window.
	int m_width{ 720 };

	// Current height of the window.
	int m_height{ 480 };

	// A handle to the actual SDL window this class wraps.
	SDL_Window* p_windowPtr{ nullptr };

	// The current mouse window coordinates.
	glm::vec2 m_mouseWindowCoordinates{ 0.0f };
		
	// A debug flag for checking if a window is initialized or not.
	bool m_windowIsInitialized{ false };
};

}