
#include "Game.h"
#include "Piece.h"
#include <iostream>


Game::Game()
{
	quit = false;
	gameReset = false;
	gameStart = false;
	timer.start();
	gameSpeed = dropSpeed.slow;
	currPlayState = playState.starting;
	softDropSpeed = dropSpeed.fast;
	currSound = 0;
	connectedPieces.clear();
	
	// Initialize each element to an empty/default state
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			pieceMatrix[i][j].id = -1; //Non existent
		}
	}
	testPiece.reset();
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

		/*case SDLK_x: //not used
			gameInput.rotateAnticlockwise = true; gameInput.pressed = true;
			break;*/

		case SDLK_m:
			sounds.mute = !sounds.mute;
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
	currSound = -1;
	//Handle events on queue
	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
			quit = true;
		
		checkKeyPress();

		if (gameInput.pressed)
			updatePlayerInput();
	}
	updateGame();
	updateRealPositions();
}

/**
* Looks at input given by the player, 
*	makes the basic changes and checks; changes game state if needed
*/
void Game::updatePlayerInput()
{
	testDrop = false;
/*PAUSE***************************/
	if (gameInput.pause)
	{
		if(timer.isPaused())
		{
			timer.unpause();

			currSound = sounds.start;
		}
		else
		{
			if(currPlayState != playState.starting && currPlayState != playState.gameOver)
			{
				timer.pause();

				currSound = sounds.start;
			}
		}
	}

	// move Piece is not paused and playable
	if (!timer.isPaused())
	{
/*START***************************/
		if (currPlayState == playState.starting || currPlayState == playState.gameOver)
		{
			if (gameInput.start)
				gameStart = true;
		}
		
		if (currPlayState == playState.playing || currPlayState == playState.finalMove)
		{
/*MOVE***************************/
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
					testPiece.animPos.animType = animTiming.move_left;

					if (currPlayState == playState.finalMove
						&& !isMoveFinal())
					{
						currPlayState = playState.playing;
					}
					currSound = sounds.move;
				}
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
					testPiece.animPos.animType = animTiming.move_right;

					if (currPlayState == playState.finalMove
						&& !isMoveFinal())
					{
						currPlayState = playState.playing;
					}
					currSound = sounds.move;
				}
			}
/*DROP***************************/
			else if (gameInput.hardDrop)
			{
				//what piece reaches a placed piece or floor first
				int y1 = testMaxFall(testPiece.pos);
				int y2 = testMaxFall(testPiece.otherPiece.pos);
				int dropY = (y1 < y2) ? y1 : y2;

				dropY--;
				testPiece.pos.y += dropY;
				testPiece.otherPiece.pos.y += dropY;
				testPiece.pos.buffer = testPiece.otherPiece.pos.buffer = 0;
			}
			else if (gameInput.softDrop && currPlayState != playState.finalMove)
			{
				testDrop = true;
				int y1 = testMaxFall(testPiece.pos);
				int y2 = testMaxFall(testPiece.otherPiece.pos);
				Position testPos = (y1 < y2) ? testPiece.pos : testPiece.otherPiece.pos;
			
				++testPos.y;
				if (isMovePossible(testPos))
				{//TODO make move down while pressed without pause
					//2, 4, 8 ( 16 % buffermove has to be 0 for smooth animation
					testPiece.bufferMove(softDropSpeed);
					testPiece.otherPiece.bufferMove(softDropSpeed);
				}
			}
