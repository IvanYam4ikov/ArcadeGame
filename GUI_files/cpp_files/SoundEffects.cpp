#include "SoundEffects.h"

#include "SDL_mixer.h"

#include <cmath>
#include <vector>

namespace {
const int sampleRate = 44100;
const double pi = 3.14159265358979323846;
bool enabled = false;

std::vector<Sint16> clickSamples;
std::vector<Sint16> brickSamples;
std::vector<Sint16> wallSamples;
std::vector<Sint16> paddleSamples;
Mix_Chunk* clickChunk = nullptr;
Mix_Chunk* brickChunk = nullptr;
Mix_Chunk* wallChunk = nullptr;
Mix_Chunk* paddleChunk = nullptr;

Mix_Chunk* makeTone(std::vector<Sint16>& samples, double startFrequency,
                    double endFrequency, int durationMs, double volume)
{
    const int frameCount = sampleRate * durationMs / 1000;
    samples.resize(frameCount * 2);
    double phase = 0.0;
    for (int frame = 0; frame < frameCount; ++frame) {
        const double progress = static_cast<double>(frame) / frameCount;
        const double frequency = startFrequency + (endFrequency - startFrequency) * progress;
        const double envelope = (1.0 - progress) * (1.0 - progress);
        phase += 2.0 * pi * frequency / sampleRate;
        const Sint16 value = static_cast<Sint16>(std::sin(phase) * 32767.0 * volume * envelope);
        samples[frame * 2] = value;
        samples[frame * 2 + 1] = value;
    }
    return Mix_QuickLoad_RAW(reinterpret_cast<Uint8*>(samples.data()),
        static_cast<Uint32>(samples.size() * sizeof(Sint16)));
}

void play(Mix_Chunk* chunk)
{
    if (enabled && chunk) Mix_PlayChannel(-1, chunk, 0);
}
}

namespace SoundEffects {
bool initialize()
{
    clickChunk = makeTone(clickSamples, 900.0, 650.0, 45, 0.18);
    brickChunk = makeTone(brickSamples, 520.0, 1050.0, 75, 0.25);
    wallChunk = makeTone(wallSamples, 240.0, 180.0, 45, 0.18);
    paddleChunk = makeTone(paddleSamples, 330.0, 520.0, 65, 0.22);
    return clickChunk && brickChunk && wallChunk && paddleChunk;
}

void shutdown()
{
    Mix_HaltChannel(-1);
    Mix_FreeChunk(clickChunk);
    Mix_FreeChunk(brickChunk);
    Mix_FreeChunk(wallChunk);
    Mix_FreeChunk(paddleChunk);
    clickChunk = brickChunk = wallChunk = paddleChunk = nullptr;
    clickSamples.clear();
    brickSamples.clear();
    wallSamples.clear();
    paddleSamples.clear();
}

void setEnabled(bool value) { enabled = value; }
bool isEnabled() { return enabled; }
void playClick() { play(clickChunk); }
void playBrick() { play(brickChunk); }
void playWall() { play(wallChunk); }
void playPaddle() { play(paddleChunk); }
}
