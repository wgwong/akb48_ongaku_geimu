#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <deque>

class Stage {
public:
	//default constructor
	Stage();
	Stage(std::string path, float x, float y);

	//getters
	std::string getPath();
	sf::RectangleShape getBox();
	sf::Texture getThumb();
	sf::Vector2f getPos();
	std::string getTitle();
	std::string getArtist();
	std::string getAlbum();
	int getYear();
	int getHighScore();
	int getTimesPlayed();
	bool getUnlocked();
	bool getHidden();
	std::deque<std::string> getRequiredSongs();

	//setters
	void setInfo(std::string title, std::string artist, std::string album, int year);
	void setProfile(int score, int times, bool u, bool h);
	void setProfile(int score, int times, bool u, bool h, std::deque<std::string> required);
	void incTimesPlayed(int i);
	void setPos(float x, float y);
	void setPos(sf::Vector2f pos);
	void changePos(float xDelta, float yDelta);
	void changePos(sf::Vector2f posDelta);
	void setRequiredSongs(std::deque<std::string> required);

private:
	//stage variables
	std::string filePath;
	sf::RectangleShape stageBox;
	sf::Texture thumbTexture;
	float xPos, yPos;
	std::deque<std::string> requiredSongs;

	//song information
	std::string songTitle, songArtist, songAlbum;
	int songYear;

	//(song) profile variables
	int highScore, timesPlayed;
	bool unlocked, hidden;
};