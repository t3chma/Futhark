#include "ActionQueue.h"
namespace fk {


ActionQueue::ActionQueue(int history) {
	if (history < 2) { history = 2; }
	userActionHistory.resize(history);
	aiActionHistory.resize(history);
}
void ActionQueue::dispatch() {
	for (auto&& queuedAction : aiActionHistory.front()) {
		queuedAction->execute();
	}
	for (auto&& queuedAction : userActionHistory.front()) {
		queuedAction->execute();
	}
}

}