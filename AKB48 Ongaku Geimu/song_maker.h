#pragma once
#include <iostream>
#include <deque>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Beat.h"
#include "Effect.h"

int song_maker(sf::RenderWindow * w, std::string name);