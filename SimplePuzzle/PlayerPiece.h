#pragma once

#include "Piece.h"



class PlayerPiece : public Piece
{
public:
	//PlayerPiece();
	using Piece::Piece;
	void reset();
	void bufferMove();
	void bufferMove(int n);
	Position getRealPos();
	Position getOtherRealPos();
	void initAnimPos();
	void setNewAnimPos();
	void setNewAnimPos(int type);

	Piece otherPiece;
	Position realPositions[2];
private:
};


