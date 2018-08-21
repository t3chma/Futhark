#pragma once
#include <unordered_map>
/* If you are getting linking errors when using this you probably forgot to define the apply function
for the struct you are defining. Look ate the bottom half of the cpp file for examples. */
#define DEFINE_SPELL(structName, type, target, name)\
	struct structName : public Spell {\
		structName() : Spell(Spell::type, Spell::target, name) {};\
		virtual Spell* copy() { return new structName(); };\
		virtual void apply(Actor* targetPtr);\
	};
class Actor;

struct Spell {
	enum Type { EARTH, AIR, WATER, FIRE } type;
	enum Target { SELF, MELEE, GUN} target;
	int uses{ 1 };
	std::string name{ "" };
	Spell(Type type, Target target, std::string name) : type(type), target(target), name(name) {};
	virtual Spell* copy() = 0;
	virtual void apply(Actor* targetPtr) = 0;
	virtual void updateSprite(Actor* targetPtr) {};
	virtual void applyPhysics(Actor* targetPtr) {};
};

class SpellCatalog {
  public:
	class Item {
	  public:
		friend class SpellCatalog;
		std::string getName() { return m_spellPtr ? m_spellPtr->name : ""; }
		virtual Spell* cast() { return m_spellPtr ? m_spellPtr->copy() : nullptr; }
	  private:
		Spell* m_spellPtr{ nullptr };
		std::unordered_map<char, Item> m_subItems;
	};
	SpellCatalog();
	SpellCatalog::Item getItem(std::string& sequence);
	void setSequence(char* sequence, Spell* spellPtr);
	void setSequence(std::string& sequence, Spell* spellPtr);
  private:
	Item m_head;
};

DEFINE_SPELL(RPChain, EARTH, GUN, "Ranged Precision Chain");
DEFINE_SPELL(SHeal, WATER, SELF, "Self Heal");
DEFINE_SPELL(MPetrify, EARTH, MELEE, "Melee Petrify");
DEFINE_SPELL(RAWeaken, AIR, GUN, "Ranged Area Weaken");
DEFINE_SPELL(MBlowBack, AIR, MELEE, "Melee Blow Back");
DEFINE_SPELL(RPBlowForward, AIR, GUN, "Ranged Precision Blow Forward");

DEFINE_SPELL(MWetten, WATER, MELEE, "Melee Wetten");
DEFINE_SPELL(MOiled, EARTH, MELEE, "Melee Oil");
DEFINE_SPELL(RASlow, WATER, GUN, "Ranged Area Slow");
DEFINE_SPELL(RPPoison, EARTH, GUN, "Ranged Percision Poison");
DEFINE_SPELL(RPKnockDown, EARTH, MELEE, "Ranged Percision Knock Down");
DEFINE_SPELL(RABond, WATER, GUN, "Ranged Area Bond");
DEFINE_SPELL(MTPAttack, AIR, MELEE, "Melee Teleport Attack");

