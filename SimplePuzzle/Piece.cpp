#include "Piece.h"


std::random_device rd;
std::mt19937 gen(rd()); //random_engine

Piece::Piece()
{
	pos = {}; //{1,1} - {8,16}
	pos.x = random(1, 8);
	pos.y = random(1, 16); //FIXME should always be top of grid
	//color = { 100, 0, 0, 1 };
	screenMultiplier = 2;
	rel_to_ScreenGridSize = GRID_SIZE * screenMultiplier; //if there is a screen size change this should change
}

int Piece::random(int low, int high)
{ //tested works
	std::uniform_int_distribution<> dist(low, high);
	return dist(gen);
}


void Piece::bufferMove()
{
	++pos.buffer;
	if (pos.buffer >= 16)
	{
		pos.buffer = 0;
		++pos.y;
	}

	if (pos.y == 16)
		pos.buffer = 0;
}

void Piece::bufferMove(int n)
{
	++pos.buffer;
	if (pos.buffer >= 16)
	{
		pos.buffer = 0;
		++pos.y;
	}
	--n;
	if (n > 0)
		bufferMove(n);

	if (pos.y == 16)
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
		pos.x * getGridSize(),
		pos.y * getGridSize() + pos.buffer * screenMultiplier
	};
	return realPos;
}

void Piece::clone(Piece clone) 
{
	id = clone.id;
	pos = clone.pos;
}
