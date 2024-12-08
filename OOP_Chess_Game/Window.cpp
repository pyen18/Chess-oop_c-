#include "Window.h"

// init static attribute
SDL_Window* Window::window = nullptr;
SDL_Renderer* Window::renderer = nullptr;
int Window::SCREEN_HEIGHT;
int Window::SCREEN_WIDTH;