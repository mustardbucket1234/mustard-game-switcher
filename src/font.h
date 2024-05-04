#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

void drawTextWrapped(const std::string &text, TTF_Font *font, SDL_Renderer *screen, int x, int y, int width, const SDL_Color &color);
void drawText(const std::string &text, TTF_Font *font, SDL_Renderer *screen, int x, int y, const SDL_Color &color);
void drawTextCentered(const std::string &text, TTF_Font *font, SDL_Renderer *screen, int x, int y, int width, const SDL_Color &color);
#endif