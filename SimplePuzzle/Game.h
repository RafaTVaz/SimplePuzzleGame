#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "Piece.h"
#include "PlayerPiece.h"
#include "Timer.h"

#define PLAY		1
#define FINAL_MOVE  2
#define BURST		3
#define WAIT		4

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

struct Speed
{
	int slow = 2;
	int normal = 2;
	int fast = 2;
};

struct GameState
{
	static const int playing = PLAY;	//play freely
	static const int finalMove = FINAL_MOVE;	//if down play ends, last second before burts
	static const int bursting = BURST;	//no play input allowed, exploding
	static const int waiting  = WAIT;	//burst happened, pieces are falling
};

struct Animation 
{
	static const int fps_falling = 6; //probs 6; changes with difficulty
	static const int fps_spriteAnim = 2;
	static const int fps_finalDecision = 3; 

	static const int line = 0;
	static const int up_right = 1;
	static const int right_down = 2;
	static const int down_left = 3;
	static const int left_up = 4;

	double elapsedTime = 0;
	double lastTime = 0;
};



class Game
{
public:
	Game();
	~Game();
	void checkKeyPress();
	void run();

	double getTimePassed();
	int getCurrentPlayState() { return currPlayState; };
	
	bool isMatrixOccupied(Position newPos);

	bool isQuit();

	PlayerPiece testPiece; //FIXME pieces should be private
	Piece pieceMatrix[8][16];
private:
	bool searchCluster();
	void resetConnected();
	int	 findGroupSize(int x, int y);
	void destroyConnected();
	void updateGame(); //every X seconds
	//void updatePlay(Piece* test); //Player input happened
	void updatePlayerInput(); //Player input happened

	void updateRealPositions();
	bool isMovePossible(Position newPos);
	bool isSideMovePossible(Position newPos);
	bool isDownMovePossible(Position newPos);
	bool isMoveFinal();
	bool isMoveFinal(Position currPos);
	int  testMaxFall(Position newPos);
	
	Timer timer;
	Speed dropSpeed;
	GameInput gameInput{};
	Animation animTiming;
	GameState playState;

	std::vector<Piece> connectedPieces;

	int linesBursted[7] = {};
	int currPlayState;
	int gameSpeed;
	int softDropSpeed;
	bool quit;


	SDL_Event event;
};

