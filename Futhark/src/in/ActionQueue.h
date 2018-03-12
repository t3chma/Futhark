#pragma once
#include <boost/circular_buffer.hpp>
#include <list>
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

/* Used to queue and execute actions;
[t3chma] */
class ActionQueue {
  public:
	/* Constructor.
	(history) How many frames of history to keep about action information.
	[t3chma] */
	ActionQueue(int history = 36000);
	// History of user QueuedActions.
	boost::circular_buffer<std::list<Action*>> userActionHistory;
	// History of AI QueuedActions.
	boost::circular_buffer<std::list<Action*>> aiActionHistory;
	/* Executed all the queued actions.
	[t3chma] */
	void dispatch();
};

}