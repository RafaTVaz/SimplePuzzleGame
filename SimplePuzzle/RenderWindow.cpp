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
	//Texture filtering Default is nearest pixel sampling, what is wanted for pixel art
	//TODO for fullscreen bool: SDL_WINDOW_FULLSCREEN_DESKTOP 
	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (window == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		
		//Get window surface is Not compatible with rendering
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
			//Initialize SDL_mixer
			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
			{
				printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
				window = NULL;
			}
			Mix_Volume(-1, MIX_MAX_VOLUME/ 3);      //sound effects
			Mix_VolumeMusic(MIX_MAX_VOLUME / 25);	//music
		}
	}

}

RenderWindow::~RenderWindow()
{
	//destroy game 
	delete gameCurrent;
	gameCurrent = NULL;

	//Destroy all loaded images, text, sprites, music
	destroyLoadedMedia();

	//Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

//receives text and screen position
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

//receives text and screen position, font and font colour
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



bool RenderWindow::loadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	int fontSize = FONT_BIG;
	fontBig = TTF_OpenFont("assets/Minecraft.ttf", fontSize);
	if (fontBig == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	fontSize = FONT_SMALL;
	fontSmall = TTF_OpenFont("assets/Minecraft.ttf", fontSize);
	if (fontSmall == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	fontSize = FONT_MICRO;
	fontMicro = TTF_OpenFont("assets/Minecraft.ttf", fontSize);
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
	spriteSheet = loadSpriteSheet("assets/Sprites-game.png");
	if (spriteSheet == NULL)
	{
		printf("Failed to load texture sprite!\n");
		success = false;
	}

	iconPNG = IMG_Load("assets/icon.png");
	if (iconPNG == NULL)
	{
		printf("Failed to load icon sprite!\n");
		success = false;
	}
	SDL_SetWindowIcon( window, iconPNG);

	//Load Sounds

	//Load music
	/*gSound_Music = Mix_LoadMUS("assets/sounds/Abstraction_-_Three_Red_Hearts_-_Rabbit_Town.wav");
	if (gSound_Music == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}*/

	//Load sound effects
	gSound_Pop = Mix_LoadWAV("assets/sounds/hit.wav");
	if (gSound_Pop == NULL)
	{
		
		
		
		("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	gSound_Input = Mix_LoadWAV("assets/sounds/input.wav");
	if (gSound_Input == NULL)
	{
		
		
		("Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	gSound_Burst = Mix_LoadWAV("assets/sounds/explosion.wav");
	if (gSound_Burst == NULL)
	{
		printf("Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	gSound_Start = Mix_LoadWAV("assets/sounds/coin.wav");
	if (gSound_Start == NULL)
	{
		printf("Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	gSound_Rotate = Mix_LoadWAV("assets/sounds/rotate.wav");
	if (gSound_Start == NULL)
	{
		printf("Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	gSound_Move = Mix_LoadWAV("assets/sounds/Move.wav");
	if (gSound_Start == NULL)
	{
		printf("Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	gSound_newWave = Mix_LoadWAV("assets/sounds/newWave.wav");
	if (gSound_Start == NULL)
	{
		printf("Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	gSound_GameOver = Mix_LoadWAV("assets/sounds/gameOver.wav");
	if (gSound_Start == NULL)
	{
		printf("Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	return success;
}

/**
* loads image and creates texture
* @returns text££ure£@@@££@s
*/
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

SDL_Surface* RenderWindow::loadSurface(const char* p_path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(p_path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", p_path, IMG_GetError());
	}
	else
	{
		////Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, 0);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", p_path, SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//return optimizedSurface;
	return optimizedSurface;
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
* @return loadTexture(path to spriteSheet), returns texture
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

/**
* clears screen and renders all things to render, and music
*/
void RenderWindow::display()
{
	//Clear screen
	SDL_RenderClear(renderer);

	renderBackground();
	renderPieces();
	renderUI();
	playSounds();
	//Update screen
	SDL_RenderPresent(renderer);
	countedFrames++;
	//If there is no music playing FIXME
	if (Mix_PlayingMusic() == 0)
	{
		//Play the music
		Mix_PlayMusic(gSound_Music, -1);
	}
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
	int shift = 5 * scale;
	SDL_Rect pos;
	pos.x = pos.y = getRealGridStart();
	pos.w = 160 * scale;
	pos.h = 288 * scale;
	SDL_RenderCopy(renderer, ui_IMG, NULL, &pos);


	char tempText[17];
	_gcvt_s(tempText, sizeof(tempText), *currFPS, 4);
	x = screenW - 20 * scale; 
	y = 5 * scale;
	drawText(tempText, x, y, fontMicro, colWhite);

/*STARTING********************************/
	if(gameCurrent->getCurrentPlayState() == gameCurrent->playState.starting)
	{
		x = screenW / 2 + shift;
		y = 11 * scale + FONT_BIG;
		//drawText("GAME OVER",				screenW/2, 10*PIXEL_SCALE, fontBig, colWhite);
		drawText("Press Space/Start", x, y, fontSmall, colWhite);
	}
/*GAME OVER********************************/
	else if (gameCurrent->getCurrentPlayState() == gameCurrent->playState.gameOver)
	{
		x = screenW / 2 + shift;
		y = 10 * scale;
		drawText("GAME OVER", x, y, fontBig, colWhite);
		drawText("Press Space/Start to continue", x, y+ scale + FONT_BIG, fontSmall, colWhite);

		x = screenW / 2 + shift;
		y = 10 + FONT_BIG; y *= scale;
		drawText("High Score:", x, y, fontBig, colWhite);
		_gcvt_s(tempText, sizeof(tempText), gameCurrent->getHighscore(), 10);
		drawText(tempText, x, y + scale + FONT_BIG, fontSmall, colWhite);

		x = screenW / 2 + shift;
		y = 50 * scale + 3 * FONT_BIG;
		drawText("Score:", x, y, fontBig, colWhite);
		_gcvt_s(tempText, sizeof(tempText), gameCurrent->getScore(), 10);
		drawText(tempText, x, y + scale + FONT_BIG, fontSmall, colWhite);
	}
/*PAUSED********************************/
	else if (gameCurrent->isTimePaused())
	{
		x = screenW / 2 + shift;
		y = 10 * scale;
		drawText("GAME PAUSED", x, y, fontBig, colWhite);
		drawText("Press P to continue", x, y + scale + FONT_BIG, fontSmall, colWhite);
	}
/*GAMING********************************/
	else
	{
		x = screenW * 3 / 5;
		y = screenH - 3* FONT_BIG*scale;

		x = screenW * 3 / 5;
		y = 50 * scale + 5 * FONT_BIG;
		drawText("High Score:", x, y, fontSmall, colWhite);
		_gcvt_s(tempText, sizeof(tempText), gameCurrent->getHighscore(), 10);
		drawText(tempText, x + 56 * scale, y, fontSmall, colWhite);

		x = screenW *3/5;
		y = 50 * scale + 3 *FONT_BIG;
		drawText("Score:", x, y, fontBig, colWhite);
		_gcvt_s(tempText, sizeof(tempText), gameCurrent->getScore(), 10);
		drawText(tempText, x + 55 * scale , y, fontBig, colWhite);

		x = screenW * 3 / 5;
		y = 50 * scale + 4 * FONT_BIG;
		drawText("Burst:", x, y, fontSmall, colWhite);
		_gcvt_s(tempText, sizeof(tempText), gameCurrent->getBurstPoints(), 10);
		drawText(tempText, x + 56 * scale, y , fontSmall, colWhite);

		x = screenW/2;
		y = 5 * scale;
		double t_time = gameCurrent->getTimePassed();
		int num = floor(log10(t_time)) > 0 ? floor(log10(t_time)): 0;
		num++;
		drawText("Time:", x, y, fontSmall, colWhite);
		_gcvt_s(tempText, sizeof(tempText), t_time, num);
		drawText(tempText, x + 25 * scale, y , fontSmall, colWhite);
	}
/*INSTRUCTIONS********************************/
	x = 10 * scale;
	y = screenH - 3 * FONT_SMALL;
	drawText("Left/Right: move Piece | Up: Hard Drop | Down: Slow Drop", x, y, fontSmall, colWhite);

	x = 10 * scale;
	y = screenH -  30;
	drawText("Z: Rotate | P: Pause | M: Mute Music", x, y, fontSmall, colWhite);		
}

void RenderWindow::playSounds()
{
	//what sound is supposed to play according to game
	int soundID = gameCurrent->getSound();
	
	if (gameCurrent->isMute()) {
		Mix_PauseMusic();
	}
	else if (Mix_PausedMusic() == 1 && gameCurrent->getCurrentPlayState()!= gameCurrent->playState.gameOver)
	{
		//Resume the music
		Mix_PlayMusic(gSound_Music, -1);
	}

	switch (soundID)
	{
	case gameCurrent->sounds.input:
		Mix_PlayChannel(-1, gSound_Input, 0);
		break;
	case gameCurrent->sounds.move:
		Mix_PlayChannel(-1, gSound_Move, 0);
		break;
	case gameCurrent->sounds.rotate:
		Mix_PlayChannel(-1, gSound_Rotate, 0);
		break;
	case gameCurrent->sounds.burst:
		Mix_PlayChannel(-1, gSound_Burst, 0);
		break;
	case gameCurrent->sounds.start:
		Mix_PlayChannel(-1, gSound_Start, 0);
		break;
	case gameCurrent->sounds.set:
		Mix_PlayChannel(-1, gSound_Pop, 0);
		break;
	case gameCurrent->sounds.pop:
		Mix_PlayChannel(-1, gSound_Input, 0);
		break;
	case gameCurrent->sounds.wave:
		Mix_PlayChannel(-1, gSound_newWave, 0);
		break;
	case gameCurrent->sounds.lost:
		Mix_PlayChannel(-1, gSound_GameOver, 0);
		Mix_PauseMusic();
		break;

	
	}	
}

int RenderWindow::getRealGridStart() {
	int pixelStart = GRID_START * PIXEL_SCALE;
	int shakeVal = gameCurrent->getShake();
	pixelStart	+= ((rand() % 2) == 0 ? -shakeVal : shakeVal);
	gameCurrent->lowerShake();
	return pixelStart;
};

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
	mapStart = getRealGridStart(); //pixel where the playable area starts

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
	*	 3 - placed
	*	 4 - final move
	*	 6 - falling (not used)
	*	 7 - gray (not used)
	*
	*	SDL_RenderCopy(renderer, spriteSheet, &spriteClips[id][animSprite], &pos);
	*/

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

bool RenderWindow::destroyLoadedMedia()
{
	//Free the sound effects
	Mix_FreeChunk(gSound_Pop);
	Mix_FreeChunk(gSound_Input);
	Mix_FreeChunk(gSound_Burst);
	Mix_FreeChunk(gSound_Start);

	gSound_Pop = NULL;
	gSound_Input = NULL;
	gSound_Burst = NULL;
	gSound_Start = NULL;

	//Free the music
	Mix_FreeMusic(gSound_Music);
	gSound_Music = NULL;



	//Free Text and Images
	if (iconPNG != NULL)
	{
		SDL_FreeSurface(iconPNG);
		iconPNG = NULL;
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
