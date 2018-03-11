#include "base/Engine.h"
#include "scenes/AITester.h"

int main(int argc, char** argv) {
	fk::Engine BloodMage;
	BloodMage.makeWindow("BloodMage", -50, -100, fk::Window::RESIZABLE);
	BloodMage.addWindowScene(0, "AITester", new AITester);
	BloodMage.setWindowScene(0, "AITester");
	BloodMage.run();
	return 0;
}