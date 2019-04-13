#include "base/Engine.h"
#include "scenes/AITester.h"


int main(int argc, char** argv) {
	fk::Engine c;
	c.makeWindow("The Sea", 0, 0, fk::Window::RESIZABLE | fk::Window::BORDERLESS);
	c.addWindowScene(0, "AITester", new AITester);
	c.setWindowScene(0, "AITester");
	c.run();
	return 0;
}