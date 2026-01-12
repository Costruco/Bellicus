#include "GameObject.hpp"

GameObject::GameObject(const char * fileName, int x, int y) {
	texture = TextureManager::loadTexture(fileName);
	
	xpos = x;
	ypos = y;
	
	src = {0,0,203,104};
	dst = {x,y,203,104};
}

GameObject::~GameObject() {
	SDL_DestroyTexture(texture);
}

void GameObject::update() {
	xpos++;
	ypos++;
	dst.x = xpos;
	dst.y = ypos;
}

void GameObject::draw() {
	SDL_RenderCopy(Game::ren,texture,&src,&dst);
}
