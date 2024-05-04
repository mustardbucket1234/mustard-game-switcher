#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstring>

void drawText(const std::string &text, TTF_Font *font, SDL_Renderer *screen, int x, int y, const SDL_Color &color)
{
	SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(screen, textSurface);
	SDL_Rect dstRect = {x, y, textSurface->w, textSurface->h};
	SDL_RenderCopy(screen, texture, NULL, &dstRect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(texture);
}

void drawTextWrapped(const std::string &text, TTF_Font *font, SDL_Renderer *screen, int x, int y, int width, const SDL_Color &color)
{
	SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, width);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(screen, textSurface);
	SDL_Rect dstRect = {x, y, textSurface->w, textSurface->h};
	SDL_RenderCopy(screen, texture, NULL, &dstRect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(texture);
}

void drawTextCentered(const std::string &text, TTF_Font *font, SDL_Renderer *screen, int x, int y, int width, const SDL_Color &color)
{
	int textWidth, textHeight;
	TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);
	drawText(text, font, screen, x + (width - textWidth) / 2, y, color);
}