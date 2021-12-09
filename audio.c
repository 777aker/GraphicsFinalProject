#include "audio.h"

// how to do audio @
// https://lazyfoo.net/tutorials/SDL/21_sound_effects_and_music/index.php


// day music
Mix_Music* dayMusic = NULL;
// night music
Mix_Music* nightMusic = NULL;

// initialize audio stuff
void initaudio() {
	// initialize SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		printf("SDL failed :'( %s\n", SDL_GetError());
		return;
	}
	// initialize SDL mixer
	// frequency 44100 standard, file format, stereo, samples
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_mixer no work %s\n", Mix_GetError());
	}
	loadMedia();
	//playday();
}

void playnight() {
	// if music is playing stop it
	// ie day is probably playing stop playing it
	if (Mix_PlayingMusic() != 0) {
		Mix_HaltMusic();
	}
	// play the night music
	// -1 means loop forever, any other value means play that many times
	Mix_PlayMusic(nightMusic, -1);
}

void playday() {
	// if music is playing stop it
	// ie night stop it
	if (Mix_PlayingMusic() != 0) {
		Mix_HaltMusic();
	}
	// play the day music
	// play that funky music
	Mix_PlayMusic(dayMusic, -1);
}

bool loadMedia() {
	// loading success flag
	bool success = true;

	dayMusic = Mix_LoadMUS("audio/Forest_Ambience.mp3");
	if (dayMusic == NULL) {
		printf("day music failed: %s\n", Mix_GetError());
		success = false;
	}

	nightMusic = Mix_LoadMUS("audio/forest.ogg");
	if (nightMusic == NULL) {
		printf("night music failed: %s\n", Mix_GetError());
		success = false;
	}

	return success;
}

void closeaudio() {
	// free the sound
	Mix_FreeMusic(dayMusic);
	Mix_FreeMusic(nightMusic);
	dayMusic = NULL;
	nightMusic = NULL;

	// destroy the SDL (awfully dramatic)
	Mix_Quit();
	SDL_Quit();
}
