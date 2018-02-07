#include "Window.h"
#include "Error.h"
namespace fk {

void Window::initialize(
	const std::string& WINDOW_NAME,
	const int& WINDOW_FLAGS,
	const int& WINDOW_WIDTH,
	const int& WINDOW_HEIGHT
) {
	m_width = WINDOW_WIDTH;
	m_height = WINDOW_HEIGHT;

	// OR all the flags.
	Uint32 flags(SDL_WINDOW_OPENGL);
	if (WINDOW_FLAGS & FULLSCREEN) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		m_width = 0;
		m_height = 0;
	}
	if (WINDOW_FLAGS & BORDERLESS) { flags |= SDL_WINDOW_BORDERLESS; }
	if (WINDOW_FLAGS & HIGH_DPI) { flags |= SDL_WINDOW_ALLOW_HIGHDPI; }
	if (WINDOW_FLAGS & MINIMIZED) { flags |= SDL_WINDOW_MINIMIZED; }
	if (WINDOW_FLAGS & INVISIBLE) { flags |= SDL_WINDOW_HIDDEN; }
	if (WINDOW_FLAGS & RESIZABLE) { flags |= SDL_WINDOW_RESIZABLE; }

	// Account for window dimensions < 1.
	if (m_height < 1 || m_width < 1) {
		SDL_DisplayMode current;
		// Gets the resolution of the users monitor
		// https://wiki.libsdl.org/SDL_GetCurrentDisplayMode
		TRY_SDL(SDL_GetCurrentDisplayMode(0, &current));
		if (m_width < 1) { m_width = current.w + m_width; }
		if (m_height < 1) { m_height = current.h + m_height; }
	}

	// Create a window with the specified name, position, dimensions, and flags
	// ^ https://wiki.libsdl.org/SDL_CreateWindow
	TRY_SDL(
		p_windowPtr = SDL_CreateWindow(
			WINDOW_NAME.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			m_width,
			m_height,
			flags
		)
	);

	// Create an OpenGL context for use with an OpenGL window, and make it current
	// ^ https://wiki.libsdl.org/SDL_GL_CreateContext
	TRY_SDL(SDL_GL_CreateContext(p_windowPtr));

	// ^ http://glew.sourceforge.net/basic.html
	TRY_GLEW(glewInit());

	// Check OpenGL version
	std::printf("\nOpenGL Version: %s", glGetString(GL_VERSION));

	// Specify the clear value for the depth buffer
	//^ https://www.opengl.org/sdk/docs/man2/xhtml/glClearDepth.xml
	TRY_GL(glClearDepth(1.0f));

	// Set the background color
	TRY_GL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

	// Vsync
	// ^ https://wiki.libsdl.org/SDL_GL_SetSwapInterval
	TRY_SDL(SDL_GL_SetSwapInterval(1));

	// Enable alpha blending
	// ^ https://www.opengl.org/sdk/docs/man2/xhtml/glEnable.xml
	TRY_GL(glEnable(GL_BLEND));
	// Tell how to blend
	// ^ https://www.opengl.org/sdk/docs/man2/xhtml/glBlendFunc.xml
	TRY_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	///Futhark::SpriteBatch::initialize();
	///Futhark::WireBatch::initialize();

	m_windowIsInitialized = true;
}

int Window::getWidth() const {
	return m_width;
}

int Window::getHeight() const {
	return m_height;
}

//void Window::setDimentions(const int& T_WINDOW_WIDTH, const int& T_WINDOW_HEIGHT) {
//	m_width = T_WINDOW_WIDTH; m_height = T_WINDOW_HEIGHT;
//}
//
//void Window::setMouseWindowCoordinates(const float& T_X, const float& T_Y) {
//	m_mouseWindowCoordinates.x = T_X; m_mouseWindowCoordinates.y = T_Y;
//}

glm::vec2 Window::getMouseWindowCoordinates() const {
	return m_mouseWindowCoordinates;
}

void Window::minimize() {
	TRY_SDL(SDL_MinimizeWindow(p_windowPtr));
}

void Window::restore() {
	TRY_SDL(SDL_RestoreWindow(p_windowPtr));
}

void Window::swapGLBuffer() {
	BREAK_IF(!m_windowIsInitialized);
	// ^ https://wiki.libsdl.org/SDL_GL_SwapWindow
	TRY_SDL(SDL_GL_SwapWindow(p_windowPtr));
}

void Window::handleEvents(const SDL_Event& sdlEvent) {
	switch (sdlEvent.type) {
	case SDL_MOUSEMOTION:
		m_mouseWindowCoordinates.x = static_cast<float>(sdlEvent.motion.x);
		m_mouseWindowCoordinates.y = static_cast<float>(sdlEvent.motion.y);
		break;
	case SDL_WINDOWEVENT:
		switch (sdlEvent.window.event) {
		case SDL_WINDOWEVENT_SHOWN:
			SDL_Log("Window %d shown", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_HIDDEN:
			SDL_Log("Window %d hidden", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_Log("Window %d exposed", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_MOVED:
			SDL_Log("Window %d moved to %d,%d",
				sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2
			);
		break;
		case SDL_WINDOWEVENT_RESIZED:
			m_width = static_cast<float>(sdlEvent.window.data1);
			m_height = static_cast<float>(sdlEvent.window.data2);
			SDL_Log(
				"Window %d resized to %dx%d",
				sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2
			);
		break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			m_width = sdlEvent.window.data1;
			m_height = sdlEvent.window.data2;
			SDL_Log(
				"Window %d size changed to %dx%d",
				sdlEvent.window.windowID, sdlEvent.window.data1, sdlEvent.window.data2
			);
		break;
		case SDL_WINDOWEVENT_MINIMIZED:
			SDL_Log("Window %d minimized", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_MAXIMIZED:
			SDL_Log("Window %d maximized", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_RESTORED:
			SDL_Log("Window %d restored", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_ENTER:
			SDL_Log("Mouse entered window %d", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_LEAVE:
			SDL_Log("Mouse left window %d", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			SDL_Log("Window %d gained keyboard focus", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			SDL_Log("Window %d lost keyboard focus", sdlEvent.window.windowID);
		break;
		case SDL_WINDOWEVENT_CLOSE:
			SDL_Log("Window %d closed", sdlEvent.window.windowID);
		break;
		default:
		break;
		}
	break;
	default:
	break;
	}
}

}