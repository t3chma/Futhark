#include "Arena.h"
#include <string>
#include "in/IOManager.h"
#include <boost/lexical_cast.hpp>

Arena::Arena(std::string& levelPath, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& w, Player::Def& pd) {
	fk::IOManager iom;
	iom.readTextFileToStringVector(levelPath, buffer);
	init(f, sb, w, t, pd);
}

Arena::Arena(std::vector<std::string>& buffer, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& w, Player::Def& pd)
	: buffer(buffer) {
	init(f, sb, w, t, pd);
}

void Arena::init(fk::TTFont& f, fk::SpriteBatch& sb, fk::World& world, fk::Texture& t, Player::Def& pd) {
	int botTeamIndex = 3;
	int lineIndex = 0;
	char mode = 'o';
	float nt = 0;
	float nx = 0;
	float ny = 0;
	float nr = 0;
	for (auto&& line : buffer) {
		map.emplace_back();
		int charIndex = 0;
		std::string number = "";
		fk::Random ranGen;
		std::string output = "";
		for (auto&& c : line) {
			if (output != "") { continue; }
			if (lineIndex < metadata) {
				fk::Vec2 cm = fk::Vec2(cam);
				fk::Vec2 move;
				fk::Vec2 n;
				fk::Vec2 o;
				switch (line[0]) {
				case 'W':
					switch (c) {
					default:
						mode = 'o';
						break;
					case 't':
						mode = c; nt = boost::lexical_cast<float>(number); number = ""; break;
					case 'x':
						mode = c; nx = boost::lexical_cast<float>(number); number = ""; break;
					case 'y':
						mode = c; ny = boost::lexical_cast<float>(number); number = ""; break;
					case 'r':
						mode = c; nr = boost::lexical_cast<float>(number); number = ""; break;
					case '-':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case '0': number += c; break;
					case ' ':
					case '\n':
						if (mode == 'r') {
							weather.emplace_back(sb);
							weather.back().t = nt;
							weather.back().d = glm::vec2(ranGen.getFloat(-nr, nr), ranGen.getFloat(-nr, nr));
						} else {
							weather.emplace_back(sb);
							weather.back().t = nt;
							weather.back().d = glm::vec2(nx, ny);
						}
						nt = 0; nx = 0; ny = 0; nr = 0;
						break;
					}
					break;
				case 'C':
					switch (c) {
					default: mode = 'o'; break;
					case '-':
					case '.':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case '0': number += c; break;
					case 't':
						mode = c; nt = boost::lexical_cast<float>(number); number = ""; break;
					case 'x':
						mode = c; nx = boost::lexical_cast<float>(number); number = ""; break;
					case 'y':
						mode = c; ny = boost::lexical_cast<float>(number); number = "";
						n = fileToMap(nx, ny);
						o = n;
						if (!startPos) { startPos = true; start = n; }
						else {
							if (choreography.size()) { o = choreography.back().position; }
							else { o = start; }
							choreography.emplace_back(sb);
							choreography.back().t = nt;
							choreography.back().position = n;
							choreography.back().sprites.emplace_back(sb, t);
							choreography.back().sprites.back().setColor(255, 255, 255, 32);
							choreography.back().sprites.back().getCanvasRef().position.z = 50;
							choreography.back().sprites.back().makeLine(o, n, 0.1);
						}
						break;
					case '<':
						mode = c; nt = boost::lexical_cast<float>(number); number = "";
						n = start;
						if (choreography.size()) { o = choreography.back().position; }
						else { o = start; }
						choreography.emplace_back(sb);
						choreography.back().t = nt;
						choreography.back().position = n;
						choreography.back().sprites.emplace_back(sb, t);
						choreography.back().sprites.back().setColor(255, 255, 255, 32);
						choreography.back().sprites.back().getCanvasRef().position.z = 50;
						choreography.back().sprites.back().makeLine(o, n, 0.1);
						break;
					}
				default: break;
				}
			} else {
				if (c > 32 && c < 127) {
					std::string newChar = "";
					switch (c) {
					case '@': c = 'I';
					case 'I': spawns.emplace_back((float)charIndex / 2.0, -(float)lineIndex / 2.0); break;
					case 'i': allSpawns.emplace_back((float)charIndex / 2.0, -(float)lineIndex / 2.0); break;
					case 'A': c = '`' - 32;
					case 'M':
					case 'L':
					case 'R':
					case 'T':
					case 'E':
					case 'H':
						pd.bd.position = fileToMap(charIndex, lineIndex);
						bots.emplace_back(sb, sb, world, pd);
						bots.back().ai = true;
						bots.back().setTeam(botTeamIndex++);
						newChar += c + 32;
						bots.back().gunPtr->setUpgrade(newChar[0]);
						bots.back().gunPtr->text.setText(newChar);
						bots.back().gunPtr->text[0].setColor(0, 255, 0, 255);
						bots.back().gunPtr->text[0].setDimensions(0.3, 0.3);
						bots.back().addCircleLimb(8);
						bots.back().limbs.back().b2Ptr->SetSensor(true);
						bots.back().limbs.back().category = "s";
						break;
					default:
						Body::Def bd;
						bd.position = fileToMap(charIndex, lineIndex);
						map.back().emplace_back(c, f, sb, world, bd, t);
						break;
					}
				}
				width = (++charIndex > width) ? charIndex : width;
			}
		}
		++lineIndex;
	}
	int i = 0;
	while (spawns.size() < 2) { spawns.emplace_back(i++, 0); }
	i = 0;
	for (auto&& forcast : weather) {
		if (i++ == w) {
			wTime = forcast.t;
		}
	}
	i = 0;
	for (auto&& sceen : choreography) {
		if (i++ == c) {
			cTime = sceen.t;
			step = (sceen.position - start) / sceen.t;
			old = sceen.position;
		}
	}
	while (buffer.size() < maxSize) { buffer.emplace_back("\n"); }
	i = 0;
	for (auto&& line : buffer) {
		if (++i > metadata) {
			line.pop_back();
			while (line.size() < maxSize) { line += " "; }
			line += "\n";
		}
	}
	cam = start;
}

