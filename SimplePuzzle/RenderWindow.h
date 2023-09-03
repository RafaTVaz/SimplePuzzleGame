#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

#include "Game.h"

class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_width, int p_height ); //TODO fullscreen bool 
	void display();
	void renderPieces();
	void renderBackground();

	void close();

	void setGame(Game* gameInstance);

	bool loadBackground(const char* p_location);
	bool loadMedia();
private:
	int screenW, screenH;

	SDL_Window*		window;
	SDL_Renderer*	renderer;
	Game*			gameCurrent;

	
	Color bgColor{30,30,30,1};
	//Loads individual image as texture
	SDL_Texture* loadTexture(const char* p_path);
	//Current displayed texture
	SDL_Texture* texture = NULL;
	SDL_Texture* backgroundIMG = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
	//Current displayed PNG image
	SDL_Surface* backgroundPNG = NULL;
};