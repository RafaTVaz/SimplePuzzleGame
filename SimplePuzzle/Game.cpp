
#include "Game.h"
#include "Piece.h"
#include <iostream>


Game::Game()
{
	quit = false;
	timer.start();
	testPiece.id = 1; //playable piece
	
	// Initialize each element to an empty/default state
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			pieceMatrix[i][j].id = -1; //Non existent
		}
	}

	// Assuming you have a pointer stored in object_matrix[i][j]
	// Check if the pointer is not nullptr (i.e., there's an object to delete)
	//if (pieceMatrix[i][j] != nullptr) {
	//	// Delete the object to avoid a memory leak
	//	delete pieceMatrix[i][j];

	//	// Set the pointer to nullptr to indicate it's now empty
	//	pieceMatrix[i][j] = nullptr;
	//}
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
			break;

		case SDLK_RIGHT:
			gameInput.moveRight = true; gameInput.pressed = true;
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

		case SDLK_p:
			gameInput.pause = true; gameInput.pressed = true;
			break;

		default:
			gameInput.pressed = false;
			break;
		}
	}
}

void Game::run()
{
	//Piece* testPiece = new Piece();

	//Handle events on queue
	while (SDL_PollEvent(&event) != 0)
	{
		//User requests quit
		if (event.type == SDL_QUIT)
		{
			quit = true;
		}

		checkKeyPress();


		//while (t_accumul > t_slice) 60fps
		if (gameInput.pressed)
		{
			updatePlay();
		}
	}
	updateGame();
}

void Game::updateGame()
{
	if(!placedPiece)
	{
		/********ANIMATION FALLING***********/
		if (isMoveFinal(testPiece.pos))
		{
			placedPiece = true;
			//save piece in Matrix
			pieceMatrix[testPiece.pos.x-1][testPiece.pos.y-1].clone(testPiece);
			printf("FINAL: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
			return;
		}

		/*
		* Calculate elapsed time since last animation
		* movement should be fixed to time ticks not frames	
		* this animation is only valid because pixel art needs to move specific pixels (don't want smooth motion)
		* else -> [movePercentage = elapsedTime/timeFrame ]
		*         [distanceToMove = speed * movePercentage]
		*/
		Uint32 currTime = timer.getTicks();
		Uint32 deltaTime = currTime - animTiming.lastTime;
		animTiming.lastTime = currTime;
		animTiming.elapsedTime += deltaTime;
		//printf("%d\n", animTiming.fps_falling);
		// divide by 1000 gives value in seconds
		if (animTiming.elapsedTime > 1000.f / animTiming.fps_falling)
		{ 
			Position temp = testPiece.pos;
			++temp.y;
			if (isMovePossible(temp))
			{
				//printf("BUFFER: \n");
				testPiece.bufferMove();
			}
			animTiming.elapsedTime = 0;
		}
	}

}


void Game::updatePlay()
{
	if (gameInput.pause)
	{
		if(timer.isPaused())
		{
			timer.unpause();
		}
		else
		{
			timer.pause();
			printf("PAUSE\n");
		}
	}

	// move Piece
	if (!timer.isPaused()) 
	{
		if (gameInput.moveLeft)
		{
			// try move
			Piece testInstance = testPiece;
			--testInstance.pos.x;
			if( isMovePossible(testInstance.pos) )
				testPiece.pos.x = testInstance.pos.x;
			//printf("Left: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
		}
		if (gameInput.moveRight)
		{
			// try move
			Piece testInstance = testPiece;
			++testInstance.pos.x;
			if (isMovePossible(testInstance.pos))
				testPiece.pos.x = testInstance.pos.x;
			//printf("Right: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
		}
		if (gameInput.hardDrop)
		{
			// try move
			Piece testInstance = testPiece;
			--testInstance.pos.y;
			if (isMovePossible(testInstance.pos)) 
			{//test if last position, if yes, no buffer so it doesnt leave frame
				testPiece.pos.y = testInstance.pos.y;
			}
			//printf("Up: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
		}
		if (gameInput.softDrop)
		{
			// try move
			Piece testInstance = testPiece;
			++testInstance.pos.y;
			if (isMovePossible(testInstance.pos))
			{//FIXME make move down while pressed without pause
				testPiece.bufferMove(4);
			}
			//printf("Down: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
		}
	}
}

//returns timer in seconds
double Game::getTimePassed()
{
	return timer.getTicks()/1000.f; 
}

bool Game::isMovePossible(Position newPos)
{
	//checks if number is between 1-8 / 1-16
	if (newPos.x - 1 <= 8 - 1 && newPos.y - 1 <= 16 - 1 && 
		newPos.x > 0 && newPos.y > 0) //TODO also check if matrix position is free
	{
		return true;
	}
	return false;
}


bool Game::isMoveFinal(Position newPos)
{
	if (newPos.y == 16) //or directly above other piece
		return true;

	return false;
}

bool Game::isQuit()
{
	return quit;
}

