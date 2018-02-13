#pragma once
#include <vector>
#include <SDL/SDL.h>
#include "../base/Window.h"
#include "../base/Utility.h"
#include <unordered_map>
#include <boost/circular_buffer.hpp>
#include <queue>
#include <xfunctional>
#include <GLM/vec2.hpp>
#include <set>
namespace fk {

class Action {
public:
	int delay{ 0 };
	virtual bool isUndoable() = 0;
	virtual void execute() = 0;
};

class Undoable : Action {
public:
	virtual bool isUndoable() override final { return true; };
	virtual void undo() = 0;
};

class Permanent : Action {
public:
	virtual bool isUndoable() override final { return false; };
};

class QueuedAction {
public:
	QueuedAction(Action& action, bool undo);
	bool undo{ false };
	Action* actionPtr{ nullptr };
};

enum class ModKey {
	SHIFT_L = SDLK_LSHIFT, SHIFT_R = SDLK_RSHIFT,
	ALT_L = SDLK_LALT, ALT_R = SDLK_RALT,
	CTRL_L = SDLK_LCTRL, CTRL_R = SDLK_RCTRL
};

enum class Key {
	// Numbers
	NUM0 = SDLK_0, NUM1 = SDLK_1, NUM2 = SDLK_2, NUM3 = SDLK_3, NUM4 = SDLK_4,
	NUM5 = SDLK_5, NUM6 = SDLK_6, NUM7 = SDLK_7, NUM8 = SDLK_8, NUM9 = SDLK_9,
	// Numpad
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
	// Misc Keys
	SPACE = SDLK_SPACE, APOSTROPHE = SDLK_BACKQUOTE, COMMA = SDLK_COMMA, PERIOD = SDLK_PERIOD,
	SEMICOLON = SDLK_SEMICOLON, QUOTE = SDLK_QUOTE, BRACKET_L = SDLK_LEFTBRACKET, BRACKET_R = SDLK_RIGHTBRACKET,
	SLASH_F = SDLK_SLASH, SLASH_B = SDLK_BACKSLASH, MINUS = SDLK_MINUS, EQUAL = SDLK_EQUALS, ENTER = SDLK_RETURN,
	RETURN = SDLK_RETURN, TAB = SDLK_TAB, BACK_SPC = SDLK_BACKSPACE, SHIFT_L = SDLK_LSHIFT, SHIFT_R = SDLK_RSHIFT,
	ALT_L = SDLK_LALT, ALT_R = SDLK_RALT, CTRL_L = SDLK_LCTRL, CTRL_R = SDLK_RCTRL
}; // 80 * 4 < 512 keys.

enum class Button {
	LEFT = SDL_BUTTON_LEFT, MIDDLE = SDL_BUTTON_MIDDLE, RIGHT = SDL_BUTTON_RIGHT,
	X1 = SDL_BUTTON_X1, X2 = SDL_BUTTON_X2
};

class UserInput {
public:

	struct KeyInfo {
		// If this key is down.
		bool down{ false };
		// The mouse position when this was last down.
		glm::ivec2 mousePos{ 0 };
		// The action to perform when this key is pressed.
		Action* downBinding{ nullptr };
		bool downUndo;
		// The action to perform when this key is unpressed.
		Action* upBinding{ nullptr };
		bool upUndo;
		// The action to perform when this key is held.
		Action* holdBinding{ nullptr };
		bool holdUndo;
	};

	struct KeyInfo {
		// If this key is down.
		bool down{ false };
		// The mouse position when this was last down.
		glm::ivec2 mousePos{ 0 };
		// The action to perform when this key is pressed.
		Action* downBinding{ nullptr };
		bool downUndo;
		// The action to perform when this key is unpressed.
		Action* upBinding{ nullptr };
		bool upUndo;
		// The action to perform when this key is held.
		Action* holdBinding{ nullptr };
		bool holdUndo;
	};

	struct MouseInfo {
		int windowID{ 0 };
		glm::ivec2 position{ 0 };
	};

	// Window handles.
	std::vector<Window*> windowPtrs;

	std::list<QueuedAction> queuedActions;

	UserInput(int mouseHistory = 3600);

	GameState update();

	bool isModKey(int key);

private:
	GameState m_poll();

	// TODO: doc
	boost::circular_buffer<std::unordered_map<std::pair<int,int>, KeyInfo>> m_keyHistory;

	// TODO: doc
	boost::circular_buffer<std::unordered_map<std::pair<int, int>, KeyInfo>> m_buttHistory;

	// TODO: doc
	boost::circular_buffer<std::unordered_map<int, KeyInfo>> m_ModHistory;

	// TODO: doc
	boost::circular_buffer<MouseInfo> m_mouseHistory;

	// TODO: doc
	std::vector<int> m_downKeys;

	// TODO: doc
	std::vector<int> m_upKeys;

	// TODO: doc
	std::vector<int> m_downButts;

	// TODO: doc
	std::vector<int> m_upButts;
};

}