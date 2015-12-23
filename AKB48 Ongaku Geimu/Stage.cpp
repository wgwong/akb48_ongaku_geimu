#include "Stage.h"

Stage::Stage() {

}

Stage::Stage(std::string path, float x, float y) {
	//set default for uninitialized variables
	//std::cout << "Attempting to load " << name << " at position " << std::to_string(x) << ", " << std::to_string(y) << "\n"; //debug

	filePath = path;
	xPos = x;
	yPos = y;

	stageBox.setSize(sf::Vector2f(100, 100));
	stageBox.setOutlineThickness(5);
	stageBox.setOutlineColor(sf::Color(rand() % 255, rand() % 255, rand() % 255, 128)); //TODO random color
	stageBox.setPosition(x, y);

	if (!thumbTexture.loadFromFile(filePath + ".png")) {
		std::cout << "ERROR: Can't load from file " << filePath << ".png\n";
	}
	songTitle = "";
	songArtist = "";
	songAlbum = "";
	songYear = 0;

	highScore = 0;
	timesPlayed = 0;
	unlocked = false;
	hidden = true;

	stageBox.setFillColor(sf::Color(200, 40, 70, 196)); //transparent for album art
}

//getters
std::string Stage::getPath() {
	return filePath;
}
sf::RectangleShape Stage::getBox() {
	return stageBox;
}
sf::Texture Stage::getThumb() {
	return thumbTexture;
}
sf::Vector2f Stage::getPos() {
	return sf::Vector2f(xPos, yPos);
}
std::string Stage::getTitle() {
	return songTitle;
}
std::string Stage::getArtist() {
	return songArtist;
}
std::string Stage::getAlbum() {
	return songAlbum;
}
int Stage::getYear() {
	return songYear;
}
int Stage::getHighScore() {
	return highScore;
}
int Stage::getTimesPlayed() {
	return timesPlayed;
}
bool Stage::getUnlocked() {
	return unlocked;
}
bool Stage::getHidden() {
	return hidden;
}
std::deque<std::string> Stage::getRequiredSongs() {
	return requiredSongs;
}

//setters
void Stage::setInfo(std::string title, std::string artist, std::string album, int year) {
	songTitle = title;
	songArtist = artist;
	songAlbum = album;
	songYear = year;
}
void Stage::setProfile(int score, int times, bool u, bool h) {
	highScore = score;
	timesPlayed = times;
	unlocked = u;
	hidden = h;
	if (unlocked) {
		stageBox.setFillColor(sf::Color(128, 128, 128, 0)); //transparent for album art
	}
	else {
		stageBox.setFillColor(sf::Color(200, 40, 70, 196)); //transparent for album art
	}
}
void Stage::setProfile(int score, int times, bool u, bool h, std::deque<std::string> required) {
	setProfile(score, times, u, h);
	requiredSongs = required;
}
void Stage::incTimesPlayed(int i) {
	timesPlayed += i;
}
void Stage::setPos(float x, float y) {
	xPos = x;
	yPos = y;
	stageBox.setPosition(x, y);
}
void Stage::setPos(sf::Vector2f pos) {
	setPos(pos.x, pos.y);
}
void Stage::changePos(float xDelta, float yDelta) {
	xPos += xDelta;
	yPos += yDelta;

	stageBox.setPosition(xPos, yPos);
}
void Stage::changePos(sf::Vector2f posDelta) {
	changePos(posDelta.x, posDelta.y);
}
void Stage::setRequiredSongs(std::deque<std::string> required) {
	requiredSongs = required;
}