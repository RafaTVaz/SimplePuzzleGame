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
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
		}
	}
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

void RenderWindow::renderBackground()
{
	//Render texture to screen
	SDL_RenderCopy(renderer, backgroundIMG, NULL, NULL);
}

void RenderWindow::renderUI()
{
	//GridBox
	int scale = 2; //screen multiplier
	SDL_Rect pos;
	pos.x = pos.y = 0;
	pos.w = 160 * scale;
	pos.h = 288 * scale;
	SDL_RenderCopy(renderer, ui_IMG, NULL, &pos);

	
	//Calculate and correct fps
	//float avgFPS = countedFrames / (gameCurrent->getTimePassed());
	//if (avgFPS > 2000000)
	//{
	//	avgFPS = 0;
	//}
	//
	//Render textures
}



void RenderWindow::renderPieces()
{
	SDL_Rect pos;
	/*renderer gets generic position from piece 
	translates it into screen coordinates
	*/
	pos.x = gameCurrent->testPiece.getRealPos().x;
	pos.y = gameCurrent->testPiece.getRealPos().y;
	pos.w = pos.h = gameCurrent->testPiece.getGridSize();
	SDL_RenderCopy(renderer, texture, NULL, &pos);
}

void RenderWindow::setGame(Game* gameInstance)
{
	gameCurrent = gameInstance;
}

bool RenderWindow::destroyTextures()
{
	if (backgroundPNG != NULL)
	{
		SDL_FreeSurface(backgroundPNG);
		backgroundPNG = NULL;
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(backgroundIMG);
	SDL_DestroyTexture(ui_IMG);
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
	IMG_Quit();
	SDL_Quit();
}


