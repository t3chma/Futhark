#pragma once
#include <unordered_map>

class Spell {
public:

};

class SpellFactory {
public:
	Spell* cast();
};

class Spells {
public:
	Spells();
	std::unordered_map<std::string, SpellFactory> catalog;
};