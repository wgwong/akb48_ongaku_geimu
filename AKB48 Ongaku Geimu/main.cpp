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

#include <sfeMovie/Movie.hpp>
#include <sfeMovie/StreamSelection.hpp>
#include <sfeMovie/Visibility.hpp>


//helper function, argument format: original string (input), delimiter, output string vector
void split(const std::string& s, char delim, std::vector<std::string>& v);

//helper function, returns a list of the initial members of a team
std::deque<std::string> getInitialTeam(std::string name) {
	std::deque<std::string> team;
	if (name == "A") {
		team.push_back("takahashi_juri.dat");
		team.push_back("takahashi_juri.dat");
		team.push_back("takahashi_juri.dat");
		team.push_back("takahashi_juri.dat");
		team.push_back("takahashi_juri.dat");
	}
	else if (name == "K") {
		team.push_back("tano_yuka.dat");
		team.push_back("abe_maria.dat");
		team.push_back("aigasa_moe.dat");
		team.push_back("shinozaki_ayana.dat");
		team.push_back("mogi_shinobu.dat");
	}
	else if (name == "B") {
		team.push_back("takahashi_juri.dat");
		team.push_back("takahashi_juri.dat");
		team.push_back("takahashi_juri.dat");
		team.push_back("takahashi_juri.dat");
		team.push_back("takahashi_juri.dat");
	}
	else if (name == "4") {
		team.push_back("takahashi_juri.dat");
		team.push_back("okada_nana.dat");
		team.push_back("kojima_mako.dat");
		team.push_back("nishino_miki.dat");
		team.push_back("omori_miyu.dat");
	}
	return team;
}

class Stage;
class Profile;

int loading_screen(std::string name);
int create_profile();
int song_selection(std::string reason);
int song_battle(Profile * profile, Stage * stg, Stage * mStg);
int song_maker(std::string name);

sf::RenderWindow window;

class MusicStream : public sf::SoundStream {
public:
	void load(const sf::SoundBuffer& buffer) {
		// extract the audio samples from the sound buffer to our own container
		m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());

		// reset the current playing position 
		m_currentSample = 0;

		// initialize the base class
		initialize(buffer.getChannelCount(), buffer.getSampleRate());
	}

private:
	virtual bool onGetData(Chunk& data) {
		// number of samples to stream every time the function is called;
		// in a more robust implementation, it should be a fixed
		// amount of time rather than an arbitrary number of samples
		const int samplesToStream = 50000;

		// set the pointer to the next audio samples to be played
		data.samples = &m_samples[m_currentSample];

		// have we reached the end of the sound?
		if (m_currentSample + samplesToStream <= m_samples.size()) {
			// end not reached: stream the samples and continue
			data.sampleCount = samplesToStream;
			m_currentSample += samplesToStream;
			return true;
		} else {
			// end of stream reached: stream the remaining samples and stop playback
			data.sampleCount = m_samples.size() - m_currentSample;
			m_currentSample = m_samples.size();
			return false;
		}
	}

	virtual void onSeek(sf::Time timeOffset) {
		// compute the corresponding sample index according to the sample rate and channel count
		m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
	}

	std::vector<sf::Int16> m_samples;
	std::size_t m_currentSample;
};

//individual character class
class Character {
public:
	Character() {

	}
	Character(std::string first, std::string last, std::string nick, std::string tm, std::string rnk, std::string pre, std::string suf, int lvl, int exp, bool u) {
		firstName = first;
		lastName = last;
		nickname = nick;
		team = tm;
		rank = rnk;
		prefix = pre;
		suffix = suf;
		level = lvl;
		experience = exp;
		unlocked = u;
		stageSongPath = "";
		infoPath = "";
		imagePath = "";
	}
	
	//getters
	std::string getInfoPath() {
		return infoPath;
	}
	std::string getImagePath() {
		return imagePath;
	}
	std::string getName() { //japanese naming convention
		return lastName + " " + firstName;
	}
	std::string getFirstName() {
		return firstName;
	}
	std::string getLastName() {
		return lastName;
	}
	std::string getNickname() {
		return nickname;
	}
	std::string getTeam() {
		return team;
	}
	std::string getRank() {
		return rank;
	}
	std::string getPrefix() {
		return prefix;
	}
	std::string getSuffix() {
		return suffix;
	}
	int getLevel() {
		return level;
	}
	int getExp() {
		return experience;
	}
	bool getUnlocked() {
		return unlocked;
	}
	std::string getStageSongPath() {
		return stageSongPath;
	}

