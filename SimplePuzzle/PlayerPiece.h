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
	Piece otherPiece;

private:
};


