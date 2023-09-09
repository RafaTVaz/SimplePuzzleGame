
#include "Game.h"
#include "Piece.h"
#include <iostream>


Game::Game()
{
	quit = false;
	timer.start();
	gameSpeed = dropSpeed.normal;
	currPlayState = playState.playing;
	softDropSpeed = dropSpeed.fast;
	connectedPieces.clear();
	
	// Initialize each element to an empty/default state
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			pieceMatrix[i][j].id = -1; //Non existent
		}
	}
	testPiece.reset();

	//printf("Piece id = %d     Other id = %d", testPiece.id, testPiece.otherPiece.id);
}

Game::~Game()
{
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

			case SDLK_z:
				gameInput.rotateClockwise = true; gameInput.pressed = true;
				break;

			case SDLK_x:
				gameInput.rotateAnticlockwise = true; gameInput.pressed = true;
				break;

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
			//printf("%d\n", testPiece.pos.buffer);
		}
	}
	updateGame();
	if (currPlayState == playState.bursting)
	{
		//updateGameMatrix(); //explode 4+ connects
		//updateScore();
		//testPiece.reset();
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

	// move Piece is not paused and playable
	if (!timer.isPaused() && (currPlayState == playState.playing || currPlayState == playState.finalMove))
	{
		if (gameInput.moveLeft)
		{
			Position testPos;
			//what Piece is most to the left or below
			if (testPiece.pos.x <= testPiece.otherPiece.pos.x && testPiece.pos.y >= testPiece.otherPiece.pos.y)
				testPos = testPiece.pos;
			else
				testPos = testPiece.otherPiece.pos;
			testPos.x--;

			if (isSideMovePossible(testPos))
			{
				testPiece.pos.x--;
				testPiece.otherPiece.pos.x--;

				if (currPlayState == playState.finalMove
					&& !isMoveFinal())
				{
					currPlayState = playState.playing;
				}
			}
			//printf("Left: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
		}
		else if (gameInput.moveRight)
		{
			Position testPos;
			//what Piece is most to the right or below
			if (testPiece.pos.x >= testPiece.otherPiece.pos.x && testPiece.pos.y >= testPiece.otherPiece.pos.y)
				testPos = testPiece.pos;
			else
				testPos = testPiece.otherPiece.pos;
			testPos.x++;

			if (isSideMovePossible(testPos))
			{
				testPiece.pos.x++;
				testPiece.otherPiece.pos.x++;

				if (currPlayState == playState.finalMove
					&& !isMoveFinal())
				{
					currPlayState = playState.playing;
				}
			}
			//printf("Right: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
		}
		else if (gameInput.hardDrop)
		{
			int y1 = testMaxFall(testPiece.pos);
			int y2 = testMaxFall(testPiece.otherPiece.pos);
			int dropY = (y1 < y2) ? y1 : y2;






			dropY--;
			testPiece.pos.y += dropY;
			testPiece.otherPiece.pos.y += dropY;
			testPiece.pos.buffer = testPiece.otherPiece.pos.buffer = 0;

			//printf("Up: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
		}
		else if (gameInput.softDrop && currPlayState != playState.finalMove)
		{
			int y1 = testMaxFall(testPiece.pos);
			int y2 = testMaxFall(testPiece.otherPiece.pos);
			Position testPos = (y1 < y2) ? testPiece.pos : testPiece.otherPiece.pos;
			
			++testPos.y;
			if (isMovePossible(testPos))
			{//FIXME make move down while pressed without pause
				//2, 4, 8 ( 16 % buffermove has to be 0 for smooth animation
				testPiece.bufferMove(softDropSpeed);
				testPiece.otherPiece.bufferMove(softDropSpeed);
			}
			//printf("Down: %d | %d \n", testPiece.pos.x, testPiece.pos.y);
		}
		else if (gameInput.rotateClockwise)
		{
			Position pPivot = testPiece.pos;
			Position pRotate = testPiece.otherPiece.pos;
			Position pTest;

			//Above, move to Right 
			if (pPivot.y > pRotate.y)
			{
				pTest = pPivot;
				pPivot.x++;
				pTest.x--;
				if (isSideMovePossible(pPivot))
				{
					testPiece.otherPiece.pos = pPivot;
				}
				else if(isSideMovePossible(pTest))
				{
					testPiece.otherPiece.pos = testPiece.pos;
					testPiece.pos.x--;
				}
			}
			//Right, move to Below 
			else if (pPivot.x < pRotate.x)
			{
				pPivot.y++;
				if (isMovePossible(pPivot) && !isMoveFinal(pPivot))
				{
					testPiece.otherPiece.pos = pPivot;
				}
				else
				{
					testPiece.otherPiece.pos = testPiece.pos;
					testPiece.pos.y--;
				}
			}
			//Below, move to Left 
			else if (pPivot.y < pRotate.y)
			{
				pTest = pPivot;
				pPivot.x--;
				pTest.x++;
				if (isSideMovePossible(pPivot))
				{
					testPiece.otherPiece.pos = pPivot;
				}
				else if(isSideMovePossible(pTest))
				{
					testPiece.otherPiece.pos = testPiece.pos;
					testPiece.pos.x++;
				}
			}
			//Left, move to Above 
			else if (pPivot.x > pRotate.x)
			{

				pPivot.y--;
				if(isMovePossible(pPivot))
				{
					testPiece.otherPiece.pos = pPivot;
				}
				else
				{
					testPiece.otherPiece.pos = testPiece.pos;
					testPiece.pos.y++;
				}
			}
		}
	}
}



