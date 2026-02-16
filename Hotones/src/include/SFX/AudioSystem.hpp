#pragma once
#include <cstdint>
#include <SoundBus.hpp>

namespace Ho_tones {

// Initialize the audio subsystem. Calls raylib `InitAudioDevice()` and configures
// a default sample rate and channel count that other systems can query.
bool InitAudioSystem(int sampleRate = 44100, int channels = 2);
void ShutdownAudioSystem();

// Access the global SoundBus instance for playing sounds.
SoundBus& GetSoundBus();

int GetAudioSampleRate();
int GetAudioChannels();

} // namespace Ho_tones
