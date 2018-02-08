#pragma once
#include <vector>
#include <SDL/SDL.h>
#include "../base/Window.h"
#include "../base/Utility.h"
namespace fk {

class UserInput {
public:

	enum Key {
		// Numbers
		NUM0 = SDLK_0, NUM1 = SDLK_1, NUM2 = SDLK_2, NUM3 = SDLK_3, NUM4 = SDLK_4,
		NUM5 = SDLK_5, NUM6 = SDLK_6, NUM7 = SDLK_7, NUM8 = SDLK_8, NUM9 = SDLK_9,
		// Num pad
		PAD0 = SDLK_0, PAD1 = SDLK_1, PAD2 = SDLK_2, PAD3 = SDLK_3, PAD4 = SDLK_4,
		PAD5 = SDLK_5, PAD6 = SDLK_6, PAD7 = SDLK_7, PAD8 = SDLK_8, PAD9 = SDLK_9,
		// Function Keys
		F1 = SDLK_F1, F2 = SDLK_F2, F3 = SDLK_F3, F4 = SDLK_F4, F5 = SDLK_F5, F6 = SDLK_F6,
		F7 = SDLK_F7, F8 = SDLK_F8, F9 = SDLK_F9, F10 = SDLK_F10, F11 = SDLK_F11, F12 = SDLK_F12,
		// Letters
		A = SDLK_a, B = SDLK_b, C = SDLK_c, D = SDLK_d, E = SDLK_e, F = SDLK_f, G = SDLK_g, H = SDLK_h,
		I = SDLK_i, J = SDLK_j, K = SDLK_k, L = SDLK_l, M = SDLK_m, N = SDLK_n, O = SDLK_o, P = SDLK_p,
		Q = SDLK_q, R = SDLK_r, S = SDLK_s, T = SDLK_t, U = SDLK_u, V = SDLK_v, W = SDLK_w, X = SDLK_x,
		Y = SDLK_y, Z = SDLK_z,
		// Misc
		SPACE = SDLK_SPACE, APOSTROPHE = SDLK_BACKQUOTE, COMMA = SDLK_COMMA, PERIOD = SDLK_PERIOD,
		SEMICOLON = SDLK_SEMICOLON, QUOTE = SDLK_QUOTE, BRACKET_L = SDLK_LEFTBRACKET,
		BRACKET_R = SDLK_RIGHTBRACKET, SLASH_F = SDLK_SLASH, SLASH_B = SDLK_BACKSLASH, MINUS = SDLK_MINUS,
		EQUAL = SDLK_EQUALS, ENTER = SDLK_RETURN, RETURN = SDLK_RETURN, TAB = SDLK_TAB,
		BACK_SPC = SDLK_BACKSPACE
	};

	// Window handles.
	std::vector<Window*> windowPtrs;

	UserInput() = default;
	~UserInput() = default;

	GameState fetch();

private:
	GameState m_poll();
};

class KeyBinding {

};

}