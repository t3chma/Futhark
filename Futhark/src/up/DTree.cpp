#include "DTree.h"
#include <fstream>
#include <sstream>
namespace fk {

	DTree::DTree(std::string& name) : m_name(name) {
		m_nodes.resize(1);
		load("DecisionTrees/" + m_name + ".dtm");
	}

	DTree::~DTree() {}

	void DTree::load(const std::string& FILE_PATH) {
		std::ifstream file(FILE_PATH);
		if (!file.fail()) {
			std::string line;
			bool foundTree{ false };
			int depth{ 1 };
			while (std::getline(file, line)) {
				if (foundTree) {
					auto index = line.size();
					while (--index >= 0) {
						char c = line[index];
						if (!isWhitespace(c)) {
							switch (c) {
							  case '{':
								if (++depth > m_depth) {
									m_depth = depth;
									m_nodes.resize((1 << depth) - 1);
								}
								break;
							  case '}':
								-depth;
								break;
							}
						}
					}
				} else if (line.size() > 1 && line[0] == '#' && line[1] == 'T') {
					foundTree = true;
					m_nodes.resize(3);
					m_nodes[0].dead = false;
				}
			}
			file.close();
		}
	}

	void DTree::save() {
		std::ofstream outFile("DecisionTrees/" + m_name + ".dtm", ios_base::trunc);
		outFile << "#Variables\n";
		for (auto&& variable : m_variables) {
			outFile << variable.first << ": " << variable.second.typeString() << "\n";
		}
		outFile << "\n#Choices\n";
		for (auto&& choice : m_choices) {
			outFile << choice.first << "\n";
		}
		outFile << "\n#Tree\n";
		// TODO: write tree
	}

	void DTree::registerVariables(std::vector<std::pair<std::string, Value>> variables) {
		for (auto&& variable : variables) { m_variables[variable.first] = variable.second; }
	}

	void DTree::registerChoices(std::vector<std::pair<std::string, int>> choices) {
		for (auto&& choice : choices) { m_choices[choice.first] = choice.second; }
	}

	int DTree::decide(int index) {

	}

	int DTree::traverse(int index, int direction) {
		switch (direction) {
		  case 1: return 2 * index + 2;
		  case -1: return 2 * index + 1;
		  default: return (index - (index % 2 ? 1 : 2)) / 2;
		}
	}
	bool DTree::m_addNodeAt(unsigned int index) {
		if (index >= m_nodes.size()) { return false; }
		else if (!m_nodes[index].dead) { return false; }
		++m_elements;
		if (m_elements > m_nodes.size()) { m_nodes.resize((1 << ++m_depth) - 1); }
		m_nodes[index].dead = false;
		m_nodes[index].route.result = 0;
		return true;
	}
};