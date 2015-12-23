#pragma once
#include <iostream>
#include <deque>
#include <unordered_map>

#include "Character.h"

//a team contains many character classes
class Team {
public:
	Team();
	//getters
	std::string getTeamName();
	std::deque<Character> * getActiveTeam();
	std::deque<Character> * getInactiveTeam();
	std::unordered_map<std::string, Character> * getInactiveTeamList();
	Character * getCaptain();

	//setters
	void setTeamName(std::string tn);
	void setActiveTeam(std::deque<Character> aTeam);
	void addActiveChar(Character c);
	void addActiveChar(int i, Character c);
	void addInactiveChar(Character c);
	void setCaptain(Character c);

private:
	std::string teamName;
	std::deque<Character> activeTeam; //the characters that are used during stage battles
	std::deque<Character> inactiveTeam; //all the other characters not used during stage battles
	std::unordered_map<std::string, Character> inactiveTeamList;
	Character captain; //pointer to the captain of the team
	Character center; //IDK if I will use this
};