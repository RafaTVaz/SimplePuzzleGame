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
	int random(int low, int high);
	Position pos;
	//Color color;
	void bufferMove();
	int getGridSize();
	Position getRealPos();
private:
	int screenMultiplier;
	int rel_to_ScreenGridSize;
};

