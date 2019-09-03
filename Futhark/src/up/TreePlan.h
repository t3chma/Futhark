//#pragma once
//#include "../out/SpriteBatch.h"
//#include "../base/Utility.h"
//#include <map>
//#include <set>
//namespace fk {
//
//class PAVTree {
//  public:
//	class Attribute {
//	  public:
//		Attribute(PAVTree& tp, std::string& name, double value = 0)
//			: treePlan(tp), name(name), value(value)
//		{ treePlan.linkNoun(*this); }
//		Attribute(PAVTree& tp, double value)
//			: treePlan(tp), name(name), value(value)
//		{ treePlan.linkNoun(*this); }
//		~Attribute() {
//			treePlan.unlinkNoun(*this);
//		}
//		std::string name{ "" };
//		PAVTree& treePlan;
//		double value;
//		inline Attribute& operator = (const Attribute& rhs) {
//			treePlan.replan(*this);
//			value = rhs.value;
//			return *this;
//		}
//		inline Attribute& operator += (const Attribute& rhs) {
//			treePlan.replan(*this);
//			value += rhs.value;
//			return *this;
//		}
//		inline Attribute& operator -= (const Attribute& rhs) {
//			treePlan.replan(*this);
//			value -= rhs.value;
//			return *this;
//		}
//		inline Attribute& operator *= (const Attribute& rhs) {
//			treePlan.replan(*this);
//			value *= rhs.value;
//			return *this;
//		}
//		inline Attribute& operator /= (const Attribute& rhs) {
//			treePlan.replan(*this);
//			value /= rhs.value;
//			return *this;
//		}
//		inline Attribute& operator %= (const Attribute& rhs) {
//			treePlan.replan(*this);
//			value = (int)rhs.value % (int)rhs.value;
//			return *this;
//		}
//		inline bool operator && (const Attribute& rhs) const { return value && rhs.value; }
//		inline bool operator || (const Attribute& rhs) const { return value || rhs.value; }
//		inline bool operator == (const Attribute& rhs) const { return value == rhs.value; }
//		inline bool operator != (const Attribute& rhs) const { return value != rhs.value; }
//		inline bool operator >= (const Attribute& rhs) const { return value >= rhs.value; }
//		inline bool operator <= (const Attribute& rhs) const { return value <= rhs.value; }
//		inline bool operator > (const Attribute& rhs) const { return value > rhs.value; }
//		inline bool operator < (const Attribute& rhs) const { return value < rhs.value; }
//		inline bool operator ! () const { return !value; }
//		inline operator bool () const { return value; }
//		inline operator char() const { return value; }
//		inline operator int() const { return value; }
//		inline operator float() const { return value; }
//		bool unaryOperation(std::string operation) {
//			switch (operation.length()) {
//			  default:
//			  case 1:
//				switch (operation[0]) {
//				  case '!': return !value;
//				}
//			}
//		}
//		bool binaryOperation(std::string operation, Attribute& rhs) {
//			Random rangen;
//			switch(operation.length()) {
//			  default: return;
//			  case 1:
//				switch (operation[0]) {
//				  default: return;
//				  case '=': return value == rhs.value;
//				  case '>': return value > rhs.value;
//				  case '<': return value < rhs.value;
//				  case 'p': return ;
//				}
//			  case 2:
//				switch (operation[0]) {
//				  default: return;
//				  case '&': return value && rhs.value;
//				  case '|': return value || rhs.value;
//				  case '=': return value == rhs.value;
//				  case '!': return value != rhs.value;
//				  case '>': return value >= rhs.value;
//				  case '<': return value <= rhs.value;
//				  case 'p': return rangen.getFloat(0, 1) > rhs.value;
//				}
//			}
//		}
//		bool trinaryOperation(std::string operation, Attribute& target, Attribute& blur) {
//			Random rangen;
//			switch (operation.length()) {
//			  default: return;
//			  case 1:
//				switch (operation[0]) {
//				  default: return;
//				  case 'n':
//					double normal = rangen.getNormal(value);
//					return normal < target.value + blur.value || normal > target.value - blur.value;
//				  case 's': return rangen.getFloat(0, 1) > 1 / (1 + pow(fk::E, -target.value));
//				}
//			}
//		}
//	};
//	class Action {
//	  public:
//		Action(char category, std::string name, float pros, float cons)
//			: category(category), name(name), pros(pros), cons(cons) {}
//		char category;
//		std::string name;
//		float cons{ 5 };
//		float pros{ 2 };
//	};
//	struct Node {
//		// Other<--This
//		Node* effect{ nullptr };
//		// This<--Other
//		std::vector<Node> causes;
//		// Describes a condition check
//		struct {
//			std::string op{ "" };
//			std::vector<Attribute*> paramPtrs;
//			int check() {
//				switch (paramPtrs.size()) {
//				  default: return; // TODO: plan for cost
//				  case 1: return paramPtrs[0]->unaryOperation(op);
//				  case 2: return paramPtrs[0]->binaryOperation(op, *paramPtrs[1]);
//				  case 3: return paramPtrs[0]->trinaryOperation(op, *paramPtrs[1], *paramPtrs[2]);
//				}
//			}
//		} condition;
//		// Action sequence associated with this.
//		std::vector<std::string> actions;
//		Node* addCause(int level, int oldLevel) {
//			// TODO: depth first creation, Use level dif to traverse, Add parent link
//			if (level > oldLevel) {
//				causes.emplace_back();
//				causes.back().effect;
//			} else if (level == oldLevel) {
//
//			} else {
//
//			}
//		};
//		std::vector<Node*> getCandidates() {
//
//		};
//	};
//	enum PathMode { VECTOR, BLENDING, DEFINING, GROWING };
//	PAVTree(SpriteBatch& sb);
//	~PAVTree();
//	void blending(std::string& buffer, int& i, std::string& word, PathMode& pathMode, PathMode& oldPathMode);
//	template <typename T>
//	struct Elements {
//		std::map<std::string, T*> map;
//		std::vector<T*> plan;
//	};
//	Elements<Attribute> attributes;
//	Elements<Action> actions;
//	std::map<std::string, std::string> paths;
//	Node happiness;
//	Node* currentNodePtr{ nullptr };
//	SpriteBatch& sb;
//	void loadFile(const std::string& fileName);
//	void makePlan();
//	inline void linkVerb(Action& action) { actions.map[action.name] = &action; };
//	inline void unlinkVerb(Action& action) { actions.map.erase(action.name); }
//	inline void linkNoun(Attribute& attribute) { attributes.map[attribute.name] = &attribute; };
//	inline void unlinkNoun(Attribute& attribute) { attributes.map.erase(attribute.name); }
//	void replan(Attribute& noun) {
//		if (std::find(attributes.plan.begin(), attributes.plan.end(), &noun) != attributes.plan.end()) { makePlan(); };
//	};
//};
//
//}