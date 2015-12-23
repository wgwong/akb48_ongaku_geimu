#include "song_battle.h"

int song_battle(sf::RenderWindow *w, Profile * profile, Stage * stage, Stage * mappedStage) {
	std::string filePath = (*stage).getPath();
	const std::string fileExtension = ".ogg";

	/*
	sf::SoundBuffer buffer;
	buffer.loadFromFile(filepath);

	MusicStream stream;
	stream.load(buffer);
	stream.play();
	*/

	//loading stage
	//load fonts
	sf::Font gameFont;
	if (!gameFont.loadFromFile("arial.ttf")) {
		//error
		std::cout << "Cannot load font\n";
	}

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

													   //pause overlay
	sf::RectangleShape pauseOverlay(sf::Vector2f(800, 600));
	pauseOverlay.setFillColor(sf::Color(0, 0, 0, 128));
	/*
	//pause menu
	sf::RectangleShape pauseMenu(sf::Vector2f(400, 400));
	pauseOverlay.setFillColor(sf::Color(0, 0, 0, 128));
	*/
	//resume button
	sf::RectangleShape resumeButton(sf::Vector2f(250, 50));
	resumeButton.setFillColor(sf::Color(0, 0, 0, 160));
	resumeButton.setPosition(275, 250);
	sf::Text resumeText;
	resumeText.setFont(gameFont);
	resumeText.setCharacterSize(20);
	resumeText.setPosition(330, 260);
	resumeText.setString("Resume playing");
	//return button
	sf::RectangleShape returnButton(sf::Vector2f(250, 50));
	returnButton.setFillColor(sf::Color(0, 0, 0, 160));
	returnButton.setPosition(275, 300);
	sf::Text returnText;
	returnText.setFont(gameFont);
	returnText.setCharacterSize(20);
	returnText.setPosition(290, 310);
	returnText.setString("Return to song selection");

	//three pathways for beats to travel/drop through
	sf::Color leftPathColor(102, 1, 46, 128); //anchor/default color
	sf::Color middlePathColor(25, 80, 70, 128); //anchor/default color
	sf::Color rightPathColor(18, 40, 76, 128); //anchor/default color
	sf::ConvexShape leftPath;
	leftPath.setPointCount(4);
	leftPath.setPoint(0, sf::Vector2f(10, 0));
	leftPath.setPoint(1, sf::Vector2f(30, 0));
	leftPath.setPoint(2, sf::Vector2f(100, 600));
	leftPath.setPoint(3, sf::Vector2f(10, 600));
	leftPath.setFillColor(leftPathColor);
	sf::ConvexShape middlePath;
	middlePath.setPointCount(4);
	middlePath.setPoint(0, sf::Vector2f(290, 0));
	middlePath.setPoint(1, sf::Vector2f(310, 0));
	middlePath.setPoint(2, sf::Vector2f(345, 600));
	middlePath.setPoint(3, sf::Vector2f(255, 600));
	middlePath.setFillColor(middlePathColor);
	sf::ConvexShape rightPath;
	rightPath.setPointCount(4);
	rightPath.setPoint(0, sf::Vector2f(570, 0));
	rightPath.setPoint(1, sf::Vector2f(590, 0));
	rightPath.setPoint(2, sf::Vector2f(590, 600));
	rightPath.setPoint(3, sf::Vector2f(500, 600));
	rightPath.setFillColor(rightPathColor);

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

	//score
	int score = 0; //starting game score

				   //text that appears on the screen telling how accurate the hit was
	sf::Text hitText;
	hitText.setFont(gameFont);
	hitText.setCharacterSize(50);
	//for text that appears on the side of the song battle
	sf::Text sideText;
	sideText.setFont(gameFont);

	//initialize character texture/sprite
	std::deque<sf::Texture> charTextures;
	std::deque<Character> * activeTeam = (*(*profile).getTeam()).getActiveTeam();

	for (int i = 0; i < (*activeTeam).size(); i++) {
		sf::Texture charTexture;
		if (!charTexture.loadFromFile("characters/team_" + (*(*profile).getTeam()).getTeamName() + "/" + (*activeTeam)[i].getImagePath())) {
			std::cout << "ERROR: Cannot load image for character " << (*activeTeam)[i].getInfoPath() << "\n";
		}
		charTextures.push_back(charTexture);
	}
	sf::Sprite charSprite;

	//load beats
	std::deque<Beat> beatList;
	std::ifstream file(filePath + ".txt");
	std::string line;
	const std::string delimiter = "@";
	if (file.is_open()) {
		while (std::getline(file, line)) {
			std::string key = line.substr(0, line.find(delimiter));
			float time = stof(line.substr(line.find(delimiter) + 1, line.length()));
			sf::CircleShape shapeGraphics(30);
			if (key == "Left") {
				//shapeGraphics.setFillColor(sf::Color(102, 1, 46, 128));
				//shapeGraphics.setFillColor(sf::Color(205, 54, 128));
				shapeGraphics.setFillColor(sf::Color(250, 127, 191));
				shapeGraphics.setOutlineThickness(5);
				shapeGraphics.setOutlineColor(sf::Color(250, 127, 191));
				shapeGraphics.setPosition(20, 0);
			}
			else if (key == "Up") {
				shapeGraphics.setPointCount(3);
				//shapeGraphics.setFillColor(sf::Color(25, 80, 70, 128));
				shapeGraphics.setFillColor(sf::Color(4, 240, 186));
				shapeGraphics.setOutlineThickness(5);
				shapeGraphics.setOutlineColor(sf::Color(4, 240, 186));
				shapeGraphics.setPosition(270, 0);
			}
			else if (key == "Right") {
				shapeGraphics.setPointCount(4);
				//shapeGraphics.setFillColor(sf::Color(18, 40, 76, 128));
				shapeGraphics.setFillColor(sf::Color(5, 220, 238));
				shapeGraphics.setOutlineThickness(5);
				shapeGraphics.setOutlineColor(sf::Color(5, 220, 238));
				shapeGraphics.setPosition(520, 0);
			}

			beatList.push_back(Beat(key, time, shapeGraphics));
		}
	}

	Beat beat = beatList.front();
	bool finished = false; //status of empty beatlist

						   //containers for beats drawn on screen
	const float pixelsPerMillisecond = .3; //ideal is .25, but when making the song, I lag a bit after hearing the beat
	const float dropDistance = 500; //how far the beat travels
	const float distanceTolerance = 50; //how far off the beat is from the marker we are willing to accept
	const float scoreMultiplier = 100; //scoring scale
	std::deque<Beat> leftBeats;
	std::deque<Beat> upBeats;
	std::deque<Beat> rightBeats;

	sf::Music music;

	bool movieMode = false;
	std::unordered_map<std::string, bool> movieList;
	movieList["Majisuka Gakuen 5 Theme"] = true;
	movieList["Escape"] = true;
	movieList["Sailor Zombie"] = true;
	if (movieList.find(filePath) != movieList.end()) {
		movieMode = true;
	}

	sfe::Movie movie;
	if (movieMode) {
		if (!movie.openFromFile(filePath + ".mp4")) {
			//error loading
		}
		music.setVolume(0); //mute the music
		movie.setPosition(sf::Vector2f(0, (600 - movie.getSize().y) / 2));
		sf::Vector2f movieSize = movie.getSize();
		float dominantAxis = std::max(movieSize.x, movieSize.y); //we intend to resize the video such that the larger axis is 600 pixels
		float scale = 600 / dominantAxis;
		movie.setScale(sf::Vector2f(scale, scale));
		movie.play();
	}
	else {
		if (!music.openFromFile(filePath + fileExtension)) {
			return -1; // error
		}

		music.play();
	}

	std::deque<Effect> effects;

	sf::Clock clock;
	bool paused = false; //keeps track of the game state, paused or active
	float totalTimeElapsed = 0.0; //keeps track of the time elapsed
								  //used for when player pauses the game, we don't keep track of the time when the game is paused
								  //since our game depends on the current time of the clock since the start
								  //and due to how sf::Clock operates (there's no way to "pause" time, only restart it)
								  //we use this variable to accumulate how long the clock has been running for before it restarts due to pausing/resuming
	music.setPlayingOffset(sf::milliseconds(clock.restart().asMilliseconds())); //sync start time of music with clock

	while ((*w).isOpen()) {
		float elapsed = clock.getElapsedTime().asMilliseconds() + totalTimeElapsed;
		//TODO replace all the "clock.getElapsedTime().asMilliseconds() + totalTimeElapsed" with elapsed
		//check if still working afterwards

		//if we finished the song
		if (((movieMode && movie.getStatus() == 0) || (!movieMode && music.getStatus() == 0)) && !paused) {
			int highScore = 0;
			int contestedScore = (*stage).getHighScore();
			if (score > contestedScore) {
				highScore = score;
			}
			else {
				highScore = contestedScore;
			}

			if (score > 0) { //requirement to pass stage, may change later
				(*stage).setProfile(highScore, (*stage).getTimesPlayed() + 1, (*stage).getUnlocked(), (*stage).getHidden());
				(*mappedStage).setProfile(highScore, (*stage).getTimesPlayed() + 1, (*stage).getUnlocked(), (*stage).getHidden());

				//std::cout << "stage high score: " << (*stage).getHighScore() << "\n";

				//do some after game stats display first before returning (manual click)
				std::ofstream file;
				file.open(filePath + ".dat");
				file << "title:" << (*stage).getTitle() << "\n";
				file << "artist:" << (*stage).getArtist() << "\n";
				file << "album:" << (*stage).getAlbum() << "\n";
				file << "year:" << (*stage).getYear() << "\n";
				file << "highest_score:" << highScore << "\n";
				file << "times_played:" << (*stage).getTimesPlayed() << "\n";
				if ((*stage).getUnlocked()) {
					file << "unlocked:" << "true" << "\n";
				}
				else {
					file << "unlocked:" << "false" << "\n";
				}
				if ((*stage).getHidden()) {
					file << "hidden:" << "true" << "\n";
				}
				else {
					file << "hidden:" << "false";
				}
				file.close();

				//update profile
				//money

				//update character data
				std::deque<Character> * aTeam = (*(*profile).getTeam()).getActiveTeam();
				for (int i = 0; i < (*aTeam).size(); i++) {
					Character * c = &(*aTeam)[i];

					//calculate exp based off of score
					(*c).incExp(score / 2); //TODO, write exp system

											//if exp > maxExp for that level, level character

											//write character data to file
					std::ofstream charFile;
					charFile.open("characters/team_" + (*c).getTeam() + "/" + (*aTeam)[i].getInfoPath());
					if (charFile.is_open()) {
						charFile << "firstName:" << (*c).getFirstName() << "\n";
						charFile << "lastName:" << (*c).getLastName() << "\n";
						charFile << "nickname:" << (*c).getNickname() << "\n";
						std::cout << "nickname: " << (*c).getNickname() << "\n";//debug
						charFile << "team:" << (*c).getTeam() << "\n";
						charFile << "rank:" << (*c).getRank() << "\n";
						charFile << "prefix:" << (*c).getPrefix() << "\n";
						charFile << "suffix:" << (*c).getSuffix() << "\n";
						charFile << "level:" << std::to_string((*c).getLevel()) << "\n";
						charFile << "experience:" << std::to_string((*c).getExp()) << "\n";
						if ((*c).getUnlocked()) {
							charFile << "unlocked:true";
						}
						else {
							charFile << "unlocked:false";
						}
					}
					else {
						std::cout << "ERROR: Cannot open character data for saving. Filepath: " << "characters/team_" + (*c).getTeam() + "/" + (*aTeam)[i].getInfoPath() << "\n";
					}
				}
			}
			else {
				//fail stage
			}

			return score; //return score
		}

		//put ready to go beats in beat queue (to appear on the screen)
		//invariant, beats with an earlier time cannot appear before beats with a later time
		//thus, we can simply use a queue to pop a single beat at a time once its ready to put in the ready vectors
		if (!paused && !finished && beat.getTime() - elapsed < 2000) {
			//if beat time is within 2 seconds of the future, start drawing and dropping it on the screen

			std::string beatKey = beat.getKey();
			if (beatKey == "Left") {
				leftBeats.push_back(beat);
			}
			else if (beatKey == "Up") {
				upBeats.push_back(beat);
			}
			else if (beatKey == "Right") {
				rightBeats.push_back(beat);
			}

			if (beatList.size() == 1) {
				//if popped last one, finished
				finished = true;
			}
			else {
				beatList.pop_front();
				beat = beatList.front();
			}
		}

		sf::Event event;

		while ((*w).pollEvent(event)) {
			if (event.type == sf::Event::KeyPressed) {
				//std::cout << event.key.code << "\n"; //debug
				if (!paused) {
					if (event.key.code == 71) { //Left
						std::string baseValues = "";
						sf::Color oC = leftCircle.getOutlineColor();
						baseValues += std::to_string(oC.r);
						baseValues += ",";
						baseValues += std::to_string(oC.g);
						baseValues += ",";
						baseValues += std::to_string(oC.b);
						effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 350.0, &leftCircle, "outline color", "255,76,165", baseValues));

						bool validHit = false;
						for (int i = 0; i < leftBeats.size(); i++) {
							Beat * it = &leftBeats.at(i);
							sf::CircleShape * graphics = (*it).getGraphicsObject();
							float distanceDiff = std::abs((*it).getPos().y - 500.0);
							if (distanceDiff <= distanceTolerance) {
								validHit = true;
								score += (distanceTolerance - distanceDiff) * scoreMultiplier;

								bool effectInProgress = false;
								for (int i = 0; i < effects.size(); i++) {
									Effect * e = &effects.at(i);
									if ((*e).checkEIP(graphics)) {
										effectInProgress = true;
										std::cout << "Effect in progress already!\n"; //debug
										effects.erase(effects.begin() + i);
									}
								}
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "outline color", "255,255,255", baseValues));
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "fill color", "255,255,255", baseValues));
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "radius size", "40",
									std::to_string((*graphics).getRadius()) + ","
									+ std::to_string((*graphics).getPosition().x) + ","
									+ std::to_string((*graphics).getPosition().y)));

								//check if there are any pre-existing effects on hitText
								//if there are, discard them because we want to discard the previous message and start over
								for (int i = 0; i < effects.size(); i++) {
									Effect * e = &effects.at(i);
									if ((*e).checkEIP(&hitText)) {
										std::cout << "Text Effect in progress already!\n"; //debug
										effects.erase(effects.begin() + i);
										break;
									}
								}
								hitText.setColor(sf::Color(255, 255, 255, 224));
								float distDelta = distanceTolerance - distanceDiff;
								if (distDelta < 10) {
									hitText.setString("Spectacular!");
									hitText.setPosition(180, 50);
								}
								else if (distDelta >= 10 && distDelta < 20) {
									hitText.setString("Amazing!");
									hitText.setPosition(220, 50);
								}
								else if (distDelta >= 20 && distDelta < 30) {
									hitText.setString("Awesome");
									hitText.setPosition(220, 50);
								}
								else if (distDelta >= 30 && distDelta < 40) {
									hitText.setString("Great");
									hitText.setPosition(250, 50);
								}
								else if (distDelta >= 40) {
									hitText.setString("Good");
									hitText.setPosition(255, 50);
								}
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 500.0, &hitText, "color transparency permanent", "0", "224"));
							}
						}
						if (!validHit) {
							score -= 1000;
							//check if there are any pre-existing effects on hitText
							//if there are, discard them because we want to discard the previous message and start over
							for (int i = 0; i < effects.size(); i++) {
								Effect * e = &effects.at(i);
								if ((*e).checkEIP(&hitText)) {
									std::cout << "Text Effect in progress already!\n"; //debug
									effects.erase(effects.begin() + i);
									break;
								}
							}
							hitText.setColor(sf::Color(255, 255, 255, 224));
							hitText.setString("Miss");
							hitText.setPosition(255, 50);
							effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 500.0, &hitText, "color transparency permanent", "0", "224"));
						}
					}
					else if (event.key.code == 73) { //Up
						std::string baseValues = "";
						sf::Color oC = middleTriangle.getOutlineColor();
						baseValues += std::to_string(oC.r);
						baseValues += ",";
						baseValues += std::to_string(oC.g);
						baseValues += ",";
						baseValues += std::to_string(oC.b);
						effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 350.0, &middleTriangle, "outline color", "55,253,206", baseValues));

						bool validHit = false;
						for (int i = 0; i < upBeats.size(); i++) {
							Beat * it = &upBeats.at(i);
							sf::CircleShape * graphics = (*it).getGraphicsObject();
							float distanceDiff = std::abs((*it).getPos().y - 500.0);
							if (distanceDiff <= distanceTolerance) {
								validHit = true;
								score += (distanceTolerance - distanceDiff) * scoreMultiplier;

								bool effectInProgress = false;
								for (int i = 0; i < effects.size(); i++) {
									Effect * e = &effects.at(i);
									if ((*e).checkEIP(graphics)) {
										effectInProgress = true;
										std::cout << "Effect in progress already!\n"; //debug
										effects.erase(effects.begin() + i);
									}
								}
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "outline color", "255,255,255", baseValues));
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "fill color", "255,255,255", baseValues));
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "radius size", "40",
									std::to_string((*graphics).getRadius()) + ","
									+ std::to_string((*graphics).getPosition().x) + ","
									+ std::to_string((*graphics).getPosition().y)));

								//check if there are any pre-existing effects on hitText
								//if there are, discard them because we want to discard the previous message and start over
								for (int i = 0; i < effects.size(); i++) {
									Effect * e = &effects.at(i);
									if ((*e).checkEIP(&hitText)) {
										std::cout << "Text Effect in progress already!\n"; //debug
										effects.erase(effects.begin() + i);
										break;
									}
								}
								hitText.setColor(sf::Color(255, 255, 255, 224));
								float distDelta = distanceTolerance - distanceDiff;
								if (distDelta < 10) {
									hitText.setString("Spectacular!");
									hitText.setPosition(180, 50);
								}
								else if (distDelta >= 10 && distDelta < 20) {
									hitText.setString("Amazing!");
									hitText.setPosition(220, 50);
								}
								else if (distDelta >= 20 && distDelta < 30) {
									hitText.setString("Awesome");
									hitText.setPosition(220, 50);
								}
								else if (distDelta >= 30 && distDelta < 40) {
									hitText.setString("Great");
									hitText.setPosition(250, 50);
								}
								else if (distDelta >= 40) {
									hitText.setString("Good");
									hitText.setPosition(255, 50);
								}
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 500.0, &hitText, "color transparency permanent", "0", "224"));
							}
						}
						if (!validHit) {
							score -= 1000;
							//check if there are any pre-existing effects on hitText
							//if there are, discard them because we want to discard the previous message and start over
							for (int i = 0; i < effects.size(); i++) {
								Effect * e = &effects.at(i);
								if ((*e).checkEIP(&hitText)) {
									std::cout << "Text Effect in progress already!\n"; //debug
									effects.erase(effects.begin() + i);
									break;
								}
							}
							hitText.setColor(sf::Color(255, 255, 255, 224));
							hitText.setString("Miss");
							hitText.setPosition(255, 50);
							effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 500.0, &hitText, "color transparency permanent", "0", "224"));
						}
					}
					else if (event.key.code == 72) { //Right
						std::string baseValues = "";
						sf::Color oC = rightSquare.getOutlineColor();
						baseValues += std::to_string(oC.r);
						baseValues += ",";
						baseValues += std::to_string(oC.g);
						baseValues += ",";
						baseValues += std::to_string(oC.b);
						effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 350.0, &rightSquare, "outline color", "53,231,220", baseValues));

						bool validHit = false;
						for (int i = 0; i < rightBeats.size(); i++) {
							Beat * it = &rightBeats.at(i);
							sf::CircleShape * graphics = (*it).getGraphicsObject();
							float distanceDiff = std::abs((*it).getPos().y - 500.0);
							if (distanceDiff <= distanceTolerance) {
								validHit = true;
								score += (distanceTolerance - distanceDiff) * scoreMultiplier;

								bool effectInProgress = false;
								for (int i = 0; i < effects.size(); i++) {
									Effect * e = &effects.at(i);
									if ((*e).checkEIP(graphics)) {
										effectInProgress = true;
										std::cout << "Effect in progress already!\n"; //debug
										effects.erase(effects.begin() + i);
									}
								}
								//if (!effectInProgress) { //if graphics doesn't have any pre-existing effects
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "outline color", "255,255,255", baseValues));
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "fill color", "255,255,255", baseValues));
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 250.0, graphics, "radius size", "40",
									std::to_string((*graphics).getRadius()) + ","
									+ std::to_string((*graphics).getPosition().x) + ","
									+ std::to_string((*graphics).getPosition().y)));
								//}

								//check if there are any pre-existing effects on hitText
								//if there are, discard them because we want to discard the previous message and start over
								for (int i = 0; i < effects.size(); i++) {
									Effect * e = &effects.at(i);
									if ((*e).checkEIP(&hitText)) {
										std::cout << "Text Effect in progress already!\n"; //debug
										effects.erase(effects.begin() + i);
										break;
									}
								}
								hitText.setColor(sf::Color(255, 255, 255, 224));
								float distDelta = distanceTolerance - distanceDiff;
								if (distDelta < 10) {
									hitText.setString("Spectacular!");
									hitText.setPosition(180, 50);
								}
								else if (distDelta >= 10 && distDelta < 20) {
									hitText.setString("Amazing!");
									hitText.setPosition(220, 50);
								}
								else if (distDelta >= 20 && distDelta < 30) {
									hitText.setString("Awesome");
									hitText.setPosition(220, 50);
								}
								else if (distDelta >= 30 && distDelta < 40) {
									hitText.setString("Great");
									hitText.setPosition(250, 50);
								}
								else if (distDelta >= 40) {
									hitText.setString("Good");
									hitText.setPosition(255, 50);
								}
								effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 500.0, &hitText, "color transparency permanent", "0", "224"));
							}
						}
						if (!validHit) {
							score -= 1000;
							//check if there are any pre-existing effects on hitText
							//if there are, discard them because we want to discard the previous message and start over
							for (int i = 0; i < effects.size(); i++) {
								Effect * e = &effects.at(i);
								if ((*e).checkEIP(&hitText)) {
									std::cout << "Text Effect in progress already!\n"; //debug
									effects.erase(effects.begin() + i);
									break;
								}
							}
							hitText.setColor(sf::Color(255, 255, 255, 224));
							hitText.setString("Miss");
							hitText.setPosition(255, 50);
							effects.push_back(Effect(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed, 500.0, &hitText, "color transparency permanent", "0", "224"));
						}
					}
				}
				if (event.key.code == 11) { //L - List beats
					for (auto it : beatList) {
						std::cout << it.toString();
					}
				}
				else if (event.key.code == 15 || event.key.code == 36) { //P - Pause/Unpause game, Esc - pauses and unpauses the game
					if (paused) {
						paused = false;
						clock.restart();
						music.play();
						music.setPlayingOffset(sf::milliseconds(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed)); //resume from where we left off
					}
					else {
						paused = true;
						totalTimeElapsed += clock.restart().asMilliseconds();
						music.stop();
					}
				}
				else if (event.key.code == 5) { //F - Finish song
					music.stop();
				}
				else if (event.key.code == 2) { //C - Clear console
					system("cls");
				}
			}
			if (event.type == sf::Event::MouseMoved) {
				sf::Vector2i localPos = sf::Mouse::getPosition((*w));

				if (paused) {
					if (localPos.x >= 275 && localPos.x <= 525
						&& localPos.y >= 250 && localPos.y <= 300) {
						//resume button
						returnButton.setFillColor(sf::Color(0, 0, 0, 160));
						resumeButton.setFillColor(sf::Color(96, 96, 96, 160));
					}
					else if (localPos.x >= 275 && localPos.x <= 525
						&& localPos.y >= 300 && localPos.y <= 350) {
						//return button
						resumeButton.setFillColor(sf::Color(0, 0, 0, 160));
						returnButton.setFillColor(sf::Color(96, 96, 96, 160));
					}
					else {
						resumeButton.setFillColor(sf::Color(0, 0, 0, 160));
						returnButton.setFillColor(sf::Color(0, 0, 0, 160));
					}
				}
			}
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i localPos = sf::Mouse::getPosition((*w));

				if (paused) {
					if (localPos.x >= 275 && localPos.x <= 525
						&& localPos.y >= 250 && localPos.y <= 300) {
						//resume button was clicked, unpause
						paused = false;
						clock.restart();
						music.play();
						music.setPlayingOffset(sf::milliseconds(clock.getElapsedTime().asMilliseconds() + totalTimeElapsed)); //resume from where we left off
					}
					else if (localPos.x >= 275 && localPos.x <= 525
						&& localPos.y >= 300 && localPos.y <= 350) {
						//return button was clicked
						return 0;
					}
				}
			}
			if (event.type == sf::Event::Closed) {
				(*w).close();
			}
		}

		if (!paused) {
			//handle effects in queue
			float nowTime = clock.getElapsedTime().asMilliseconds() + totalTimeElapsed;
			for (int i = 0; i < effects.size(); i++) {
				Effect * it = &effects.at(i);
				if ((*it).getTimeDelta(nowTime) <= (*it).getDuration()) {
					(*it).transition(nowTime);
				}
				else {
					(*it).expire(); //marks the effect as done so that removal can clean up
				}
			}
			//remove beats that already passed
			leftBeats.erase(std::remove_if(leftBeats.begin(), leftBeats.end(), [](Beat b) {
				return b.getPos().y > 600;
			}), leftBeats.end());
			upBeats.erase(std::remove_if(upBeats.begin(), upBeats.end(), [](Beat b) {
				return b.getPos().y > 600;
			}), upBeats.end());
			rightBeats.erase(std::remove_if(rightBeats.begin(), rightBeats.end(), [](Beat b) {
				return b.getPos().y > 600;
			}), rightBeats.end());

			//remove effects that expired
			effects.erase(std::remove_if(effects.begin(), effects.end(), [](Effect e) {
				return e.getExpired();
			}), effects.end());
		}


		//drawing stage
		//set default background black
		(*w).clear(sf::Color::Black);
		//draw movie if movie stage
		if (movieMode) {
			movie.update();
			(*w).draw(movie);
		}
		else {
			(*w).draw(backSprite); //background
		}

		//score text
		std::string scoreStr = std::to_string(score);
		sideText.setString(scoreStr); //update score
		int scoreXPos = 775 - 28 * scoreStr.length(); //calculate x position so that score shifts accordingly if increased
		sideText.setPosition(scoreXPos, 10);
		sideText.setCharacterSize(50);
		(*w).draw(sideText);

		for (int i = 0; i < 5; i++) {
			charSprite.setTexture(charTextures[i]);
			if (i == 0) {
				Character * captain = (*(*profile).getTeam()).getCaptain();

				charSprite.setScale(sf::Vector2f(.5, .5));
				charSprite.setPosition(625, 100);

				sideText.setCharacterSize(15);
				//team captain's level
				sideText.setString("Level " + std::to_string((*captain).getLevel()));
				sideText.setPosition(625, 300);
				(*w).draw(sideText);
				//team captain's name
				sideText.setString((*captain).getName());
				sideText.setPosition(625, 320);
				(*w).draw(sideText);
			}
			else {
				charSprite.setScale(sf::Vector2f(.23, .23));
				if (i == 1 || i == 2) {
					charSprite.setPosition(625 + 80 * (i % 2), 100 + 250);
				}
				else {
					charSprite.setPosition(625 + 80 * (i % 2), 100 + 250 + 100);
				}
			}
			(*w).draw(charSprite);
		}

		//pathways
		int colorTime = int(elapsed) % 8000;
		const int colorAdder = 70;
		float colorMultiplier = ((colorTime % 2000) * 1.0) / 2000.0;
		float colorAdderResult = colorAdder;
		if (colorTime < 2000) {
			colorAdderResult *= colorMultiplier;
		}
		else if (colorTime >= 2000 && colorTime < 4000) {
			colorAdderResult *= (1 - colorMultiplier);
		}
		else if (colorTime >= 4000 && colorTime < 6000) {
			colorAdderResult *= -1;
			colorAdderResult *= colorMultiplier;
		}
		else if (colorTime >= 6000) {
			colorAdderResult *= -1;
			colorAdderResult *= (1 - colorMultiplier);
		}
		//std::cout << "time: " << colorTime << ", cm: " << colorMultiplier << ", result: " << colorAdderResult << "\n"; //debug
		leftPath.setFillColor(sf::Color(std::max(leftPathColor.r + colorAdderResult, 0.0f),
			std::max(leftPathColor.g + colorAdderResult / 2, 0.0f),
			std::max(leftPathColor.b + colorAdderResult / 2, 0.0f), leftPathColor.a));
		middlePath.setFillColor(sf::Color(std::max(middlePathColor.r + colorAdderResult, 0.0f),
			std::max(middlePathColor.g + colorAdderResult / 2, 0.0f),
			std::max(middlePathColor.b + colorAdderResult / 2, 0.0f), middlePathColor.a));
		rightPath.setFillColor(sf::Color(std::max(rightPathColor.r + colorAdderResult, 0.0f),
			std::max(rightPathColor.g + colorAdderResult / 2, 0.0f),
			std::max(rightPathColor.b + colorAdderResult / 2, 0.0f), rightPathColor.a));
		(*w).draw(leftPath);
		(*w).draw(middlePath);
		(*w).draw(rightPath);
		//lower 3 buttons
		(*w).draw(leftCircle);
		(*w).draw(middleTriangle);
		(*w).draw(rightSquare);
		//active beats
		for (int i = 0; i < leftBeats.size(); i++) {
			Beat * it = &leftBeats.at(i);
			//cycle through active beats and reposition them
			float expectedTime = (*it).getTime();
			float curTime = clock.getElapsedTime().asMilliseconds() + totalTimeElapsed;
			float result = dropDistance - pixelsPerMillisecond * (expectedTime - curTime);

			sf::CircleShape * graphics = (*it).getGraphicsObject();
			const float beatSize = 30;

			if (!paused) {
				bool effectInProgress = false;
				for (int i = 0; i < effects.size(); i++) {
					Effect * e = &effects.at(i);
					if ((*e).checkEIP(graphics)) {
						effectInProgress = true;
					}
				}
				//make the beat grow as it travels downward
				if (!effectInProgress) {
					(*graphics).setRadius(std::min(result / dropDistance * beatSize, beatSize));
				}
				(*graphics).setPosition((*graphics).getPosition().x, result);
				(*it).setY(result);
			}

			(*w).draw(*graphics);
		}
		for (int i = 0; i < upBeats.size(); i++) {
			Beat * it = &upBeats.at(i);
			//cycle through active beats and reposition them
			float expectedTime = (*it).getTime();
			float curTime = clock.getElapsedTime().asMilliseconds() + totalTimeElapsed;
			float result = dropDistance - pixelsPerMillisecond * (expectedTime - curTime);

			sf::CircleShape * graphics = (*it).getGraphicsObject();
			const float beatSize = 30;

			if (!paused) {
				bool effectInProgress = false;
				for (int i = 0; i < effects.size(); i++) {
					Effect * e = &effects.at(i);
					if ((*e).checkEIP(graphics)) {
						effectInProgress = true;
					}
				}
				//make the beat grow as it travels downward
				if (!effectInProgress) {
					float sizeDelta = beatSize - (result / dropDistance * beatSize); //centers the enlarging beat
					(*graphics).setRadius(std::min(result / dropDistance * beatSize, beatSize));
					(*graphics).setPosition(middleTriangle.getPosition().x + sizeDelta, result);
				}
				else {
					(*graphics).setPosition((*graphics).getPosition().x, result);
				}
				(*it).setY(result);
			}

			(*w).draw(*graphics);
		}
		for (int i = 0; i < rightBeats.size(); i++) {
			Beat * it = &rightBeats.at(i);
			//cycle through active beats and reposition them
			float expectedTime = (*it).getTime();
			float curTime = clock.getElapsedTime().asMilliseconds() + totalTimeElapsed;
			float result = dropDistance - pixelsPerMillisecond * (expectedTime - curTime);

			sf::CircleShape * graphics = (*it).getGraphicsObject();
			const float beatSize = 30;

			if (!paused) {
				bool effectInProgress = false;
				for (int i = 0; i < effects.size(); i++) {
					Effect * e = &effects.at(i);
					if ((*e).checkEIP(graphics)) {
						effectInProgress = true;
					}
				}
				//make the beat grow as it travels downward
				if (!effectInProgress) {
					float sizeDelta = beatSize - (result / dropDistance * beatSize); //centers the enlarging beat
					(*graphics).setRadius(std::min(result / dropDistance * beatSize, beatSize));
					(*graphics).setPosition(rightSquare.getPosition().x + sizeDelta * 2, result);
				}
				else {
					(*graphics).setPosition((*graphics).getPosition().x, result);
				}
				(*it).setY(result);
			}

			(*w).draw(*graphics);
		}
		(*w).draw(hitText);
		if (paused) {
			(*w).draw(pauseOverlay); //darkens the entire screen when paused
			(*w).draw(resumeButton); //resume button
			(*w).draw(resumeText); //resume text
			(*w).draw(returnButton); //return button
			(*w).draw(returnText); //return text
		}

		//end current frame
		(*w).display();
	}

	return 0;
}