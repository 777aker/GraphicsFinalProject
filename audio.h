#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>

// I guess do the audio
void initaudio();
// load sounds
bool loadMedia();
// close the stuff
void closeaudio();
// play our sound effects
void playnight();
void playday();

#endif // !AUDIO_H
