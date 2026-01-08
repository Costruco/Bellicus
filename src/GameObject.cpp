#include "GameObject.hpp"

GameObject::GameObject(SDL_Renderer * ren, const char * fileName, int x, int y) {
	this->ren = ren;
	texture = TextureManager::LoadTexture(ren, fileName);
	
	local = {x,y};
	srcRect = {0,0,203,104};
	dstRect = {x,y,203,104};
}

GameObject::~GameObject() {
	SDL_DestroyTexture(texture);
}

void GameObject::update() {
	local.x++;
	local.y++;
	dstRect.x = local.x;
	dstRect.y = local.y;
}

void GameObject::render() {
	SDL_RenderCopy(ren,texture,&srcRect,&dstRect);
}
