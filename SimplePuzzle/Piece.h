#pragma once
//#include "Game.h"  //can't make include loop
#include <random>
#include <cmath>



#define GRID_STEPS	4
#define GRID_SIZE	16
#define M_PI		3.14159265358979323846

/*
*Position inside 8x16 grid,
* one grid part take X steps (GRID_STEPS) to get through
* 32 by 64 in case of GRID_STEPS == 4
* minus edges of texture of course
*/
struct Position
{
	int x, y;   //{1, 1} - {8, 16}
	int buffer; //0 - 15   0 means grid position, rest is animation
};

struct Color
{
	int r, g, b, a;
};

//use to animate from original pos to current
struct PositionTransition
{
	int ctr = 0;
	int max = 9; //10 frames of anim during rotation
	int animType = -1; //0: line | 1: 12->3 | 2: 3->6  | 3:  6->9 | 4: 9->12
	double lastTime = 0;
	double elapsedTime = 0;
};


class Piece
{
public:
	Piece();
	//~Piece();

	int id = 0;  //4 different pieces {0-3} not active = -1
	Position pos;
	//Position realPos;
	bool connected = false;
	int countrFinal = 0;	//counter time before piece is set
	int jumpStep = 8;

	int random(int low, int high);

	void bufferMove();
	void bufferMove(int n);

	int getGridSize();
	int getRealBuffer();
	Position getRealPos();
	
	//void reset();
	void burst();
	void clone(Piece clone);
	PositionTransition animPos = {};
	int screenMultiplier;
	int rel_to_ScreenGridSize;
protected:
};

