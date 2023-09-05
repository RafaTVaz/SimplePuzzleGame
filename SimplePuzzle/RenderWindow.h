#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

#include "Game.h"

#define GRID_START  0  //0 or 0.5
#define PIXEL_SCALE 2  //depends on size of window?

class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_width, int p_height ); //TODO fullscreen bool 
	
	void display();
	void displayUI();
	void renderPieces();
	void renderBackground();
	void renderUI();

	bool loadMedia();

	void setGame(Game* gameInstance, double* p_fps);
	void close();

private:
	int getRelGridStart() {
		return GRID_START * PIXEL_SCALE * gameCurrent->testPiece.getGridSize();
	};
	void drawText(const char* text, int x, int y);
	void drawText(const char* text, int x, int y, TTF_Font* font, SDL_Color color);
	bool loadBackground(const char* p_location);
	bool destroyTextures();

	int screenW, screenH;
	double* currFPS = 0;
	int countedFrames = 0;

	Game* gameCurrent;

	/******************SDL RENDERING************************/
	SDL_Window*		window;
	SDL_Renderer*	renderer;


	Color bgColor{30,30,30,1};

	SDL_Color colBlack = { 30, 30, 30, 255 };
	SDL_Color colWhite = { 200, 200, 200, 255 };


	//Loads individual image as texture
	SDL_Texture* loadTexture(const char* p_path);
	//Current displayed texture
	TTF_Font*	 fontBig = NULL;
	TTF_Font*    fontSmall = NULL;
	SDL_Texture* texture = NULL;
	SDL_Texture* backgroundIMG = NULL;
	SDL_Texture* ui_IMG = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
	//Current displayed PNG image
	SDL_Surface* backgroundPNG = NULL;
};