/*ROTATE***************************/
			else if (gameInput.rotateClockwise)
			{
				//checks if piece can rotate freely or has to move to rotate

				int type = -1;
				Position pPivot = testPiece.pos;
				Position pRotate = testPiece.otherPiece.pos;
				Position pTest;
				testPiece.initAnimPos();

			//Above, move to Right 
				if (pPivot.y > pRotate.y)
				{
					type = animTiming.up_right;
					pTest = pPivot;
					pPivot.x++;
					pTest.x--;
					if (isSideMovePossible(pPivot))
					{
						testPiece.otherPiece.pos = pPivot;
						testPiece.setNewAnimPos(); 
						currSound = sounds.rotate;
					}
					else if(isSideMovePossible(pTest))
					{
						testPiece.otherPiece.pos = testPiece.pos;
						testPiece.pos.x--;
						testPiece.setNewAnimPos(type);
						currSound = sounds.rotate;
					}
				}
			//Right, move to Below 
				else if (pPivot.x < pRotate.x)
				{
					type = animTiming.right_down;
					pPivot.y++;
					pTest = pPivot;
					pTest.y++;
					if (isMovePossible(pPivot) && isDownMovePossible(pPivot))
					{
						testPiece.otherPiece.pos = pPivot;
						testPiece.setNewAnimPos(); 
						currSound = sounds.rotate;
					}
					else
					{
						testPiece.otherPiece.pos = testPiece.pos;
						testPiece.pos.y--;
						testPiece.setNewAnimPos(type);
						currSound = sounds.rotate;
					}
				}
			//Below, move to Left 
				else if (pPivot.y < pRotate.y)
				{
					type = animTiming.down_left;
					pTest = pPivot;
					pPivot.x--;
					pTest.x++;
					if (isSideMovePossible(pPivot))
					{
						testPiece.otherPiece.pos = pPivot;
						testPiece.setNewAnimPos(); 

						currSound = sounds.rotate;
					}
					else if(isSideMovePossible(pTest))
					{
						testPiece.otherPiece.pos = testPiece.pos;
						testPiece.pos.x++;
						testPiece.setNewAnimPos(type);
						currSound = sounds.rotate;
					}
				}
			//Left, move to Above 
				else if (pPivot.x > pRotate.x)
				{
					type = animTiming.left_up;
					pPivot.y--;
					if(isMovePossible(pPivot) )
					{
						testPiece.otherPiece.pos = pPivot;
						testPiece.setNewAnimPos(); 
						currSound = sounds.rotate;
					}
					else
					{
						testPiece.otherPiece.pos = testPiece.pos;
						testPiece.pos.y++;
						testPiece.setNewAnimPos(type);
						currSound = sounds.rotate;
					}
				}
			}
		}
	}
}

