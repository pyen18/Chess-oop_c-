#pragma once
#include <SDL.h>

/// <summary>
/// class create window and render for any class using.
/// </summary>
class Window {
public:	
	// control window
	static int SCREEN_HEIGHT;
	static int SCREEN_WIDTH;
	static SDL_Window* window;
	static SDL_Renderer* renderer;
};
