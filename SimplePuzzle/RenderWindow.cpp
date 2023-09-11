#include "RenderWindow.h"

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


RenderWindow::RenderWindow(const char* p_title, int p_width, int p_height)
	:window(NULL), renderer(NULL), gameCurrent(NULL)
{
	screenW = p_width;
	screenH = p_height;
	//TODO for fullscreen bool: SDL_WINDOW_FULLSCREEN_DESKTOP 
	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		//Get window surface Not compatible with rendering
		/*screenSurface = SDL_GetWindowSurface(window);

		if (screenSurface == NULL)
		{
			printf("Surface could not be created! SDL Error: %s\n", SDL_GetError());
			window = NULL;
		}*/

		//Create renderer for window
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC |SDL_RENDERER_ACCELERATED);
		if (renderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			window = NULL;
		}
		else
		{
			//Initialize renderer color
			SDL_SetRenderDrawColor(renderer,bgColor.r, bgColor.g, bgColor.b, bgColor.a);

			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				window = NULL;
			}
			//Initialize SDL_ttf
			if (TTF_Init() == -1)
			{
				printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
				window = NULL;
			}
		}
	}
}

RenderWindow::~RenderWindow()
{
	//destroy game 
	delete gameCurrent;
	gameCurrent = NULL;

	//Destroy all loaded images, text, sprites
	destroyTextures();

	//Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;


	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}


void RenderWindow::drawText(const char* text, int x, int y) 
{
	SDL_assert(text);

	SDL_Surface* pSurface = TTF_RenderText_Blended(fontBig, text, colWhite);
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pSurface);
	int w, h;
	SDL_QueryTexture(pTexture, NULL, NULL, &w, &h);
	SDL_Rect dstRect = { x, y, w, h };
	SDL_RenderCopy(renderer, pTexture, nullptr, &dstRect);
	SDL_DestroyTexture(pTexture);
	SDL_FreeSurface(pSurface);
}

void RenderWindow::drawText(const char* text, int x, int y, TTF_Font* font, SDL_Color color)
{
	SDL_assert(text);

	SDL_Surface* pSurface = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pSurface);
	int w, h;
	SDL_QueryTexture(pTexture, NULL, NULL, &w, &h);
	SDL_Rect dstRect = { x, y, w, h };
	SDL_RenderCopy(renderer, pTexture, nullptr, &dstRect);
	SDL_DestroyTexture(pTexture);
	SDL_FreeSurface(pSurface);
}

