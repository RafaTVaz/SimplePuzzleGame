//FIXME
/*This source code copyrighted by Lazy Foo' Productions 2004-2023
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>


#include "RenderWindow.h"
#include "Game.h"


//Screen dimension constants  4x3 ratio old school look
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

//Starts up SDL and creates window
bool init();

//calculates and prints average fps
void printFPS(float& t_elapsed, Uint64 t_start, Uint64 t_end, int& t_counter);

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
RenderWindow* gWindow = NULL;

//The game logic we'll be running
Game* gameLogic = NULL;


/*********************************************************/


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = new RenderWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT);

		if (gWindow == NULL)
		{
			success = false;
		}
	}


	return success;
}

void printFPS(float& t_elapsed, Uint64 t_start, Uint64 t_end, int& t_counter)
{
	t_elapsed += (t_end - t_start) / (float)SDL_GetPerformanceFrequency();
	if (t_counter > 60)
	{
		t_elapsed = t_elapsed / t_counter;
		std::cout << "Current FPS: " << std::to_string(1.0f / t_elapsed) << std::endl;
		t_counter = 0;
		t_elapsed = 0;
	}
}

void close()
{
	//Free resources and close SDL
	gWindow->close();
}



int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load Background
		//if (!gWindow->loadBackground("assets/background.png"))
		if (!gWindow->loadMedia())
		{
			printf("Failed to load Background!\n");
		}
		else
		{
			
			////Main loop flag
			//bool quit = false;

			////Event handler
			//SDL_Event e;

			//initialize Game
			gameLogic = new Game();
			gWindow->setGame(gameLogic);
			
			int counter = 0; float elapsed = 0;
			Uint64 start = 0, end = 0;
			//While application is running
			while (!gameLogic->isQuit())
			{
				counter++;
				start = SDL_GetPerformanceCounter();
				//game logic aka game happens before being drawn
				gameLogic->run();
				gWindow->display();


				end = SDL_GetPerformanceCounter();
				/*print average framerate ~60fps 
				sets elapsed&counter to 0 every second*/
				printFPS(elapsed, start, end, counter);
			}
		}
	}

	close();

	return 0;
}
