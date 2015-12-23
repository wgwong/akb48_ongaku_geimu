#include "MusicStream.h"


void MusicStream::load(const sf::SoundBuffer& buffer) {
	// extract the audio samples from the sound buffer to our own container
	m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());

	// reset the current playing position 
	m_currentSample = 0;

	// initialize the base class
	initialize(buffer.getChannelCount(), buffer.getSampleRate());
}


bool MusicStream::onGetData(Chunk& data) {
	// number of samples to stream every time the function is called;
	// in a more robust implementation, it should be a fixed
	// amount of time rather than an arbitrary number of samples
	const int samplesToStream = 50000;

	// set the pointer to the next audio samples to be played
	data.samples = &m_samples[m_currentSample];

	// have we reached the end of the sound?
	if (m_currentSample + samplesToStream <= m_samples.size()) {
		// end not reached: stream the samples and continue
		data.sampleCount = samplesToStream;
		m_currentSample += samplesToStream;
		return true;
	}
	else {
		// end of stream reached: stream the remaining samples and stop playback
		data.sampleCount = m_samples.size() - m_currentSample;
		m_currentSample = m_samples.size();
		return false;
	}
}

void MusicStream::onSeek(sf::Time timeOffset) {
	// compute the corresponding sample index according to the sample rate and channel count
	m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
}
