#pragma once
#include "Grunt.h"


Actor::State* makeState(AgroClass aClass, Actor& actor) {
	switch(aClass) {
		case GRUNT: return new Grunt::Agro(actor);
		default: return nullptr;
	}

}