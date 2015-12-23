#pragma once
#include <fstream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sfeMovie/Movie.hpp>
#include <sfeMovie/StreamSelection.hpp>
#include <sfeMovie/Visibility.hpp>

#include "Profile.h"
#include "Stage.h"
#include "Beat.h"
#include "Effect.h"

int song_battle(sf::RenderWindow * w, Profile * profile, Stage * stg, Stage * mStg);