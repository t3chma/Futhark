#pragma once
#include <GLEW\glew.h>
#include <SDL\SDL.h>
#include <iostream>
#define LOG_LINE(message) std::cout << message << "\n " << __FILE__ << ":" << __LINE__ << "\n";
#ifdef _DEBUG
	#define TRY_GL(func) clearGLErrors(); func; if (!(logGLCall(#func, __FILE__, __LINE__))) { __debugbreak(); }
	#define TRY_SDL(func) if (func) { logSDLCall(#func, __FILE__, __LINE__); __debugbreak(); }
	#define TRY_GLEW(func) GLenum error = func; if (error != GLEW_OK) { logGLEWCall(#func, __FILE__, __LINE__, error); __debugbreak(); }
	#define ASSERT_BREAK(condition, message) if (condition) { LOG_LINE(message); __debugbreak(); }
#else
	#define TRY_GL(func) func;
	#define TRY_SDL(func) func;
	#define TRY_GLEW(func) func;
	#define ASSERT_BREAK(condition, message) if (condition) { LOG_LINE(message); }
#endif
namespace fk {

static void clearGLErrors() {
	while (glGetError() != GL_NO_ERROR);
}

static bool logGLCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << "): " << function << "\n " << file << ":" << line << "\n";
		return false;
	}
	return true;
}

static void logSDLCall(const char* function, const char* file, int line) {
	std::cout << "[SDL Error] (" << SDL_GetError() << "): " << function << "\n " << file << ":" << line << "\n";
}

static void logGLEWCall(const char* function, const char* file, int line, GLenum error) {
	std::cout << "[GLEW Error] (" << error << "): " << function << "\n " << file << ":" << line << "\n";
}

}