#pragma once
#include <string>
#include <map>
#include <vector>
#include <any>
namespace fk {

class DTree {
  public:
	struct Condition {};
	struct Node {
		unsigned int result{ 0 };
		char pass{ 0 };
		bool dead{ true };
	};
	class Value {
		union ValuePtr {
			int* i;
			float* f;
			char* c;
			std::string* s;
		} vp;
	  public:
		enum Type {
			INT,
			FLOAT,
			CHAR,
			STRING
		} t{ INT };
		Value inline operator = (int* const & rhs) { t = INT; vp.i = rhs; return *this; }
		Value inline operator = (float* const & rhs) { t = FLOAT; vp.f = rhs; return *this; }
		Value inline operator = (char* const & rhs) { t = CHAR; vp.c = rhs; return *this; }
		Value inline operator = (std::string* const & rhs) { t = STRING; vp.s = rhs; return *this; }
		std::any inline value() {
			switch (t) {
			  case INT: return std::make_any<int>(*vp.i);
			  case FLOAT: return std::make_any<float>(*vp.f);
			  case CHAR: return std::make_any<char>(*vp.c);
			  case STRING: return std::make_any<std::string>(*vp.s);
			}
		}
		Type inline type() { return t; }
		std::string inline typeString() {
			switch (t) {
			  case INT: return "int";
			  case FLOAT: return "float";
			  case CHAR: return "char";
			  case STRING: return "string";
			  default: return "?";
			};
		}

	};
	DTree(std::string& name);
	~DTree();
	void load(const std::string& file);
	void save();
	void registerVariables(std::vector<std::pair<std::string, Value>> variables);
	void registerChoices(std::vector<std::pair<std::string, int>> choices);
	int decide(int index);
	int traverse(int index, int direction);
	bool inline isWhitespace(char c) {
		switch (c) {
		  default:
			return false;
		  case ' ':
		  case '\t':
		  case '\n':
			return true;
		};
	}
  private:
	bool m_addNodeAt(unsigned int index, bool);
	void m_splitNode();
	int m_depth{ 0 };
	int m_elements{ 0 };
	std::vector<Node> m_nodes;
	std::string m_name;
	std::vector<std::string> m_dTreeData;
	std::map<std::string, Value> m_variables;
	std::map<std::string, int> m_choices;
};

}