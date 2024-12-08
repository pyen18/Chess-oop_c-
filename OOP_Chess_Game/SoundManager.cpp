#include "SoundManager.h"

SoundManager::SoundManager() {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		throw string("Cannot initialize audio!");
	}

	//Mix_VolumeMusic(10);
	Mix_Volume(-1, 128);
	Mix_VolumeMusic(50);
	// Load background musics
	bgMenu = Mix_LoadMUS("../Assets/Sounds/bgMenu.mp3");
	bgPvP = Mix_LoadMUS("../Assets/Sounds/PvP.mp3");
	bgPvE_easy = Mix_LoadMUS("../Assets/Sounds/PvE_Easy.mp3");
	bgPvE_hard = Mix_LoadMUS("../Assets/Sounds/PvE_Hard.mp3");
	bgReplay = Mix_LoadMUS("../Assets/Sounds/replay.mp3");
	
	// Load sound effects
	efMove = Mix_LoadWAV("../Assets/Sounds/move.wav");
	efCapture = Mix_LoadWAV("../Assets/Sounds/capture.wav");
	efClickBtn = Mix_LoadWAV("../Assets/Sounds/click.wav");
	efWin = Mix_LoadWAV("../Assets/Sounds/win.wav");
	efDraw = Mix_LoadWAV("../Assets/Sounds/draw.wav");

}

SoundManager::~SoundManager() {
	Mix_HaltMusic();
	Mix_FreeChunk(efMove);
	Mix_FreeChunk(efCapture);
	Mix_FreeChunk(efClickBtn);
	Mix_FreeChunk(efWin);
	Mix_FreeChunk(efDraw);
	Mix_FreeMusic(bgMenu);
	Mix_FreeMusic(bgPvP);
	Mix_FreeMusic(bgPvE_hard);
	Mix_FreeMusic(bgPvE_easy);
	Mix_FreeMusic(bgReplay);
	Mix_CloseAudio();
}

bool SoundManager::getIsMuted() const {
	return isMuted;
}

void SoundManager::setThemeMusicVolume(int themeMusicVolume) {
}

void SoundManager::setEventMusicVolume(int eventMusicVolume) {
}

int SoundManager::getThemeMusicVolume() const {
	return 0;
}

int SoundManager::getEventMusicVolume() const {
	return 0;
}

void SoundManager::playMainMenuMusic() {
	Mix_HaltMusic();
	Mix_PlayMusic(bgMenu, -1);
	playing = bgMenu;
	if (isMuted) Mix_PauseMusic();
}

void SoundManager::playPvPMusic() {
	Mix_HaltMusic();
	Mix_PlayMusic(bgPvP, -1);
	playing = bgPvP;
	if (isMuted) Mix_PauseMusic();
}

void SoundManager::playPvEHardMusic() {
	Mix_HaltMusic();
	Mix_PlayMusic(bgPvE_hard, -1);
	playing = bgPvE_hard;
	if (isMuted) Mix_PauseMusic();
}

void SoundManager::playPvEEasyMusic() {
	Mix_HaltMusic();
	Mix_PlayMusic(bgPvE_easy, -1);
	playing = bgPvE_easy;
	if (isMuted) Mix_PauseMusic();
}

void SoundManager::playReplayMusic() {
	Mix_HaltMusic();
	Mix_PlayMusic(bgReplay, -1);
	playing = bgReplay;
	if (isMuted) Mix_PauseMusic();
}

void SoundManager::playWinSound() {
	if (!isMuted) Mix_PlayChannel(-1, efWin, 0);
}

void SoundManager::playDrawSound() {
	if (!isMuted) Mix_PlayChannel(-1, efDraw, 0);
}

void SoundManager::playPieceMoveSound() {
	if (!isMuted) Mix_PlayChannel(-1, efMove, 0);
}

void SoundManager::playCapturingSound() {
	if (!isMuted) Mix_PlayChannel(-1, efCapture, 0);
}

void SoundManager::playClickBtnSound() {
	if (!isMuted) Mix_PlayChannel(-1, efClickBtn, 0);
}

void SoundManager::muteXunmute() {
	if (isMuted) {
		Mix_Resume(-1);
		if (Mix_PlayingMusic) Mix_ResumeMusic();
		else Mix_PlayMusic(playing, -1);
	}
	else {
		Mix_Pause(-1);
		Mix_PauseMusic();
	}
	isMuted = !isMuted;
}

