#pragma once
#include "actors/Person.h"

struct Job {
	enum Type { TRAIN, BUILD, STATE, BUFF, RELOCATE } type;
	struct Train {
		enum Type { SCIENTIST, MECHANIC, DOCTOR, POLITICIAN, INFORMANT, OFFICIAL } type;
		Person* traineePtr{ nullptr };
	};
	struct Build {
		enum Type { SPARTAIN } type;
		Person* mechanicPtr{ nullptr };
	};
	struct State {
		enum Type { FEED, HOUSE, SHELTER, WORK, TERRITORY, COUNTER } type;
		int amount{ 10 };
	};
	struct Buff {
		enum Type { MOOD, ARMOR, COERCION, SUPPLIES} type;
		int effectRadius{ 0 };
	};
	struct Relocate {
		Building* hostPtr{ nullptr };
		Building* workPtr{ nullptr };
	};
	bool passive;
	union Specs {
		/*
		(jobType) type of job. TRAIN, PRODUCE, or BUFF Job::Type
		(subtype) subtype of job. Job::Type::Type
		(option) radius of buff, or amount of people affected.
		*/
		Specs(Type jobType, int subtype, int option) {
			switch (jobType) {
			  case TRAIN: train.type = (Train::Type)subtype; break;
			  case BUILD: build.type = (Build::Type)subtype; break;
			  case STATE: state.type = (State::Type)subtype; break; state.amount = option; ;
			  case BUFF: buff.type = (Buff::Type)subtype; buff.effectRadius = option; break;
			  default: break;
			}
		}
		Train train;
		Build build;
		State state;
		Buff buff;
		Relocate relocate;
	} specs;
	/*
	(passive) if this job is passive.
	(jobType) type of job. TRAIN, PRODUCE, or BUFF. Job::Type
	(subtype) subtype of job. Job::Type::Type
	(option) radius of buff, or amount of people affected.
	*/
	Job(bool passive, Type jobType, int subtype = 0, int option = 0) :
		passive(passive), type(jobType), specs(jobType, subtype, option) {};
	// How many people can operate
	int labor{ 0 };
	// How much money is needed to function;
	int cost{ 0 };
	// How much work needs to be done per operation;
	float work{ 1 };
};

class Operation {
public:
	Operation(fk::TextureCache& textures) : textures(textures) { };
	fk::TextureCache& textures;
	void update();
	void toggleMenu();
	enum class Skill { DOMESTIC, SCIENTIFIC, POLITICAL } skill{ Skill::DOMESTIC };
	int stations{ 0 };
	std::vector<Person> workers;
	Job* currentJobPtr;
	std::vector<Job> options;
	void queueOption(int place);
};