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
		printf("Failed to load texture image!\n");
		success = false;
	}
	texture = loadTexture("assets/sprite-1.png");
	if (texture == NULL)
	{
		printf("Failed to load texture image!\n");
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
	//Apply the PNG image directly to window, no scaling
	// SDL Surface is CPU-bound
		//SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, NULL);

	//Apply the image stretched
	//SDL_Rect stretchRect;
	//stretchRect.x = 0;
	//stretchRect.y = 0;
	//stretchRect.w = screenW;
	//stretchRect.h = screenH;
	//SDL_BlitScaled(backgroundPNG, NULL, screenSurface, &stretchRect);
	////Update the surface
	//SDL_UpdateWindowSurface(window);


	//Clear screen
	SDL_RenderClear(renderer);

	//Render texture to screen
	SDL_RenderCopy(renderer, backgroundIMG, NULL, NULL);

	//SDL_Rect pos;
	///*renderer gets generic position from piece 
	//translates it into screen coordinates
	//*/
	//pos.x = gameCurrent->testPiece.pos.x * 10;
	//
	//pos.y = gameCurrent->testPiece.pos.y;
	//pos.w = 64; pos.h = 64;
	//SDL_RenderCopy(renderer, texture, NULL, &pos);
	renderPieces();

	//Update screen
	SDL_RenderPresent(renderer);
}

void RenderWindow::renderBackground()
{

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


void RenderWindow::close()
{
	//Free loaded image
	if (backgroundPNG != NULL) 
	{
		SDL_FreeSurface(backgroundPNG);
		backgroundPNG = NULL;
	}
	if (texture != NULL || backgroundIMG != NULL)
	{
		SDL_DestroyTexture(texture);
		SDL_DestroyTexture(backgroundIMG);
		texture = NULL;
		backgroundIMG = NULL;
	}


	//Destroy window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void RenderWindow::setGame(Game* gameInstance)
{
	gameCurrent = gameInstance;
}