	//setters
	void setInfoPath(std::string i) {
		infoPath = i;
	}
	void setImagePath(std::string i) {
		imagePath = i;
	}
	void setFirstName(std::string f) {
		firstName = f;
	}
	void setLastName(std::string l) {
		lastName = l;
	}
	void setNickname(std::string n) {
		nickname = n;
	}
	void setTeam(std::string t) {
		team = t;
	}
	void setRank(std::string r) {
		rank = r;
	}
	void setPrefix(std::string p) {
		prefix = p;
	}
	void setSuffix(std::string s) {
		suffix = s;
	}
	void setLevel(int l) {
		level = l;
	}
	void setExp(int e) {
		experience = e;
	}
	void incExp(int a) {
		experience += a;
	}
	void levelUp() {
		level += 1;
		experience = 0;
		//do checks for unlocking stuff when level up
	}
	void setUnlocked(bool u) {
		unlocked = u;
	}
	void setStageSongPath(std::string s) {
		stageSongPath = s;
	}
private:
	std::string infoPath;
	std::string imagePath;
	std::string firstName;
	std::string lastName;
	std::string nickname;
	std::string team;
	int cardNumber; //probably won't use this
	std::string rank; // C, B, A, S, SS, SSS
	std::string prefix; //allows for special versions of a character card (different from base card)
	std::string suffix; // <basic>, EX, SP
	int level;
	int experience;
	bool unlocked;
	std::string stageSongPath;
	/* not yet implemented
	smart
	cute
	cool
	beauty
	skills
	*/
};

//a team contains many character classes
class Team {
public:
	Team() {

	}
	//getters
	std::string getTeamName() {
		return teamName;
	}
	std::deque<Character> * getActiveTeam() {
		return &activeTeam;
	}
	std::deque<Character> * getInactiveTeam() {
		return &inactiveTeam;
	}
	std::unordered_map<std::string, Character> * getInactiveTeamList() {
		return &inactiveTeamList;
	}
	Character * getCaptain() {
		return &captain;
	}

	//setters
	void setTeamName(std::string tn) {
		teamName = tn;
	}
	void setActiveTeam(std::deque<Character> aTeam) {
		activeTeam = aTeam;
	}
	void addActiveChar(Character c) {
		activeTeam.push_back(c);
	}
	void addActiveChar(int i, Character c) {
		activeTeam[i] = c;
	}
	void addInactiveChar(Character c) {
		inactiveTeam.push_back(c);
		inactiveTeamList[c.getName()] = c;
	}
	void setCaptain(Character c) {
		captain = c;
	}
	
private:
	std::string teamName;
	std::deque<Character> activeTeam; //the characters that are used during stage battles
	std::deque<Character> inactiveTeam; //all the other characters not used during stage battles
	std::unordered_map<std::string, Character> inactiveTeamList;
	Character captain; //pointer to the captain of the team
	Character center; //IDK if I will use this
};

//contains information about the player / player profile & stats
class Profile {
public:
	Profile() {

	}
	Profile(std::string pn) {
		playerName = pn;
		money = 0.0;
	}
	Profile(std::string pn, Team tm) {
		playerName = pn;
		team = tm;
		money = 0.0;
	}
	Profile(std::string pn, Team tm, float m) {
		playerName = pn;
		team = tm;
		money = m;
	}

	//getters
	std::string getPlayerName() {
		return playerName;
	}
	//returns the address of the team
	Team *getTeam() {
		return &team;
	}
	float getMoney() {
		return money;
	}

	//setters
	void setPlayerName(std::string pn) {
		playerName = pn;
	}
	//sets team pointer to the address of the parameter
	void setTeam(Team t) {
		team = t;
	}
	void setMoney(float m) {
		money = m;
	}
	void incMoney(float m) {
		money += m;
	}

private:
	std::string playerName;
	Team team;
	float money;
};

