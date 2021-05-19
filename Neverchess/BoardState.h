#pragma once
#include "Chess.h"

static const int WHITE = 0;
static const int BLACK = 1;
static const int PIECE_CODE_LENGTH = 7;
static const int BOARD_LENGTH = 8;
static const int ANN_INPUT_LENGTH = BOARD_LENGTH * BOARD_LENGTH * PIECE_CODE_LENGTH
+ 1 // turn
+ 2 // white castles possible
+ 2 // black castles possible
+ 8; // en passant column

enum class PieceCode
{
	EMPTY = 0b0000000,
	W_KING = 0b0000001,
	W_QUEEN = 0b0000010,
	W_PAWN = 0b0000100,
	W_KNIGHT = 0b0001000,
	W_BISHOP = 0b0010000,
	W_ROOK = 0b0100000,
	B_KING = 0b1000001,
	B_QUEEN = 0b1000010,
	B_PAWN = 0b1000100,
	B_KNIGHT = 0b1001000,
	B_BISHOP = 0b1010000,
	B_ROOK = 0b1100000
};

struct MoveData
{
	PieceCode upgrade = PieceCode::EMPTY;
	bool shortCastle = false;
	bool longCastle = false;
	bool doublePawnMove = false;
	bool enPassant = false;
	int xStart = -1;
	int yStart = -1;
	int xEnd = -1;
	int yEnd = -1;
};

struct BoardState
{
	PieceCode _pieces[BOARD_LENGTH * BOARD_LENGTH] = { PieceCode::EMPTY };
	bool _turn = WHITE;
	bool _kingMoved[2] = { false, false };
	bool _kRookMoved[2] = { false, false };
	bool _qRookMoved[2] = { false, false };
	int _enPassant = -1;

	BoardState(const BoardState& other)
	{
		for (int y = 0; y < BOARD_LENGTH; ++y)
			for (int x = 0; x < BOARD_LENGTH; ++x)
				_pieces[y * BOARD_LENGTH + x] = other._pieces[y * BOARD_LENGTH + x];
		_turn = other._turn;
		_kingMoved[WHITE] = other._kingMoved[WHITE];
		_kingMoved[BLACK] = other._kingMoved[BLACK];
		_kRookMoved[WHITE] = other._kRookMoved[WHITE];
		_kRookMoved[BLACK] = other._kRookMoved[BLACK];
		_qRookMoved[WHITE] = other._qRookMoved[WHITE];
		_qRookMoved[BLACK] = other._qRookMoved[BLACK];
		_enPassant = other._enPassant;
	}

	bool operator==(const BoardState& other)
	{
		for (int y = 0; y < BOARD_LENGTH; ++y)
			for (int x = 0; x < BOARD_LENGTH; ++x)
				if (_pieces[y * BOARD_LENGTH + x] != other._pieces[y * BOARD_LENGTH + x])
					return false;

		return _turn == other._turn
			&& _kingMoved[WHITE] == other._kingMoved[WHITE] && _kingMoved[BLACK] == other._kingMoved[BLACK]
			&& _kRookMoved[WHITE] == other._kRookMoved[WHITE] && _kRookMoved[BLACK] == other._kRookMoved[BLACK]
			&& _qRookMoved[WHITE] == other._qRookMoved[WHITE] && _qRookMoved[BLACK] == other._qRookMoved[BLACK]
			&& _enPassant == other._enPassant;
	}
};
