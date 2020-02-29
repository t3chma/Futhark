#include "Building.h"

Building::~Building() {
	delete peopleTextPtr;
	delete healthTextPtr;
}

void Building::setType(Type t) {
	type = t;
	fk::Texture tex(0);
	sprites.back().setColor(255, 255, 255, 255);
	reset();
	armorV = 1;
	armorP = 1;
	sight = 2;
	switch (t) {
	case CAPITOL:
		tex = textures.get("buildings/capital.png", 1);
		placement = Placement::OUTSKIRT;
		op.skill = Operation::Skill::POLITICAL;
		op.options.emplace_back(true, Job::BUFF, Job::Buff::MOOD, 2);
		op.options.emplace_back(false, Job::TRAIN, Job::Train::POLITICIAN);
		op.options.back().labor = 10;
		op.options.back().cost = 0;
		op.options.back().work = 10;
		break;
	case FARM:
		tex = textures.get("buildings/farm.png", 1);
		placement = Placement::OUTSKIRT;
		op.options.emplace_back(false, Job::STATE, Job::State::FEED);
		op.options.back().labor = 0;
		op.options.back().cost = 0;
		op.options.back().work = 0;
		break;
	case RADIO:
		tex = textures.get("buildings/radio.png", 1);
		op.skill = Operation::Skill::POLITICAL;
		placement = Placement::STATE;
		op.options.emplace_back(false, Job::BUFF, Job::Buff::MOOD, 4);
		op.options.back().labor = 5;
		op.options.back().cost = 0;
		op.options.back().work = 5;
		break;
	case SHIELD:
		tex = textures.get("buildings/shield.png", 1);
		op.skill = Operation::Skill::SCIENTIFIC;
		op.options.emplace_back(true, Job::BUFF, Job::Buff::ARMOR, 1);
		op.options.back().labor = 5;
		op.options.back().cost = 0;
		op.options.back().work = 5;
		break;
	case LIBRARY:
		tex = textures.get("buildings/library.png", 1);
		op.skill = Operation::Skill::SCIENTIFIC;
		op.options.emplace_back(true, Job::BUFF, Job::Buff::COERCION, -3);
		op.options.back().labor = 5;
		op.options.back().cost = 0;
		op.options.back().work = 5;
		break;
	case WAREHOUSE:
		tex = textures.get("buildings/warehouse.png", 1);
		op.skill = Operation::Skill::DOMESTIC;
		op.options.emplace_back(false, Job::BUFF, Job::Buff::SUPPLIES, 2);
		op.options.back().labor = 10;
		op.options.back().cost = 0;
		op.options.back().work = 10;
		break;
	case TRAVEL:
		tex = textures.get("buildings/travel.png", 1);
		op.skill = Operation::Skill::POLITICAL;
		op.options.emplace_back(false, Job::RELOCATE);
		op.options.back().labor = 10;
		op.options.back().cost = 0;
		op.options.back().work = 10;
		break;
	case TURRET:
		tex = textures.get("buildings/turret.png", 1);
		op.skill = Operation::Skill::SCIENTIFIC;
		op.options.emplace_back(true, Job::STATE, Job::State::COUNTER, 1);
		op.options.back().labor = 10;
		op.options.back().cost = 0;
		op.options.back().work = 10;
		break;
	case FORUM:
		tex = textures.get("buildings/forum.png", 1);
		op.skill = Operation::Skill::DOMESTIC;
		op.options.emplace_back(true, Job::STATE, Job::State::TERRITORY, 1);
		op.options.back().labor = 25;
		op.options.back().cost = 0;
		op.options.back().work = 25;
		break;
	case BANK:
		tex = textures.get("buildings/bank.png", 1);
		op.skill = Operation::Skill::DOMESTIC;
		op.options.emplace_back(true, Job::STATE, Job::State::WORK);
		op.options.back().labor = 25;
		op.options.back().cost = 0;
		op.options.back().work = 25;
		break;
	case SUBURB:
		tex = textures.get("buildings/suburb.png", 1);
		break;
	case SETTLEMENT:
		tex = textures.get("buildings/settlement.png", 1);
		break;
	case COUNTRY:
		tex = textures.get("buildings/country.png", 1);
		break;
	case OUTPOST:
		tex = textures.get("buildings/outpost.png", 1);
		break;
	case CABIN:
		tex = textures.get("buildings/cabin.png", 1);
		break;
	case UTILITY:
		tex = textures.get("buildings/utility.png", 1);
		break;
	case LIGHT:
		tex = textures.get("buildings/light.png", 1);
		break;
	case HEAVY:
		tex = textures.get("buildings/heavy.png", 1);
		break;
	case COMBAT:
		tex = textures.get("buildings/combat.png", 1);
		break;
	case TRAINING:
		tex = textures.get("buildings/training.png", 1);
		break;
	case INTEL:
		tex = textures.get("buildings/intel.png", 1);
		break;
	case UNIVERSITY:
		tex = textures.get("buildings/university.png", 1);
		break;
	case HOSPITAL:
		tex = textures.get("buildings/hospital.png", 1);
		break;
	case TRADE:
		tex = textures.get("buildings/trade.png", 1);
		break;
	case STATE:
		tex = textures.get("buildings/state.png", 1);
		break;
	default:
		armorV = 0;
		armorP = 0;
		sight = 0;
		sprites.back().setColor(255, 255, 255, 0);
		break;
	}
	sprites.back().setTexture(tex);
}

inline void Building::draw() {
	std::string peopleText = people > 0 ? std::to_string(people) : "";
	std::string healthText = health < 10 ? std::to_string(health) : "";
	peopleTextPtr->setText(peopleText);
	healthTextPtr->setText(healthText);
}