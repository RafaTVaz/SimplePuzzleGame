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
	int fontSize = 28;
	fontBig = TTF_OpenFont("assets/lazy.ttf", fontSize);
	if (fontBig == NULL)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	fontSize = 12;
	fontSmall = TTF_OpenFont("assets/lazy.ttf", fontSize);
	if (fontSmall == NULL)
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
	texture = loadTexture("assets/sprite-1.png");
	if (texture == NULL)
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
	SDL_Rect pos;
	pos.x = pos.y = getRelGridStart();
	pos.w = 160 * scale;
	pos.h = 288 * scale;
	SDL_RenderCopy(renderer, ui_IMG, NULL, &pos);

	//Calculate and correct fps
	double avgFPS = countedFrames / (gameCurrent->getTimePassed());
	if (avgFPS > 2000000)
	{
		avgFPS = 0;
	}
	char tempText[17];
	_gcvt_s(tempText, sizeof(tempText), *currFPS, 8);
	drawText(tempText, screenW/2, 20, fontSmall, colWhite);
}


void RenderWindow::renderPieces()
{
	SDL_Rect pos;
	/*renderer gets generic position from piece 
	translates it into screen coordinates
	*/
	pos.x = gameCurrent->testPiece.getRealPos().x + getRelGridStart();
	pos.y = gameCurrent->testPiece.getRealPos().y + getRelGridStart();
	pos.w = pos.h = gameCurrent->testPiece.getGridSize();
	SDL_RenderCopy(renderer, texture, NULL, &pos);
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
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(backgroundIMG);
	SDL_DestroyTexture(ui_IMG);
	fontBig = NULL;
	fontSmall = NULL;
	texture = NULL;
	backgroundIMG = NULL;
	ui_IMG = NULL;

	return true;
}

void RenderWindow::close()
{
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