void Game::updateGame()
{
	Uint32 currTime = timer.getTicks();
	Uint32 deltaTime = currTime - animTiming.lastTime;
	bool burstEnd = false;
	bool burst = false;
	

	animTiming.lastTime = currTime;
	animTiming.elapsedTime += deltaTime;

	switch (currPlayState)
	{
	case playState.playing:
		if (isMoveFinal() )
		{
			currPlayState = playState.finalMove;
			animTiming.elapsedTime = 0;
		}

		//BUFFER MOVE
		else if (animTiming.elapsedTime >= 1000.f / animTiming.fps_falling // if elapsed time > 1/2 second
			&& !gameInput.softDrop)
		{
			Position temp = testPiece.pos;
			++temp.y;
			if (isMovePossible(temp))
				testPiece.bufferMove();

			animTiming.elapsedTime = 0;
		}

		break;
	case playState.finalMove:

		//FINAL DECISION TIME BEFORE PIECE BEING PLACED
		if (animTiming.elapsedTime > 1000.f / animTiming.fps_finalDecision)
		{
			currPlayState = playState.waiting;
			//prepare matrix for burst check
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 16; j++) {
					pieceMatrix[i][j].connected = false;
				}
			}
			//printf("FINALMOVE!!!: %f | %d \n", animTiming.elapsedTime, animTiming.fps_finalDecision);

			//save piece in Matrix
			pieceMatrix[testPiece.pos.x][testPiece.pos.y].clone(testPiece);
			pieceMatrix[testPiece.otherPiece.pos.x][testPiece.otherPiece.pos.y].clone(testPiece.otherPiece);
			animTiming.elapsedTime = 0;
		}
		
		break;

	case playState.bursting:

		burstEnd = true;
		burst = false;


		if (searchCluster()) {
			burstEnd = false;
			destroyConnected();
			currPlayState = playState.waiting;
		}
			
		


		if (burstEnd)
		{
			//calculate all points and update
			//std::fill(linesBursted, linesBursted + 7, 0);
			//printf("BURSTEND\n");
			currPlayState = playState.playing;
			testPiece.reset();
			animTiming.elapsedTime = 0;
		}
		break;

	case playState.waiting:

		bool falling = false;
		bool animated = false;
		Position temp;
		//start from the bottom and go up
		for (int i = 0; i < 8; i++) {
			for (int j = 15; j > 0; j--) {
				if (pieceMatrix[i][j].id == -1 && pieceMatrix[i][j - 1].id >= 0)
				{
					temp = pieceMatrix[i][j - 1].pos;
					falling = true;
					//BUFFER MOVE
					if (animTiming.elapsedTime >= 1000.f / 30)//animTiming.elapsedTime >= 1000.f / animTiming.fps_falling)
					{
						animated = true;
						++temp.y;
						if (isMovePossible(temp))
						{//this makes it fall like sand in minecraft, line by line. looks cute 
							pieceMatrix[i][j - 1].bufferMove();
							if (pieceMatrix[i][j - 1].pos.buffer == 0)
							{
								pieceMatrix[i][j].clone(pieceMatrix[i][j - 1]);
								pieceMatrix[i][j - 1].burst();
							}
						}

					}
				}
			}
		}

		if(animated)
			animTiming.elapsedTime = 0;

		if (!falling && animTiming.elapsedTime >= 1000.f / 6)
			currPlayState = playState.bursting;
		

		break;
	}
}

/**
* search matrix for pieces, recursively count connected pieces
* @returns if cluster (4+ Pieces together) was found
*/
bool Game::searchCluster() 
{
	int curr_id = 0;
	int groupSize = 0;

	//search matrix for pieces, recursively count connected pieces
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {

			curr_id = pieceMatrix[i][j].id;
			if (curr_id >= 0 && !pieceMatrix[i][j].connected)
			{
				connectedPieces.clear();
				//find recursively 
				groupSize = findGroupSize(i, j);
				//printf("Size: %d\n", groupSize);
				if (groupSize >= 4)
				{
					resetConnected();
					return true;

				}
			}

		}
	}
	resetConnected();
	connectedPieces.clear();
	return false;
}

void Game::resetConnected()
{
	//reset matrix
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			if (pieceMatrix[i][j].connected)
			{
				pieceMatrix[i][j].connected = false;
			}
		}
	}
}