/**
* Checks current gameState and acts accordingly
* does everything that is automated, with or without player input
*/
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
	case playState.starting:
		if(gameStart)
		{
			resetGame();
			fillMatrix();
			currPlayState = playState.waiting; //blocks fall 
			gameStart = false;
			currSound = sounds.start;
		}
		
		break;
	case playState.gameOver:
		if (gameStart)
		{
			resetScore();
			currPlayState = playState.starting;
			
		}
		
		break;
	/**
	* looks at playerPiece and checks 
	* 1: if Piece can be placed (finalMove) 
	* 2: if it's time to animate (buffer fall)
	*/
	case playState.playing:
		if (isMoveFinal() )
		{
			currPlayState = playState.finalMove;
			animTiming.elapsedTime = 0;
			currSound = sounds.pop;
		}

		//BUFFER MOVE
		else if (animTiming.elapsedTime >= 1000.f / (gameSpeed*2))
		{// if elapsed time > 1/2 second 
			Position temp = testPiece.pos;
			++temp.y;
			if (isMovePossible(temp))
			{
				testPiece.animPos.animType = animTiming.buffer;
				testPiece.bufferMove(1);
				testPiece.otherPiece.bufferMove(1);
			}
			animTiming.elapsedTime = 0;
		}

		break;
	/**
	* FINAL DECISION TIME BEFORE PIECE BEING PLACED
	*	if time(fps_finalDecision) for final decision has passed
	*	checks if game over:
	*	if not, piece is placed inside matrix (clone)
	*/
	case playState.finalMove:
		if (animTiming.elapsedTime > 1000.f / animTiming.fps_finalDecision)
		{
			if (isGameOver())
			{
				currPlayState = playState.gameOver;
				animTiming.elapsedTime = 0;
				currSound = sounds.lost;
				break;
			}
			testPiece.connected = false;
			currPlayState = playState.waiting;
			//prepare matrix for burst check
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 16; j++) {
					pieceMatrix[i][j].connected = false;
				}
			}
			//save piece in Matrix
			pieceMatrix[testPiece.pos.x][testPiece.pos.y].clone(testPiece);
			pieceMatrix[testPiece.otherPiece.pos.x][testPiece.otherPiece.pos.y].clone(testPiece.otherPiece);
			animTiming.elapsedTime = 0;

			currSound = sounds.set;
		}
		break;
	/**
	* checks if cluster of pieces need to be burst
	*	loops with playState.waiting to keep burstin after pieces fall
	*/
	case playState.bursting:
		burstEnd = true;

		if (searchCluster()) { //if there is a cluster of 4+ pieces
			burstEnd = false;
			addBurstPoints();
			destroyConnected();
			currPlayState = playState.waiting;
			currSound = sounds.burst;
			animTiming.screenShake += 5;
		}
			
		if (burstEnd) //if no cluster is found
		{
			if (gameReset) 
			{
				gameReset = false;
				resetScore();
				timer.start();
				currPlayState = playState.playing;
				break;
			}
			//calculate all points and update
			testPiece.reset();
			score.burstScore = score.currBurstPoints;
			addToScore();

			/**
			* speed up after speedUpTime (15) seconds
			* also drops a line of pieces
			* goes to playState.waiting because line needs to fall
			*/
			if (timer.getTicks() / 1000 > dropSpeed.speedUpTime) 
			{
				dropSpeed.speedUpTime += dropSpeed.speedUpTick;
				fillLine();
				currSound = sounds.wave;

				if (gameSpeed < dropSpeed.impossible) //if <max speed
				{
					gameSpeed++;
					/*if (gameSpeed > dropSpeed.fast) 
						dropSpeed.speedUpTick *= 2;*/
				}
				animTiming.elapsedTime = 0;
				currPlayState = playState.waiting;
				break;
			}

			currPlayState = playState.playing;
			animTiming.elapsedTime = 0;
		}

		break;
	/**
	* Updates buffer and positions of matrix pieces until they can no longer fall
	*	goes playState.bursting after all pieces are set
	*/
	case playState.waiting:

		bool falling = false;
		bool animated = false;
		int a, b;
		Position temp;
		//start from the bottom and go up
		for (int j = 15; j > 0; j--) {
			for (int i = 0; i < 8; i++) {
				if (pieceMatrix[i][j].id == -1 && pieceMatrix[i][j - 1].id >= 0)
				{
					temp = pieceMatrix[i][j - 1].pos;
					falling = true;
					//BUFFER MOVE
					if (animTiming.elapsedTime >= 1000.f / 60)//animTiming.elapsedTime >= 1000.f / animTiming.fps_falling)
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
							}//else?
						}
					}
				}
			}
		}

		if(animated)
		{
			animTiming.elapsedTime = 0;
		}

		if (!falling && animTiming.elapsedTime >= 1000.f / 6)
		{
			currPlayState = playState.bursting;
		}
		break;
	}
}


//after starting fresh or game over
void Game::resetGame() 
{
	timer.start();
	gameSpeed = dropSpeed.slow;
	softDropSpeed = 2;
	connectedPieces.clear();

	// Initialize each element to an empty/default state
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			pieceMatrix[i][j].id = -1; //Non existent
		}
	}
	testPiece.reset();
	gameReset = true;
}

//fills top of the matrix with <=40 pieces to start of the game
void Game::fillMatrix()
{
	int maxPieces = 40;
	int ctr = 0;
	// Initialize each element to an empty/default state
	for (int j = 1; j < 16; j++) {
		for (int i = 0; i < 8; i++) {
			if(pieceMatrix[i][j].id == -1)
				pieceMatrix[i][j].id = testPiece.random(-1, 3); //nonexistent or colored piece
			if (pieceMatrix[i][j].id != -1)
			{
				pieceMatrix[i][j].pos = Position{ i, j };
				ctr++;
			}
			if (ctr >= maxPieces)
				goto endofloop;
		}
	}
endofloop:;
}

