#include "Window.h"
#include "Error.h"
namespace fk {

void Window::initialize(
	const std::string& T_WINDOW_NAME,
	const Flags& T_WINDOW_FLAGS,
	const unsigned int& T_WINDOW_WIDTH,
	const unsigned int& T_WINDOW_HEIGHT
) {
	m_width = T_WINDOW_WIDTH;
	m_height = T_WINDOW_HEIGHT;
	Uint32 flags(SDL_WINDOW_OPENGL);

	if (T_WINDOW_FLAGS & FULLSCREEN) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		m_width = 0;
		m_height = 0;
	}
	if (T_WINDOW_FLAGS & BORDERLESS) { flags |= SDL_WINDOW_BORDERLESS; }
	if (T_WINDOW_FLAGS & HIGH_DPI) { flags |= SDL_WINDOW_ALLOW_HIGHDPI; }
	if (T_WINDOW_FLAGS & MINIMIZED) { flags |= SDL_WINDOW_MINIMIZED; }
	if (T_WINDOW_FLAGS & INVISIBLE) { flags |= SDL_WINDOW_HIDDEN; }
	if (T_WINDOW_FLAGS & RESIZABLE) { flags |= SDL_WINDOW_RESIZABLE; }

	if (m_height < 1 || m_width < 1) {
		SDL_DisplayMode current;

		if (SDL_GetCurrentDisplayMode(0, &current) != 0) {
			// Error
			SDL_Log("\nCould not get display mode for video display #%d: %s", 0, SDL_GetError());
		} else {
			if (m_width == 0) m_width = current.w;
			if (m_height == 0) m_height = current.h;
			if (m_width < 0) m_width = current.w + m_width;
			if (m_height < 0) m_height = current.h + m_height;
		}
	}

	// Use this function to create a window with the specified position, dimensions, and flags
	// https://wiki.libsdl.org/SDL_CreateWindow
	p_windowPtr = SDL_CreateWindow(
		T_WINDOW_NAME.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_width,
		m_height,
		flags
	);
	ASSERT_BREAK(p_windowPtr == nullptr) {
		printf("\nFailed to create SDL window: %s", SDL_GetError());
		//Errors::fatal("", __FILE__, (long)__LINE__);
	}

	// Use this function to create an OpenGL context for use with
	// an OpenGL window, and make it current
	// https://wiki.libsdl.org/SDL_GL_CreateContext
	ASSERT_BREAK(SDL_GL_CreateContext(p_windowPtr) == nullptr, "Could not make gl context")

	// http://glew.sourceforge.net/basic.html
	TRY_GLEW(glewInit());

	// Check OpenGL version
	std::printf("\nOpenGL Version: %s", glGetString(GL_VERSION));

	// Set the background color
	TRY_GL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

	// Vsync
	// https://wiki.libsdl.org/SDL_GL_SetSwapInterval
	TRY_SDL(SDL_GL_SetSwapInterval(1));

	// Enable alpha blending
	// https://www.opengl.org/sdk/docs/man2/xhtml/glEnable.xml
	glEnable(GL_BLEND);
	// Tell how to blend
	// https://www.opengl.org/sdk/docs/man2/xhtml/glBlendFunc.xml
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Futhark::SpriteBatch::initialize();
	//Futhark::WireBatch::initialize();

	m_windowIsInitialized = true;
}

int Window::getWidth() const {
	return m_width;
}

int Window::getHeight() const {
	return m_height;
}

void Window::setDimentions(const int& T_WINDOW_WIDTH, const int& T_WINDOW_HEIGHT) {
	m_width = T_WINDOW_WIDTH; m_height = T_WINDOW_HEIGHT;
}

void Window::setMouseWindowCoordinates(const float& T_X, const float& T_Y) {
	m_mouseWindowCoordinates.x = T_X; m_mouseWindowCoordinates.y = T_Y;
}

glm Window::getMouseWindowCoordinates() const {
	return m_mouseWindowCoordinates;
}

void Window::minimize() {
	SDL_MinimizeWindow(p_windowPtr);
}

void Window::restore() {
	SDL_RestoreWindow(p_windowPtr);
}

void Window::swapGLBuffer() {
	ASSERT_BREAK(!m_windowIsInitialized, "Window not initialized");
	// ^ https://wiki.libsdl.org/SDL_GL_SwapWindow
	SDL_GL_SwapWindow(p_windowPtr);
}

}