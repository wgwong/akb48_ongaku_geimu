#include "create_profile.h"

int create_profile(sf::RenderWindow * w) {

	//load background
	sf::Texture backTexture;
	if (!backTexture.loadFromFile("title_screen.png")) {
		//error
	}
	backTexture.setSmooth(true);
	backTexture.setRepeated(false);
	sf::Sprite backSprite;
	backSprite.setTexture(backTexture);
	backSprite.setColor(sf::Color(255, 255, 255, 192)); // 3/4th transparent

														//load font
	sf::Font font;
	font.loadFromFile("arial.ttf");

	//player name variables
	sf::RectangleShape nameBar;
	nameBar.setSize(sf::Vector2f(300, 50));
	nameBar.setFillColor(sf::Color(32, 32, 32, 192));
	nameBar.setPosition(((*w).getSize().x - nameBar.getSize().x) / 2, ((*w).getSize().y - nameBar.getSize().y) / 2);
	sf::RectangleShape nameCursor;
	nameCursor.setSize(sf::Vector2f(2, 40)); //the line cursor that shows up when typing
	nameCursor.setFillColor(sf::Color(224, 224, 224));
	bool currentlyTyping = false;
	std::string nameInputString = ""; //what the user types
	sf::Text nameText;
	nameText.setFont(font);
	nameText.setCharacterSize(30);
	nameText.setColor(sf::Color(224, 224, 224));
	nameText.setString("Enter your name");
	nameText.setPosition(((*w).getSize().x - nameBar.getLocalBounds().width) / 2, nameBar.getPosition().y + (nameBar.getSize().y - nameText.getLocalBounds().height) / 2);

	//team constants
	const int numTeams = 4;
	const std::string teamNames[4] = { "A", "K", "B", "4" };
	std::unordered_map<std::string, sf::Color> teamColors;
	teamColors["A"] = sf::Color(245, 156, 194, 192);
	teamColors["K"] = sf::Color(135, 187, 45, 192);
	teamColors["B"] = sf::Color(109, 182, 217, 192);
	teamColors["4"] = sf::Color(255, 211, 0, 192);

	//team variables
	sf::RectangleShape teamRect;
	teamRect.setSize(sf::Vector2f(100, 100));
	sf::Text text;
	text.setFont(font);
	std::unordered_map<std::string, bool> teamHighlights; //tells us whether a team rectangle is highlighted or not, prevents default coloring before drawing
	teamHighlights["A"] = false;
	teamHighlights["K"] = false;
	teamHighlights["B"] = false;
	teamHighlights["4"] = false;
	float teamRectSpacing = ((*w).getSize().x - numTeams * teamRect.getSize().x) / (numTeams + 1); //spacing between each team rectangle

																									 //bool
	bool nameEntered = false; //need to enter name first

	sf::Clock clock; //used for the blinking rate of the name cursor

	while ((*w).isOpen()) {
		sf::Event event;

		while ((*w).pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				(*w).close();
			}
			if (event.type == sf::Event::MouseMoved) {
				sf::Vector2i localPos = sf::Mouse::getPosition((*w));

				if (nameEntered) {
					//check if mouse is hovering over any of the team rectangles
					for (int i = 0; i < numTeams; i++) {
						float boxXPos = teamRectSpacing + (i * (teamRect.getSize().x + teamRectSpacing));
						float boxYPos = ((*w).getSize().y - teamRect.getSize().y) / 2;

						if (localPos.x >= boxXPos && localPos.x <= boxXPos + teamRect.getSize().x
							&& localPos.y >= boxYPos && localPos.y <= boxYPos + teamRect.getSize().y) {
							teamHighlights[teamNames[i]] = true;
						}
						else {
							teamHighlights[teamNames[i]] = false;
						}
					}
				}
				else {

				}
			}
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i localPos = sf::Mouse::getPosition((*w));

				if (nameEntered) {
					//check if any of the team rectangles have been clicked
					for (int i = 0; i < numTeams; i++) {
						float boxXPos = teamRectSpacing + (i * (teamRect.getSize().x + teamRectSpacing));
						float boxYPos = ((*w).getSize().y - teamRect.getSize().y) / 2;

						if (localPos.x >= boxXPos && localPos.x <= boxXPos + teamRect.getSize().x
							&& localPos.y >= boxYPos && localPos.y <= boxYPos + teamRect.getSize().y) {

							//register username and team, save profile
							std::ofstream file;
							file.open("profile.dat");
							file << "playerName:" << nameInputString << "\n";
							file << "money:0\n"; //debug
							file << "teamName:" << teamNames[i] << "\n";
							file << "active:\n{\n";
							std::deque<std::string> charList = getInitialTeam(teamNames[i]); //get the initial characters in the team
							for (int i = 0; i < charList.size(); i++) {
								file << "\t" << charList[i] << "\n";
							}
							file << "}\n";
							file << "inactive:\n{\n}\n";
							file << "captain:" << charList[0];
							file.close();

							return 0; //return to song selection
						}
					}
				}
				else {
					//if the nameBar was clicked, go into typing mode
					if (localPos.x >= nameBar.getPosition().x && localPos.x <= nameBar.getPosition().x + nameBar.getSize().x
						&& localPos.y >= nameBar.getPosition().y && localPos.y <= nameBar.getPosition().y + nameBar.getSize().y) {
						nameBar.setFillColor(sf::Color(32, 32, 32, 224));
						currentlyTyping = true;
					}
					else {
						nameBar.setFillColor(sf::Color(32, 32, 32, 192));
						currentlyTyping = false;
					}
				}
			}
			if (event.type == sf::Event::TextEntered) {
				if (!nameEntered && currentlyTyping) {
					int keyVal = event.text.unicode;
					if (keyVal < 128) {
						//only accept alphanumeric characters or spaces into the nameInputString
						if (keyVal == 32 || (keyVal >= 65 && keyVal <= 90)
							|| (keyVal >= 97 && keyVal <= 122)
							|| (keyVal >= 48 && keyVal <= 57)) {
							//don't append to input if it will make the text exceed the visual bounds of the name bar
							if (nameBar.getSize().x - nameText.getLocalBounds().width > 50) {
								nameInputString += static_cast<char>(event.text.unicode);
							}
						}
						else if (keyVal == 8) {
							//backspace, remove last character in nameInputString
							if (nameInputString.length() > 0) {
								nameInputString = nameInputString.substr(0, nameInputString.length() - 1);
							}
						}
						else if (keyVal == 13) {
							//carriage return/enter, if length > 0 and go to team selection mode
							if (nameInputString.length() > 0) {
								nameEntered = true;
							}
						}
					}
				}
			}
		}

		(*w).clear(sf::Color());

		(*w).draw(backSprite); //draw background

		if (nameEntered) {
			//draw title
			text.setCharacterSize(40);
			text.setString("Select your team");
			text.setPosition(sf::Vector2f(((*w).getSize().x - text.getLocalBounds().width) / 2,
				((*w).getSize().y - text.getLocalBounds().height) / 16));
			(*w).draw(text);

			//draw the team rectangles
			text.setCharacterSize(30);
			for (int i = 0; i < numTeams; i++) {

				teamRect.setPosition(teamRectSpacing + (i * (teamRect.getSize().x + teamRectSpacing)), ((*w).getSize().y - teamRect.getSize().y) / 2);
				if (teamHighlights[teamNames[i]]) {
					//if mouse is hovering over the box, make it more opaque
					sf::Color oriColor = teamColors[teamNames[i]];
					teamRect.setFillColor(sf::Color(oriColor.r, oriColor.g, oriColor.b, 224));
				}
				else {
					//otherwise, color as normal
					teamRect.setFillColor(teamColors[teamNames[i]]);
				}

				(*w).draw(teamRect);

				text.setString("Team " + teamNames[i]);
				text.setPosition(int(teamRect.getPosition().x + (teamRect.getSize().x - text.getLocalBounds().width) / 2), teamRect.getPosition().y + teamRect.getSize().y);
				(*w).draw(text);
			}
		}
		else {
			(*w).draw(nameBar);
			if (!currentlyTyping) {
				nameText.setString("Enter your name");
			}
			else {
				nameText.setString(nameInputString);
			}
			nameText.setPosition(((*w).getSize().x - nameText.getLocalBounds().width) / 2, nameBar.getPosition().y + ((nameBar.getSize().y - nameText.getLocalBounds().height) / 2));
			(*w).draw(nameText);
			if (currentlyTyping) {
				nameCursor.setPosition(nameText.getPosition().x + nameText.getLocalBounds().width + 2, nameBar.getPosition().y + ((nameBar.getSize().y - nameBar.getSize().y) / 2));
				float timeElapsed = clock.getElapsedTime().asMilliseconds() % 1000;
				//only show the cursor every halfsecond to achieve a blinking text cursor
				if (timeElapsed <= 500) {
					(*w).draw(nameCursor);
				}
			}
		}

		(*w).display();
	}

	return 0;
}