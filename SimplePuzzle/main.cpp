/**
*	BAHBLE - 13/09/2023
* 
* A simple Puyo style game,
* made to learn C++ and SDL
* Everything but the font was made by me
* 
*	Pixel Art made with Asesprite
*	Sound Effects made with DrPetter's sfxr [http://www.drpetter.se/project_sfxr.html]
*	Main song is from [https://abstractionmusic.bandcamp.com/album/three-red-hearts]
*	Implemented SDL with the help of LazyFoo's lovely tutorial [https://lazyfoo.net/tutorials/SDL]
*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>


#include "RenderWindow.h"
#include "Game.h"


//Screen dimension constants  1x1 ratio for old school look
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

//Starts up SDL and creates window
bool init();

//calculates and prints average fps
void calculateFPS(float& t_elapsed, Uint64 t_start, Uint64 t_end, int& t_counter, double& t_fps);

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
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		
		
		
		
		("SDL could not initialize! SDL Error: %s\n", SDL_GetError());	
		success = false;
	}
	else
	{
		//Create window
		gWindow = new RenderWindow("Simple Game", SCREEN_WIDTH, SCREEN_HEIGHT);

		if (gWindow == NULL)
			success = false;
	}

	return success;
}

void calculateFPS(float& t_elapsed, Uint64 t_start, Uint64 t_end, int& t_counter, double& t_fps)
{
	t_elapsed += (t_end - t_start) / (float)SDL_GetPerformanceFrequency();
	if (t_counter > 60)
	{
		t_elapsed = t_elapsed / t_counter;
		t_fps = 1.0f / t_elapsed;
		//std::cout << "Current FPS: " << std::to_string(t_fps) << std::endl;
		t_counter = 0;
		t_elapsed = 0;
	}
}

void close()
{
	//Free resources and close SDL

	//delete gameLogic; //is deleted in gwindow
	delete gWindow;
	gameLogic = NULL;
	gWindow = NULL;
}



int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		
		
		("Failed to initialize!\n");
	}
	else
	{
		//Load Textures, Text and Music
		if (!gWindow->loadMedia())
		{
			
			
			("Failed to load Background!\n");
		}
		else
		{
			
			int counter = 0; float elapsed = 0;
			double fps = 0;
			Uint64 start = 0, end = 0;

			//initialize Game
			gameLogic = new Game();
			gWindow->setGame(gameLogic, &fps);
			
			//Main loop; While application is running
			while (!gameLogic->isQuit())
			{
				counter++;
				start = SDL_GetPerformanceCounter();
				//game logic aka game happens before being drawn
				gameLogic->run();
				gWindow->display();

				/*print average framerate ~60fps 
				sets elapsed&counter to 0 every second*/
				end = SDL_GetPerformanceCounter();
				calculateFPS(elapsed, start, end, counter, fps);
			}
		}
	}

	close();

	return 0;
}
