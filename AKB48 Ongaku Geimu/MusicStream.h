#pragma once
#include <vector>
#include <SFML\Audio.hpp>

class MusicStream : public sf::SoundStream {
public:
	void load(const sf::SoundBuffer& buffer);

private:
	virtual bool onGetData(Chunk& data);

	virtual void onSeek(sf::Time timeOffset);

	std::vector<sf::Int16> m_samples;
	std::size_t m_currentSample;
};