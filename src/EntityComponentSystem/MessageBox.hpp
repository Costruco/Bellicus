#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "Components.hpp"
#include "Vector2D.hpp"
#include "ScreenAlignment.hpp"
#include "ResourceManager.hpp"
#include <string>
#include <vector>
#include <format>

class MessageBoxComponent : public Component {
    private:
        TransformComponent * transform;
        ResourceManager::FontPtr font;
        ScreenAlignment alignment;
        std::vector<std::string> lines;
        std::vector<std::vector<float*>> values;

    public:
        MessageBoxComponent() = default;
        MessageBoxComponent(const std::vector<std::string>& lines, const std::vector<std::vector<float*>>& values, 
            ScreenAlignment alignment = ScreenAlignment::CENTER) :
            lines(lines),
            values(values),
            alignment(alignment) {}
        
        void init() override {
            transform = &entity->getComponent<TransformComponent>();
            int maxWidth = 0, maxHeight = TTF_FontLineSkip(font.get());
            for (size_t i = 0; i < lines.size(); i++) {
                std::string line = lines[i];
                for (const auto& value : values[i]) {
                    line = line.replace(line.find("[]"),2,"{}");
                    line = std::vformat(line,std::make_format_args(*value));
                    int width;
                    TTF_SizeUTF8(font.get(),line.c_str(),&width,nullptr);
                    maxWidth = std::max(maxWidth,width);
                }
            }
            transform->width = maxWidth;
            transform->height = maxHeight*lines.size();
        }

        void draw() override {
            SDL_FRect dst = {transform->position.x,transform->position.y,transform->width,transform->height};
            ResourceManager::drawMessageBox(font,&dst,lines,{255,255,255,255},{0,0,0,255},{0,0,0,128});
        }
};