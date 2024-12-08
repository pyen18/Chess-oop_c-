#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "Window.h"
#include "Pieces.h"

class Image {
private:
	SDL_Rect rect;
	SDL_Texture* texture;
	//SDL_Surface* img;
public:
	Image();
	Image(SDL_Rect rect, const std::string& imagePath);
	~Image();
	SDL_Rect getRectangle();
	void setRectangle(SDL_Rect rect);
	void makeBlend(int opacity);
	void setTexture(SDL_Texture* t);
	SDL_Texture* getTexture();
	void renderImage();
	void destroy();
};


