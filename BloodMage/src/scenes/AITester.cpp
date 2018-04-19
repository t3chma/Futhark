#include "AITester.h"
#include "base\Utility.h"
#include <iostream>


void AITester::create(fk::Tools& tools) {
	std::vector<fk::Shader> shaders{
		tools.shaders.get("Sprite.vert"),
		tools.shaders.get("Sprite.frag"),
		tools.shaders.get("Sprite.geom")
	};
	textBatchPtr = new fk::SpriteBatch(true);
	map.spriteRenderer.setShaders(shaders);
	cam.setDimensions(tools.windowPtr->getDimensions());
	cam.setZoom(70);

	map.resize(500, 500, tools.textures.get("DevGray128.png", 1));
	map.setFloorTexture(Terrain::Floor::GRASS, tools.textures.get("Grass128.png", 1));
	map.setFloorTexture(Terrain::Floor::DIRT, tools.textures.get("Dirt128.png", 1));
	map.setFloorTexture(Terrain::Floor::TOXIN, tools.textures.get("Toxin128.png", 1));
	map.setFloorTexture(Terrain::Floor::OIL, tools.textures.get("Oil128.png", 1));
	map.setFloorTexture(Terrain::Floor::WATER, tools.textures.get("Water128.png", 1));

	map.setFluidTexture(Terrain::Fluid::TOXIN, tools.textures.get("Toxin128.png", 1));
	map.setFluidTexture(Terrain::Fluid::WATER, tools.textures.get("Water128.png", 1));
	map.setFluidTexture(Terrain::Fluid::OIL, tools.textures.get("Oil128.png", 1));

	map.setVaporTexture(Terrain::Vapor::FIRE, tools.textures.get("Fire128.png", 1));
	map.setVaporTexture(Terrain::Vapor::STEAM, tools.textures.get("Steam128.png", 1));
	map.setVaporTexture(Terrain::Vapor::POISON, tools.textures.get("Poison128.png", 1));

	map.setStaticObjectTexture(Terrain::Object::DEV, tools.textures.get("Wall128.png", 1));
	map.setStaticObjectTexture(Terrain::Object::DOOR, tools.textures.get("Door128.png", 1));
	map.setStaticObjectTexture(Terrain::Object::TREE, tools.textures.get("Tree128.png", 1));

	ActorDef ad;
	ad.textures.push_back(tools.textures.get("Circle.png", 1));
	ad.textures.push_back(tools.textures.get("BigSpurt.png", 1));
	ad.textures.push_back(tools.textures.get("BigSpurt.png", 1));
	ad.textures.push_back(tools.textures.get("BigSlash.png", 1));
	ad.textures.push_back(tools.textures.get("BigSlash.png", 1));
	ad.position.x = 0;
	ad.position.y = 0;
	new Player(map, &tools.ui, ad);

	std::vector<fk::Texture> textures;
	textures.push_back(tools.textures.get("Selector.png", 1));
	mousePtr = new Mouse(map, textures);

	tools.fonts.get("Fonts/eldermagic.ttf").generateCharSprites("  Fonts Work!", *textBatchPtr, glm::vec2(1.0));
}
void AITester::destroy(fk::Tools& tools) {

}
void AITester::open(fk::Tools& tools) {

}
void AITester::close(fk::Tools& tools) {

}
void AITester::update(fk::Tools& tools) {
	cam.update();
	cam.setPosition(map.actorPtrs[0]->getPosition());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	map.render(cam);
	map.spriteRenderer.render(*textBatchPtr, cam.getTransScaledMatrix());
	map.update(cam);
	mousePtr->position = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).position);
	mousePtr->updateBody();
	mousePtr->updateSprite();
	if (tools.ui.getKeyInfo(fk::Key::APOSTROPHE).downFrames == 1) { getCommandLine(tools); }
	if (tools.ui.getKeyInfo(fk::Key::CTRL_L).downFrames > 1) {
		int downFrames = tools.ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames;
		if (downFrames && rightBind) { edit(tools, downFrames, rightBind, rightInfo); }
		downFrames = tools.ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames;
		if (downFrames && leftBind) { edit(tools, downFrames, leftBind, leftInfo); }
	}
}

