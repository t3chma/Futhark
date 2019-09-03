//#include "TreePlan.h"
//#include "../in/IOManager.h"
//#include "../out/Error.h"
//namespace fk {
//
//
//PAVTree::PAVTree(SpriteBatch& sb) : sb(sb) {
//
//}
//
//
//PAVTree::~PAVTree() {
//
//}
//
//void fk::PAVTree::loadFile(const std::string& FILE_NAME) {
//	IOManager ioManager;
//	std::string buffer;
//	ioManager.readTextFileToString("Plans/" + FILE_NAME, buffer);
//	std::string word{ "" };
//	std::string* pathDefPtr{ nullptr };
//	bool root{ false };
//	currentNodePtr = &happiness;
//	PathMode oldPathMode{ VECTOR };
//	PathMode pathMode{ VECTOR };
//	char mode{ ' ' };
//	unsigned int tabs{ 0 };
//	std::vector<int> localIndecies;
//	for (int i = 0; i < buffer.size(); ++i) {
//		// Select Mode
//		switch (buffer[i]) {
//		  case '(':
//			// "Path("
//			if (word.length()) {
//				localIndecies.clear();
//				// If "Root("
//				if (word == "Root") {
//					;
//				} else {
//					
//				}
//			}
//			// "( or (Path"
//			else {
//				
//			}
//		  case '[':
//		  case '{':
//			tabs = 0;
//			word = "";
//			mode = buffer[i];
//			break;
//		}
//		// Handle modes
//		switch (mode) {
//		  default:
//			break;
//		  case '(':
//			break;
//		  case '[':
//			break;
//		  case '{':
//			break;
//		}
//		// Select path mode at '('
//		if (word.back() == '(') {
//			
//		}
//		// Handle path mode
//		if (pathMode != VECTOR) {
//			switch (pathMode) {
//			  default:
//				break;
//			  // Run till '\n'
//			  case BLENDING:
//				blending(buffer, i, word, pathMode, oldPathMode);
//				break;
//			  // Inscribe
//			  case DEFINING:
//				*pathDefPtr += buffer[i];
//				break;
//			}
//		}
//		// Collect word and clear at any white space
//		switch (buffer[i]) {
//		  default: word += buffer[i];  break;
//		  case '\n':
//			  tabs = 0;
//			  word = "";
//			  break;
//		  case '\t': ++tabs;
//		  case ' ':
//			word = "";
//		  break;
//		}
//	}
//}
//
//void PAVTree::blending(
//	std::string& buffer, int& i, std::string& word,
//	PAVTree::PathMode& pathMode, PAVTree::PathMode& oldPathMode
//) {
//	if (buffer[i] == '\n') {
//		// If path specified
//		if (word.length()) {
//			i -= ('(' + word).length();
//			auto result = paths.find(word);
//			// If found paste path to be processed
//			if (result != paths.end()) {
//				buffer.replace(i, word.length(), paths[word]);
//				// If in root
//				if (oldPathMode == GROWING) {
//
//				}
//			}
//			// Otherwise make empty action to be processed
//			else { buffer.replace(--i, word.length() + 1, "{"); }
//			word = "";
//		}
//		// Else is blended sequence
//		else {
//			// If in root
//			if (oldPathMode == GROWING) {
//
//			}
//		}
//		pathMode = oldPathMode;
//	}
//}