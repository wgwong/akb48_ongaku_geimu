#include "Profile.h"

//contains information about the player / player profile & stats
Profile::Profile() {

}
Profile::Profile(std::string pn) {
	playerName = pn;
	money = 0.0;
}
Profile::Profile(std::string pn, Team tm) {
	playerName = pn;
	team = tm;
	money = 0.0;
}
Profile::Profile(std::string pn, Team tm, float m) {
	playerName = pn;
	team = tm;
	money = m;
}

//getters
std::string Profile::getPlayerName() {
	return playerName;
}
//returns the address of the team
Team * Profile::getTeam() {
	return &team;
}
float Profile::getMoney() {
	return money;
}

//setters
void Profile::setPlayerName(std::string pn) {
	playerName = pn;
}
//sets team pointer to the address of the parameter
void Profile::setTeam(Team t) {
	team = t;
}
void Profile::setMoney(float m) {
	money = m;
}
void Profile::incMoney(float m) {
	money += m;
}