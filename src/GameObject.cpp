#include "GameObject.hpp"

GameObject::GameObject(const char * fileName, int x, int y) {
	texture = TextureManager::loadTexture(fileName);
	
	local = {x,y};
	src = {0,0,203,104};
	dst = {x,y,203,104};
}

GameObject::~GameObject() {
	SDL_DestroyTexture(texture);
}

void GameObject::update() {
	local.x++;
	local.y++;
	dst.x = local.x;
	dst.y = local.y;
}

void GameObject::render() {
	SDL_RenderCopy(Game::ren,texture,&src,&dst);
}
