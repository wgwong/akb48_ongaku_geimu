#pragma once
#include <SFML/Graphics.hpp>

class Beat {
public:
	/*
	Constructor
	time must be in milliseconds
	*/
	Beat(std::string key, float time);
	Beat(std::string key, float time, sf::CircleShape shape);

	//getters
	float getTime();
	std::string getKey();
	sf::CircleShape getGraphics();
	sf::CircleShape * getGraphicsObject();

	sf::Vector2f getPos();

	//setters
	void setTime(float t);
	void setKey(std::string k);
	void setX(float newX);
	void setY(float newY);

	std::string toString();

private:
	std::string keyHit; //the key that was pressed
	float timeLocation; //time in which the beat was hit
	float xPos;
	float yPos; //makeshift solution: can't modify shapeGraphics member field's position permanently
	sf::CircleShape shapeGraphics; //graphics to draw
};