bool RenderWindow::loadBackground(const char* p_path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(p_path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", p_path, IMG_GetError());
		return false;
	}
	else
	{
		////Convert surface to screen format
		backgroundPNG = SDL_ConvertSurface(loadedSurface, screenSurface->format, 0);
		if (backgroundPNG == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", p_path, SDL_GetError());
			return false;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//return optimizedSurface;
	return true;
}



bool RenderWindow::loadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	int fontSize = FONT_BIG;
	fontBig = TTF_OpenFont("assets/lazy.ttf", fontSize);
	if (fontBig == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	fontSize = FONT_SMALL;
	fontSmall = TTF_OpenFont("assets/lazy.ttf", fontSize);
	if (fontSmall == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	fontSize = FONT_MICRO;
	fontMicro = TTF_OpenFont("assets/lazy.ttf", fontSize);
	if (fontMicro == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	//Load PNG texture
	backgroundIMG = loadTexture("assets/background-blue.png");
	if (backgroundIMG == NULL)
	{
		printf("Failed to load texture background!\n");
		success = false;
	}
	ui_IMG = loadTexture("assets/Frame.png");
	if (backgroundIMG == NULL)
	{
		printf("Failed to load texture UI image!\n");
		success = false;
	}
	/*texture = loadSpriteSheet("assets/Sprites-Page.png");
	if (texture == NULL)
	{
		printf("Failed to load texture sprite!\n");
		success = false;
	}*/
	spriteSheet = loadSpriteSheet("assets/PuyoPuyo-Sprites.png");
	if (spriteSheet == NULL)
	{
		printf("Failed to load texture sprite!\n");
		success = false;
	}
	return success;
}

SDL_Texture* RenderWindow::loadTexture(const char* p_path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(p_path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", p_path, IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", p_path, SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

/**
* Creates spriteClips acording to the spriteSheet png
* 
* has to be redone manually every time because sprite sheets
* don't follow specific rules, if a new sprite is added,
* changing the PNG dims, this needs to be rewritten
* 
* Blocks of sprites representing each piece type (color)
* this PNG has space for 84 sprites
* 	// [ 1 | 3 ]
*	// [ 2 | 4 ]
* 
* @return loadTexture(path to spriteSheet)
*/
SDL_Texture* RenderWindow::loadSpriteSheet(const char* p_path)
{
	int i, j;
	int clipWidth = GRID_SIZE;
	int clipHeight = GRID_SIZE;

	int texgridW = 14; //number of sprites in x dir
	int texgridH = 6;  //number of sprites in y dir

	// Initialize spriteClips
	for ( i = 0; i < NUM_PIECES; i++) {
		for ( j = 0; j < NUM_SPRITES; j++) {
			spriteClips[i][j].w = clipWidth;           // Set the width
			spriteClips[i][j].h = clipHeight;          // Set the height
		}
	}


	// [ o |   ]
	// [   |   ]
	i = 0;
	for ( j = 0; j < NUM_SPRITES; j++) {
		/**spriteSheetPos.x = [ border ] + [   column   ] **/
		spriteClips[i][j].x = 1*(j/3 +1) + j/3 * clipWidth;
		/**spriteSheetPos.y = [ border ] + [    line    ]**/
		spriteClips[i][j].y = 1*(j%3 +1) + j%3 * clipHeight;
	}

	// [   |   ]
	// [ o |   ]
	i = 1;
	for (j = 0; j < NUM_SPRITES; j++) {
		/**spriteSheetPos.x = [ border ] + [   column   ] **/
		spriteClips[i][j].x = 1 * (j / 3 + 1) + j / 3 * clipWidth;
		/**spriteSheetPos.y = [ border ] + [    line    ]**/
		spriteClips[i][j].y = 1 * (j % 3 + 1) + j % 3 * clipHeight + (texgridH/2 * (1 + clipHeight));
	}

	// [   | o ]
	// [   |   ]
	i = 2;
	for (j = 0; j < NUM_SPRITES; j++) {
		/**spriteSheetPos.x = [ border ] + [   column   ] **/
		spriteClips[i][j].x = 1 * (j / 3 + 1) + j / 3 * clipWidth  + 1*(texgridW/2 * (1 + clipWidth));
		/**spriteSheetPos.y = [ border ] + [    line    ]**/
		spriteClips[i][j].y = 1 * (j % 3 + 1) + j % 3 * clipHeight;
	}

	// [   |   ]
	// [   | o ]
	i = 3;
	for (j = 0; j < NUM_SPRITES; j++) {
		/**spriteSheetPos.x = [ border ] + [   column   ] **/
		spriteClips[i][j].x = 1 * (j / 3 + 1) + j / 3 * clipWidth  + (texgridW/2 * (1 + clipWidth));
		/**spriteSheetPos.y = [ border ] + [    line    ]**/
		spriteClips[i][j].y = 1 * (j % 3 + 1) + j % 3 * clipHeight + (texgridH/2 * (1 + clipHeight));
	}

	return loadTexture(p_path);
}

void RenderWindow::display()
{
	//Clear screen
	SDL_RenderClear(renderer);

	renderBackground();
	renderPieces();
	renderUI();
	//Update screen
	SDL_RenderPresent(renderer);
	countedFrames++;
}

void RenderWindow::displayUI()
{//not working
	renderUI();
	//Update screen
	SDL_RenderPresent(renderer);
}

void RenderWindow::renderBackground()
{
	//Render texture to screen
	SDL_RenderCopy(renderer, backgroundIMG, NULL, NULL);
}

void RenderWindow::renderUI()
{
	//GridBox
	int scale = PIXEL_SCALE; //screen multiplier
	int x, y;
	SDL_Rect pos;
	pos.x = pos.y = getRelGridStart();
	pos.w = 160 * scale;
	pos.h = 288 * scale;
	SDL_RenderCopy(renderer, ui_IMG, NULL, &pos);


	char tempText[17];
	_gcvt_s(tempText, sizeof(tempText), *currFPS, 4);
	x = screenW - 20 * scale; 
	y = 5 * scale;
	drawText(tempText, x, y, fontMicro, colWhite);

	if(gameCurrent->getCurrentPlayState() == gameCurrent->playState.starting)
	{
		x = screenW / 2;
		y = 11 * scale + FONT_BIG;
		//drawText("GAME OVER",				screenW/2, 10*PIXEL_SCALE, fontBig, colWhite);
		drawText("Press Space/Start", x, y, fontSmall, colWhite);
	}
	else if (gameCurrent->getCurrentPlayState() == gameCurrent->playState.gameOver)
	{
		x = screenW / 2;
		y = 10 * scale;
		drawText("GAME OVER", x, y, fontBig, colWhite);
		drawText("Press Space/Start to continue", x, y+ scale + FONT_BIG, fontSmall, colWhite);
	}
		
		x = screenW / 2;
		y = 50 * scale + FONT_BIG;
		drawText("HIGH SCORE:", x, y, fontBig, colWhite);
		_gcvt_s(tempText, sizeof(tempText), gameCurrent->getHighscore(), 4);
		drawText(tempText, x, y + scale + FONT_BIG, fontSmall, colWhite);

}

/**
*	Renderer gets generic position from piece
*	translates it into screen coordinates
*	@returns rectangle where Sprite will be drawn on screen
*/
SDL_Rect RenderWindow::updateRectPos(Piece tempPiece, int t_mapStart, int t_pixels)
{
	SDL_Rect newRect;
	newRect.x = tempPiece.getRealPos().x + t_mapStart;
	newRect.y = tempPiece.getRealPos().y + t_mapStart;
	newRect.w = newRect.h = t_pixels;
	return newRect;
}

std::array<SDL_Rect, 2> RenderWindow::updatePlayerRectPos(int t_mapStart, int t_pixels)
{
	std::array<SDL_Rect, 2> newRect;
	//Player Piece
	newRect[0].x = gameCurrent->testPiece.getRealPos().x + t_mapStart;
	newRect[0].y = gameCurrent->testPiece.getRealPos().y + t_mapStart;
	newRect[0].w = newRect[0].h = t_pixels;

	//Other Piece
	newRect[1].x = gameCurrent->testPiece.getOtherRealPos().x + t_mapStart;
	newRect[1].y = gameCurrent->testPiece.getOtherRealPos().y + t_mapStart;
	newRect[1].w = newRect[1].h = t_pixels;

	return newRect;
}

void RenderWindow::renderPieces()
{
	SDL_Rect pos;

	int id = 0;
	int animSprite = 1;
	spriteSize = gameCurrent->testPiece.getGridSize(); //16x16
	mapStart = getRelGridStart(); //pixel where the playable area starts

	

	/************Render Player Pieces**************/

	renderPlayPieces();

	/************Render Board Pieces***************/
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			id = gameCurrent->pieceMatrix[i][j].id;
			if (id >= 0)
			{
				//animSprite depends on state of piece, connected or not etc
				animSprite = 3;
				pos = updateRectPos(gameCurrent->pieceMatrix[i][j], mapStart, spriteSize);
				SDL_RenderCopy(renderer, spriteSheet, &spriteClips[id][animSprite], &pos);
			}
		}
	}
}

void RenderWindow::renderPlayPieces()
{
	std::array<SDL_Rect, 2> playerPos;
	SDL_Rect otherPos;
	int animSprite = 1;
	/**
	* animSprite:
	*	 0 - normal
	*	 1 - highligh normal
	*	 3 - squish
	*	 4 - white
	*	 6 - long (falling)
	*	 7 - wide eye
	*
	*	SDL_RenderCopy(renderer, spriteSheet, &spriteClips[id][animSprite], &pos);
	*/
	//has to be animation depepndante not buffer
	// animSprite = gameCurrent->testPiece.pos.buffer % 3 * 3; //changes between 0 and 3 

	if (gameCurrent->getCurrentPlayState() == FINAL_MOVE)
		animSprite = 4;
	if (gameCurrent->getCurrentPlayState() == PLAY || gameCurrent->getCurrentPlayState() == FINAL_MOVE)
	{
		playerPos = updatePlayerRectPos( mapStart, spriteSize);
		SDL_RenderCopy(renderer, spriteSheet, &spriteClips[gameCurrent->testPiece.id][animSprite], &playerPos[0]);

		otherPos = updateRectPos(gameCurrent->testPiece.otherPiece, mapStart, spriteSize);
		SDL_RenderCopy(renderer, spriteSheet, &spriteClips[gameCurrent->testPiece.otherPiece.id][0], &playerPos[1]);
	}


}

void RenderWindow::setGame(Game* gameInstance, double* p_fps)
{
	gameCurrent = gameInstance;
	currFPS = p_fps;
}

bool RenderWindow::destroyTextures()
{
	if (backgroundPNG != NULL)
	{
		SDL_FreeSurface(backgroundPNG);
		backgroundPNG = NULL;
	}
	TTF_CloseFont(fontBig);
	TTF_CloseFont(fontSmall);
	TTF_CloseFont(fontMicro);
	SDL_DestroyTexture(spriteSheet);
	SDL_DestroyTexture(backgroundIMG);
	SDL_DestroyTexture(ui_IMG);
	fontBig = NULL;
	fontSmall = NULL;
	fontMicro = NULL;
	spriteSheet = NULL;
	backgroundIMG = NULL;
	ui_IMG = NULL;

	return true;
}