void AITester::getCommandLine(fk::Tools& tools) {
	tools.windowPtr->minimize();
	printf("Which button would you like to bind?\n");
	printf("(l) Left mouse button\n");
	printf("(r) Right mouse button\n");
	std::string input;
	std::getline(std::cin, input);
	int* bindPtr = nullptr;
	int* infoPtr = nullptr;
	//std::function<void(fk::Tools& tools)>* funcPtr = nullptr;
	switch (input[0]) {
	  case 'l':
		bindPtr = &leftBind;
		infoPtr = &leftInfo;
	  break;
	  case 'r':
		bindPtr = &rightBind;
		infoPtr = &rightInfo;
	  break;
	  default:
		tools.windowPtr->restore();
	  return;
	}
	printf("Which type of action would you like to bind?\n");
	printf("(x) None\n");
	printf("(g) Ground placer\n");
	printf("(f) Fluid placer\n");
	printf("(v) Vapor placer\n");
	printf("(o) Object placer\n");
	printf("(p) Prop placer\n");
	printf("(a) Actor placer\n");
	printf("(b) Behavior modifier\n");
	std::getline(std::cin, input);
	switch (input[0]) {
	  case 'x':
		*bindPtr = NONE;
	  break;
	  case 'g':
		printf("Which ground would you like to place?\n");
		printf("(x) Delete\n");
		printf("(d) Dev\n");
		printf("(g) Grass\n");
		printf("(b) Dirt\n");
		printf("(w) Water\n");
		printf("(o) Oil\n");
		printf("(t) Toxin\n");
		std::getline(std::cin, input);
		switch (input[0]) {
		  case 'x':
			*bindPtr = FLOOR_NONE;
		  break;
		  case 'd':
			*bindPtr = FLOOR_DEV;
		  break;
		  case 'g':
			*bindPtr = FLOOR_GRASS;
		  break;
		  case 'b':
			*bindPtr = FLOOR_DIRT;
		  break;
		  case 'w':
			*bindPtr = FLOOR_WATER;
		  break;
		  case 'o':
			  *bindPtr = FLOOR_OIL;
		  break;
		  case 't':
			*bindPtr = FLOOR_TOXIN;
		  break;
		  default:
		  break;
		}
	  break;
	  case 'f':
		printf("Which fluid would you like to place?\n");
		printf("(x) Delete\n");
		printf("(w) Water\n");
		printf("(o) Oil\n");
		printf("(t) Toxin\n");
		std::getline(std::cin, input);
		switch (input[0]) {
		  case 'x':
			*bindPtr = FLUID_NONE;
		  break;
		  case 'w':
			*bindPtr = FLUID_WATER;
		  break;
		  case 'o':
			*bindPtr = FLUID_OIL;
		  break;
		  case 't':
			*bindPtr = FLUID_TOXIN;
		  break;
		  default:
		  break;
		}
	  break;
	  case 'v':
		printf("Which vapor would you like to place?\n");
		printf("(x) Delete\n");
		printf("(f) Fire\n");
		printf("(p) Poison\n");
		printf("(s) Steam\n");
		std::getline(std::cin, input);
		switch (input[0]) {
		  case 'x':
			*bindPtr = VAPOR_NONE;
		  break;
		  case 'f':
			*bindPtr = VAPOR_FIRE;
		  break;
		  case 'p':
			*bindPtr = VAPOR_POISON;
		  break;
		  case 's':
			*bindPtr = VAPOR_STEAM;
		  break;
		  default:
		  break;
		}
	  break;
	  case 'o':
		printf("Which object would you like to place?\n");
		printf("(x) Delete\n");
		printf("(w) Wall\n");
		printf("(d) Door\n");
		printf("(t) Tree\n");
		std::getline(std::cin, input);
		switch (input[0]) {
		  case 'x':
			*bindPtr = OBJECT_NONE;
		  break;
		  case 'w':
			*bindPtr = OBJECT_WALL;
		  break;
		  case 'd':
			*bindPtr = OBJECT_DOOR;
		  break;
		  case 't':
			*bindPtr = OBJECT_TREE;
		  break;
		  default:
		  break;
		}
		if (input[0] != 'x') {
			printf("How much halth will the object have?\n");
			std::getline(std::cin, input);
			*infoPtr = atoi(input.c_str());
		}
	  break;
	  case 'p':
		printf("Which prop would you like to place?\n");
		printf("(x) Delete\n");
		printf("(b) Box\n");
		std::getline(std::cin, input);
		switch (input[0]) {
		  case 'x':
			*bindPtr = PROP_NONE;
		  break;
		  case 'b':
			*bindPtr = PROP_BOX;
		  break;
		  default:
		  break;
		}
	  break;
	  case 'a':
		printf("Which actor would you like to place?\n");
		printf("(x) Delete\n");
		printf("(g) Grunt\n");
		printf("(s) Spear\n");
		std::getline(std::cin, input);
		switch (input[0]) {
		  case 'x':
			*bindPtr = ACTOR_NONE;
		  break;
		  case 'g':
			*bindPtr = ACTOR_GRUNT;
		  break;
		  case 's':
			*bindPtr = ACTOR_SPEAR;
		  break;
		  default:
		  break;
		}
	  break;
	  case 'b':
		*bindPtr = BEHAVIOR;
	  default:
	  break;
	}
	tools.windowPtr->restore();
}

