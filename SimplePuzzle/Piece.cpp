#include "Piece.h"


std::random_device rd;
std::mt19937 gen(rd()); //random_engine

Piece::Piece()
{
	id = random(0, 3);
	pos = {}; //{1,1} - {8,16}
	
	screenMultiplier = 2;
	rel_to_ScreenGridSize = GRID_SIZE * screenMultiplier; //if there is a screen size change this should change
}


int Piece::random(int low, int high)
{ //tested works
	std::uniform_int_distribution<> dist(low, high);
	return dist(gen);
}

/**
*	Because of Pixel art speed nedds to be locked to pixels and divisible by 16
*	jumps need to be 2 by 2 NOT SURE 
*
*/
void Piece::bufferMove()
{//14 because 16 in already beginning of next position
	pos.buffer+=2;
	if (pos.buffer > 14)
	{
		pos.buffer = 0;
		++pos.y;
	}

	if (pos.y == 15)
		pos.buffer = 0;
}

void Piece::bufferMove(int n)
{
	pos.buffer += 2;
	if (pos.buffer >= 15 - 2*n)
	{
		pos.buffer = 0;
		++pos.y;
		return;
	}
	--n;
	if (n > 0)
		bufferMove(n);

	if (pos.y == 15)
		pos.buffer = 0;
}

int Piece::getGridSize()
{
	rel_to_ScreenGridSize = GRID_SIZE * screenMultiplier;
	return rel_to_ScreenGridSize;
}

Position Piece::getRealPos()
{
	Position realPos{
		(pos.x+1) * getGridSize(),
		(pos.y+1) * getGridSize() + pos.buffer * screenMultiplier
	};
	return realPos;
}

void Piece::clone(Piece clone) 
{
	id = clone.id;
	pos = clone.pos;
	connected = false;
	//pos.buffer = 0;
}

void Piece::burst()
{
	id = -1;
	connected = false;
	countrFinal = 0;
}
