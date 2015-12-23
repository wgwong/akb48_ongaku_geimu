#include "song_selection.h"
#include "song_battle.h"
#include "song_maker.h"

int song_selection(sf::RenderWindow * w, std::string reason) {
	//character database
	std::unordered_map<std::string, Character> charMap;

	//load character database
	std::ifstream metaCharFile;
	metaCharFile.open("characters/character_database.dat");
	std::string metaCharLine;

	if (metaCharFile.is_open()) {
		const std::string delimiter = ":";

		std::string folderPath = "";
		std::string infoPath = "";
		std::string imagePath = "";
		std::string stageSongPath = "";

		while (std::getline(metaCharFile, metaCharLine)) {
			if (metaCharLine == "{") {

			}
			else if (metaCharLine == "}") {
				//finished loading meta character information, load character data from file
				//std::cout << "finished loading folder: " << folderPath << ", info: " << infoPath << ", img: " << imagePath << "\n"; //debug

				//character information
				std::string firstName = "";
				std::string lastName = "";
				std::string nickname = "";
				std::string team = "";

				//character profile
				std::string rank = "";
				std::string prefix = "";
				std::string suffix = "";
				int level = 0;
				int experience = 0;
				bool unlocked = false;

				std::ifstream charFile("characters/" + folderPath + infoPath);
				std::string charLine;

				if (charFile.is_open()) {
					while (std::getline(charFile, charLine)) {
						std::string charKey = charLine.substr(0, charLine.find(delimiter));
						std::string charValue = charLine.substr(charLine.find(delimiter) + 1, charLine.length());

						//std::cout << "key:" << charKey << ":" << "\nvalue:" << charValue << ":\n"; //debug
						if (charKey == "firstName") {
							firstName = charValue;
						}
						else if (charKey == "lastName") {
							lastName = charValue;
						}
						else if (charKey == "nickname") {
							nickname = charValue;
						}
						else if (charKey == "team") {
							team = charValue;
						}
						else if (charKey == "rank") {
							rank = charValue;
						}
						else if (charKey == "suffix") {
							suffix = charValue;
						}
						else if (charKey == "level") {
							level = stoi(charValue);
						}
						else if (charKey == "experience") {
							experience = stoi(charValue);
						}
						else if (charKey == "unlocked") {
							if (charValue == "yes" || charValue == "true") {
								unlocked = true;
							}
						}
					}
				}
				else {
					std::cout << "ERROR: Cannot load character info data for into database!\n\tExpected filepath: " << "characters/" + folderPath + infoPath << "\n";
				}
				//finished loading character profile, load character into character database
				Character c(firstName, lastName, nickname, team, rank, prefix, suffix, level, experience, unlocked);
				c.setInfoPath(infoPath);
				c.setImagePath(imagePath);
				c.setStageSongPath(stageSongPath);
				charMap[infoPath] = c;

				//reset variables
				firstName = "";
				lastName = "";
				nickname = "";
				team = "";
				rank = "";
				prefix = "";
				suffix = "";
				level = 0;
				experience = 0;
				unlocked = false;
			}
			else {
				int len = metaCharLine.rfind("\t") + 1;
				std::string key = metaCharLine.substr(len, metaCharLine.find(delimiter) - len);
				std::string value = metaCharLine.substr(metaCharLine.find(delimiter) + 1, metaCharLine.length());

				//std::cout << "key:" << key << ":" << "\nvalue:" << value << ":\n\n"; //debug
				if (key == "folderPath") {
					folderPath = value;
				}
				else if (key == "infoPath") {
					infoPath = value;
				}
				else if (key == "imagePath") {
					imagePath = value;
				}
				else if (key == "stageSongPath") {
					stageSongPath = value;
				}
			}
		}
	}
	else {
		std::cout << "FATAL ERROR: Cannot load character database!\n"; //debug
	}

	/*
	std::cout << "charMap contents" << "\n"; //debug
	std::unordered_map<std::string, Character>::iterator it;
	for (it = charMap.begin(); it != charMap.end(); it++) {
	std::cout << it->first << ":" << it->second.getLastName() << "\n";
	}*/

	//load user profile
	Profile userProfile;
	bool profileLoaded = false;
	bool newProfile = false; //whether the profile was created just now
	std::ifstream profileFile;

	while (!profileLoaded) {
		profileFile.open("profile.dat");
		if (profileFile.is_open()) {
			std::string line = "";
			const std::string delimiter = ":";
			std::string key = "";

			std::string pn = "";
			float m = 0.0;
			Team tm;

			while (std::getline(profileFile, line)) {
				key = line.substr(0, line.find(delimiter));
				if (key == "playerName") {
					pn = line.substr(line.find(delimiter) + 1, line.length());
				}
				if (key == "money") {
					m = stof(line.substr(line.find(delimiter) + 1, line.length()));
				}
				if (key == "teamName") {
					tm.setTeamName(line.substr(line.find(delimiter) + 1, line.length()));
				}
				if (key == "active") {
					while (std::getline(profileFile, line)) {
						if (line == "{") {
						}
						else if (line == "}") {
							break;
						}
						else {
							std::string charName = line.substr(line.rfind("\t") + 1, line.length());
							tm.addActiveChar(charMap[charName]);
						}
					}
				}
				if (key == "inactive") {
					while (std::getline(profileFile, line)) {
						if (line == "{") {
						}
						else if (line == "}") {
							break;
						}
						else {
							tm.addInactiveChar(charMap[line]);
						}
					}
				}
				if (key == "captain") {
					tm.setCaptain(charMap[line.substr(line.find(delimiter) + 1, line.length())]);
				}
			}
			userProfile.setPlayerName(pn);
			userProfile.setTeam(tm);
			userProfile.setMoney(m);
			profileLoaded = true;
		}
		else {
			//if user profile doesn't exist and we're in song selection for a stage battle, take player to profile creation screen
			if (reason == "battle") {
				create_profile(w);

				newProfile = true;
			}
		}
	}

	//after profile, play overture
	if (reason == "battle" && newProfile) {
		sf::SoundBuffer sf;
		sf.loadFromFile("sounds/battle.ogg");
		sf::Sound s;
		s.setBuffer(sf);
		s.play();
		while (s.getStatus() != 0) {
		}
		Stage stage("overture", 0, 0);
		stage.setInfo("overture", "AKB48", "Team A 1st Stage -PARTY ga Hajimaru yo-", 2005);
		Stage stage2("overture", 0, 0);
		stage2.setInfo("overture", "AKB48", "Team A 1st Stage -PARTY ga Hajimaru yo-", 2005);
		song_battle(w, &userProfile, &stage, &stage2);
	}

	//set mouse cursor to 0,0 so that we don't accidentally highlight a song right when we enter
	sf::Mouse::setPosition(sf::Vector2i(20, 20), *w);

	//loading stage
	//load fonts
	sf::Font gameFont;
	if (!gameFont.loadFromFile("arial.ttf")) {
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
	backSprite.setColor(sf::Color(255, 255, 255, 192)); // 3/4th transparent

														//Load title text
	sf::Text titleText;
	titleText.setFont(gameFont);
	titleText.setCharacterSize(50);
	titleText.setPosition(250, 70);
	titleText.setString("Select a stage");

	std::deque<Stage> stageList;
	std::unordered_map<std::string, Stage> stageMap; //used by stages to reference other stages for info in O(1) lookup

	std::ifstream file;
	file.open("song_database.dat");
	std::string line;

	if (file.is_open()) {
		const std::string delimiter = ":";

		std::string songPath = "";
		int level = 0;
		int order = 0;
		std::deque<std::string> requiredSongs;

		while (std::getline(file, line)) {
			if (line == "{") {

			}
			else if (line == "}") {
				//finished loading stage information, load song information and profile from data
				//import song information and song profile
				//song information
				std::string songTitle = "";
				std::string songArtist = "";
				std::string songAlbum = "";
				int songYear = 0;
				//song profile
				int highScore = 0;
				int timesPlayed = 0;
				bool unlocked = false;
				bool hidden = true;

				std::ifstream songFile(songPath + ".dat");
				std::string songLine;

				if (songFile.is_open()) {
					while (std::getline(songFile, songLine)) {
						std::string songKey = songLine.substr(0, songLine.find(delimiter));
						std::string songValue = songLine.substr(songLine.find(delimiter) + 1, songLine.length());

						//std::cout << "key:" << songKey << ":" << "\nvalue:" << songValue << ":\n"; //debug
						if (songKey == "title") {
							songTitle = songValue;
						}
						else if (songKey == "artist") {
							songArtist = songValue;
						}
						else if (songKey == "album") {
							songAlbum = songValue;
						}
						else if (songKey == "year") {
							songYear = stoi(songValue);
						}
						else if (songKey == "highest_score") {
							highScore = stoi(songValue);
						}
						else if (songKey == "times_played") {
							timesPlayed = stoi(songValue);
						} /*
						  else if (songKey == "unlocked") {
						  if (songValue == "yes" || songValue == "true") {
						  unlocked = true;
						  }
						  }*/
						else if (songKey == "hidden") {
							if (songValue == "no" || songValue == "false") {
								hidden = false;
							}
						}
					}
				}

				//finished loading song profile, create stage
				Stage stg(songPath, 125 + ((125 + 100) * order), 250 + (600 * level));
				stg.setInfo(songTitle, songArtist, songAlbum, songYear);

				if (requiredSongs.size() == 0) {
					//std::cout << "no requirements for " << songName << "\n"; //debug
					unlocked = true;
					stg.setProfile(highScore, timesPlayed, unlocked, hidden);
				}
				else {
					int unlockCount = 0;
					for (int i = 0; i < requiredSongs.size(); i++) {
						std::string sName = requiredSongs.at(i);
						//std::cout << "\trequired song: " << sName << "\n"; //debug
						if (stageMap[sName].getTimesPlayed() > 0) {
							unlockCount++;
						}
					}
					if (unlockCount == requiredSongs.size()) {
						unlocked = true;
						std::cout << songTitle << " was unlocked!\n"; //debug
					}
					stg.setProfile(highScore, timesPlayed, unlocked, hidden, requiredSongs);
				}

				//add stage to stage map and stage list
				stageMap[songPath] = stg;
				stageList.push_back(stg);

				//reset variables
				songPath = "";
				level = 1;
				order = 1;
				requiredSongs.clear();
			}
			else {
				int len = line.rfind("\t") + 1;
				std::string key = line.substr(len, line.find(delimiter) - len);
				std::string value = line.substr(line.find(delimiter) + 1, line.length());

				//std::cout << "key:" << key << ":" << "\nvalue:" << value << ":\n\n"; //debug
				if (key == "path") {
					songPath = value;
				}
				else if (key == "level") {
					level = stoi(value);
				}
				else if (key == "order") {
					order = stoi(value);
				}
				else if (key == "required_songs") {
					if (value.find(',') != -1) {
						std::vector<std::string> requiredSongV;
						split(value, ',', requiredSongV);
						//std::cout << "requiredSongV size: " << requiredSongV.size() << "\n"; //debug
						for (int i = 0; i < requiredSongV.size(); i++) {
							requiredSongs.push_back(requiredSongV.at(i));
						}
					}
					else {
						if (value != "none") {
							requiredSongs.push_back(value);
						}
					}
					//TODO FUTURE, requirements besides songs (i.e. badges)
				}
			}
		}
	}

	sf::Texture thumbTexture;
	sf::Sprite thumbSprite;
	sf::Text thumbText;

	//since song box border colors are randomized,
	//the makeshift solution of highlighting the border colors
	//involves using a single hover box that gets drawn over any song boxes that the mouse is hovering over
	bool enableHover = false;
	sf::RectangleShape hoverBox;
	hoverBox.setSize(sf::Vector2f(100, 100));
	hoverBox.setFillColor(sf::Color(128, 128, 128, 0)); //transparent for album art
	hoverBox.setOutlineThickness(5);
	hoverBox.setOutlineColor(sf::Color(255, 255, 255, 92));

	//initialize arrows
	const float screenHeight = 600;
	const float screenWidth = 800;
	const float height = 20;
	const float dip = 30;
	const float width = 40;
	float startX = screenWidth / 2 - width;
	//up arrow button
	float startYUp = height + dip + 20;
	sf::ConvexShape upArrowLeft, upArrowRight;
	upArrowLeft.setPointCount(4);
	upArrowLeft.setPoint(0, sf::Vector2f(startX, startYUp));
	upArrowLeft.setPoint(1, sf::Vector2f(startX + width, startYUp - dip));
	upArrowLeft.setPoint(2, sf::Vector2f(startX + width, startYUp - height - dip));
	upArrowLeft.setPoint(3, sf::Vector2f(startX, startYUp - height));
	upArrowLeft.setFillColor(sf::Color(128, 128, 128, 128));
	upArrowRight.setPointCount(4);
	upArrowRight.setPoint(0, sf::Vector2f(startX + width, startYUp - dip));
	upArrowRight.setPoint(1, sf::Vector2f(startX + width * 2, startYUp));
	upArrowRight.setPoint(2, sf::Vector2f(startX + width * 2, startYUp - height));
	upArrowRight.setPoint(3, sf::Vector2f(startX + width, startYUp - height - dip));
	upArrowRight.setFillColor(sf::Color(128, 128, 128, 160));
	//down arrow button
	float startYDown = screenHeight - height - dip - 20;
	sf::ConvexShape downArrowLeft, downArrowRight;
	downArrowLeft.setPointCount(4);
	downArrowLeft.setPoint(0, sf::Vector2f(startX, startYDown));
	downArrowLeft.setPoint(1, sf::Vector2f(startX + width, startYDown + dip));
	downArrowLeft.setPoint(2, sf::Vector2f(startX + width, startYDown + height + dip));
	downArrowLeft.setPoint(3, sf::Vector2f(startX, startYDown + height));
	downArrowLeft.setFillColor(sf::Color(128, 128, 128, 128));
	downArrowRight.setPointCount(4);
	downArrowRight.setPoint(0, sf::Vector2f(startX + width, startYDown + dip));
	downArrowRight.setPoint(1, sf::Vector2f(startX + width * 2, startYDown));
	downArrowRight.setPoint(2, sf::Vector2f(startX + width * 2, startYDown + height));
	downArrowRight.setPoint(3, sf::Vector2f(startX + width, startYDown + height + dip));
	downArrowRight.setFillColor(sf::Color(128, 128, 128, 160));

	//transparency threshhold constants
	const float posTolUp = 200;
	const float posTolDown = 500;

	sf::Music backgroundMusic;
	if (!backgroundMusic.openFromFile("song_selection.wav")) {
		std::cout << "ERROR: Can't open song_selection.wav\n"; //debug
	}
	backgroundMusic.setVolume(75);
	backgroundMusic.setLoop(true);
	backgroundMusic.play();

	sf::Music previewMusic;
	previewMusic.setVolume(0); //start preview music at 0 volume

	while ((*w).isOpen()) {
		sf::Event event;

		while ((*w).pollEvent(event)) {
			if (event.type == sf::Event::MouseMoved) {

				sf::Vector2i localPos = sf::Mouse::getPosition((*w));

				bool enablePreview = false;
				for (int i = 0; i < stageList.size(); i++) {
					Stage * stage = &stageList.at(i);

					//don't consider highlighting songbox if
					//1. it's locked
					//2. it's above or below a certain transparency threshhold
					if (!(*stage).getUnlocked() ||
						((*stage).getPos().y < posTolUp - 50 || (*stage).getPos().y + 100 > posTolDown + 50)
						) {
						//posTolUp - 100 & posTolDown + 100 means we still consider them until they've traveled half their height
						//after passing the transparency threshhold
						//meaning we will still highlight semi-transparent songboxes
						continue;
					}

					//if hovering over a songbox
					if (localPos.x >= (*stage).getPos().x && localPos.x <= (*stage).getPos().x + 100
						&& localPos.y >= (*stage).getPos().y && localPos.y <= (*stage).getPos().y + 100) {
						enablePreview = true;
						enableHover = true;
						hoverBox.setPosition((*stage).getPos());
						//status: 0 = stopped/not playing, 1 = paused?, 2 = playing
						if (previewMusic.getStatus() != 2) {
							previewMusic.openFromFile((*stage).getPath() + ".ogg");

							int playPoint = (previewMusic.getDuration().asMilliseconds()) / 4;

							backgroundMusic.pause();
							previewMusic.play();
							//play starting from the quarterpoint of the song
							previewMusic.setPlayingOffset(sf::milliseconds(playPoint));
						}
					}
				}
				//if hovering over the up arrow
				float slopeLeft = -1 * dip / width;
				float xDeltaLeft = localPos.x - startX;
				float yUpperLeft = slopeLeft * xDeltaLeft + startYUp - height;
				float yLowerLeft = yUpperLeft + height;

				float slopeRight = 1 * dip / width;
				float xDeltaRight = localPos.x - startX - width;
				float yUpperRight = slopeRight * xDeltaRight + startYUp - height - dip;
				float yLowerRight = yUpperRight + height;

				if ((localPos.x >= startX && localPos.x <= startX + width
					&& localPos.y >= yUpperLeft && localPos.y <= yLowerLeft) || //inside left side of the up arrow
					(localPos.x >= startX + width && localPos.x <= startX + 2 * width
						&& localPos.y >= yUpperRight && localPos.y <= yLowerRight)) {
					upArrowLeft.setFillColor(sf::Color(128, 128, 128, 224));
					upArrowRight.setFillColor(sf::Color(128, 128, 128, 224));
				}
				else {
					upArrowLeft.setFillColor(sf::Color(128, 128, 128, 160));
					upArrowRight.setFillColor(sf::Color(128, 128, 128, 160));
				}

				//if hovering over the down arrow
				slopeLeft = dip / width;
				xDeltaLeft = localPos.x - startX;
				yUpperLeft = slopeLeft * xDeltaLeft + startYDown;
				yLowerLeft = yUpperLeft + height;

				slopeRight = -1 * dip / width;
				xDeltaRight = localPos.x - startX - width;
				yUpperRight = slopeRight * xDeltaRight + startYDown + dip;
				yLowerRight = yUpperRight + height;

				/*
				std::cout << "slope: " << slopeRight << ", deltaX: " << xDeltaRight << "\n"; //debug
				std::cout << "mouse x: " << localPos.x << ", y: " << localPos.y << "\n"; //debug
				std::cout << "yUpperLeft: " << yUpperRight << ", Lower: " << yLowerRight << "\n\n"; //debug
				*/

				if ((localPos.x >= startX && localPos.x <= startX + width
					&& localPos.y >= yUpperLeft && localPos.y <= yLowerLeft) || //inside left side of the down arrow
					(localPos.x >= startX + width && localPos.x <= startX + 2 * width
						&& localPos.y >= yUpperRight && localPos.y <= yLowerRight)) {
					downArrowLeft.setFillColor(sf::Color(128, 128, 128, 224));
					downArrowRight.setFillColor(sf::Color(128, 128, 128, 224));
				}
				else {
					downArrowLeft.setFillColor(sf::Color(128, 128, 128, 160));
					downArrowRight.setFillColor(sf::Color(128, 128, 128, 160));
				}

				if (!enablePreview) {
					enableHover = false;
					previewMusic.setVolume(0);
					previewMusic.stop();
					if (backgroundMusic.getStatus() == 0 || backgroundMusic.getStatus() == 1) {
						backgroundMusic.play();
					}
				}
			}
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				bool moving = false; //whether the stage is moving because of arrow clicks

				sf::Vector2i localPos = sf::Mouse::getPosition((*w));

				//if the up arrow was clicked
				float slopeLeft = -1 * dip / width;
				float xDeltaLeft = localPos.x - startX;
				float yUpperLeft = slopeLeft * xDeltaLeft + startYUp - height;
				float yLowerLeft = yUpperLeft + height;

				float slopeRight = 1 * dip / width;
				float xDeltaRight = localPos.x - startX - width;
				float yUpperRight = slopeRight * xDeltaRight + startYUp - height - dip;
				float yLowerRight = yUpperRight + height;

				if ((localPos.x >= startX && localPos.x <= startX + width
					&& localPos.y >= yUpperLeft && localPos.y <= yLowerLeft) || //inside left side of the down arrow
					(localPos.x >= startX + width && localPos.x <= startX + 2 * width
						&& localPos.y >= yUpperRight && localPos.y <= yLowerRight)) {

					for (int i = 0; i < stageList.size(); i++) {
						Stage(*it) = &stageList.at(i);
						(*it).changePos(0, +200);
					}
					moving = true;
				}

				//if the down arrow was clicked
				slopeLeft = dip / width;
				xDeltaLeft = localPos.x - startX;
				yUpperLeft = slopeLeft * xDeltaLeft + startYDown;
				yLowerLeft = yUpperLeft + height;

				slopeRight = -1 * dip / width;
				xDeltaRight = localPos.x - startX - width;
				yUpperRight = slopeRight * xDeltaRight + startYDown + dip;
				yLowerRight = yUpperRight + height;

				if ((localPos.x >= startX && localPos.x <= startX + width
					&& localPos.y >= yUpperLeft && localPos.y <= yLowerLeft) || //inside left side of the down arrow
					(localPos.x >= startX + width && localPos.x <= startX + 2 * width
						&& localPos.y >= yUpperRight && localPos.y <= yLowerRight)) {

					for (int i = 0; i < stageList.size(); i++) {
						Stage(*it) = &stageList.at(i);
						(*it).changePos(0, -200);
					}
					moving = true;
				}

				//check if any song boxes were clicked
				if (!moving) {
					for (int i = 0; i < stageList.size(); i++) {
						Stage * stage = &stageList.at(i);

						//don't accept clicks on a song box if its locked
						if (!(*stage).getUnlocked()) {
							continue;
						}

						if (localPos.x >= (*stage).getPos().x && localPos.x <= (*stage).getPos().x + 100
							&& localPos.y >= (*stage).getPos().y && localPos.y <= (*stage).getPos().y + 100) {

							previewMusic.setVolume(0);
							previewMusic.stop(); //stop preview music
							backgroundMusic.pause(); //pause background music, resume once we finish the song
							if (reason == "battle") {
								sf::SoundBuffer sf;
								sf.loadFromFile("sounds/battle.ogg");
								sf::Sound s;
								s.setBuffer(sf);
								s.play();
								while (s.getStatus() != 0) {

								}
								//loading_screen("sounds/battle");
								//enter stage battle
								if (song_battle(w, &userProfile, stage, &stageMap[(*stage).getPath()]) > 0) {

									for (int i = 0; i < stageList.size(); i++) {
										Stage *it = &stageList.at(i);

										//check all locked songs if any were unlocked after the song battle
										if (!(*it).getUnlocked()) {
											std::cout << "examining song: " << (*it).getTitle() << "\n"; //debug
											std::deque<std::string> requiredSongList = (*it).getRequiredSongs();
											if (requiredSongList.size() > 0) {
												int unlockCount = 0;
												for (int i = 0; i < requiredSongList.size(); i++) {
													std::string sName = requiredSongList.at(i);
													//std::cout << "\tchecking requirement_song: " << sName << "\n";
													//std::cout << "\tincidentally the score is: " << stageMap[sName].getHighScore() << "\n";
													if (stageMap[sName].getTimesPlayed() > 0) {
														//std::cout << "\tpassed\n"; //debug
														unlockCount++;
													}
													else {
														//std::cout << "\tfailed\n"; //debug
													}
												}
												if (unlockCount == requiredSongList.size()) {
													(*it).setProfile((*it).getHighScore(), (*it).getTimesPlayed(), true, (*it).getHidden());
													std::cout << "" << (*it).getTitle() << " was unlocked!\n"; //debug
												}
											}
											else {
												//I don't know when a locked song would not have any requirements
												//so this conditional branch should not occur
											}
										}
									}
								}
								else {
									//we failed the song battle
								}
							}
							else if (reason == "make") {
								sf::Music m; //we use sf::Music instead of sf::Sound because sf::Music has getDuration()
								m.openFromFile("sounds/load.ogg");
								m.play();
								float totalDuration = m.getDuration().asMilliseconds();
								while (m.getStatus() != 0) {
									/*
									float curTime = m.getPlayingOffset().asMilliseconds();
									//start fade 75% into the sound
									float fadeThreshhold = .25;
									float curTimePercent = curTime / totalDuration;
									sf::View view = window.getView();
									sf::RectangleShape screenCover(sf::Vector2f(800,600)); //black cover that fades the window
									screenCover.setFillColor(sf::Color());

									if (curTimePercent >= fadeThreshhold) {
									std::cout << "\tin\n"; //debug
									window.clear(sf::Color());
									window.setView(view);
									window.draw(view);
									sf::Color fadeColor(0, 0, 0, 255.0 * (curTimePercent - fadeThreshhold) * (1/(1-fadeThreshhold)));
									std::cout << "\talpha: " << std::to_string(fadeColor.a) << "\n";
									screenCover.setFillColor(fadeColor);
									window.draw(screenCover);
									window.display();
									}*/
								}
								//loading_screen("sounds/load");
								song_maker(w, (*stage).getPath()); //enter stage battle
							}
							backgroundMusic.play();
						}
					}
				}
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == 36) {//Esc
					return 0;
				}
			}
			if (event.type == sf::Event::Closed) {
				(*w).close();
			}
		}

		//if currently previewing music
		if (previewMusic.getStatus() == 2) {
			int playPoint = (previewMusic.getDuration().asMilliseconds()) / 4;
			int curPlayPoint = previewMusic.getPlayingOffset().asMilliseconds();

			const int transDuration = 1000; //how long the transition (fade) takes
			const float startVolume = 50.0;
			float volPerTime = startVolume / transDuration; //in milliseconds
			int timeDelta = curPlayPoint - playPoint;

			const int allowedDuration = 20000; //how long we allow the preview to play before fading, in milliseconds
			const int delay = 1000; //how long the delay should be after song fades and before song restarts

									//in the beginning, increase volume from 0 to startVolume (smooth transition) for previewing
			if (timeDelta < transDuration) {
				float finalVol = volPerTime * timeDelta;
				previewMusic.setVolume(finalVol);
			}

			//start fade/lowering volume after a certain period of time
			if (timeDelta > allowedDuration) {
				/*
				std::cout << "VALUES: "
				<< "\n\tvolPerTime: " << volPerTime
				<< "\n\ttimeDelta: " << timeDelta
				<< "\n\tallowedDuration: " << allowedDuration
				<< "\n\tfinal: " << std::to_string(startVolume - volPerTime * (timeDelta - allowedDuration)) << "\n\n";; //debug
				*/
				float finalVol = startVolume - volPerTime * (timeDelta - allowedDuration);
				if (finalVol >= 0.0) {
					previewMusic.setVolume(finalVol);
				}
				else {
					previewMusic.setVolume(0);
				}
			}
			//restart song after transition finishes
			if (timeDelta > allowedDuration + transDuration + delay) {
				previewMusic.setVolume(0);
				previewMusic.play();
				previewMusic.setPlayingOffset(sf::milliseconds(playPoint));
			}
		}

		//drawing stage
		(*w).clear(sf::Color::Black);

		(*w).draw(backSprite);
		(*w).draw(titleText);

		for (int i = 0; i < stageList.size(); i++) {
			Stage * stage = &stageList.at(i);

			sf::Vector2f pos = (*stage).getPos();
			sf::RectangleShape thumbBox = (*stage).getBox();
			thumbTexture = (*stage).getThumb();
			thumbSprite.setTexture(thumbTexture);
			thumbSprite.setScale(sf::Vector2f(.166f, 0.166f));
			thumbSprite.setPosition(pos.x, pos.y);

			//change transparency if above or below a certain height so as to not get in the way of the "select stage" text and arrow buttons
			if (pos.y < posTolUp) {
				float m = (posTolUp - pos.y) / posTolUp * 1.5; //1.5 to accelerate fade effect
				thumbSprite.setColor(sf::Color(255, 255, 255, 255 * (1 - m)));
				sf::Color curColor = thumbBox.getOutlineColor();
				thumbBox.setOutlineColor(sf::Color(curColor.r, curColor.g, curColor.b, curColor.a * (1 - m)));
				sf::Color curTextColor = thumbText.getColor();
				thumbText.setColor(sf::Color(curTextColor.r, curTextColor.g, curTextColor.b, 255 * (1 - m)));
			}
			else if (pos.y < 600 && pos.y + 100 > posTolDown) { //don't transform on unseen songboxes
																//the +100 is because we want to know when the bottom of the box reaches 400, not the top
				float m = (pos.y - posTolDown) / (600 - posTolDown) * 1.5; //1.5 to accelerate fade effect
																		   /*
																		   std::cout << "y: " << pos.y << "\n";
																		   std::cout << "m: " << m << "\n";
																		   */
																		   //TODO DEBUG transition fade not smooth (and resets)
				thumbSprite.setColor(sf::Color(255, 255, 255, 255 * (1 - m)));
				sf::Color curColor = thumbBox.getOutlineColor();
				thumbBox.setOutlineColor(sf::Color(curColor.r, curColor.g, curColor.b, curColor.a * (1 - m)));
				sf::Color curTextColor = thumbText.getColor();
				thumbText.setColor(sf::Color(curTextColor.r, curTextColor.g, curTextColor.b, 255 * (1 - m)));
			}
			//draw album art first, then box container in this order
			//reason: if a stage is locked, we want to cover the album art with red to denote its locked status
			(*w).draw(thumbSprite);
			(*w).draw(thumbBox);

			sf::Vector2f boxPos = thumbBox.getPosition();
			float boxWidth = thumbBox.getSize().y;
			const int thumbTextCharSize = 12;
			const int borderSize = 5;
			std::string thumbTextString = "";
			thumbText.setFont(gameFont);
			thumbText.setCharacterSize(thumbTextCharSize);

			thumbTextString = (*stage).getTitle();
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5);
			(*w).draw(thumbText);

			thumbTextString = "Artist: " + (*stage).getArtist();
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5 + 12);
			(*w).draw(thumbText);

			thumbTextString = "Album: " + (*stage).getAlbum();
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5 + 24);
			(*w).draw(thumbText);

			thumbTextString = "Year: " + std::to_string((*stage).getYear());
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5 + 36);
			(*w).draw(thumbText);

			thumbTextString = "High Score: " + std::to_string((*stage).getHighScore());
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5 + 48);
			(*w).draw(thumbText);
		}

		//draw arrow buttons
		(*w).draw(upArrowLeft);
		(*w).draw(upArrowRight);
		(*w).draw(downArrowLeft);
		(*w).draw(downArrowRight);

		//draw hoverbox if hovering over a song
		if (enableHover) {
			(*w).draw(hoverBox);
		}

		(*w).display();
	}


	return 0;
}