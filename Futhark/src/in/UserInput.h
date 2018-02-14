#pragma once
#include <vector>
#include <SDL/SDL.h>
#include "../base/Window.h"
#include "../base/Utility.h"
#include <unordered_map>
#include <boost/preprocessor/debug/error.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/functional/hash.hpp>
#include <queue>
#include <GLM/vec2.hpp>
namespace fk {

class Action {
public:
	Action* undo{ nullptr };
	virtual void execute() = 0;
};

enum class Source { UI, AI };

class QueuedAction {
public:
	Action* actionPtr{ nullptr };
	Source source{ Source::AI };
	QueuedAction(Action* actionPtr, Source source);
};

enum class Trigger { PRESS, UNPRESS, HOLD };

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
	RETURN = SDLK_RETURN, TAB = SDLK_TAB, BACK_SPC = SDLK_BACKSPACE,
	// Mod Keys
	SHIFT_L = SDLK_LSHIFT, SHIFT_R = SDLK_RSHIFT,
	ALT_L = SDLK_LALT, ALT_R = SDLK_RALT,
	CTRL_L = SDLK_LCTRL, CTRL_R = SDLK_RCTRL
}; // 80 * 4 < 512 keys.

// Subset of Key.
enum class ModKey {
	NO_MOD = 0,
	SHIFT_L = SDLK_LSHIFT, SHIFT_R = SDLK_RSHIFT,
	ALT_L = SDLK_LALT, ALT_R = SDLK_RALT,
	CTRL_L = SDLK_LCTRL, CTRL_R = SDLK_RCTRL
};

enum class Button {
	LEFT = SDL_BUTTON_LEFT, MIDDLE = SDL_BUTTON_MIDDLE, RIGHT = SDL_BUTTON_RIGHT,
	X1 = SDL_BUTTON_X1, X2 = SDL_BUTTON_X2
};

class UserInput {
public:

	struct KeyInfo {
		// How many frames this has been down for.
		long downFrames{ 0 };
		// The mouse position when this was last pressed.
		glm::ivec2 pressPos{ 0 };
		// The mouse position when this was last unpressed.
		glm::ivec2 unpressPos{ 0 };
	};

	struct KeyBinding : public KeyInfo {
		// The action to perform when this key is pressed.
		Action* pressBinding{ nullptr };
		// The action to perform when this key is unpressed.
		Action* unpressBinding{ nullptr };
		// The action to perform when this key is held.
		Action* holdBinding{ nullptr };
	};

	struct MouseInfo {
		int windowID{ 0 };
		glm::ivec2 position{ 0 };
	};

	// Window handles.
	std::vector<Window*> windowPtrs;

	// Queued actions
	std::list<QueuedAction> queuedActions;

	UserInput(int mouseHistory = 3600);

	void bind(Trigger trigger, ModKey modKey, Key key, Action* actionPtr);
	void bind(Trigger trigger, Key key, Action* actionPtr);
	
	GameState poll();

	MouseInfo getMouseInfo(unsigned int framesAgo);
	KeyInfo getKeyInfo(Key key);
	KeyInfo getButtInfo(Button butt);

	void dispatch();

private:

	// History of mouse placement.
	boost::circular_buffer<MouseInfo> m_mouseHistory;

	// Raw SDL key codes to key data (this includes mod keys).
	std::unordered_map<int, KeyInfo> m_keyStats;
	// Raw SDL button codes to key data.
	std::unordered_map<int, KeyInfo> m_buttStats;
	// Binding info for modkey and key pairs.
	std::unordered_map<
		std::pair<ModKey, KeyInfo*>,
		KeyBinding, 
		boost::hash<std::pair<ModKey, KeyInfo*>>
	> m_boundStats;

	// The currently down mod keys.
	std::vector<ModKey> m_downMods;
	// The currently down keys (includes mod keys).
	std::vector<KeyInfo*> m_downPtrs;
	// The just unpressed keys (includes mod keys).
	std::vector<KeyInfo*> m_unpressedPtrs;

	// Polls SDL for key/button info.
	GameState m_pollSDL();

	void m_keyEvent(bool down, int keyID, bool notButt);

	bool m_isModKey(int key);
};

}