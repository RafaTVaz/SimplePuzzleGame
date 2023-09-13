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
	bool rotateClockwise = false;			
	bool rotateAnticlockwise = false;		// Not used, no point in complicating
	bool pause = false;
};

//speed values and aditional help for changing speeds
struct Speed
{
	static const int slow = 1;
	static const int normal = 2;
	static const int fast = 3;
	static const int ultra = 4;
	static const int impossible = 5;

	int speedUpTick = 15;	//seconds
	int speedUpTime= speedUpTick;
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
{//fps here is frames in a second, so 3 = 0.333 of a second
	static const int fps_falling = 3;
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
	int screenShake = 0; //ammount of pixels to be moved
};

struct Score {
	int highscore = 0;
	int currBurstPoints = 0;
	int burstScore = 0;
	int currScore = 0;
	int burstCount = 0;

	bool newHighScore = false;
};

struct Sounds {
	static const int input = 0;
	static const int move = 1;
	static const int rotate = 2;
	static const int burst = 3;
	static const int start = 4;
	static const int set = 5;
	static const int pop = 6;
	static const int wave = 7;
	static const int lost = 8;

	bool mute = false;
};

class Game
{
public:
	Game();
	~Game();
	void checkKeyPress();
	void run();

	double getTimePassed() { return timer.getTicks() / 1000.f; };
	bool isTimePaused() { return timer.isPaused(); };
	int getCurrentPlayState() { return currPlayState; };
	int getHighscore() { return score.highscore; };
	int getBurstPoints() { return score.burstScore; };
	int getScore() { return score.currScore; };
	int getSound() { return currSound; };
	int getShake() { return animTiming.screenShake; };
	void lowerShake() { if(animTiming.screenShake>0)animTiming.screenShake--; };
	bool isMute() {return sounds.mute;};
	
	bool isMatrixOccupied(Position newPos);

	bool isQuit() {	return quit;};

	PlayerPiece testPiece;
	Piece pieceMatrix[8][16];
	GameState playState;
	Sounds sounds;
private:
	void fillMatrix();
	void fillLine();
	bool searchCluster();
	void resetConnected();
	int	 findGroupSize(int x, int y);
	void addBurstPoints();
	void addToScore();
	void resetScore();
	void destroyConnected();
	void updateGame(); 
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

	int currPlayState;
	int currSound;
	int gameSpeed;
	int softDropSpeed;
	bool quit;
	bool gameReset;
	bool gameStart;
	bool testDrop = false;


	SDL_Event event;
};

