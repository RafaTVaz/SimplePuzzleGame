#include "PlayerPiece.h"


void PlayerPiece::reset()
{
	id = random(0, 3);
	pos = {}; //{1,1} - {8,16}
	pos.x = random(1, 6);
	pos.y = 0;//random(1, 16); //FIXME should always be top of grid
	connected = false;
	countrFinal = 0;


	otherPiece.pos.x = pos.x + ((rand() % 2) == 0 ? -1 : 1);//left or right of PlayerPiece
	otherPiece.pos.y = pos.y;
	otherPiece.id = random(0, 3);
	
	otherPiece.connected = false;
	otherPiece.countrFinal = 0;
}

/**
*	Because of Pixel art speed nedds to be locked to pixels and divisible by 16
*	jumps need to be 2 by 2 NOT SURE
*
*/
void PlayerPiece::bufferMove() 
{//14 because 16 in already beginning of next position
	otherPiece.bufferMove();

	pos.buffer += 2;
	if (pos.buffer > 14)
	{
		pos.buffer = 0;
		++pos.y;
	}

	if (pos.y == 15)
		pos.buffer = 0;

	
}

void PlayerPiece::bufferMove(int n)
{
	pos.buffer += 2;
	if (pos.buffer >= 15 - 2 * n)
	{
		pos.buffer = 0;
		++pos.y;
		return;
	}
	--n;
	if (n > 0)
		bufferMove(n);

	if (pos.y == 15)
	{
		pos.buffer = 0;
	}
}