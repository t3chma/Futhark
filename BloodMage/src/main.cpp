#include "base/Engine.h"
#include "scenes/AITester.h"

int main(int argc, char** argv) {
	fk::Engine game;
	game.makeWindow("Sample Name", -100, -100, fk::Window::RESIZABLE);
	game.addWindowScene(0, "AITester", new AITester);
	game.setWindowScene(0, "AITester");
	game.run();
	return 0;
}