fk::Vec2 Arena::fileToMap(float charIndex, float lineIndex) {
	b2Vec2 m;
	m.x = charIndex / 2.0;
	m.y = -lineIndex / 2.0;
	return m;
}

fk::Vec2 Arena::mapToFile(fk::Vec2 gridPos) {
	b2Vec2 m;
	m.x = gridPos.x * 2.0;
	m.y = -gridPos.y * 2.0;
	return m;
}

Arena::~Arena() {
}

void Arena::draw() {
	for (auto&& line : map) {
		for (auto&& block : line) {
			block.draw();
		}
	}
	for (auto&& bot : bots) { bot.draw(); }
	for (auto&& frame : choreography) { frame.draw(); }
	for (auto&& wind : weather) { wind.draw(); }
}

void Arena::update(fk::UserInput& ui) {
	int pops = 0;
	for (auto&& line : map) {
		for (auto&& block : line) {
			block.update(ui);
		}
		line.remove_if([](TextBlock& tb) { return tb.health < 1; });
	}
	int i = 0;
	if (!freezeCam) {
		for (auto&& sceen : choreography) {
			if (i++ == c) {
				if (cTime-- == 0) {
					++c;
					if (c >= choreography.size()) { c = 0; }
				} else if (cTime < 0) {
					cTime = sceen.t;
					step = (sceen.position - old) / sceen.t;
					old = sceen.position;
				}
				cam += step;
				break;
			}
		}
	}
	i = 0;
	for (auto&& forcast : weather) {
		if (i++ == w) {
			if (wTime-- == 0) {
				++w;
				if (w >= weather.size()) { w = 0; }
			} else if (wTime < 0) {
				wTime = forcast.t;
			}
			gravity = forcast.d;
			break;
		} 
	}
	for (auto&& bot : bots) { bot.update(ui); }
}

char* Arena::getCharAt(glm::ivec2 levelIndex) {
	if (
		levelIndex.x < 0 || levelIndex.y < metadata
		|| levelIndex.x > maxSize || levelIndex.y > maxSize - 2
	) { return nullptr; }
	else { return &buffer[levelIndex.y][levelIndex.x]; }
}
