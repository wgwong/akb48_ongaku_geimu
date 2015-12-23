#include "song_maker.h"

int song_maker(sf::RenderWindow *w, std::string filePath) {
	std::string fileExtension = ".ogg";

	//load background
	sf::Texture backTexture;
	if (!backTexture.loadFromFile(filePath + ".png")) {
		//error
	}
	backTexture.setSmooth(true);
	backTexture.setRepeated(false);
	sf::Sprite backSprite;
	backSprite.setTexture(backTexture);
	backSprite.setColor(sf::Color(255, 255, 255, 96)); // 1/2 transparent

													   //three bottom buttons
	sf::CircleShape leftCircle(30);
	leftCircle.setFillColor(sf::Color(102, 1, 46, 128));
	leftCircle.setOutlineThickness(5);
	leftCircle.setOutlineColor(sf::Color(205, 54, 128));
	sf::CircleShape middleTriangle(30, 3);
	middleTriangle.setFillColor(sf::Color(25, 80, 70, 128));
	middleTriangle.setOutlineThickness(5);
	middleTriangle.setOutlineColor(sf::Color(5, 203, 156));
	sf::CircleShape rightSquare(30, 4);
	rightSquare.setFillColor(sf::Color(18, 40, 76, 128));
	rightSquare.setOutlineThickness(5);
	rightSquare.setOutlineColor(sf::Color(3, 181, 170));

	leftCircle.setPosition(20, 500);
	middleTriangle.setPosition(270, 500);
	rightSquare.setPosition(520, 500);

	sf::Music music;
	if (!music.openFromFile(filePath + fileExtension))
		return -1; // error
	music.play();

	sf::Clock clock;
	music.setPlayingOffset(sf::milliseconds(clock.restart().asMilliseconds())); //sync start time of music with clock

	std::vector<Beat> beatList;
	std::deque<Effect> effects;

	while ((*w).isOpen()) {
		sf::Event event;

		while ((*w).pollEvent(event)) {
			if (event.type == sf::Event::KeyPressed) {
				std::cout << event.key.code << "\n"; //debug
				if (event.key.code == 71) {
					beatList.push_back(Beat("Left", clock.getElapsedTime().asMilliseconds()));
					std::cout << beatList.back().toString();

					std::string baseValues = "";
					sf::Color oC = leftCircle.getOutlineColor();
					baseValues += std::to_string(oC.r);
					baseValues += ",";
					baseValues += std::to_string(oC.g);
					baseValues += ",";
					baseValues += std::to_string(oC.b);
					effects.push_back(Effect(clock.getElapsedTime().asMilliseconds(), 350.0, &leftCircle, "outline color", "255,76,165", baseValues));
				}
				else if (event.key.code == 72) {
					beatList.push_back(Beat("Right", clock.getElapsedTime().asMilliseconds()));
					std::cout << beatList.back().toString();

					std::string baseValues = "";
					sf::Color oC = rightSquare.getOutlineColor();
					baseValues += std::to_string(oC.r);
					baseValues += ",";
					baseValues += std::to_string(oC.g);
					baseValues += ",";
					baseValues += std::to_string(oC.b);
					effects.push_back(Effect(clock.getElapsedTime().asMilliseconds(), 350.0, &rightSquare, "outline color", "53,231,220", baseValues));

				}
				else if (event.key.code == 73) {
					beatList.push_back(Beat("Up", clock.getElapsedTime().asMilliseconds()));
					std::cout << beatList.back().toString();

					std::string baseValues = "";
					sf::Color oC = middleTriangle.getOutlineColor();
					baseValues += std::to_string(oC.r);
					baseValues += ",";
					baseValues += std::to_string(oC.g);
					baseValues += ",";
					baseValues += std::to_string(oC.b);
					effects.push_back(Effect(clock.getElapsedTime().asMilliseconds(), 350.0, &middleTriangle, "outline color", "55,253,206", baseValues));

				}
				else if (event.key.code == 11) { //L - List beats
					for (auto it : beatList) {
						std::cout << it.toString();
					}
				}
				else if (event.key.code == 18) { //S - Save beats

				}
				else if (event.key.code == 22) { //W - Write to file
					std::ofstream file;
					file.open(filePath + ".txt");
					for (auto it : beatList) {
						file << it.getKey() << "@" << it.getTime() << "\n";
					}
					file.close();
				}
				else if (event.key.code == 17) { //R - Reset beats
					beatList.clear();
				}
				else if (event.key.code == 2) { //C - Clear console
					system("cls");
				}
				else if (event.key.code == 36) {//Esc
					return 0;
				}
			}
			if (event.type == sf::Event::Closed) {
				(*w).close();
			}
		}
		//handle effects in queue
		float nowTime = clock.getElapsedTime().asMilliseconds();
		for (int i = 0; i < effects.size(); i++) {
			Effect * it = &effects.at(i);
			if ((*it).getTimeDelta(nowTime) <= (*it).getDuration()) {
				(*it).transition(nowTime);
			}
			else {
				(*it).expire(); //marks the effect as done so that removal can clean up
			}
		}

		//remove effects that expired
		effects.erase(std::remove_if(effects.begin(), effects.end(), [](Effect e) {
			return e.getExpired();
		}), effects.end());

		//drawing stage
		(*w).clear(sf::Color::Black);

		(*w).draw(backSprite);
		(*w).draw(leftCircle);
		(*w).draw(middleTriangle);
		(*w).draw(rightSquare);

		(*w).display();
	}

	return 0;
}