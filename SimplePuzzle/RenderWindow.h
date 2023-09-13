#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <array>
#include <cmath>


#include "Game.h"

#define GRID_START  5	//0 or 0.5
#define PIXEL_SCALE 2	//depends on size of window?
#define NUM_PIECES	4	//num of different colored pieces
#define NUM_SPRITES 21  //num of sprites per piece
#define FONT_MICRO	12
#define FONT_SMALL	18
#define FONT_BIG	32


class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_width, int p_height ); //TODO fullscreen bool 
	~RenderWindow();
	void display();

	bool loadMedia();

	void setGame(Game* gameInstance, double* p_fps);

private:
	void displayUI();
	void renderPieces();
	void renderBackground();
	void renderUI();
	void playSounds();
	int getRealGridStart() {
		return GRID_START * PIXEL_SCALE;// *gameCurrent->testPiece.getGridSize();
	};
	SDL_Rect updateRectPos(Piece tempPiece, int t_mapStart, int t_pixels);
	std::array<SDL_Rect, 2> updatePlayerRectPos(int t_mapStart, int t_pixels);
	void drawText(const char* text, int x, int y);
	void drawText(const char* text, int x, int y, TTF_Font* font, SDL_Color color);
	bool loadBackground(const char* p_location);
	void renderPlayPieces();
	bool destroyLoadedMedia();

	int spriteSize = 0; //16x16
	int mapStart = 0;
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

	//one square per sprite from sprite sheet
	//					[Piece id  ][anition frame]
	SDL_Rect spriteClips[NUM_PIECES][NUM_SPRITES] = {};


	//Loads individual image as texture
	SDL_Texture* loadTexture(const char* p_path);
	SDL_Texture* loadSpriteSheet(const char* p_path);
	//Current displayed texture
	TTF_Font*	 fontBig = NULL;
	TTF_Font*	 fontSmall = NULL;
	TTF_Font*    fontMicro = NULL;
	SDL_Texture* spriteSheet = NULL;
	SDL_Texture* backgroundIMG = NULL;
	SDL_Texture* ui_IMG = NULL;

	//The music that will be played
	Mix_Music* gSound_Music = NULL; //gameOver?

	//The sound effects that will be used
	Mix_Chunk* gSound_Pop = NULL;
	Mix_Chunk* gSound_Input = NULL;
	Mix_Chunk* gSound_Move = NULL;
	Mix_Chunk* gSound_Rotate = NULL;
	Mix_Chunk* gSound_Burst = NULL;
	Mix_Chunk* gSound_newWave = NULL;
	Mix_Chunk* gSound_Start = NULL;
	Mix_Chunk* gSound_GameOver = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
	//Current displayed PNG image
	SDL_Surface* backgroundPNG = NULL;
};