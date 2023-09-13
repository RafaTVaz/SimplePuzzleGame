#include "PlayerPiece.h"


void PlayerPiece::reset()
{
	id = random(0, 3);
	pos = {}; //{1,1} - {8,16}
	pos.x = random(1, 6);
	pos.y = -1;//random(1, 16); //FIXME should always be top of grid
	connected = true;
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
*	this specific override is not really needed but could be useful for polishing smoother falling
*/
void PlayerPiece::bufferMove() 
{//14 because 16 in already beginning of next position
	//otherPiece.bufferMove();

	pos.buffer += jumpStep;
	if (pos.buffer > 16 - jumpStep)
	{
		pos.buffer = 0;
		++pos.y;
	}

	if (pos.y == 15)
		pos.buffer = 0;
}

void PlayerPiece::bufferMove(int n)
{
	//otherPiece.bufferMove(n);
	pos.buffer += jumpStep;
	if (pos.buffer > (16 - jumpStep))
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

//@returns real on screen pixel position
Position PlayerPiece::getRealPos()
{
	if (animPos.animType > 0)
	{
		return realPositions[0];
	}
	return Piece::getRealPos();
	
}

//@returns real on screen pixel position
Position PlayerPiece::getOtherRealPos()
{

	if (animPos.animType >= 0)
	{
		return realPositions[1];
	}
	return otherPiece.getRealPos();

}


void PlayerPiece::initAnimPos()
{
	animPos.animType = -1;
	animPos.ctr = 0;
}

void PlayerPiece::setNewAnimPos()
{
	animPos.animType = 0;
	animPos.ctr = 0;
}
void PlayerPiece::setNewAnimPos(int type)
{
	animPos.animType = type;
	animPos.ctr = 0;
}