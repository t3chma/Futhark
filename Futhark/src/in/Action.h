#pragma once
#include <functional>
namespace fk {


/* Represents a game action.
[t3chma] */
struct Action {
	Action() = default;
	// A pointer to the undo action if available.
	virtual void undo() {};
	// The action this class represents.
	virtual void execute() = 0;
};

}