void AITester::edit(fk::Tools& tools, int downFrames, int bind, int info) {
	ActorDef ad;
	PropDef pd;
	switch (bind) {
	  case FLOOR_NONE:
		map.placeFloor(Terrain::Floor::NONE, mousePtr->position);
	  break;
	  case FLOOR_DEV:
		map.placeFloor(Terrain::Floor::DEV, mousePtr->position);
	  break;
	  case FLOOR_GRASS:
		map.placeFloor(Terrain::Floor::GRASS, mousePtr->position);
	  break;
	  case FLOOR_DIRT:
		map.placeFloor(Terrain::Floor::DIRT, mousePtr->position);
	  break;
	  case FLOOR_WATER:
		map.placeFloor(Terrain::Floor::WATER, mousePtr->position);
	  break;
	  case FLOOR_OIL:
		map.placeFloor(Terrain::Floor::OIL, mousePtr->position);
	  break;
	  case FLOOR_TOXIN:
		map.placeFloor(Terrain::Floor::TOXIN , mousePtr->position);
	  break;
	  case FLUID_NONE:
		map.placeFluid(Terrain::Fluid::NONE, mousePtr->position);
	  break;
	  case FLUID_WATER:
		map.placeFluid(Terrain::Fluid::WATER, mousePtr->position);
	  break;
	  case FLUID_OIL:
		map.placeFluid(Terrain::Fluid::OIL, mousePtr->position);
	  break;
	  case FLUID_TOXIN:
		map.placeFluid(Terrain::Fluid::TOXIN, mousePtr->position);
	  break;
	  case VAPOR_NONE:
		map.placeVapor(Terrain::Vapor::NONE, mousePtr->position);
	  break;
	  case VAPOR_FIRE:
		map.placeVapor(Terrain::Vapor::FIRE, mousePtr->position);
	  break;
	  case VAPOR_POISON:
		map.placeVapor(Terrain::Vapor::POISON, mousePtr->position);
	  break;
	  case VAPOR_STEAM:
		map.placeVapor(Terrain::Vapor::STEAM, mousePtr->position);
	  break;
	  case OBJECT_NONE:
		map.placeStaticObject(Terrain::Object::NONE, info, mousePtr->position);
	  break;
	  case OBJECT_WALL:
		map.placeStaticObject(Terrain::Object::DEV, info, mousePtr->position);
	  break;
	  case OBJECT_DOOR:
		map.placeStaticObject(Terrain::Object::DOOR, info, mousePtr->position);
	  break;
	  case OBJECT_TREE:
		map.placeStaticObject(Terrain::Object::TREE, info, mousePtr->position);
	  break;
	  case PROP_NONE:
		if (downFrames == 1 && mousePtr->objectPtr && mousePtr->objectPtr->category == "prop") {
			mousePtr->objectPtr->despawn = true;
		}
	  break;
	  case PROP_BOX:
		if (downFrames == 1) {
			pd.texture = tools.textures.get("Box128.png", 1);
			pd.physics = true;
			new Prop(map, pd);
		}
	  break;
	  case ACTOR_NONE:
		if (downFrames == 1 && mousePtr->objectPtr && mousePtr->objectPtr->category == "actor") {
			mousePtr->objectPtr->despawn = true;
		}
	  break;
	  case ACTOR_GRUNT:
		if (downFrames == 1) {
			ad.position = mousePtr->position;
			ad.textures.push_back(tools.textures.get("Circle.png", 1));
			ad.textures.push_back(tools.textures.get("Swipe.png", 1));
			ad.textures.push_back(tools.textures.get("Swipe.png", 1));
			new Grunt(map, ad);
		}
	  break;
	  case ACTOR_SPEAR:
		if (downFrames == 1) {
			ad.position = mousePtr->position;
			ad.textures.push_back(tools.textures.get("Circle.png", 1));
			ad.textures.push_back(tools.textures.get("Stab.png", 1));
			new Spear(map, ad);
		}
	  break;
	  case BEHAVIOR:
		if (downFrames == 1) {
			if (mousePtr->objectPtr) {
				if (mousePtr->objectPtr->category == "actor") {
					if (mousePtr->acotrEditPtr) { mousePtr->acotrEditPtr->hideNodes(); }
					mousePtr->acotrEditPtr = static_cast<Actor*>(mousePtr->objectPtr);
					mousePtr->acotrEditPtr->showNodes();
					mousePtr->orderEditPtr = nullptr;
				} else if (mousePtr->objectPtr->category == "order") {
					Order* order = static_cast<Order*>(mousePtr->objectPtr);
					if (order->ownerPtr == mousePtr->acotrEditPtr) { mousePtr->orderEditPtr = order; }
				}
			} else if (mousePtr->acotrEditPtr) {
				std::vector<fk::Texture> textures;
				textures.push_back(tools.textures.get("test.png", 1));
				textures.push_back(tools.textures.get("arrow.png", 1));
				if (mousePtr->orderEditPtr) { mousePtr->orderEditPtr->addNext(textures, mousePtr->position); }
				else { mousePtr->acotrEditPtr->addOrder(textures, mousePtr->position); }
			}
		}
	  break;
	  default:
	  break;
	}
}
