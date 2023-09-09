#pragma once
//#include "Game.h"  //can't make include loop
#include <random>

#define GRID_STEPS 4
#define GRID_SIZE 16
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



class Piece
{
public:
	Piece();
	//~Piece();

	int id = 0;  //4 different pieces {0-3} not active = -1
	Position pos;
	bool connected = false;
	int countrFinal = 0;	//counter time before piece is set

	int random(int low, int high);

	void bufferMove();
	void bufferMove(int n);

	int getGridSize();
	Position getRealPos();
	//void reset();
	void burst();
	void clone(Piece clone);
private:
	int screenMultiplier;
	int rel_to_ScreenGridSize;
};

