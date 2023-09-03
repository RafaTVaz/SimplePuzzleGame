
#include "Game.h"
#include "Piece.h"

Game::Game()
{
	quit = false;
}


/**
checks specific keyboard presses and updates 
gameInput struct accordinngly
**/
void Game::checkKeyPress() 
{
	//reset test: all values to null
	gameInput = {};
	// user presses a key
	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
			case SDLK_ESCAPE:
				quit = true;
				break;

			case SDLK_SPACE:
				gameInput.start = true; gameInput.pressed = true;
				break;

			case SDLK_LEFT:
				gameInput.moveLeft = true; gameInput.pressed = true;
				printf("LEFT\n");
				break;

			case SDLK_RIGHT:
				gameInput.moveRight = true; gameInput.pressed = true;
				printf("RIGHT\n");
				break;

			case SDLK_UP:
				gameInput.hardDrop = true; gameInput.pressed = true;
				break;

			case SDLK_DOWN:
				gameInput.softDrop = true; gameInput.pressed = true;
				break;

			//case SDLK_z:
			//	gameInput.rotateClockwise = true; gameInput.pressed = true;
			//	break;

			//case SDLK_x:
			//	gameInput.rotateAnticlockwise = true; gameInput.pressed = true;
			//	break;

			//case SDLK_p:
			//	gameInput.pause = true; gameInput.pressed = true;
			//	break;

			default:
				gameInput.pressed = false;
				break;
		}
	}
}

void Game::run() 
{

	Piece* testPiece = new Piece();
	//Handle events on queue
	while (SDL_PollEvent(&event) != 0)
	{
		//User requests quit
		if (event.type == SDL_QUIT)
		{
			quit = true;
		}

		checkKeyPress();
		if (gameInput.pressed)
		{
			updatePlay();
		}
	}
}


void Game::updatePlay()
{
	// move horizontally
	if (gameInput.moveLeft)
	{
		// try move
		Piece testInstance = testPiece;
		--testInstance.pos.x;
		if( isMovePossible(testInstance.pos) )
			testPiece.pos.x = testInstance.pos.x;
		printf("Left: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
	}
	if (gameInput.moveRight)
	{
		// try move
		Piece testInstance = testPiece;
		++testInstance.pos.x;
		if (isMovePossible(testInstance.pos))
			testPiece.pos.x = testInstance.pos.x;
		printf("Right: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
	}
	if (gameInput.hardDrop)
	{
		// try move
		Piece testInstance = testPiece;
		--testInstance.pos.y;
		if (isMovePossible(testInstance.pos))
			testPiece.pos.y = testInstance.pos.y;
		printf("Up: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
	}
	if (gameInput.softDrop)
	{
		// try move
		Piece testInstance = testPiece;
		++testInstance.pos.y;
		if (isMovePossible(testInstance.pos))
			testPiece.pos.y = testInstance.pos.y;
		printf("Down: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
	}
}

bool Game::isMovePossible(Position newPos)
{
	//checks if number is between 1-8 / 1-16
	if (newPos.x - 1 <= 8 - 1 && newPos.x - 1 <= 16 - 1)
		return true;
	return false;
}



bool Game::isQuit()
{
	return quit;
}

