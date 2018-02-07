#pragma once
#include <SDL/SDL.h>
#include <string>
#include <GLM/vec2.hpp>
namespace fk {

/* Window class using SDL
[t3chma] */
class Window {
public:

	enum Flags {
		BORDERED = 1 << 0,
		FULLSCREEN = 1 << 1,
		BORDERLESS = 1 << 2,
		HIGH_DPI = 1 << 3,
		MINIMIZED = 1 << 4,
		INVISIBLE = 1 << 5,
		RESIZABLE = 1 << 6,
	};

	/* Launch the SDL window.
	(WINDOW_NAME) The name of the window.
	(WINDOW_FLAGS) ORable flags: INVISIBLE, FULLSCREEN, BORDERLESS.
	(WINDOW_WIDTH) The width of the window.
	(WINDOW_HEIGHT) The height of the window.
	[T3chma : 2018/01/31] */
	void initialize(
		const std::string& WINDOW_NAME = "Default Window Name",
		const Flags& WINDOW_FLAGS = BORDERED,
		const int& WINDOW_WIDTH = 1000,
		const int& WINDOW_HEIGHT = 500
	);

	int getWidth() const;
	int getHeight() const;
	void setDimentions(const int& WINDOW_WIDTH, const int& WINDOW_HEIGHT);
	void setMouseWindowCoordinates(const float& X, const float& Y);
	glm::vec2 getMouseWindowCoordinates() const;
	void minimize();
	void restore();

	/* Swap the second OpenGL screen buffer to the screen.
	[T3chma : 2018/01/31] */
	void swapGLBuffer();

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