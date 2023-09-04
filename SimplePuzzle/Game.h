#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

#include "Piece.h"
#include "Timer.h"


struct GameInput
{
	bool pressed = false;
	bool start = false;
	bool moveLeft = false;
	bool moveRight = false;
	bool hardDrop = false; //up
	bool softDrop = false; //down
	bool rotateClockwise = false;			// TODO: Check going the right way!
	bool rotateAnticlockwise = false;		// TODO: Check going the right way!
	bool pause = false;

//#ifdef _DEBUG
//	bool debugChangePiece = false;
//	bool debugMoveLeft = false;
//	bool debugMoveRight = false;
//	bool debugMoveUp = false;
//	bool debugMoveDown = false;
//#endif
};

struct Animation 
{
	const int fps_falling = 6; //probs 6; changes with difficulty
	const int fps_spriteAnim = 2;

	Uint32 elapsedTime = 0;
	Uint32 lastTime = 0;
};


class Game
{
public:
	Game();
	void checkKeyPress();
	void run();
	void updateGame(); //every X seconds
	//void updatePlay(Piece* test); //Player input happened
	void updatePlay(); //Player input happened
	double getTimePassed();
	bool isMovePossible(Position newPos);

	bool isQuit();

	Piece testPiece; 
private:
	Animation animTiming;
	Timer timer;
	GameInput gameInput{}; //FIXME should be in private
	SDL_Event event;
	int gameSpeed = 1;
	bool quit;
};
