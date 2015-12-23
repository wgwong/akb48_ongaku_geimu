#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <cmath> // math functions such as abs, min, max
#include <stdlib.h> //for system call to clear screen, rand
#include <deque>
#include <unordered_map>
#include <algorithm> //for std::remove_if
#include <exception>


#include "Stage.h"
#include "Profile.h"
#include "Team.h"
#include "Character.h"

#include "Effect.h"
#include "Beat.h"

#include "MusicStream.h"

#include "song_selection.h"
#include "song_battle.h"
#include "song_maker.h"

int loading_screen(std::string name);

sf::RenderWindow window;

int main() {
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8; //smoother shapes
	window.create(sf::VideoMode(800, 600), "AKB48 Ongaku Geimu (alpha) v0.4", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	/*
	window.setMouseCursorVisible(false); // Hide cursor
	sf::View fixed = window.getView(); // Create a fixed view

	// Load image and create sprite
	sf::Texture mouseTexture;
	mouseTexture.loadFromFile("cursors/mayuyu.png");
	sf::Sprite mouseSprite(mouseTexture);
	mouseSprite.rotate(-45);
	*/

	srand(time(NULL)); //seed random based on time

	//load fonts
	sf::Font gameFont;
	if (!gameFont.loadFromFile("gothic.ttf")) {
		//error
		std::cout << "Cannot load font\n";
	}
	sf::Font japaneseFont;
	if (!japaneseFont.loadFromFile("msmincho.ttc")) {
		//error
		std::cout << "Cannot load font\n";
	}

	//load background
	sf::Texture backTexture;
	if (!backTexture.loadFromFile("title_screen.png")) {
		//error
	}
	backTexture.setSmooth(true);
	backTexture.setRepeated(false);
	sf::Sprite backSprite;
	backSprite.setTexture(backTexture);
	backSprite.setColor(sf::Color(255, 255, 255, 224)); //semi-transparent

	//Load title text
	sf::Text titleText;
	titleText.setCharacterSize(50);

	//load buttons
	sf::RectangleShape startButton(sf::Vector2f(300,100));
	startButton.setFillColor(sf::Color(200, 50, 5, 128));
	startButton.setOutlineThickness(5);
	startButton.setOutlineColor(sf::Color(200, 50, 5, 128));
	startButton.setPosition(250, 250);

	sf::RectangleShape createButton(sf::Vector2f(300, 100));
	createButton.setFillColor(sf::Color(10, 150, 55, 192));
	createButton.setOutlineThickness(5);
	createButton.setOutlineColor(sf::Color(10, 150, 55, 192));
	createButton.setPosition(250, 400);

	//load button text
	sf::Text startText;
	startText.setCharacterSize(50);

	sf::Text createText;
	createText.setCharacterSize(40);

	//load music player graphics
	//music player black container
	sf::RectangleShape musicBox(sf::Vector2f(311, 30));
	musicBox.setFillColor(sf::Color(32, 32, 32, 224));
	musicBox.setPosition(sf::Vector2f(484, 565));
	//play button
	sf::CircleShape playButton;
	playButton.setPointCount(3);
	playButton.setRotation(210);
	playButton.setRadius(10);
	playButton.setFillColor(sf::Color(224, 224, 224, 224));
	playButton.setOutlineColor(sf::Color(128, 128, 128, 224));
	playButton.setPosition(sf::Vector2f(498, 593));
	//stop button
	sf::CircleShape stopButton;
	stopButton.setPointCount(4);
	stopButton.setRadius(11);
	stopButton.setRotation(45);
	stopButton.setFillColor(sf::Color(224, 224, 224, 224));
	stopButton.setOutlineColor(sf::Color(128, 128, 128, 224));
	stopButton.setPosition(sf::Vector2f(523, 565));
	//skip left button
	sf::CircleShape skipLeftButton;
	skipLeftButton.setPointCount(3);
	skipLeftButton.setRotation(30);
	skipLeftButton.setRadius(5);
	skipLeftButton.setFillColor(sf::Color(224, 224, 224, 224));
	skipLeftButton.setOutlineColor(sf::Color(128, 128, 128, 224));
	//skip right button
	sf::CircleShape skipRightButton;
	skipRightButton.setPointCount(3);
	skipRightButton.setRotation(210);
	skipRightButton.setRadius(5);
	skipRightButton.setFillColor(sf::Color(224, 224, 224, 224));
	skipRightButton.setOutlineColor(sf::Color(128, 128, 128, 224));
	//music text used for displaying time and song title
	sf::Text musicText;
	musicText.setFont(gameFont);
	musicText.setCharacterSize(12);
	//duration bar
	sf::RectangleShape timeDurBar(sf::Vector2f(100, 10));
	timeDurBar.setFillColor(sf::Color(160, 160, 160, 224));
	timeDurBar.setPosition(sf::Vector2f(632, 582));
	//current time bar
	sf::RectangleShape curTimeBar(sf::Vector2f(5, 12));
	curTimeBar.setFillColor(sf::Color(224, 224, 224, 224));
	//song name bar container
	sf::RectangleShape songNameBar(sf::Vector2f(170,10));
	songNameBar.setFillColor(sf::Color(16, 16, 16, 160));
	songNameBar.setPosition(sf::Vector2f(597, 568));
	//volume container
	sf::RectangleShape volBar(sf::Vector2f(20,24));
	volBar.setFillColor(sf::Color(16, 16, 16, 160));
	volBar.setPosition(sf::Vector2f(772, 568));
	//current volume
	sf::RectangleShape curVolBar;
	curVolBar.setFillColor(sf::Color(224, 224, 224, 224));
	//divider for buttons
	sf::RectangleShape buttonDivider(sf::Vector2f(2, 30));
	buttonDivider.setFillColor(sf::Color(160, 160, 160, 224));

	//randomly play music on title screen
	bool musicPermaStop = false; //permanently stops music (do not play next music) if stop button was pressed
	std::deque<std::string> previousPlayedSongs; //holds previous played songs (for skip left button)
	sf::Music music;
	std::string titleMusic[5] = { "Heavy Rotation", "Aitakatta", "Koi Suru Fortune Cookie", "Halloween Night", "Yankee Machine Gun" };
	std::string musicChoice = titleMusic[rand() % 5];
	if (!music.openFromFile(musicChoice + ".ogg")) {
		return -1; // error
	}
	music.setVolume(50);
	music.play();

	while (window.isOpen()) {
		//window.setMouseCursorVisible(false); // Hide cursor whenever we return to the title screen
		//if music ended, pick another song from our title screen jukebox (could be the same song again)
		if (music.getStatus() == 0 && !musicPermaStop) {
			previousPlayedSongs.push_back(musicChoice); //finished playing current song, push it to previous played song list
			musicChoice = titleMusic[rand() % 5];
			if (!music.openFromFile(musicChoice + ".ogg")) {
				return -1; // error
			}
			music.play();
		}

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::TextEntered) {
				std::cout << event.text.unicode << "\n"; //debug
			}
			if (event.type == sf::Event::MouseMoved) {
				sf::Vector2i localPos = sf::Mouse::getPosition(window);

				if (localPos.x >= 250 && localPos.x <= 550
					&& localPos.y >= 250 && localPos.y <= 350) {
					//start button
					startButton.setFillColor(sf::Color(200, 50, 5, 224));
					startButton.setOutlineColor(sf::Color(200, 50, 5, 224));
				}
				else if (localPos.x >= 250 && localPos.x <= 550
					&& localPos.y >= 400 && localPos.y <= 500) {
					//create button
					createButton.setFillColor(sf::Color(10, 150, 55, 224));
					createButton.setOutlineColor(sf::Color(10, 150, 55, 224));
				}
				else if (localPos.x >= 484 && localPos.x <= 509
					&& localPos.y >= 565 && localPos.y <= 595) {
					//play button
					playButton.setFillColor(sf::Color(35, 149, 207, 224));
					//reset colors of other buttons
					stopButton.setFillColor(sf::Color(224, 224, 224, 224));
					skipLeftButton.setFillColor(sf::Color(224, 224, 224, 224));
					skipRightButton.setFillColor(sf::Color(224, 224, 224, 224));
				}
				else if (localPos.x >= 511 && localPos.x <= 536
					&& localPos.y >= 565 && localPos.y <= 595) {
					//stop button
					stopButton.setFillColor(sf::Color(35, 149, 207, 224));
					//reset colors of other buttons
					playButton.setFillColor(sf::Color(224, 224, 224, 224));
					skipLeftButton.setFillColor(sf::Color(224, 224, 224, 224));
					skipRightButton.setFillColor(sf::Color(224, 224, 224, 224));
				}
				else if (localPos.x >= 538 && localPos.x <= 563
					&& localPos.y >= 565 && localPos.y <= 595) {
					//skip left button
					skipLeftButton.setFillColor(sf::Color(35, 149, 207, 224));
					//reset colors of other buttons
					playButton.setFillColor(sf::Color(224, 224, 224, 224));
					stopButton.setFillColor(sf::Color(224, 224, 224, 224));
					skipRightButton.setFillColor(sf::Color(224, 224, 224, 224));
				}
				else if (localPos.x >= 565 && localPos.x <= 590
					&& localPos.y >= 565 && localPos.y <= 595) {
					//skip right button
					skipRightButton.setFillColor(sf::Color(35, 149, 207, 224));
					//reset colors of other buttons
					playButton.setFillColor(sf::Color(224, 224, 224, 224));
					stopButton.setFillColor(sf::Color(224, 224, 224, 224));
					skipLeftButton.setFillColor(sf::Color(224, 224, 224, 224));
				}
				else {
					//reset colors
					startButton.setFillColor(sf::Color(200, 50, 5, 192));
					createButton.setFillColor(sf::Color(10, 150, 55, 192));
					startButton.setOutlineColor(sf::Color(200, 50, 5, 192));
					createButton.setOutlineColor(sf::Color(10, 150, 55, 192));
					playButton.setFillColor(sf::Color(224, 224, 224, 224));
					stopButton.setFillColor(sf::Color(224, 224, 224, 224));
					skipLeftButton.setFillColor(sf::Color(224, 224, 224, 224));
					skipRightButton.setFillColor(sf::Color(224, 224, 224, 224));
				}
			}
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i localPos = sf::Mouse::getPosition(window);

				if (localPos.x >= 250 && localPos.x <= 550
					&& localPos.y >= 250 && localPos.y <= 350) {
					//start button
					//std::cout << "Starting campaign\n"; //debug
					music.stop();
					//window.setMouseCursorVisible(true);
					song_selection(&window, "battle");
				}
				else if (localPos.x >= 250 && localPos.x <= 550
					&& localPos.y >= 400 && localPos.y <= 500) {
					//create button
					//std::cout << "Starting beat maker\n"; //debug
					music.stop();
					//window.setMouseCursorVisible(true);
					song_selection(&window, "make");
				}
				else if (localPos.x >= 484 && localPos.x <= 509
					&& localPos.y >= 565 && localPos.y <= 595) {
					//play button
					std::cout << "status code: " << music.getStatus() << "\n"; //debug
					if (music.getStatus() == 0 || music.getStatus() == 1) {
						std::cout << "\ttrying to play music\n"; //debug
						music.play();
						musicPermaStop = false;
					}
					else if (music.getStatus() == 2) {
						std::cout << "\ttrying to pause music\n"; //debug
						music.pause();
						musicPermaStop = false;
					}
					std::cout << "\tcode now: " << music.getStatus() << "\n"; //debug
				}
				else if (localPos.x >= 511 && localPos.x <= 536
					&& localPos.y >= 565 && localPos.y <= 595) {
					//stop button
					std::cout << "stopping music\n"; //debug
					music.stop();
					musicPermaStop = true;
					std::cout << "\tcode now: " << music.getStatus() << "\n"; //debug
				}
				else if (localPos.x >= 538 && localPos.x <= 563
					&& localPos.y >= 565 && localPos.y <= 595) {
					//skip left button
					if (previousPlayedSongs.size() > 0) {
						//play the most recent previously played song
						music.stop();
						musicChoice = previousPlayedSongs.back();
						previousPlayedSongs.pop_back();
						music.openFromFile(musicChoice + ".ogg");
						music.play();
					}
					else {
						music.setPlayingOffset(sf::milliseconds(0)); //just reset current song if we don't have any previous played songs
					}
				}
				else if (localPos.x >= 565 && localPos.x <= 590
					&& localPos.y >= 565 && localPos.y <= 595) {
					//skip right button
					music.stop();
				}
				else if (localPos.x >= 632 && localPos.x <= 732
					&& localPos.y >= 582 && localPos.y <= 592) {
					//set playing offset for music
					float xDelta = localPos.x - timeDurBar.getPosition().x;
					float timeMultiplier = xDelta / timeDurBar.getSize().x;
					music.setPlayingOffset(sf::milliseconds( int(music.getDuration().asMilliseconds() * timeMultiplier) ));
				}
				else if (localPos.x >= volBar.getPosition().x && localPos.x <= volBar.getPosition().x + volBar.getSize().x
					&& localPos.y >= volBar.getPosition().y && localPos.y <= volBar.getPosition().y + volBar.getSize().y) {
					//set volume
					float yDelta = localPos.y - volBar.getPosition().y;
					float volMultiplier = (volBar.getSize().y - yDelta) / volBar.getSize().y;
					const int maxVolume = 100.0;
					music.setVolume(maxVolume * volMultiplier);
				}
			}
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		//drawing stage
		window.clear(sf::Color::Black);

		window.draw(backSprite);

		titleText.setFont(gameFont);
		titleText.setString("AKB48");
		titleText.setPosition(200, 20);
		window.draw(titleText);
		
		//hacky-ish way of displaying unicode non-English characters on the title screen
		//this is hardcoded, so can't do this for long japanese inputs
		sf::Uint32 titleUniCharList[5] = { sf::Uint32(38899), sf::Uint32(27005), sf::Uint32(12466), sf::Uint32(12452), sf::Uint32(12512) };
		titleText.setFont(japaneseFont);
		for (int i = 0; i < 5; i++) {
			titleText.setString(titleUniCharList[i]);
			titleText.setPosition(300 + 50 * (i+1), 20);
			window.draw(titleText);
		}
		
		window.draw(startButton);
		window.draw(createButton);

		//hacky-ish way of displaying unicode non-English characters on the title screen
		//this is hardcoded, so can't do this for long japanese inputs
		sf::Uint32 startUniCharList[6] = { sf::Uint32(12461), sf::Uint32(12515), sf::Uint32(12531), sf::Uint32(12506), sf::Uint32(12540), sf::Uint32(12531) };
		startText.setFont(japaneseFont);
		for (int i = 0; i < 6; i++) {
			startText.setString(startUniCharList[i]);
			startText.setPosition(250 + 50 * i, 265);
			window.draw(startText);
		}
		//hacky-ish way of displaying unicode non-English characters on the title screen
		//this is hardcoded, so can't do this for long japanese inputs
		sf::Uint32 createUniCharList[7] = { sf::Uint32(12499), sf::Uint32(12540), sf::Uint32(12488), sf::Uint32(12513), sf::Uint32(12540), sf::Uint32(12459), sf::Uint32(12540) };
		createText.setFont(japaneseFont);
		for (int i = 0; i < 7; i++) {
			createText.setString(createUniCharList[i]);
			createText.setPosition(260 + 40 * i, 420);
			window.draw(createText);
		}

		//draw music player
		window.draw(musicBox);
		window.draw(playButton);
		buttonDivider.setPosition(sf::Vector2f(509, 565));
		window.draw(buttonDivider);
		window.draw(stopButton);
		buttonDivider.setPosition(sf::Vector2f(536, 565));
		window.draw(buttonDivider);
		skipLeftButton.setPosition(sf::Vector2f(544, 572));
		window.draw(skipLeftButton);
		skipLeftButton.setPosition(sf::Vector2f(554, 572));
		window.draw(skipLeftButton);
		buttonDivider.setPosition(sf::Vector2f(563, 565));
		window.draw(buttonDivider);
		skipRightButton.setPosition(sf::Vector2f(574, 585));
		window.draw(skipRightButton);
		skipRightButton.setPosition(sf::Vector2f(584, 585));
		window.draw(skipRightButton);
		buttonDivider.setPosition(sf::Vector2f(590, 565));
		window.draw(buttonDivider);
		window.draw(timeDurBar);
		//calculate position of curTimeBar based on current position in the song
		sf::Time musicCurTime = music.getPlayingOffset();
		sf::Time musicTotalDur = music.getDuration();
		curTimeBar.setPosition(sf::Vector2f(632 +
			(float(musicCurTime.asMilliseconds())/musicTotalDur.asMilliseconds()) *
			(timeDurBar.getSize().x-curTimeBar.getSize().x), 581));
		window.draw(curTimeBar);
		window.draw(songNameBar);
		//display song title
		musicText.setPosition(sf::Vector2f(597, 565));
		musicText.setString(musicChoice);
		window.draw(musicText);
		//display current time in song
		musicText.setPosition(sf::Vector2f(597, 580));
		std::string leftSideCurTime = std::to_string(int(musicCurTime.asSeconds()) / 60);
		if (leftSideCurTime.length() == 1) {
			leftSideCurTime = "0" + leftSideCurTime;
		}
		std::string rightSideCurTime = std::to_string(int(musicCurTime.asSeconds()) % 60);
		if (rightSideCurTime.length() == 1) {
			rightSideCurTime = "0" + rightSideCurTime;
		}
		musicText.setString(leftSideCurTime + ":" + rightSideCurTime);
		window.draw(musicText);
		//display total song duration
		musicText.setPosition(sf::Vector2f(737, 580));
		std::string leftSideTotalDur = std::to_string(int(musicTotalDur.asSeconds()) / 60);
		if (leftSideTotalDur.length() == 1) {
			leftSideTotalDur = "0" + leftSideTotalDur;
		}
		std::string rightSideTotalDur = std::to_string(int(musicTotalDur.asSeconds()) % 60);
		if (rightSideTotalDur.length() == 1) {
			rightSideTotalDur = "0" + rightSideTotalDur;
		}
		musicText.setString(leftSideTotalDur + ":" + rightSideTotalDur);
		window.draw(musicText);
		window.draw(volBar);
		const float maxVolume = 100.0;
		float curVolume = music.getVolume();
		curVolBar.setSize(sf::Vector2f(volBar.getSize().x, volBar.getSize().y * curVolume/maxVolume));
		curVolBar.setPosition(sf::Vector2f(volBar.getPosition().x, volBar.getPosition().y + volBar.getSize().y - curVolBar.getSize().y));
		window.draw(curVolBar);

		/*
		// Set mouse sprite position        
		mouseSprite.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
		window.draw(mouseSprite);
		*/

		window.display();
	}
	return 0;
}

//fake loading screen for smooth transition purposes
int loading_screen(std::string filePath) {
	//std::cout << "load\n"; //debug

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(filePath + ".ogg")) {
		std::cout << "Cannot load " << filePath << ".ogg"; //debug
	}
	sf::Sound sound;
	sound.setBuffer(buffer);
	sound.play();

	sf::Clock clock;

	/*
	while (clock.getElapsedTime().asMilliseconds() < 1000) {
		//window.clear(sf::Color(0, 0, 0));
		window.draw()
		window.display();
	}*/

	while (sound.getStatus() != 0) {
		window.clear(sf::Color(0, 0, 0));
		window.display();
	}

	return 0;
}