#include "base/Engine.h"
#include "scenes/AITester.h"

int main(int argc, char** argv) {
	fk::Engine game;
	game.initializeWindow("Sample Name", fk::Window::BORDERED, -100, 0);
	game.addScene("AITester", new AITester);
	game.setScene("AITester");
	return 0;
}