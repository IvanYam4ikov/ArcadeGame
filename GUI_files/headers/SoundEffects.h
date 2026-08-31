#ifndef SOUND_EFFECTS_H
#define SOUND_EFFECTS_H

namespace SoundEffects {
bool initialize();
void shutdown();
void setEnabled(bool enabled);
bool isEnabled();
void playClick();
void playBrick();
void playWall();
void playPaddle();
}

#endif
