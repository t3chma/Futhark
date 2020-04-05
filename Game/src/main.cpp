#include "base/Engine.h"
#include "scenes/GameWorld.h"


int main(int argc, char** argv) {
	fk::Engine Biomech;
	Biomech.makeWindow("Sumogun", 0, 0, fk::Window::RESIZABLE || fk::Window::BORDERLESS);
	Biomech.addWindowScene(0, "GameWorld", new GameWorld);
	Biomech.setWindowScene(0, "GameWorld");
	Biomech.run();
	return 0;
}