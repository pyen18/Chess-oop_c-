#include "Image.h"

Image::Image() {
	this->rect = { 0,0,0,0 };
	this->texture = nullptr;
	//this->img = nullptr;
}
Image::Image(SDL_Rect rect, const std::string& imagePath) {
	this->rect = rect;
	this->texture = nullptr;
	//
	SDL_Surface* img = IMG_Load(imagePath.c_str());
	if (!img) {
		std::cout << "can't load\n";
		return;
	}
	this->texture = SDL_CreateTextureFromSurface(Window::renderer, img);
	SDL_FreeSurface(img);
}
Image::~Image() {
	//SDL_FreeSurface(this->img);
	//this->img = nullptr;
	SDL_DestroyTexture(this->texture);
	this->texture = nullptr;
}
SDL_Rect Image::getRectangle() {
	return this->rect;
}
void Image::setRectangle(SDL_Rect rect) {
	this->rect = rect;
}
void Image::renderImage() {
	if (!texture || !(&rect) || !Window::renderer) std::cout << "ERROR\n";
	SDL_RenderCopy(Window::renderer, this->texture, nullptr, &this->rect);
}
void Image::setTexture(SDL_Texture* t) {
	SDL_DestroyTexture(this->texture);
	this->texture = nullptr;
	this->texture = t;
}
SDL_Texture* Image::getTexture() {
	return this->texture;
}
void Image::makeBlend(int opacity) {
	SDL_SetTextureAlphaMod(this->texture, opacity);
}
void Image::destroy() {
	//SDL_FreeSurface(this->img);
	//this->img = nullptr;
	SDL_DestroyTexture(this->texture);
	this->texture = nullptr;
}