class Stage {
public:
	//default constructor
	Stage() {

	}
	Stage(std::string path, float x, float y) {
		//set default for uninitialized variables
		//std::cout << "Attempting to load " << name << " at position " << std::to_string(x) << ", " << std::to_string(y) << "\n"; //debug

		filePath = path;
		xPos = x;
		yPos = y;

		stageBox.setSize(sf::Vector2f(100, 100));
		stageBox.setOutlineThickness(5);
		stageBox.setOutlineColor(sf::Color(rand()%255, rand()%255, rand()%255, 128)); //TODO random color
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
	std::string getPath() {
		return filePath;
	}
	sf::RectangleShape getBox() {
		return stageBox;
	}
	sf::Texture getThumb() {
		return thumbTexture;
	}
	sf::Vector2f getPos() {
		return sf::Vector2f(xPos, yPos);
	}
	std::string getTitle() {
		return songTitle;
	}
	std::string getArtist() {
		return songArtist;
	}
	std::string getAlbum() {
		return songAlbum;
	}
	int getYear() {
		return songYear;
	}
	int getHighScore() {
		return highScore;
	}
	int getTimesPlayed() {
		return timesPlayed;
	}
	bool getUnlocked() {
		return unlocked;
	}
	bool getHidden() {
		return hidden;
	}
	std::deque<std::string> getRequiredSongs() {
		return requiredSongs;
	}

	//setters
	void setInfo(std::string title, std::string artist, std::string album, int year) {
		songTitle = title;
		songArtist = artist;
		songAlbum = album;
		songYear = year;
	}
	void setProfile(int score, int times, bool u, bool h) {
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
	void setProfile(int score, int times, bool u, bool h, std::deque<std::string> required) {
		setProfile(score, times, u, h);
		requiredSongs = required;
	}
	void incTimesPlayed(int i) {
		timesPlayed += i;
	}
	void setPos(float x, float y) {
		xPos = x;
		yPos = y;
		stageBox.setPosition(x, y);
	}
	void setPos(sf::Vector2f pos) {
		setPos(pos.x, pos.y);
	}
	void changePos(float xDelta, float yDelta) {
		xPos += xDelta;
		yPos += yDelta;

		stageBox.setPosition(xPos, yPos);
	}
	void changePos(sf::Vector2f posDelta) {
		changePos(posDelta.x, posDelta.y);
	}
	void setRequiredSongs(std::deque<std::string> required) {
		requiredSongs = required;
	}

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

class Beat {
public:
	/*
		Constructor
		time must be in milliseconds
	*/
	Beat(std::string key, float time) {
		keyHit = key;
		timeLocation = time;
		shapeGraphics = sf::CircleShape(50);
		xPos = 0.0;
		yPos = 0.0;
	}
	Beat(std::string key, float time, sf::CircleShape shape) {
		keyHit = key;
		timeLocation = time;
		shapeGraphics = shape;
		xPos = shape.getPosition().x;
		yPos = shape.getPosition().y;
	}

	//getters
	float getTime() {
		return timeLocation;
	}
	std::string getKey() {
		return keyHit;
	}
	sf::CircleShape getGraphics() {
		return shapeGraphics;
	}
	sf::CircleShape * getGraphicsObject() {
		return &shapeGraphics;
	}

	sf::Vector2f getPos() {
		return sf::Vector2f(xPos, yPos);
	}

	//setters
	void setTime(float t) {
		timeLocation = t;
	}
	void setKey(std::string k) {
		keyHit = k;
	}
	void setX(float newX) {
		xPos = newX;
	}
	void setY(float newY) {
		yPos = newY;
	}

	std::string toString() {
		return keyHit + " @ " + std::to_string(timeLocation / 1000) + "s @ position ("
			+ std::to_string(xPos) + ", " + std::to_string(yPos) + ")\n";
	}
private:
	std::string keyHit; //the key that was pressed
	float timeLocation; //time in which the beat was hit
	float xPos;
	float yPos; //makeshift solution: can't modify shapeGraphics member field's position permanently
	sf::CircleShape shapeGraphics; //graphics to draw
};

class Effect {
public:
	Effect(float start, float dur, sf::Shape * o, std::string what, std::string desc, std::string base) {
		expired = false;
		startTime = start;
		duration = dur;
		obj = o;
		whatEffect = what;
		effectDesc = desc;
		baseValues = base;
	}
	Effect(float start, float dur, sf::Text * t, std::string what, std::string desc, std::string base) {
		expired = false;
		startTime = start;
		duration = dur;
		tObj = t;
		whatEffect = what;
		effectDesc = desc;
		baseValues = base;
	}

	//check effect in progress, return true if an effect is already modifying the parameter
	bool checkEIP(sf::Shape * s) {
		return obj == s;
	}
	bool checkEIP(sf::Text * t) {
		return tObj == t;
	}

	bool getExpired() {
		return expired;
	}

	float getDuration() {
		return duration;
	}

	//get the time delta between now & start time of effect
	float getTimeDelta(float now) {
		return now - startTime;
	}

	void expire() {
		expired = true;
	}

	//accepts time (in milliseconds) and calculates the current transition of the effect through a time delta
	//now must be less than duration (calculate this externally beforehand before calling this function) or weird stuff happens
	void transition(float now) {
		float delta = now - startTime;

		float multiplier = 0.0;

		if (whatEffect.find("permanent") == -1) {
			if (delta <= duration / 2) { //flow & ebb of effect
				multiplier = delta / (duration / 2);
			}
			else if (delta > duration / 2) {
				delta -= duration / 2;
				multiplier = 1 - delta / (duration / 2);
			}
		}
		else {
			multiplier = delta / duration;
		}
		
		doEffect(multiplier);
	}

private:
	bool expired; //marks whether effect is done, used in removal/cleanup
	float startTime;
	float duration;
	sf::Shape * obj;
	sf::Text * tObj;
	std::string whatEffect;
	std::string effectDesc;
	std::string baseValues;

	//m - multiplier between 0.0 and 1.0
	void doEffect(float m) {
		//std::cout << "Effect on " << std::to_string((*obj).getPosition().y ) << " m: " << std::to_string(m) << "\n"; //debug
		if (whatEffect == "outline color") {
			char delimiter = ',';
			std::vector<std::string> values;
			split(effectDesc, delimiter, values);

			std::vector<std::string> oriColors;
			split(baseValues, delimiter, oriColors);

			float rAdd = (stof(values.at(0)) - stof(oriColors.at(0))) * m;
			float gAdd = (stof(values.at(1)) - stof(oriColors.at(1))) * m;
			float bAdd = (stof(values.at(2)) - stof(oriColors.at(2))) * m;
			float rNew = stof(oriColors.at(0)) + rAdd;
			float gNew = stof(oriColors.at(1)) + gAdd;
			float bNew = stof(oriColors.at(2)) + bAdd;

			if (values.size() == 3) {
				(*obj).setOutlineColor(sf::Color(rNew, gNew, bNew));
			}
			else if (values.size() == 4) {
				float tAdd = (stof(values.at(3)) - stof(oriColors.at(3))) * m;
				float tNew = stof(oriColors.at(3)) + tAdd;
				(*obj).setOutlineColor(sf::Color(rNew, gNew, bNew, tNew));
			}
		} else if (whatEffect == "fill color") {
			char delimiter = ',';
			std::vector<std::string> values;
			split(effectDesc, delimiter, values);

			std::vector<std::string> oriColors;
			split(baseValues, delimiter, oriColors);

			float rAdd = (stof(values.at(0)) - stof(oriColors.at(0))) * m;
			float gAdd = (stof(values.at(1)) - stof(oriColors.at(1))) * m;
			float bAdd = (stof(values.at(2)) - stof(oriColors.at(2))) * m;
			float rNew = stof(oriColors.at(0)) + rAdd;
			float gNew = stof(oriColors.at(1)) + gAdd;
			float bNew = stof(oriColors.at(2)) + bAdd;

			if (values.size() == 3) {
				(*obj).setFillColor(sf::Color(rNew, gNew, bNew));
			}
			else if (values.size() == 4) {
				float tAdd = (stof(values.at(3)) - stof(oriColors.at(3))) * m;
				float tNew = stof(oriColors.at(3)) + tAdd;
				(*obj).setFillColor(sf::Color(rNew, gNew, bNew, tNew));
			}
		}
		else if (whatEffect == "radius size") {
			char delimiter = ',';
			std::vector<std::string> oriValues;
			split(baseValues, delimiter, oriValues);

			float sizeDelta = (stof(effectDesc) - stof(oriValues.at(0))) * m;
			float newSize = stof(oriValues.at(0)) + sizeDelta;
			try {
				sf::CircleShape * cObj = dynamic_cast<sf::CircleShape*>(obj); //hacky, reverting from abstract base to derived again...
				if (cObj) {
					(*cObj).setRadius(newSize);

					sf::Vector2f newPos(stof(oriValues.at(1)) - sizeDelta, stof(oriValues.at(2)) - sizeDelta);

					(*cObj).setPosition(newPos);
						//resizing starts from the top-left, so if we want a "center" effect
						//subtract half of the growth on x and y
				}
				else {
					std::cout << "ERROR: cObj is a null pointer!\n"; //debug
				}
			}
			catch (...) { //TODO unsafe, make it null pointer exception later
				std::cout << "ERROR: could not dynamically cast obj\n";
			}
		}
		else if (whatEffect.find("color transparency") != -1) {
			float delta = (stof(effectDesc) - stof(baseValues)) * m;
			float newAlpha = stof(baseValues) + delta;
			if (tObj) {
				sf::Color oriColors = (*tObj).getColor();
				(*tObj).setColor(sf::Color(oriColors.r, oriColors.g, oriColors.b, newAlpha));
			}
		}
	}

};

//const std::string& s, char delim, std::vector<std::string>& v
void split(const std::string& s, char delim, std::vector<std::string>& v) {
	auto i = 0;
	auto pos = s.find(delim);
	while (pos != std::string::npos) {
		v.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);

		if (pos == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

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
					song_selection("battle");
				}
				else if (localPos.x >= 250 && localPos.x <= 550
					&& localPos.y >= 400 && localPos.y <= 500) {
					//create button
					//std::cout << "Starting beat maker\n"; //debug
					music.stop();
					//window.setMouseCursorVisible(true);
					song_selection("make");
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

int create_profile() {

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
	nameBar.setPosition((window.getSize().x - nameBar.getSize().x) / 2, (window.getSize().y - nameBar.getSize().y) / 2);
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
	nameText.setPosition((window.getSize().x - nameBar.getLocalBounds().width) / 2, nameBar.getPosition().y + (nameBar.getSize().y - nameText.getLocalBounds().height)/2);

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
	float teamRectSpacing = (window.getSize().x - numTeams * teamRect.getSize().x) / (numTeams + 1); //spacing between each team rectangle

	//bool
	bool nameEntered = false; //need to enter name first

	sf::Clock clock; //used for the blinking rate of the name cursor

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::MouseMoved) {
				sf::Vector2i localPos = sf::Mouse::getPosition(window);

				if (nameEntered) {
					//check if mouse is hovering over any of the team rectangles
					for (int i = 0; i < numTeams; i++) {
						float boxXPos = teamRectSpacing + (i * (teamRect.getSize().x + teamRectSpacing));
						float boxYPos = (window.getSize().y - teamRect.getSize().y) / 2;

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
				sf::Vector2i localPos = sf::Mouse::getPosition(window);

				if (nameEntered) {
					//check if any of the team rectangles have been clicked
					for (int i = 0; i < numTeams; i++) {
						float boxXPos = teamRectSpacing + (i * (teamRect.getSize().x + teamRectSpacing));
						float boxYPos = (window.getSize().y - teamRect.getSize().y) / 2;

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

		window.clear(sf::Color());

		window.draw(backSprite); //draw background

		if (nameEntered) {
			//draw title
			text.setCharacterSize(40);
			text.setString("Select your team");
			text.setPosition(sf::Vector2f((window.getSize().x - text.getLocalBounds().width) / 2,
				(window.getSize().y - text.getLocalBounds().height) / 16));
			window.draw(text);

			//draw the team rectangles
			text.setCharacterSize(30);
			for (int i = 0; i < numTeams; i++) {

				teamRect.setPosition(teamRectSpacing + (i * (teamRect.getSize().x + teamRectSpacing)), (window.getSize().y - teamRect.getSize().y) / 2);
				if (teamHighlights[teamNames[i]]) {
					//if mouse is hovering over the box, make it more opaque
					sf::Color oriColor = teamColors[teamNames[i]];
					teamRect.setFillColor(sf::Color(oriColor.r, oriColor.g, oriColor.b, 224));
				}
				else {
					//otherwise, color as normal
					teamRect.setFillColor(teamColors[teamNames[i]]);
				}

				window.draw(teamRect);

				text.setString("Team " + teamNames[i]);
				text.setPosition(int(teamRect.getPosition().x + (teamRect.getSize().x - text.getLocalBounds().width) / 2), teamRect.getPosition().y + teamRect.getSize().y);
				window.draw(text);
			}
		}
		else {
			window.draw(nameBar);
			if (!currentlyTyping) {
				nameText.setString("Enter your name");
			}
			else {
				nameText.setString(nameInputString);
			}
			nameText.setPosition((window.getSize().x - nameText.getLocalBounds().width) / 2, nameBar.getPosition().y + ((nameBar.getSize().y - nameText.getLocalBounds().height) / 2) );
			window.draw(nameText);
			if (currentlyTyping) {
				nameCursor.setPosition(nameText.getPosition().x + nameText.getLocalBounds().width + 2, nameBar.getPosition().y + ((nameBar.getSize().y - nameBar.getSize().y) / 2));
				float timeElapsed = clock.getElapsedTime().asMilliseconds() % 1000;
				//only show the cursor every halfsecond to achieve a blinking text cursor
				if (timeElapsed <= 500) {
					window.draw(nameCursor);
				}
			}
		}

		window.display();
	}

	return 0;
}

int song_selection(std::string reason) {
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
				create_profile();

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
		song_battle(&userProfile, &stage, &stage2);
	}

	//set mouse cursor to 0,0 so that we don't accidentally highlight a song right when we enter
	sf::Mouse::setPosition(sf::Vector2i(20, 20), window);

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
	downArrowLeft.setPoint(1, sf::Vector2f(startX+width,startYDown+dip));
	downArrowLeft.setPoint(2, sf::Vector2f(startX+width, startYDown+height+dip));
	downArrowLeft.setPoint(3, sf::Vector2f(startX, startYDown+height));
	downArrowLeft.setFillColor(sf::Color(128, 128, 128, 128));
	downArrowRight.setPointCount(4);
	downArrowRight.setPoint(0, sf::Vector2f(startX+width, startYDown+dip));
	downArrowRight.setPoint(1, sf::Vector2f(startX+width*2, startYDown));
	downArrowRight.setPoint(2, sf::Vector2f(startX+width*2, startYDown+height));
	downArrowRight.setPoint(3, sf::Vector2f(startX+width, startYDown+height+dip));
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

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::MouseMoved) {

				sf::Vector2i localPos = sf::Mouse::getPosition(window);

				bool enablePreview = false;
				for (int i = 0; i < stageList.size(); i++) {
					Stage * stage = &stageList.at(i);
					
					//don't consider highlighting songbox if
					//1. it's locked
					//2. it's above or below a certain transparency threshhold
					if ( !(*stage).getUnlocked() ||
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

							int playPoint = (previewMusic.getDuration().asMilliseconds())/4;

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

				if ( (localPos.x >= startX && localPos.x <= startX + width
					&& localPos.y >= yUpperLeft && localPos.y <= yLowerLeft) || //inside left side of the down arrow
					(localPos.x >= startX + width && localPos.x <= startX + 2*width
						&& localPos.y >= yUpperRight && localPos.y <= yLowerRight) ) {
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
				
				sf::Vector2i localPos = sf::Mouse::getPosition(window);

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
								if (song_battle(&userProfile, stage, &stageMap[(*stage).getPath()] ) > 0) {

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
								song_maker((*stage).getPath()); //enter stage battle
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
				window.close();
			}
		}

		//if currently previewing music
		if (previewMusic.getStatus() == 2) {
			int playPoint = (previewMusic.getDuration().asMilliseconds()) / 4;
			int curPlayPoint = previewMusic.getPlayingOffset().asMilliseconds();

			const int transDuration = 1000; //how long the transition (fade) takes
			const float startVolume = 50.0;
			float volPerTime = startVolume/transDuration; //in milliseconds
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
		window.clear(sf::Color::Black);

		window.draw(backSprite);
		window.draw(titleText);

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
				thumbBox.setOutlineColor (sf::Color(curColor.r, curColor.g, curColor.b, curColor.a * (1 - m)));
				sf::Color curTextColor = thumbText.getColor();
				thumbText.setColor(sf::Color(curTextColor.r, curTextColor.g, curTextColor.b, 255 * (1 - m)));
			}
			else if (pos.y < 600 && pos.y + 100 > posTolDown) { //don't transform on unseen songboxes
				//the +100 is because we want to know when the bottom of the box reaches 400, not the top
				float m = (pos.y - posTolDown) / (600-posTolDown) * 1.5; //1.5 to accelerate fade effect
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
			window.draw(thumbSprite);
			window.draw(thumbBox);

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
			window.draw(thumbText);

			thumbTextString = "Artist: " + (*stage).getArtist();
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5 + 12);
			window.draw(thumbText);

			thumbTextString = "Album: " + (*stage).getAlbum();
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5 + 24);
			window.draw(thumbText);

			thumbTextString = "Year: " + std::to_string((*stage).getYear());
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5 + 36);
			window.draw(thumbText);

			thumbTextString = "High Score: " + std::to_string((*stage).getHighScore());
			thumbText.setString(thumbTextString);
			thumbText.setPosition(int(boxPos.x + (boxWidth - thumbText.getLocalBounds().width) / 2), pos.y + 100 + 5 + 48);
			window.draw(thumbText);
		}

		//draw arrow buttons
		window.draw(upArrowLeft);
		window.draw(upArrowRight);
		window.draw(downArrowLeft);
		window.draw(downArrowRight);

		//draw hoverbox if hovering over a song
		if (enableHover) {
			window.draw(hoverBox);
		}

		window.display();
	}
	

	return 0;
}

int song_battle(Profile * profile, Stage * stage, Stage * mappedStage) {
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
		movie.setPosition(sf::Vector2f(0, (600 - movie.getSize().y)/2));
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

	while (window.isOpen()) {
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
					(*c).incExp(score/2); //TODO, write exp system

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

		while (window.pollEvent(event)) {
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
										effects.erase(effects.begin()+i);
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
				sf::Vector2i localPos = sf::Mouse::getPosition(window);

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
				sf::Vector2i localPos = sf::Mouse::getPosition(window);

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
				window.close();
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
		window.clear(sf::Color::Black);
		//draw movie if movie stage
		if (movieMode) {
			movie.update();
			window.draw(movie);
		}
		else {
			window.draw(backSprite); //background
		}

		//score text
		std::string scoreStr = std::to_string(score);
		sideText.setString(scoreStr); //update score
		int scoreXPos = 775 - 28 * scoreStr.length(); //calculate x position so that score shifts accordingly if increased
		sideText.setPosition(scoreXPos, 10);
		sideText.setCharacterSize(50);
		window.draw(sideText);

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
				window.draw(sideText);
				//team captain's name
				sideText.setString((*captain).getName());
				sideText.setPosition(625, 320);
				window.draw(sideText);
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
			window.draw(charSprite);
		}

			//pathways
		int colorTime = int(elapsed) % 8000;
		const int colorAdder = 70;
		float colorMultiplier = ((colorTime % 2000) * 1.0)/2000.0;
		float colorAdderResult = colorAdder;
		if (colorTime < 2000) {
			colorAdderResult *= colorMultiplier;
		}
		else if (colorTime >= 2000 && colorTime < 4000) {
			colorAdderResult *= (1-colorMultiplier);
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
			std::max(leftPathColor.g + colorAdderResult/2, 0.0f),
			std::max(leftPathColor.b + colorAdderResult/2, 0.0f), leftPathColor.a));
		middlePath.setFillColor(sf::Color(std::max(middlePathColor.r + colorAdderResult, 0.0f),
			std::max(middlePathColor.g + colorAdderResult/2, 0.0f),
			std::max(middlePathColor.b + colorAdderResult/2, 0.0f), middlePathColor.a));
		rightPath.setFillColor(sf::Color(std::max(rightPathColor.r + colorAdderResult, 0.0f),
			std::max(rightPathColor.g + colorAdderResult/2, 0.0f),
			std::max(rightPathColor.b + colorAdderResult/2, 0.0f), rightPathColor.a));
		window.draw(leftPath);
		window.draw(middlePath);
		window.draw(rightPath);
			//lower 3 buttons
		window.draw(leftCircle);
		window.draw(middleTriangle);
		window.draw(rightSquare);
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

			window.draw(*graphics);
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

			window.draw(*graphics);
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

			window.draw(*graphics);
		}
		window.draw(hitText);
		if (paused) {
			window.draw(pauseOverlay); //darkens the entire screen when paused
			window.draw(resumeButton); //resume button
			window.draw(resumeText); //resume text
			window.draw(returnButton); //return button
			window.draw(returnText); //return text
		}

		//end current frame
		window.display();
	}

	return 0;
}

int song_maker(std::string filePath) {
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

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
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
				window.close();
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
		window.clear(sf::Color::Black);

		window.draw(backSprite);
		window.draw(leftCircle);
		window.draw(middleTriangle);
		window.draw(rightSquare);

		window.display();
	}

	return 0;
}