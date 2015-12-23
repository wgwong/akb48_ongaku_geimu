#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "util.h"

class Effect {
public:
	Effect(float start, float dur, sf::Shape * o, std::string what, std::string desc, std::string base);
	Effect(float start, float dur, sf::Text * t, std::string what, std::string desc, std::string base);

	//check effect in progress, return true if an effect is already modifying the parameter
	bool checkEIP(sf::Shape * s);
	bool checkEIP(sf::Text * t);

	bool getExpired();

	float getDuration();

	//get the time delta between now & start time of effect
	float getTimeDelta(float now);

	void expire();

	//accepts time (in milliseconds) and calculates the current transition of the effect through a time delta
	//now must be less than duration (calculate this externally beforehand before calling this function) or weird stuff happens
	void transition(float now);

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
	void doEffect(float m);
};