//fills top line with pieces (not line 0 to keep that free for player to move)
void Game::fillLine()
{
	int j = 1;
	// Initialize each element to an empty/default state
	for (int i = 0; i < 8; i++) {
		if (pieceMatrix[i][j].id == -1)
			pieceMatrix[i][j].id = testPiece.random(-1, 3); //nonexistent or colored piece
		if (pieceMatrix[i][j].id != -1)
		{
			pieceMatrix[i][j].pos = Position{ i, j };
		}
	}
}

/**
* Checks buffer, player input and position to put the pieces in their correct position on screen
* in game only 16x8 positions exist; this would lead to very choppy gameplay
* needs to draw sprites in different position to give the game a smooth look
*/
void Game::updateRealPositions()
{
	if(testPiece.animPos.animType >= 0)
	{
	/*****************Animation Timers***********************/
		Uint32 currTime = timer.getTicks();
		Uint32 deltaTime = currTime - testPiece.animPos.lastTime;

		testPiece.animPos.lastTime = currTime;
		testPiece.animPos.elapsedTime += deltaTime;

/****Rotation***********************/
	/**
	* rotates other piece smoothly around playerpiece
	* maxCounter is number of positions the rotation has
	* writes to realPos because the animation is only for the renderer, 
	* game does not care about animations only positions and buffer
	* 
	* Rotate 90º clockwise, no point in rotating anticlockwise, 
	*  game does not become more fun, just more buttons
	*/

	//rotation worked normally
		if (testPiece.animPos.animType == 0)
		{
			int* counter = &testPiece.animPos.ctr;
			double maxCounter = testPiece.animPos.max;

			/*****************Animation Calculation***********************/
			if (testPiece.animPos.elapsedTime >= 1000.f / 60)
			{
				int otherX, otherY, pivotX, pivotY;
				int radius = 16; //16*2
				double angle, angRad;

				angle = -90.f + (90 / maxCounter) * (*counter);
				angRad = M_PI * (angle / 180);

				Position pivot, rotPoint;
				pivot = testPiece.pos;
				rotPoint = testPiece.otherPiece.pos;

				//Real on screen position
				pivotX = (pivot.x + 1) * testPiece.getGridSize();
				pivotY = (pivot.y + 1) * testPiece.getGridSize() + testPiece.getRealBuffer();

				otherX = (rotPoint.x + 1) * testPiece.getGridSize();
				otherY = (rotPoint.y + 1) * testPiece.getGridSize() + testPiece.getRealBuffer();

				int x = cos(angRad) * (otherX - pivotX)
					- sin(angRad) * (otherY - pivotY) + pivotX;

				int y = sin(angRad) * (otherX - pivotX)
					+ cos(angRad) * (otherY - pivotY) + pivotY;
				// added to both coordinates   -------^

				Position realPos{ x,y };
				testPiece.realPositions[0] = testPiece.getRealPos();
				testPiece.realPositions[1] = realPos;
				*counter = *counter + 1;
				testPiece.animPos.elapsedTime = 0;
			}

			if (*counter >= maxCounter)
			{
				testPiece.animPos.animType = -1;
				*counter = 0;
			}
		}
	//obstruction in roation
		else if(testPiece.animPos.animType > 0 && testPiece.animPos.animType < 5) 
		{
			int* counter = &testPiece.animPos.ctr;
			double maxCounter = 3; //  animations need to be int when   60/frames -> 60/4=15ms
			int x, y;
		//Up to Right
			if (testPiece.animPos.animType == animTiming.up_right)
			{//orig shift  left | other shift down
				if (testPiece.animPos.elapsedTime >= 1000.f / 60)
				{
					//shift = 3/4 | 2/4 | 1/4 | 0/4  per tick
					int shift = ((maxCounter - *counter) / (maxCounter + 1)) * testPiece.getGridSize();
					//testPiece
					x = (testPiece.pos.x + 1) * testPiece.getGridSize() + shift;
					y = (testPiece.pos.y + 1) * testPiece.getGridSize() + testPiece.getRealBuffer();
					
					Position realPos{ x,y };
					testPiece.realPositions[0] = realPos;

					//otherPiece
					x = (testPiece.otherPiece.pos.x + 1) * testPiece.getGridSize(); 
					y = (testPiece.otherPiece.pos.y + 1) * testPiece.getGridSize() - shift
						+ testPiece.getRealBuffer();

					Position realOtherPos{ x,y };
					testPiece.realPositions[1] = realOtherPos;

					*counter = *counter + 1;
					testPiece.animPos.elapsedTime = 0;
				}

				if (*counter >= maxCounter)
				{
					testPiece.animPos.animType = -1;
					*counter = 0;
				}
			}
		//Right to Down
			else if (testPiece.animPos.animType == animTiming.right_down)
			{//orig shift up | other shift left
				if (testPiece.animPos.elapsedTime >= 1000.f / 60)
				{
					//shift = 3/4 | 2/4 | 1/4 | 0/4  per tick
					int shift = ((maxCounter - *counter) / (maxCounter + 1)) * testPiece.getGridSize();
					//testPiece
					x = (testPiece.pos.x + 1) * testPiece.getGridSize() ;
					y = (testPiece.pos.y + 1) * testPiece.getGridSize() + shift + testPiece.getRealBuffer();

					Position realPos{ x,y };
					testPiece.realPositions[0] = realPos;

					//otherPiece
					x = (testPiece.otherPiece.pos.x + 1) * testPiece.getGridSize() + shift;
					y = (testPiece.otherPiece.pos.y + 1) * testPiece.getGridSize() 
						+ testPiece.getRealBuffer();

					Position realOtherPos{ x,y };
					testPiece.realPositions[1] = realOtherPos;

					*counter = *counter + 1;
					testPiece.animPos.elapsedTime = 0;
				}

				if (*counter >= maxCounter)
				{
					testPiece.animPos.animType = -1;
					*counter = 0;
				}
			}
		//Down to Left
			else if (testPiece.animPos.animType == animTiming.down_left)
			{//orig shift right | other shift up
				if (testPiece.animPos.elapsedTime >= 1000.f / 60)
				{
					//shift = 3/4 | 2/4 | 1/4 | 0/4  per tick
					int shift = ((maxCounter - *counter) / (maxCounter + 1)) * testPiece.getGridSize();
					//testPiece
					x = (testPiece.pos.x + 1) * testPiece.getGridSize() - shift;
					y = (testPiece.pos.y + 1) * testPiece.getGridSize() + testPiece.getRealBuffer();

					Position realPos{ x,y };
					testPiece.realPositions[0] = realPos;

					//otherPiece
					x = (testPiece.otherPiece.pos.x + 1) * testPiece.getGridSize();
					y = (testPiece.otherPiece.pos.y + 1) * testPiece.getGridSize() + shift
						+ testPiece.getRealBuffer();

					Position realOtherPos{ x,y };
					testPiece.realPositions[1] = realOtherPos;

					*counter = *counter + 1;
					testPiece.animPos.elapsedTime = 0;
				}

				if (*counter >= maxCounter)
				{
					testPiece.animPos.animType = -1;
					*counter = 0;
				}
			}
		//Left to Up
			else if (testPiece.animPos.animType == animTiming.left_up)
			{//orig shift down | other shift right
				if (testPiece.animPos.elapsedTime >= 1000.f / 60)
				{
					//shift = 3/4 | 2/4 | 1/4 | 0/4  per tick
					int shift = ((maxCounter - *counter) / (maxCounter + 1)) * testPiece.getGridSize();
					//testPiece
					x = (testPiece.pos.x + 1) * testPiece.getGridSize();
					y = (testPiece.pos.y + 1) * testPiece.getGridSize() - shift + testPiece.getRealBuffer();

					Position realPos{ x,y };
					testPiece.realPositions[0] = realPos;

					//otherPiece
					x = (testPiece.otherPiece.pos.x + 1) * testPiece.getGridSize() - shift;
					y = (testPiece.otherPiece.pos.y + 1) * testPiece.getGridSize()
						+ testPiece.getRealBuffer();

					Position realOtherPos{ x,y };
					testPiece.realPositions[1] = realOtherPos;

					*counter = *counter + 1;
					testPiece.animPos.elapsedTime = 0;
				}

				if (*counter >= maxCounter)
				{
					testPiece.animPos.animType = -1;
					*counter = 0;
				}
			}
		}
/****Buffer Move***********************/
	/**
	* for buffer not to feel choppy there needs to be a position interpolation
	* with pixel art movement needs to be pixel by pixel
	*	can't use interpolator but need to calculate how many pixels are moved
	*/
		else if(testPiece.animPos.animType == animTiming.buffer) //buffer Move
		{
			int* counter = &testPiece.animPos.ctr;
			double maxCounter = 4;
			int x, y;
			if (testPiece.animPos.elapsedTime >= 1000.f / 60)
			{
				//shift = 7/8 | 6/8 | 5/8 | 4/8 etc per tick
				int shift = (maxCounter - (*counter * 2))  * testPiece.getScreenMultiplier();
				//testPiece
				x = (testPiece.pos.x + 1) * testPiece.getGridSize();
				y = (testPiece.pos.y + 1) * testPiece.getGridSize() - shift + testPiece.getRealBuffer();

				Position realPos{ x,y };
				testPiece.realPositions[0] = realPos;

				//otherPiece
				x = (testPiece.otherPiece.pos.x + 1) * testPiece.getGridSize();
				y = (testPiece.otherPiece.pos.y + 1) * testPiece.getGridSize() - shift + testPiece.getRealBuffer();

				Position realOtherPos{ x,y };
				testPiece.realPositions[1] = realOtherPos;

				*counter = *counter + 1;
				testPiece.animPos.elapsedTime = 0;
			}
			if (*counter >= maxCounter)
			{
				testPiece.animPos.animType = -1;
				*counter = 0;
			}
		}
/****Left/Right Move***********************/
	//same thing that is happening to buffer move basically
		else if (testPiece.animPos.animType == animTiming.move_right) //buffer Move
		{
			int* counter = &testPiece.animPos.ctr;
			double maxCounter = 4;
			int x, y;
			if (testPiece.animPos.elapsedTime >= 1000.f / 60)
			{
				//shift = 7/8 | 6/8 | 5/8 | 4/8 etc per tick
				int shift = (maxCounter - (*counter * 2)) * testPiece.getScreenMultiplier();
				//testPiece
				x = (testPiece.pos.x + 1) * testPiece.getGridSize() - shift;
				y = (testPiece.pos.y + 1) * testPiece.getGridSize() + testPiece.getRealBuffer();

				Position realPos{ x,y };
				testPiece.realPositions[0] = realPos;

				//otherPiece
				x = (testPiece.otherPiece.pos.x + 1) * testPiece.getGridSize() - shift;
				y = (testPiece.otherPiece.pos.y + 1) * testPiece.getGridSize() + testPiece.getRealBuffer();

				Position realOtherPos{ x,y };
				testPiece.realPositions[1] = realOtherPos;

				*counter = *counter + 1;
				testPiece.animPos.elapsedTime = 0;
			}
			if (*counter >= maxCounter / 2)
			{
				testPiece.animPos.animType = -1;
				*counter = 0;
			}
		}
		else if (testPiece.animPos.animType == animTiming.move_left) //buffer Move
		{
			int* counter = &testPiece.animPos.ctr;
			//double maxCounter = testPiece.jumpStep / 2;
			double maxCounter = 4;
			int x, y;
			if (testPiece.animPos.elapsedTime >= 1000.f / 60)
			{
				//shift = 7/8 | 6/8 | 5/8 | 4/8 etc per tick
				//int shift = (maxCounter - (*counter * 2)) * testPiece.screenMultiplier;
				int shift = (maxCounter - (*counter * 2)) * testPiece.getScreenMultiplier();
				//testPiece
				x = (testPiece.pos.x + 1) * testPiece.getGridSize() + shift;
				y = (testPiece.pos.y + 1) * testPiece.getGridSize() + testPiece.getRealBuffer();

				Position realPos{ x,y };
				testPiece.realPositions[0] = realPos;

				//otherPiece
				x = (testPiece.otherPiece.pos.x + 1) * testPiece.getGridSize() + shift;
				y = (testPiece.otherPiece.pos.y + 1) * testPiece.getGridSize() + testPiece.getRealBuffer();

				Position realOtherPos{ x,y };
				testPiece.realPositions[1] = realOtherPos;

				*counter = *counter + 1;
				testPiece.animPos.elapsedTime = 0;
			}
			//if (*counter >= maxCounter / 2)
			if (*counter >= maxCounter )
			{
				testPiece.animPos.animType = -1;
				*counter = 0;
			}
		}
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

/**
* find connected pieces recursively
*	when returning adds one to counter
*	counter is the size of the cluster
* @returns cluster size
*/
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

/**
*	Burst:	4 pieces is 100, 5 = 200, 6 = 300 etc.
* 
*	if second burst in one move
*			4 pieces is 200, 5 = 400, 6 = 600 etc.
*/
void Game::addToScore()
{
	score.burstCount = 0;
	score.currScore += score.currBurstPoints;
	score.currBurstPoints = 0;

	if (score.currScore > score.highscore)
		score.highscore = score.currScore;
}

void Game::addBurstPoints()
{
	score.burstCount++;
	score.currBurstPoints += (connectedPieces.size() - 3) * 100 * score.burstCount;
}

//bursts all pieces in the cluster connectedPieces
void Game::destroyConnected()
{
	
	for (int i = 0; i < connectedPieces.size(); i++)
	{
		pieceMatrix[connectedPieces[i].pos.x][connectedPieces[i].pos.y].burst();
	}

	connectedPieces.clear();
}

void Game::resetScore()
{
	score.burstCount = 0;
	score.currBurstPoints = 0;
	score.burstScore = 0;
	score.currScore = 0;
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
		newPos.x >= 0 && newPos.y >= -1)				//MIN GRID
	{
		return true;
	}
	return false;
}

//@returns if position is free and in bounds
bool Game::isSideMovePossible(Position newPos)
{
	Position temp = newPos;
	if(isMovePossible( newPos))
	{//check sides
		if (newPos.y < 0 && newPos.buffer == 0)
			return true;
		if (newPos.buffer > 0)
			temp.y++;
		if (!isMatrixOccupied(temp))
			return true;
	}
	return false;
}

//@returns if position below is free & in bounds
bool Game::isDownMovePossible(Position newPos)
{
	if (isMoveFinal(newPos))
	{
		return false;
	}
	newPos.y++;
	if (isMatrixOccupied(newPos))
	{
		return false;
	}

	return true;
}


//@returns if player/other piece is end at of play area or has a piece below
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
//@returns if piece is end at of play area or has a piece below
bool Game::isMoveFinal(Position currPos)
{
	if (currPos.y == 15) //or directly above other piece
		return true;

	currPos.y++;
	if(isMatrixOccupied(currPos) && currPos.buffer == 0)
	{
		return true;
	}

	return false;
}

bool Game::isGameOver()
{
	if (isMoveFinal() && (testPiece.pos.y < 0 || testPiece.otherPiece.pos.y < 0))
		return true;

	return false;
}

//@returns how many free positions Piece has below
int Game::testMaxFall(Position newPos)
{
	int ctr = 0;
	int temp = 0;
	if (newPos.y < 0) { //keep deleted steps to add later
		temp = -1 * newPos.y;
		newPos.y = 0;
	}
	while (isMovePossible(newPos) && !isMatrixOccupied(newPos))
	{
		newPos.y++;
		ctr++;
	}

	return ctr+temp;
}




/****************Not Used*********************/
//void Game::bufferRealPosition(int i, int j)
//{
//	int* counter = &testPiece.animPos.ctr;
//	double maxCounter = testPiece.jumpStep / 2;
//	int x, y;
//
//	//shift = 7/8 | 6/8 | 5/8 | 4/8 etc per tick
//	int shift = (maxCounter - (*counter * 2)) * testPiece.screenMultiplier;
//	//testPiece
//	x = (pieceMatrix[i][j].pos.x + 1) * testPiece.getGridSize();
//	y = (pieceMatrix[i][j].pos.y + 1) * testPiece.getGridSize() - shift + pieceMatrix[i][j].getRealBuffer();
//
//	Position realPos{ x,y, 1 };
//	pieceMatrix[i][j].realPos = realPos;
//}