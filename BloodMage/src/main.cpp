#include "base/Engine.h"
#include "scenes/AITester.h"

int main(int argc, char** argv) {
	fk::Engine game;
	game.initializeWindow("Sample Name", fk::Window::BORDERED | fk::Window::RESIZABLE, -100, -100);
	game.addScene("AITester", new AITester);
	game.setScene("AITester");
	game.run();
	return 0;
}