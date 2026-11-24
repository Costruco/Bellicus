#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <format>
#include <string>
#include <vector>

#include "Components.hpp"
#include "ResourceManager.hpp"
#include "ScreenAlignment.hpp"
#include "Utils/String.hpp"
#include "Utils/Vector2D.hpp"

class MessageBoxComponent : public Component {
    private:
        TransformComponent * transform = nullptr;
        ResourceManager::FontPtr font;
        ScreenAlignment alignment = ScreenAlignment::CENTER;
        std::vector<std::string> lines;
        std::vector<std::vector<float>> values;
        std::vector<std::vector<SDL_Color>> textColors;
        SDL_Color textColor = {255,255,255,255};
        SDL_Color borderColor = {0,0,0,255};
        SDL_Color backgroundColor = {0,0,0,128};
        int padding = 2;
        int lineSpacing = 3;
        float scale = 1.0f;

    public:
        MessageBoxComponent() = default;
        MessageBoxComponent(const std::string& path, int fontSize,const std::vector<std::string>& lines,
            ScreenAlignment alignment = ScreenAlignment::CENTER, float scale = 1.0f) :
            lines(lines),
            alignment(alignment),
            scale(scale) {
                setFont(path,fontSize);
            }
        
        void setColor(SDL_Color text, SDL_Color border, SDL_Color background) {
            textColor = text;
            borderColor = border;
            backgroundColor = background;
        }

        void setTextColors(const std::vector<std::vector<SDL_Color>>& textColors) {
            this->textColors = textColors;
        }

        void setScale(float scale) {
            this->scale = scale;
            if (transform)
                transform->scale = scale;
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
            if (scale != 1.0f)
                transform->scale = scale;
            int maxWidth = 0, maxHeight = TTF_FontLineSkip(font.get());
            for (size_t i = 0; i < lines.size(); i++) {
                std::string line = lines[i];
                safeReplace(line,"/center");
                safeReplace(line,"/right");
                while (line.find("[") != std::string::npos && line.find("]") != std::string::npos) {
                    safeReplace(line,"[","{");
                    safeReplace(line,"]","}");
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
                if (i < values.size()) {
                    for (const auto& value : values[i]) {
                        safeReplace(line,"[","{");
                        safeReplace(line,"]","}");
                        line = std::vformat(line,std::make_format_args(value));
                    }
                }
                linesCopy.push_back(line);
            }
            Vector2D pos = transform->getPosition();
            SDL_FRect dst = {pos.x,pos.y,transform->width,transform->height};
            ResourceManager::drawMessageBox(font,&dst,alignment,padding,lineSpacing,linesCopy,
                textColor,borderColor,backgroundColor,transform->getScale(),textColors);
        }
};