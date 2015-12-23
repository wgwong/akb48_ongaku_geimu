#include "Beat.h"

/*
Constructor
time must be in milliseconds
*/
Beat::Beat(std::string key, float time) {
	keyHit = key;
	timeLocation = time;
	shapeGraphics = sf::CircleShape(50);
	xPos = 0.0;
	yPos = 0.0;
}
Beat::Beat(std::string key, float time, sf::CircleShape shape) {
	keyHit = key;
	timeLocation = time;
	shapeGraphics = shape;
	xPos = shape.getPosition().x;
	yPos = shape.getPosition().y;
}

//getters
float Beat::getTime() {
	return timeLocation;
}
std::string Beat::getKey() {
	return keyHit;
}
sf::CircleShape Beat::getGraphics() {
	return shapeGraphics;
}
sf::CircleShape * Beat::getGraphicsObject() {
	return &shapeGraphics;
}

sf::Vector2f Beat::getPos() {
	return sf::Vector2f(xPos, yPos);
}

//setters
void Beat::setTime(float t) {
	timeLocation = t;
}
void Beat::setKey(std::string k) {
	keyHit = k;
}
void Beat::setX(float newX) {
	xPos = newX;
}
void Beat::setY(float newY) {
	yPos = newY;
}

std::string Beat::toString() {
	return keyHit + " @ " + std::to_string(timeLocation / 1000) + "s @ position ("
		+ std::to_string(xPos) + ", " + std::to_string(yPos) + ")\n";
}