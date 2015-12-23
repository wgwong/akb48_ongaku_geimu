#pragma once
#include <iostream>

#include "Team.h"

//contains information about the player / player profile & stats
class Profile {
public:
	Profile();
	Profile(std::string pn);
	Profile(std::string pn, Team tm);
	Profile(std::string pn, Team tm, float m);

	//getters
	std::string getPlayerName();
	//returns the address of the team
	Team *getTeam();
	float getMoney();

	//setters
	void setPlayerName(std::string pn);
	//sets team pointer to the address of the parameter
	void setTeam(Team t);
	void setMoney(float m);
	void incMoney(float m);

private:
	std::string playerName;
	Team team;
	float money;
};