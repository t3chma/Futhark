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
		int sign = 1;
		std::string output = "";
		for (auto&& c : line) {
			if (output != "") { continue; }
			if (lineIndex < 4) {
				switch (line[0]) {
				case 'X':
					output = line.substr(2);
					output.pop_back();
					cam.x = boost::lexical_cast<float>(output) / 2.0;
					break;
				case 'Y':
					output = line.substr(2);
					output.pop_back();
					cam.y = -(boost::lexical_cast<float>(output) - 1) / 2.0;
					break;
				case 'W':
					switch (c) {
					default:
						mode = 'o';
						break;
					case 't':
						mode = c; nt = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
					case 'x':
						mode = c; nx = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
					case 'y':
						mode = c; ny = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
					case 'r':
						mode = c; nr = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
					case '-': sign = -sign; break;
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case '0': number += c; break; break;
					case ' ':
					case '\n':
						if (mode == 'r') {
							weather.push_back(Wind());
							weather.back().t = nt;
							weather.back().d = glm::vec2(ranGen.getFloat(-nr, nr), ranGen.getFloat(-nr, nr));
						}
						else {
							weather.push_back(Wind());
							weather.back().t = nt;
							weather.back().d = glm::vec2(nx, ny);
						}
						nt = 0; nx = 0; ny = 0; nr = 0; sign = 1;
						break;
					}
					break;
				case 'C':
					switch (c) {
					default: mode = 'o'; break;
					case 't':
						mode = c; nt = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
					case 'x':
						mode = c; nx = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
					case 'y':
						mode = c; ny = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
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
					case ' ':
					case '\n':
						choreography.push_back(Move());
						choreography.back().t = nt;
						choreography.back().step = glm::vec2(nx / nt / 2, -ny / nt / 2);
						break;
					}
					break;
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
						pd.bd.position.y = -(float)lineIndex / 2.0;
						pd.bd.position.x = (float)charIndex / 2.0;
						bots.emplace_back(sb, sb, world, pd);
						bots.back().ai = true;
						bots.back().setTeam(botTeamIndex++);
						newChar += c + 32;
						bots.back().gunPtr->setUpgrade(newChar[0]);
						bots.back().gunPtr->text.setText(newChar);
						bots.back().gunPtr->text[0].setColor(0, 255, 0, 255);
						bots.back().gunPtr->text[0].setDimensions(0.3, 0.3);
						bots.back().addCircleLimb(5);
						bots.back().limbs.back().b2Ptr->SetSensor(true);
						bots.back().limbs.back().category = "s";
						break;
					default:
						Body::Def bd;
						bd.position.y = -(float)lineIndex / 2.0;
						bd.position.x = (float)charIndex / 2.0;
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
	ctime = choreography[c].t;
	wtime = weather[w].t;
	while (buffer.size() < maxSize) { buffer.emplace_back("\n"); }
	i = 0;
	for (auto&& line : buffer) {
		if (++i > 4) {
			line.pop_back();
			while (line.size() < maxSize) { line += " "; }
			line += "\n";
		}
	}
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
}

void Arena::update(fk::UserInput& ui) {
	int pops = 0;
	for (auto&& line : map) {
		for (auto&& block : line) {
			block.update(ui);
		}
		line.remove_if([](TextBlock& tb) { return tb.health < 1; });
	}
	if (choreography.size() && !freezeCam) {
		ctime -= 1;
		cam += choreography[c].step;
		if (ctime < 0) {
			++c;
			if (c >= choreography.size()) {
				c = 0; }
			ctime = choreography[c].t;
		};
	}
	if (weather.size()) {
		wtime -= 1;
		gravity = weather[w].d;
		if (wtime < 0) {
			++w;
			if (w >= weather.size()) {
				w = 0; }
			wtime = weather[w].t;
		};
	}
	for (auto&& bot : bots) { bot.update(ui); }
}

char* Arena::getCharAt(glm::ivec2 levelIndex) {
	if (
		levelIndex.x < 0 || levelIndex.y < 2
		|| levelIndex.x > maxSize || levelIndex.y > maxSize - 2
	) { return nullptr; }
	else { return &buffer[levelIndex.y][levelIndex.x]; }
}
