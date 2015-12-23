#include "Effect.h"

Effect::Effect(float start, float dur, sf::Shape * o, std::string what, std::string desc, std::string base) {
	expired = false;
	startTime = start;
	duration = dur;
	obj = o;
	whatEffect = what;
	effectDesc = desc;
	baseValues = base;
}
Effect::Effect(float start, float dur, sf::Text * t, std::string what, std::string desc, std::string base) {
	expired = false;
	startTime = start;
	duration = dur;
	tObj = t;
	whatEffect = what;
	effectDesc = desc;
	baseValues = base;
}

//check effect in progress, return true if an effect is already modifying the parameter
bool Effect::checkEIP(sf::Shape * s) {
	return obj == s;
}
bool Effect::checkEIP(sf::Text * t) {
	return tObj == t;
}

bool Effect::getExpired() {
	return expired;
}

float Effect::getDuration() {
	return duration;
}

//get the time delta between now & start time of effect
float Effect::getTimeDelta(float now) {
	return now - startTime;
}

void Effect::expire() {
	expired = true;
}

//accepts time (in milliseconds) and calculates the current transition of the effect through a time delta
//now must be less than duration (calculate this externally beforehand before calling this function) or weird stuff happens
void Effect::transition(float now) {
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

//m - multiplier between 0.0 and 1.0
void Effect::doEffect(float m) {
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
	}
	else if (whatEffect == "fill color") {
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