#include <SFX/AudioSystem.hpp>
#include <raylib.h>

namespace Ho_tones {

static int g_sampleRate = 44100;
static int g_channels = 2;

bool InitAudioSystem(int sampleRate, int channels) {
    g_sampleRate = sampleRate > 0 ? sampleRate : 44100;
    g_channels = channels > 0 ? channels : 2;

    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }

    SetMasterVolume(1.0f);
    return IsAudioDeviceReady();
}

void ShutdownAudioSystem() {
    // Stop any queued sounds via the global SoundBus before closing audio device.
    GetSoundBus().StopAll();
    if (IsAudioDeviceReady()) CloseAudioDevice();
}

SoundBus& GetSoundBus() {
    static SoundBus bus;
    return bus;
}

int GetAudioSampleRate() { return g_sampleRate; }
int GetAudioChannels() { return g_channels; }

} // namespace Ho_tones
