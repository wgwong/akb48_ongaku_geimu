#include "Team.h"

//a team contains many character classes
Team::Team() {

}
//getters
std::string Team::getTeamName() {
	return teamName;
}
std::deque<Character> * Team::getActiveTeam() {
	return &activeTeam;
}
std::deque<Character> * Team::getInactiveTeam() {
	return &inactiveTeam;
}
std::unordered_map<std::string, Character> * Team::getInactiveTeamList() {
	return &inactiveTeamList;
}
Character * Team::getCaptain() {
	return &captain;
}

//setters
void Team::setTeamName(std::string tn) {
	teamName = tn;
}
void Team::setActiveTeam(std::deque<Character> aTeam) {
	activeTeam = aTeam;
}
void Team::addActiveChar(Character c) {
	activeTeam.push_back(c);
}
void Team::addActiveChar(int i, Character c) {
	activeTeam[i] = c;
}
void Team::addInactiveChar(Character c) {
	inactiveTeam.push_back(c);
	inactiveTeamList[c.getName()] = c;
}
void Team::setCaptain(Character c) {
	captain = c;
}