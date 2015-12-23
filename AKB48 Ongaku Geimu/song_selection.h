#pragma once
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>

#include <SFML/Audio.hpp>

#include "Character.h"
#include "Profile.h"
#include "Stage.h"
#include "create_profile.h"

int song_selection(sf::RenderWindow * w, std::string reason);