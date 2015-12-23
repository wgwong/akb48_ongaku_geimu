#pragma once
#include <iostream>

//individual character class
class Character {
public:
	Character();
	Character(std::string first, std::string last, std::string nick, std::string tm, std::string rnk, std::string pre, std::string suf, int lvl, int exp, bool u);

	//getters
	std::string getInfoPath();
	std::string getImagePath();
	std::string getName();
	std::string getFirstName();
	std::string getLastName();
	std::string getNickname();
	std::string getTeam();
	std::string getRank();
	std::string getPrefix();
	std::string getSuffix();
	int getLevel();
	int getExp();
	bool getUnlocked();
	std::string getStageSongPath();

	//setters
	void setInfoPath(std::string i);
	void setImagePath(std::string i);
	void setFirstName(std::string f);
	void setLastName(std::string l);
	void setNickname(std::string n);
	void setTeam(std::string t);
	void setRank(std::string r);
	void setPrefix(std::string p);
	void setSuffix(std::string s);
	void setLevel(int l);
	void setExp(int e);
	void incExp(int a);
	void levelUp();
	void setUnlocked(bool u);
	void setStageSongPath(std::string s);

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