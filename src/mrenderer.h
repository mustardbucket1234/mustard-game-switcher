#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

namespace Mustard
{

    struct TextureData
    {
        std::string filename;
        SDL_Surface *surface;
        SDL_Texture *texture;
        int width;
        int height;
        double aspect;
    };

    class Renderer
    {
    public:
        std::unordered_map<std::string, TextureData> textureMap;
        SDL_Renderer *renderer;

        Renderer(SDL_Renderer *renderer) : renderer(renderer) {}

        ~Renderer()
        {
            // Release textures and clean up resources if needed
            for (auto &pair : textureMap)
            {
                SDL_DestroyTexture(pair.second.texture);
            }
            textureMap.clear();
        }

        SDL_Texture *get(const std::string &filename)
        {
            TextureData *data = getData(filename);
            if (data)
            {
                return data->texture;
            }
            return nullptr;
        }

        TextureData* getData(const std::string &filename)
        {
            // Check if texture is already cached
            if (textureMap.find(filename) != textureMap.end())
            {
                return &textureMap[filename];
            }
        
            // Load surface and texture
            SDL_Surface *surface = IMG_Load(filename.c_str());
            if (!surface)
            {
                std::cerr << "Failed to load surface: " << SDL_GetError() << std::endl;
                return nullptr;
            }
        
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture)
            {
                std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
                SDL_FreeSurface(surface);
                return nullptr;
            }
        
            // Cache the texture
            TextureData data{filename, surface, texture, surface->w, surface->h, (double)surface->w / (double)surface->h};
            textureMap[filename] = data;
            return &textureMap[filename];
        }

        void draw(std::string &filename, int x, int y, int w, int h)
        {
            SDL_Texture *texture = get(filename);
            if (texture)
            {
                draw(texture, x, y, w, h);
            }
        }
        void draw(SDL_Texture *texture, int x, int y, int w, int h)
        {
            SDL_Rect destRect = {x, y, w, h};
            SDL_RenderCopy(renderer, texture, NULL, &destRect);
        }

        void drawScaled(std::string &filename, double centerX, double centerY, double w, double h, double rotation, double scale)
        {
            SDL_Texture *texture = get(filename);
            if (texture)
            {
                drawScaled(texture, centerX, centerY, w, h, rotation, scale);
            }
        }

        void drawScaled(SDL_Texture *texture, double centerX, double centerY, double w, double h, double rotation, double scale)
        {
            SDL_Rect destRect = {
                static_cast<int>(centerX - ((w * 0.5) * scale)),
                static_cast<int>(centerY - ((h * 0.5) * scale)),
                static_cast<int>(w * scale),
                static_cast<int>(h * scale)};

            SDL_RenderCopyEx(renderer, texture, NULL, &destRect, rotation, NULL, SDL_FLIP_NONE);
        }

        void drawPreserveAspect(std::string &filename, double centerX, double centerY, double w, double h, double rotation, double scale)
        {
            auto *data = getData(filename);
            if (data)
            {
                if (data->aspect < 1)
                {
                    drawScaled(filename, centerX, centerY, w, w / data->aspect, rotation, scale);
                }
                else
                {
                    drawScaled(filename, centerX, centerY, h * data->aspect, h, rotation, scale);
                }
            }
        }

        // Draw a rectangle
        void drawRect(double x, double y, double w, double h, SDL_Color color)
        {
            if (color.a < 255)
            {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            }
            else
            {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_Rect rect = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};
            SDL_RenderFillRect(renderer, &rect);
        }
    };

}
#endif