#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <format>
#include <string>
#include <vector>

#include "Components.hpp"
#include "ResourceManager.hpp"
#include "ScreenAlignment.hpp"
#include "Vector2D.hpp"

class MessageBoxComponent : public Component {
    private:
        TransformComponent * transform;
        ResourceManager::FontPtr font;
        ScreenAlignment alignment;
        std::vector<std::string> lines;
        std::vector<std::vector<float>> values;
        SDL_Color textColor = {255,255,255,255};
        SDL_Color borderColor = {0,0,0,255};
        SDL_Color backgroundColor = {0,0,0,128};
        int padding = 2;
        int lineSpacing = 3;

    public:
        MessageBoxComponent() = default;
        MessageBoxComponent(const std::string& path, int fontSize,const std::vector<std::string>& lines,
            ScreenAlignment alignment = ScreenAlignment::CENTER) :
            lines(lines),
            alignment(alignment) {
                setFont(path,fontSize);
            }
        
        void setColor(SDL_Color text, SDL_Color border, SDL_Color background) {
            textColor = text;
            borderColor = border;
            backgroundColor = background;
        }

        void setValues(const std::vector<std::vector<float>>& values) {
            this->values = values;
        }

        void setFont(const std::string& path, int fontSize) {
            font = ResourceManager::loadFont(path, fontSize);
        }

        void init() override {
            if (!entity->hasComponent<TransformComponent>())
                entity->addComponent<TransformComponent>();
            transform = &entity->getComponent<TransformComponent>();
            int maxWidth = 0, maxHeight = TTF_FontLineSkip(font.get());
            for (size_t i = 0; i < lines.size(); i++) {
                std::string line = lines[i];
                if (line.find("/center") != std::string::npos)
                    line.replace(line.find("/center"),7,"");
                if (line.find("/right") != std::string::npos)
                    line.replace(line.find("/right"),6,"");
                while (line.find("[") != std::string::npos && line.find("]") != std::string::npos) {
                    line = line.replace(line.find("["),1,"{");
                    line = line.replace(line.find("]"),1,"}");
                    float aux = 1.0f;
                    line = std::vformat(line,std::make_format_args(aux));
                }
                int width;
                TTF_SizeUTF8(font.get(),line.c_str(),&width,nullptr);
                maxWidth = std::max(maxWidth,width);
            }
            transform->width = maxWidth;
            transform->height = maxHeight*lines.size();
        }

        void draw() override {
            std::vector<std::string> linesCopy;
            for (size_t i = 0; i < lines.size(); i++) {
                std::string line = lines[i];
                for (const auto& value : values[i]) {
                    line = line.replace(line.find("["),1,"{");
                    line = line.replace(line.find("]"),1,"}");
                    line = std::vformat(line,std::make_format_args(value));
                }
                linesCopy.push_back(line);
            }
            SDL_FRect dst = {transform->position.x,transform->position.y,transform->width,transform->height};
            ResourceManager::drawMessageBox(font,&dst,alignment,padding,lineSpacing,linesCopy,
                textColor,borderColor,backgroundColor);
        }
};