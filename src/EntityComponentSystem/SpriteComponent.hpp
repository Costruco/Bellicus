#pragma once

#include <SDL2/SDL.h>

#include <algorithm>
#include <map>
#include <string>

#include "Animation.hpp"
#include "Components.hpp"
#include "ResourceManager.hpp"

class SpriteComponent : public Component {
	private:
		TransformComponent * transform = nullptr;
		ResourceManager::TexturePtr texture;
		SDL_Rect src{};
		SDL_FRect dst{};
		SDL_FPoint texture_center{};
		
		bool animated = false;
		bool linear = false;
		bool loop = true;
		int frames = 1;
		int frameDelay = 100;
		int columns = 0;
		Uint32 animStartTime = 0;
	
	public:
		int animIndex = 0;
		std::map<std::string,Animation> animations;
		
		SpriteComponent() = default;
		SpriteComponent(const std::string& path) {
			setTexture(path);
		}
		SpriteComponent(const ResourceManager::TexturePtr& tex) {
			setTexture(tex);
		}
		SpriteComponent(const std::string& path, bool isAnimated, bool isLinear = false) {
			setTexture(path, isLinear);
			if (animated = isAnimated) {
				Animation idle = Animation(0,2,500);
				Animation walk = Animation(1,2,500);
				
				animations.emplace("idle",idle);
				animations.emplace("walk",walk);
				
				play("idle");
			}
		}
		SpriteComponent(const ResourceManager::TexturePtr& tex, int nFrames, int delay, bool shouldLoop = true, int nColumns = 0) {
			setTexture(tex);
			if (nFrames > 1) {
				animated = true;
				animations.emplace("default", Animation(0, nFrames, std::max(1, delay), shouldLoop, nColumns));
				play("default");
			}
		}
		SpriteComponent(const std::string& path, int nFrames, int delay, bool shouldLoop = true, int nColumns = 0, bool isLinear = false) {
			setTexture(path, isLinear);
			if (nFrames > 1) {
				animated = true;
				animations.emplace("default", Animation(0, nFrames, std::max(1, delay), shouldLoop, nColumns));
				play("default");
			}
		}
		~SpriteComponent() {
		}
		
		void setTexture(const std::string& path, bool isLinear = false) {
			linear = isLinear;
			texture = ResourceManager::loadTexture(path,isLinear);
		}

		void setTexture(const ResourceManager::TexturePtr& tex) {
			texture = tex;
		}

		bool isLinear() const {
			return linear;
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
			if ((transform->width == 0.0f || transform->height == 0.0f) && texture) {
				int w = 0, h = 0;
				SDL_QueryTexture(texture.get(), nullptr, nullptr, &w, &h);
				int cols = (animated && columns > 0) ? columns : ((animated && frames > 0) ? frames : 1);
				int rows = (animated && columns > 0 && frames > 0) ? ((frames + columns - 1) / columns) : 1;
				if (transform->width == 0.0f)
					transform->width = static_cast<float>(w / std::max(1, cols));
				if (transform->height == 0.0f)
					transform->height = static_cast<float>(h / std::max(1, rows));
			}
			src = {0,0,static_cast<int>(transform->width),static_cast<int>(transform->height)};
		}
		
		void update() override {
			src.w = static_cast<int>(transform->width);
			src.h = static_cast<int>(transform->height);
			if (animated && frames > 0 && frameDelay > 0) {
				Uint32 elapsed = SDL_GetTicks() - animStartTime;
				int currentFrame = static_cast<int>(elapsed / static_cast<Uint32>(frameDelay));
				if (loop) {
					currentFrame %= frames;
				} else {
					currentFrame = std::min(currentFrame, frames - 1);
				}
				int cols = (columns > 0) ? columns : frames;
				int col = currentFrame % cols;
				int row = animIndex + (currentFrame / cols);
				src.x = src.w * col;
				src.y = src.h * row;
			} else {
				src.x = 0;
				src.y = animIndex * src.h;
			}
		}
		
		void draw() override {
			texture_center = {(transform->width/2.0f+transform->center_offset.x)*transform->getScale(),
							  (transform->height/2.0f+transform->center_offset.y)*transform->getScale()};
			Vector2D pos = transform->getPosition();
			dst = {pos.x-texture_center.x,
				   pos.y-texture_center.y,
				   transform->width*transform->getScale(),
				   transform->height*transform->getScale()};
			ResourceManager::drawTexture(texture,&src,&dst,transform->getDirection(),&texture_center);
		}
		
		void play(const std::string& animName) {
			if (!animated) return;
			auto it = animations.find(animName);
			if (it == animations.end()) return;
			animIndex = it->second.index;
			frames = it->second.frames;
			frameDelay = it->second.frameDelay;
			loop = it->second.loop;
			columns = it->second.columns;
			animStartTime = SDL_GetTicks();
		}
};