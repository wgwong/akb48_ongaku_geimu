#include "util.h"

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