int Game::findGroupSize(int x, int y)
{
	int n = 1;
	int id = pieceMatrix[x][y].id;
	pieceMatrix[x][y].connected = true;
	Position temp = {};
		temp.x = x; temp.y = y;

	connectedPieces.push_back(pieceMatrix[x][y]);

	//clockwise search; id = id is same color
	
	/*******UP*******/
	temp.x = x, temp.y = y-1;
	if (isMovePossible(temp) && isMatrixOccupied(temp) &&
		pieceMatrix[temp.x][temp.y].id == id && !pieceMatrix[temp.x][temp.y].connected)
	{
		
		pieceMatrix[temp.x][temp.y].connected = true;
		n += findGroupSize(temp.x, temp.y);
	}

	/*******RIGHT*******/
	temp.x = x + 1, temp.y = y; 
		if (isMovePossible(temp) && isMatrixOccupied(temp) &&
			pieceMatrix[temp.x][temp.y].id == id && !pieceMatrix[temp.x][temp.y].connected)
		{

			pieceMatrix[temp.x][temp.y].connected = true;
			n += findGroupSize(temp.x, temp.y);
		}

	/*******DOWN*******/
	temp.x = x, temp.y = y + 1;
	if (isMovePossible(temp) && isMatrixOccupied(temp) &&
		pieceMatrix[temp.x][temp.y].id == id && !pieceMatrix[temp.x][temp.y].connected)
	{

		pieceMatrix[temp.x][temp.y].connected = true;
		n += findGroupSize(temp.x, temp.y);
	}

	/*******LEFT*******/
	temp.x = x - 1, temp.y = y;
	if (isMovePossible(temp) && isMatrixOccupied(temp) &&
		pieceMatrix[temp.x][temp.y].id == id && !pieceMatrix[temp.x][temp.y].connected)
	{

		pieceMatrix[temp.x][temp.y].connected = true;
		n += findGroupSize(temp.x, temp.y);
	}

	return n;
}

void Game::destroyConnected()
{
	printf("DESTROY CONNECTED\n");

	//burst all pieces in the cluster
	for (int i = 0; i < connectedPieces.size(); i++)
	{
		pieceMatrix[connectedPieces[i].pos.x][connectedPieces[i].pos.y].burst();
	}

	connectedPieces.clear();
	
	
}

//returns timer in seconds
double Game::getTimePassed()
{
	return timer.getTicks()/1000.f; 
}

bool Game::isMatrixOccupied(Position newPos)
{
	//if playpiece is in grid spot 5x7, when need to check matrix 5x8
	
	if (pieceMatrix[newPos.x][newPos.y].id >= 0)
		return true;
	return false;
}

/**
* Checks if in bounds
*	@returns if position is in grid
*/
bool Game::isMovePossible(Position newPos)
{
	Position temp = newPos;
	//checks if in play Grid
	if (newPos.x  <= 7  && newPos.y  <= 15  &&		//MAX GRID
		newPos.x >= 0 && newPos.y >= 0)				//MIN GRID
	{
		return true;
	}
	return false;
}

//checks if position is free and in bounds
bool Game::isSideMovePossible(Position newPos)
{
	Position temp = newPos;
	if(isMovePossible( newPos))
	{//check sides
		if (newPos.buffer > 0)
			temp.y++;
		if (!isMatrixOccupied(temp))
			return true;
	}
	return false;
}

//deprecated?
bool Game::isDownMovePossible(Position newPos)
{
	if (isMovePossible(newPos))
	{
		if ((newPos.buffer + gameSpeed) < 14)
			return true;
	}
	return false;
}

int Game::testMaxFall(Position newPos)
{
	int ctr = 0;
	while (isMovePossible(newPos) && !isMatrixOccupied(newPos))
	{
		newPos.y++;
		ctr++;
	}
	return ctr;
}

/**
*	Checks if piece is end at of play area or has a piece below
*/
bool Game::isMoveFinal()
{
	Position currPos = testPiece.pos;
	
	if (isMoveFinal(testPiece.otherPiece.pos))
		return true;

	if (currPos.y == 15) //or directly above other piece
		return true;

	/*if playpiece is in grid spot 5x7, when need to check matrix 5x8*/
	currPos.y++;
	if (isMatrixOccupied(currPos) && currPos.buffer == 0)
			return true;
	

	return false;
}

bool Game::isMoveFinal(Position currPos)
{
	if (currPos.y == 15) //or directly above other piece
		return true;


	currPos.y++;
	if(isMatrixOccupied(currPos) && currPos.buffer == 0)
	{
		/*printf("CurrPos: %d | %d    downPos: %d | %d   -  downMatrix: %d | %d\n", 
			testPiece.pos.x, testPiece.pos.y, testPiece.pos.x, testPiece.pos.y+1, 
			pieceMatrix[currPos.x][currPos.y + 1].pos.x, pieceMatrix[currPos.x][currPos.y + 1].pos.y);*/
		return true;
	}

	return false;
}



bool Game::isQuit()
{
	return quit;
}

