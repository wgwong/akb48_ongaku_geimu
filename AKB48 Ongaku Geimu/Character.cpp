#include "Character.h"

//individual character class
Character::Character() {

}
Character::Character(std::string first, std::string last, std::string nick, std::string tm, std::string rnk, std::string pre, std::string suf, int lvl, int exp, bool u) {
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
std::string Character::getInfoPath() {
	return infoPath;
}
std::string Character::getImagePath() {
	return imagePath;
}
std::string Character::getName() { //japanese naming convention
	return lastName + " " + firstName;
}
std::string Character::getFirstName() {
	return firstName;
}
std::string Character::getLastName() {
	return lastName;
}
std::string Character::getNickname() {
	return nickname;
}
std::string Character::getTeam() {
	return team;
}
std::string Character::getRank() {
	return rank;
}
std::string Character::getPrefix() {
	return prefix;
}
std::string Character::getSuffix() {
	return suffix;
}
int Character::getLevel() {
	return level;
}
int Character::getExp() {
	return experience;
}
bool Character::getUnlocked() {
	return unlocked;
}
std::string Character::getStageSongPath() {
	return stageSongPath;
}

//setters
void Character::setInfoPath(std::string i) {
	infoPath = i;
}
void Character::setImagePath(std::string i) {
	imagePath = i;
}
void Character::setFirstName(std::string f) {
	firstName = f;
}
void Character::setLastName(std::string l) {
	lastName = l;
}
void Character::setNickname(std::string n) {
	nickname = n;
}
void Character::setTeam(std::string t) {
	team = t;
}
void Character::setRank(std::string r) {
	rank = r;
}
void Character::setPrefix(std::string p) {
	prefix = p;
}
void Character::setSuffix(std::string s) {
	suffix = s;
}
void Character::setLevel(int l) {
	level = l;
}
void Character::setExp(int e) {
	experience = e;
}
void Character::incExp(int a) {
	experience += a;
}
void Character::levelUp() {
	level += 1;
	experience = 0;
	//do checks for unlocking stuff when level up
}
void Character::setUnlocked(bool u) {
	unlocked = u;
}
void Character::setStageSongPath(std::string s) {
	stageSongPath = s;
}