#pragma once
#include <iostream>
#include <vector>
#include <deque>

//const std::string& s, char delim, std::vector<std::string>& v
void split(const std::string& s, char delim, std::vector<std::string>& v);

//helper function, returns a list of the initial members of a team
std::deque<std::string> getInitialTeam(std::string name);