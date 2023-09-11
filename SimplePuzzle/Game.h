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
};

struct Speed
{
	int slow = 2;
	int normal = 2;
	int fast = 2;
};

struct GameState
{
	static const int starting = 0;
	static const int playing  =	PLAY;	//play freely
	static const int finalMove= FINAL_MOVE;	//if down play ends, last second before burts
	static const int bursting =	BURST;	//no play input allowed, exploding
	static const int waiting  = WAIT;	//burst happened, pieces are falling
	static const int gameOver = 5;
};

struct Animation 
{
	static const int fps_falling = 1; //probs 6; changes with difficulty
	static const int fps_spriteAnim = 2;
	static const int fps_finalDecision = 3; 

	static const int line = 0;
	static const int up_right = 1;
	static const int right_down = 2;
	static const int down_left = 3;
	static const int left_up = 4;
	static const int buffer = 5;
	static const int move_left = 6;
	static const int move_right = 7;

	double elapsedTime = 0;
	double lastTime = 0;
};

struct Score {
	int highscore = 0;
	int currBurstPoints = 0;
	int currScore = 0;
	int burstCount = 0;
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
	int getHighscore() { return score.highscore; };
	int getBurstPoints() { return score.currBurstPoints; };
	int getScore() { return score.currBurstPoints; };
	
	bool isMatrixOccupied(Position newPos);

	bool isQuit();

	PlayerPiece testPiece; //FIXME pieces should be private
	Piece pieceMatrix[8][16];
	GameState playState;
private:
	void fillMatrix();
	bool searchCluster();
	void resetConnected();
	int	 findGroupSize(int x, int y);
	void addBurstPoints();
	void addToScore();
	void resetScore();
	void destroyConnected();
	void updateGame(); //every X seconds
	//void updatePlay(Piece* test); //Player input happened
	void updatePlayerInput(); //Player input happened

	void updateRealPositions();
	//void bufferRealPosition(int i, int j);
	bool isMovePossible(Position newPos);
	bool isSideMovePossible(Position newPos);
	bool isDownMovePossible(Position newPos);
	bool isMoveFinal();
	bool isMoveFinal(Position currPos);
	bool isGameOver();
	int  testMaxFall(Position newPos);
	void resetGame();
	
	Timer timer;
	Speed dropSpeed;
	GameInput gameInput{};
	Animation animTiming;
	Score score;

	std::vector<Piece> connectedPieces;

	int linesBursted[7] = {};
	int currPlayState;
	int gameSpeed;
	int softDropSpeed;
	bool quit;
	bool gameReset;
	bool gameStart;
	bool testDrop = false;


	SDL_Event event;
};

