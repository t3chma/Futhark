#include "SpellCatalog.h"
#include "actors/Actor.h"

SpellCatalog::SpellCatalog() {
	setSequence("wx", new RPChain());
	setSequence("ss", new SHeal());
	setSequence("ac", new MPetrify());
	setSequence("zz", new RAWeaken());
	setSequence("sw", new MBlowBack());
	setSequence("sx", new RPBlowForward());

	setSequence("saa", new MWetten());
	setSequence("sxx", new MOiled());
	setSequence("wxs", new RASlow());
	setSequence("wwc", new RPPoison());
	setSequence("wsx", new RPKnockDown());
	setSequence("qse", new RABond());
	setSequence("wws", new MTPAttack());
}

SpellCatalog::Item SpellCatalog::getItem(std::string& sequence) {
	Item &item = m_head;
	while(sequence != "") {
		auto selection = item.m_subItems.find(sequence[0]);
		if (selection == item.m_subItems.end()) {
			return Item();
		} else {
			item = item.m_subItems[sequence[0]];
			sequence = sequence.substr(1);
		}
	}
	return item;
}

void SpellCatalog::setSequence(char* sequence, Spell* spellPtr) {
	setSequence(std::string(sequence), spellPtr);
}
void SpellCatalog::setSequence(std::string& sequence, Spell* spellPtr) {
	Item &item = m_head;
	while (sequence != "") {
		item = item.m_subItems[sequence[0]];
		sequence = sequence.substr(1);
	}
	item.m_spellPtr = spellPtr;
}

void RPChain::apply(Actor* targetPtr) {
	//
}

void SHeal::apply(Actor* targetPtr) {
	targetPtr->health += this->uses;
}

void MPetrify::apply(Actor* targetPtr) {
	//
}

void RAWeaken::apply(Actor* targetPtr) {
	//
}

void MBlowBack::apply(Actor* targetPtr) {
	//
}

void RPBlowForward::apply(Actor* targetPtr) {
	//
}


void MWetten::apply(Actor* targetPtr) {
	//
}

void MOiled::apply(Actor* targetPtr) {
	//
}

void RASlow::apply(Actor* targetPtr) {
	//
}

void RPPoison::apply(Actor* targetPtr) {
	//
}

void RPKnockDown::apply(Actor* targetPtr) {
	//
}

void RABond::apply(Actor* targetPtr) {
	//
}

void MTPAttack::apply(Actor* targetPtr) {
	//
}