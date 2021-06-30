#include "Chess.h"

int nvc::Chess::pieceCodeToSide(const PieceCode& pieceCode)
{
	return (int)pieceCode >> (PIECE_CODE_LENGTH - 1);
}

std::vector<nvc::MoveData> nvc::Chess::genRawMoves(const BoardState& boardStateData)
{
	std::vector<MoveData> moves;
	PieceCode piece;
	for (int y = 0; y < BOARD_LENGTH; ++y)
	{
		for (int x = 0; x < BOARD_LENGTH; ++x)
		{
			piece = boardStateData._pieces[y * BOARD_LENGTH + x];
			if (piece == PieceCode::EMPTY)
				continue;
			if (pieceCodeToSide(piece) != boardStateData._turn)
				continue;
			genRawPieceMoves(boardStateData, moves, x, y);
		}
	}
	return moves;
}

void nvc::Chess::genRawPieceMoves(const BoardState& boardState, std::vector<MoveData>& moves, int x, int y)
{
	switch (boardState._pieces[y * BOARD_LENGTH + x])
	{
	case PieceCode::EMPTY:
		break;
	case PieceCode::W_KING: case PieceCode::B_KING:
		genRawMovesKing(moves, boardState, x, y);
		break;
	case PieceCode::W_QUEEN: case PieceCode::B_QUEEN:
		genRawMovesQueen(moves, boardState, x, y);
		break;
	case PieceCode::W_ROOK: case PieceCode::B_ROOK:
		genRawMovesRook(moves, boardState, x, y);
		break;
	case PieceCode::W_BISHOP: case PieceCode::B_BISHOP:
		genRawMovesBishop(moves, boardState, x, y);
		break;
	case PieceCode::W_KNIGHT: case PieceCode::B_KNIGHT:
		genRawMovesKnight(moves, boardState, x, y);
		break;
	case PieceCode::W_PAWN: case PieceCode::B_PAWN:
		genRawMovesPawn(moves, boardState, x, y);
		break;
	}
}

void nvc::Chess::genRawMovesKing(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	int xDestination, yDestination, pieceIndex;
	for (int y = -1; y < 2; ++y)
	{
		for (int x = -1; x < 2; ++x)
		{
			if (x == 0 && y == 0)
				continue;

			xDestination = xPiece + x;
			yDestination = yPiece + y;

			if (xDestination < 0 || xDestination >= BOARD_LENGTH || yDestination < 0 || yDestination >= BOARD_LENGTH)
				continue;

			pieceIndex = yDestination * BOARD_LENGTH + xDestination;

			if (boardState._pieces[pieceIndex] != PieceCode::EMPTY && pieceCodeToSide(boardState._pieces[pieceIndex]) == boardState._turn)
				continue;

			MoveData move;
			move.xStart = xPiece;
			move.yStart = yPiece;
			move.xEnd = xDestination;
			move.yEnd = yDestination;
			moves.push_back(std::move(move));
		}
	}
	// Castles
	if (shortCastleAvailable(boardState))
	{
		MoveData move;
		move.shortCastle = true;
		move.xStart = 4;
		move.xEnd = 6;
		move.yStart = boardState._turn ? 7 : 0;
		move.yEnd = move.yStart;
		moves.push_back(std::move(move));
	}
	if (longCastleAvailable(boardState))
	{
		MoveData move;
		move.longCastle = true;
		move.xStart = 4;
		move.xEnd = 2;
		move.yStart = boardState._turn ? 7 : 0;
		move.yEnd = move.yStart;
		moves.push_back(std::move(move));
	}
}

void nvc::Chess::genRawMovesQueen(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	genMovesDir(moves, boardState, xPiece, yPiece, -1, 0);
	genMovesDir(moves, boardState, xPiece, yPiece, 0, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, 0);
	genMovesDir(moves, boardState, xPiece, yPiece, 0, -1);
	genMovesDir(moves, boardState, xPiece, yPiece, -1, -1);
	genMovesDir(moves, boardState, xPiece, yPiece, -1, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, -1);
}

void nvc::Chess::genRawMovesBishop(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	genMovesDir(moves, boardState, xPiece, yPiece, -1, -1);
	genMovesDir(moves, boardState, xPiece, yPiece, -1, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, -1);
}

void nvc::Chess::genRawMovesRook(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	genMovesDir(moves, boardState, xPiece, yPiece, -1, 0);
	genMovesDir(moves, boardState, xPiece, yPiece, 0, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, 0);
	genMovesDir(moves, boardState, xPiece, yPiece, 0, -1);
}

void nvc::Chess::genRawMovesKnight(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	int x = xPiece - 2;
	int y = yPiece - 1;
	const PieceCode* pieces = boardState._pieces;
	const int turn = boardState._turn;
	if (x > -1 && y > -1 && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || pieceCodeToSide(pieces[y * BOARD_LENGTH + x]) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	y = yPiece + 1;
	if (x > -1 && y < BOARD_LENGTH && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || pieceCodeToSide(pieces[y * BOARD_LENGTH + x]) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	x = xPiece + 2;
	if (x < BOARD_LENGTH && y < BOARD_LENGTH && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || pieceCodeToSide(pieces[y * BOARD_LENGTH + x]) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	y = yPiece - 1;
	if (x < BOARD_LENGTH && y < -1 && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || pieceCodeToSide(pieces[y * BOARD_LENGTH + x]) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	x = xPiece - 1;
	y = yPiece - 2;
	if (x > -1 && y > -1 && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || pieceCodeToSide(pieces[y * BOARD_LENGTH + x]) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	x = xPiece + 1;
	if (x < BOARD_LENGTH && y > -1 && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || pieceCodeToSide(pieces[y * BOARD_LENGTH + x]) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	y = yPiece + 2;
	if (x < BOARD_LENGTH && y < BOARD_LENGTH && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || pieceCodeToSide(pieces[y * BOARD_LENGTH + x]) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	x = xPiece - 1;
	if (x > -1 && y < BOARD_LENGTH && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || pieceCodeToSide(pieces[y * BOARD_LENGTH + x]) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
}

void nvc::Chess::genRawMovesPawn(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	const PieceCode* pieces = boardState._pieces;
	const int turn = boardState._turn;
	const int enPassant = boardState._enPassant;
	int yDir = turn ? -1 : 1;
	if (pieces[(yPiece + yDir) * BOARD_LENGTH + xPiece] == PieceCode::EMPTY)
	{
		// regular move
		MoveData baseMove;
		baseMove.xStart = xPiece;
		baseMove.yStart = yPiece;
		baseMove.xEnd = xPiece;
		baseMove.yEnd = yPiece + yDir;
		if ((turn && yPiece + yDir == 0) || (!turn && yPiece + yDir == BOARD_LENGTH - 1))
		{
			// upgrade if at the end of the board
			baseMove.upgrade = turn ? PieceCode::B_QUEEN : PieceCode::W_QUEEN;
			MoveData upgradeToKnight;
			upgradeToKnight.xStart = xPiece;
			upgradeToKnight.yStart = yPiece;
			upgradeToKnight.xEnd = xPiece;
			upgradeToKnight.yEnd = yPiece + yDir;
			upgradeToKnight.upgrade = turn ? PieceCode::B_KNIGHT : PieceCode::W_KNIGHT;
			moves.push_back(std::move(upgradeToKnight));
		}
		moves.push_back(std::move(baseMove));
		if ((!turn && yPiece == 1 || turn && yPiece == 6) && pieces[(yPiece + 2 * yDir) * BOARD_LENGTH + xPiece] == PieceCode::EMPTY)
		{
			// double move
			MoveData moveDouble;
			moveDouble.xStart = xPiece;
			moveDouble.yStart = yPiece;
			moveDouble.xEnd = xPiece;
			moveDouble.yEnd = yPiece + 2 * yDir;
			moveDouble.doublePawnMove = true;
			moves.push_back(std::move(moveDouble));
		}
	}
	if (xPiece > 0 && pieces[(yPiece + yDir) * BOARD_LENGTH + xPiece - 1] != PieceCode::EMPTY && ((int)pieces[(yPiece + yDir) * BOARD_LENGTH + xPiece - 1] >> (PIECE_CODE_LENGTH - 1)) != turn)
	{
		// take towards low x coord
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = xPiece - 1;
		move.yEnd = yPiece + yDir;
		if ((turn && yPiece + yDir == 0) || (!turn && yPiece + yDir == 7))
		{
			// upgrade if at the end of the board
			move.upgrade = turn ? PieceCode::B_QUEEN : PieceCode::W_QUEEN;
			MoveData upgradeToKnight;
			upgradeToKnight.xStart = xPiece;
			upgradeToKnight.yStart = yPiece;
			upgradeToKnight.xEnd = xPiece - 1;
			upgradeToKnight.yEnd = yPiece + yDir;
			upgradeToKnight.upgrade = turn ? PieceCode::B_KNIGHT : PieceCode::W_KNIGHT;
			moves.push_back(std::move(upgradeToKnight));
		}
		moves.push_back(std::move(move));
	}
	if (xPiece < BOARD_LENGTH - 1 && pieces[(yPiece + yDir) * BOARD_LENGTH + xPiece + 1] != PieceCode::EMPTY && ((int)pieces[(yPiece + yDir) * BOARD_LENGTH + xPiece + 1] >> (PIECE_CODE_LENGTH - 1)) != turn)
	{
		// take towards high x coord
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = xPiece + 1;
		move.yEnd = yPiece + yDir;
		if ((turn && yPiece + yDir == 0) || (!turn && yPiece + yDir == 7))
		{
			// upgrade if at the end of the board
			move.upgrade = turn ? PieceCode::B_QUEEN : PieceCode::W_QUEEN;
			MoveData upgradeToKnight;
			upgradeToKnight.xStart = xPiece;
			upgradeToKnight.yStart = yPiece;
			upgradeToKnight.xEnd = xPiece + 1;
			upgradeToKnight.yEnd = yPiece + yDir;
			upgradeToKnight.upgrade = turn ? PieceCode::B_KNIGHT : PieceCode::W_KNIGHT;
			moves.push_back(std::move(upgradeToKnight));
		}
		moves.push_back(std::move(move));
	}
	if (enPassant != -1)
	{
		if (turn && yPiece == 3 && (xPiece == enPassant - 1 || xPiece == enPassant + 1))
		{
			MoveData move;
			move.xStart = xPiece;
			move.yStart = yPiece;
			move.xEnd = enPassant;
			move.yEnd = yPiece + yDir;
			move.enPassant = true;
			moves.push_back(std::move(move));
		}
		else if (!turn && yPiece == 4 && (xPiece == enPassant - 1 || xPiece == enPassant + 1))
		{
			MoveData move;
			move.xStart = xPiece;
			move.yStart = yPiece;
			move.xEnd = enPassant;
			move.yEnd = yPiece + yDir;
			move.enPassant = true;
			moves.push_back(std::move(move));
		}
	}
}

void nvc::Chess::genMovesDir(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece, int xDir, int yDir)
{
	const PieceCode* pieces = boardState._pieces;
	const int turn = boardState._turn;
	int x = xPiece + xDir;
	int y = yPiece + yDir;
	while (x > -1 && x < BOARD_LENGTH && y > -1 && y < BOARD_LENGTH)
	{
		if (pieces[y * BOARD_LENGTH + x] != PieceCode::EMPTY)
		{
			if ((int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn)
			{
				MoveData lastMove;
				lastMove.xStart = xPiece;
				lastMove.yStart = yPiece;
				lastMove.xEnd = x;
				lastMove.yEnd = y;
				moves.push_back(std::move(lastMove));
			}
			break;
		}
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
		x += xDir;
		y += yDir;
	}
}

bool nvc::Chess::shortCastleAvailable(const BoardState& boardState)
{
	int row = boardState._turn ? 7 : 0;
	return !boardState._kingMoved[boardState._turn]
		&& !boardState._kRookMoved[boardState._turn]
		&& boardState._pieces[row * BOARD_LENGTH + 5] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 6] == PieceCode::EMPTY
		&& !squareThreatened(boardState._pieces, boardState._turn, 4, row)
		&& !squareThreatened(boardState._pieces, boardState._turn, 5, row)
		&& !squareThreatened(boardState._pieces, boardState._turn, 6, row);
}

bool nvc::Chess::longCastleAvailable(const BoardState& boardState)
{
	int row = boardState._turn ? 7 : 0;
	return !boardState._kingMoved[boardState._turn]
		&& !boardState._qRookMoved[boardState._turn]
		&& boardState._pieces[row * BOARD_LENGTH + 1] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 2] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 3] == PieceCode::EMPTY
		&& !squareThreatened(boardState._pieces, boardState._turn, 2, row)
		&& !squareThreatened(boardState._pieces, boardState._turn, 3, row)
		&& !squareThreatened(boardState._pieces, boardState._turn, 4, row);
}

bool nvc::Chess::squareThreatened(const PieceCode pieces[], int myTurn, int xTarget, int yTarget)
{
	for (int yBoard = 0; yBoard < BOARD_LENGTH; ++yBoard)
	{
		for (int xBoard = 0; xBoard < BOARD_LENGTH; ++xBoard)
		{
			if (pieces[yBoard * BOARD_LENGTH + xBoard] == PieceCode::EMPTY || pieceCodeToSide(pieces[yBoard * BOARD_LENGTH + xBoard]) == myTurn)
				continue;
			if (pieceCanThreatenSquare(pieces, xBoard, yBoard, xTarget, yTarget))
				return true;
		}
	}
	return false;
}

bool nvc::Chess::pieceCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
{
	switch (pieces[yPiece * BOARD_LENGTH + xPiece])
	{
	case PieceCode::W_KING: case PieceCode::B_KING:
		return kingCanThreatenSquare(xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_QUEEN: case PieceCode::B_QUEEN:
		return queenCanThreatenSquare(pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_ROOK: case PieceCode::B_ROOK:
		return rookCanThreatenSquare(pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_BISHOP: case PieceCode::B_BISHOP:
		return bishopCanThreatenSquare(pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_KNIGHT: case PieceCode::B_KNIGHT:
		return knightCanThreatenSquare(pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_PAWN: case PieceCode::B_PAWN:
		return pawnCanThreatenSquare(pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	}
	return false;
}

bool nvc::Chess::kingCanThreatenSquare(int xPiece, int yPiece, int xTarget, int yTarget)
{
	return abs(xTarget - xPiece) <= 1 && abs(yTarget - yPiece) <= 1;
}

bool nvc::Chess::queenCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
{
	if (!(xTarget == xPiece || yTarget == yPiece || (abs(xTarget - xPiece) == abs(yTarget - yPiece))))
		return false;

	int xDir = 0;
	int yDir = 0;

	if (xTarget == xPiece)
		yDir = (yTarget - yPiece) / abs(yTarget - yPiece);
	else if (yTarget == yPiece)
		xDir = (xTarget - xPiece) / abs(xTarget - xPiece);
	else
	{
		xDir = (xTarget - xPiece) / abs(xTarget - xPiece);
		yDir = (yTarget - yPiece) / abs(yTarget - yPiece);
	}

	int x = xPiece + xDir;
	int y = yPiece + yDir;

	while (!(x == xTarget && y == yTarget))
	{
		if (pieces[y * BOARD_LENGTH + x] != PieceCode::EMPTY)
			return false;
		x += xDir;
		y += yDir;
	}
	return true;
}

bool nvc::Chess::bishopCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
{
	if (abs(xTarget - xPiece) != abs(yTarget - yPiece))
		return false;
	int xDir = (xTarget - xPiece) / abs(xTarget - xPiece);
	int yDir = (yTarget - yPiece) / abs(yTarget - yPiece);
	int x = xPiece + xDir;
	int y = yPiece + yDir;
	while (!(x == xTarget && y == yTarget))
	{
		if (pieces[y * BOARD_LENGTH + x] != PieceCode::EMPTY)
			return false;
		x += xDir;
		y += yDir;
	}
	return true;
}

bool nvc::Chess::knightCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
{
	if (abs(xTarget - xPiece) == 2 && abs(yTarget - yPiece) == 1 || abs(xTarget - xPiece) == 1 && abs(yTarget - yPiece) == 2)
		return true;
	return false;
}

bool nvc::Chess::rookCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
{
	if (!(xTarget == xPiece || yTarget == yPiece))
		return false;

	int xDir = 0;
	int yDir = 0;

	if (xTarget != xPiece)
		xDir = (xTarget - xPiece) / abs(xTarget - xPiece);
	if (yTarget != yPiece)
		yDir = (yTarget - yPiece) / abs(yTarget - yPiece);

	int x = xPiece + xDir;
	int y = yPiece + yDir;
	while (!(x == xTarget && y == yTarget))
	{
		if (pieces[y * BOARD_LENGTH + x] != PieceCode::EMPTY)
			return false;
		x += xDir;
		y += yDir;
	}
	return true;
}

bool nvc::Chess::pawnCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
{
	int yDir = 0;
	if (pieces[yPiece * BOARD_LENGTH + xPiece] == PieceCode::W_PAWN)
		yDir = 1;
	else if (pieces[yPiece * BOARD_LENGTH + xPiece] == PieceCode::B_PAWN)
		yDir = -1;
	if (yPiece + yDir == yTarget && abs(xTarget - xPiece) == 1)
		return true;
	return false;
}

bool nvc::Chess::moveIsLegal(const BoardState& boardState, const MoveData& move)
{
	if (move.xStart < 0 || move.xStart >= BOARD_LENGTH || move.yStart < 0 || move.yStart >= BOARD_LENGTH
		|| move.xEnd < 0 || move.xEnd >= BOARD_LENGTH || move.yEnd < 0 || move.yEnd >= BOARD_LENGTH)
	{
		return false;
	}
	switch (boardState._pieces[move.yStart * BOARD_LENGTH + move.xStart])
	{
	case PieceCode::W_KING:
	case PieceCode::B_KING:
		return moveIsLegalKing(boardState, move);

	case PieceCode::W_QUEEN:
	case PieceCode::B_QUEEN:
		return moveIsLegalQueen(boardState, move);

	case PieceCode::W_BISHOP:
	case PieceCode::B_BISHOP:
		return moveIsLegalBishop(boardState, move);

	case PieceCode::W_ROOK:
	case PieceCode::B_ROOK:
		return moveIsLegalRook(boardState, move);

	case PieceCode::W_KNIGHT:
	case PieceCode::B_KNIGHT:
		return moveIsLegalKnight(boardState, move);

	case PieceCode::W_PAWN:
	case PieceCode::B_PAWN:
		return moveIsLegalPawn(boardState, move);
	}
	return false;
}

float nvc::Chess::boardValue(const BoardState& boardState)
{
	float value = 0.0f;
	PieceCode piece;
	for (int y = 0; y < BOARD_LENGTH; ++y)
	{
		for (int x = 0; x < BOARD_LENGTH; ++x)
		{
			piece = boardState._pieces[y * BOARD_LENGTH + x];
			if (piece != PieceCode::EMPTY && piece != PieceCode::W_KING && piece != PieceCode::B_KING)
				value += pieceValues.at(piece);
		}
	}
	return value;
}

bool nvc::Chess::moveIsLegalKing(const BoardState& boardState, const MoveData& move)
{
	int row = boardState._turn == 0 ? 0 : 7;
	// SHORT CASTLE
	if (move.shortCastle
		&& move.yStart == row
		&& move.yEnd == row
		&& move.xStart == 4
		&& (move.xEnd == move.xStart + 2)
		&& !boardState._kingMoved[boardState._turn]
		&& !boardState._kRookMoved[boardState._turn]
		&& boardState._pieces[row * BOARD_LENGTH + 5] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 6] == PieceCode::EMPTY
		&& !squareThreatened(boardState._pieces, boardState._turn, 4, row)
		&& !squareThreatened(boardState._pieces, boardState._turn, 5, row)
		&& !squareThreatened(boardState._pieces, boardState._turn, 6, row))
		return true;

	// LONG CASTLE
	if (move.longCastle
		&& move.yStart == row
		&& move.yEnd == row
		&& move.xStart == 4
		&& move.xEnd == move.xStart - 2
		&& !boardState._kingMoved[boardState._turn]
		&& !boardState._qRookMoved[boardState._turn]
		&& boardState._pieces[row * BOARD_LENGTH + 1] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 2] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 3] == PieceCode::EMPTY
		&& !squareThreatened(boardState._pieces, boardState._turn, 2, row)
		&& !squareThreatened(boardState._pieces, boardState._turn, 3, row)
		&& !squareThreatened(boardState._pieces, boardState._turn, 4, row))
		return true;

	// REGULAR MOVE
	if (boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd] != PieceCode::EMPTY && pieceCodeToSide(boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd]) == boardState._turn)
		return false;
	int xEndDistance = abs(move.xEnd - move.xStart);
	int yEndDistance = abs(move.yEnd - move.yStart);
	if (xEndDistance == 0 && yEndDistance == 0)
		return false;
	if (xEndDistance > 1 || yEndDistance > 1)
		return false;
	return true;
}

bool nvc::Chess::moveIsLegalQueen(const BoardState& boardState, const MoveData& move)
{
	const PieceCode& target = boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd];
	if (target != PieceCode::EMPTY && pieceCodeToSide(target) == boardState._turn)
		return false;
	if (((move.xStart == move.xEnd && move.yStart != move.yEnd) || (move.xStart != move.xEnd && move.yStart == move.yEnd)) || (abs(move.xEnd - move.xStart) == abs(move.yEnd - move.yStart)))
		return squaresAreEmpty(boardState._pieces, move.xStart, move.yStart, move.xEnd, move.yEnd);
	return false;
}

bool nvc::Chess::moveIsLegalRook(const BoardState& boardState, const MoveData& move)
{
	const PieceCode& target = boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd];
	if (target != PieceCode::EMPTY && pieceCodeToSide(target) == boardState._turn)
		return false;
	if (((move.xStart == move.xEnd) && (move.yStart != move.yEnd) || (move.xStart != move.xEnd) && (move.yStart == move.yEnd)))
		return squaresAreEmpty(boardState._pieces, move.xStart, move.yStart, move.xEnd, move.yEnd);
	return false;
}

bool nvc::Chess::moveIsLegalBishop(const BoardState& boardState, const MoveData& move)
{
	const PieceCode& target = boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd];
	if (target != PieceCode::EMPTY && pieceCodeToSide(target) == boardState._turn)
		return false;
	if (abs(move.xEnd - move.xStart) == abs(move.yEnd - move.yStart))
		return squaresAreEmpty(boardState._pieces, move.xStart, move.yStart, move.xEnd, move.yEnd);
	return false;
}

bool nvc::Chess::moveIsLegalKnight(const BoardState& boardState, const MoveData& move)
{
	const PieceCode& target = boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd];
	if (target != PieceCode::EMPTY && pieceCodeToSide(target) == boardState._turn)
		return false;
	if (move.xEnd == move.xStart + 2 || move.xEnd == move.xStart - 2)
		if (move.yEnd == move.yStart + 1 || move.yEnd == move.yStart - 1)
			return true;
	if (move.xEnd == move.xStart + 1 || move.xEnd == move.xStart - 1)
		if (move.yEnd == move.yStart + 2 || move.yEnd == move.yStart - 2)
			return true;
	return false;
}

bool nvc::Chess::moveIsLegalPawn(const BoardState& boardState, const MoveData& move)
{
	const PieceCode& target = boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd];
	int dir = boardState._turn == 0 ? 1 : -1;
	// EN PASSANT
	if (move.enPassant
		&& move.xEnd == boardState._enPassant
		&& (move.xEnd == move.xStart + 1 || move.xEnd == move.xStart - 1)
		&& move.yEnd == move.yStart + dir
		&& move.yStart == (boardState._turn == 0 ? 4 : 3))
		return true;
	// DOUBLE MOVE
	else if (move.doublePawnMove
		&& move.yStart == (boardState._turn == 0 ? 1 : 6)
		&& move.xEnd == move.xStart
		&& move.yEnd == move.yStart + 2 * dir
		&& boardState._pieces[(move.yStart + dir) * BOARD_LENGTH + move.xStart] == PieceCode::EMPTY
		&& boardState._pieces[(move.yStart + 2 * dir) * BOARD_LENGTH + move.xStart] == PieceCode::EMPTY)
		return true;
	// TAKE
	else if ((move.xEnd == move.xStart + 1 || move.xEnd == move.xStart - 1)
		&& move.yEnd == move.yStart + dir
		&& target != PieceCode::EMPTY
		&& pieceCodeToSide(target) != boardState._turn)
		return true;
	// NORMAL MOVE
	else if (move.xEnd == move.xStart && move.yEnd == move.yStart + dir
		&& target == PieceCode::EMPTY)
		return true;
	return false;
}

bool nvc::Chess::squaresAreEmpty(const PieceCode* pieces, int xStart, int yStart, int xEnd, int yEnd)
{
	int xDir = 0;
	int yDir = 0;

	if (xEnd == xStart)
		yDir = (yEnd - yStart) / abs(yEnd - yStart);
	else if (yEnd == yStart)
		xDir = (xEnd - xStart) / abs(xEnd - xStart);
	else if (abs(xEnd - xStart) == abs(yEnd - yStart))
	{
		xDir = (xEnd - xStart) / abs(xEnd - xStart);
		yDir = (yEnd - yStart) / abs(yEnd - yStart);
	}
	else
		return false;

	int x = xStart + xDir;
	int y = yStart + yDir;
	while (x != xEnd || y != yEnd)
	{
		if (pieces[y * BOARD_LENGTH + x] != PieceCode::EMPTY)
			return false;
		x += xDir;
		y += yDir;
	}
	return true;
}

void nvc::Chess::playMove(BoardState& boardState, const MoveData& move)
{
	if (move.longCastle)
	{
		int y = boardState._turn ? BOARD_LENGTH - 1 : 0;
		boardState._pieces[y * BOARD_LENGTH + 2] = boardState._pieces[y * BOARD_LENGTH + 4];
		boardState._pieces[y * BOARD_LENGTH + 3] = boardState._pieces[y * BOARD_LENGTH];
		boardState._pieces[y * BOARD_LENGTH + 4] = PieceCode::EMPTY;
		boardState._pieces[y * BOARD_LENGTH] = PieceCode::EMPTY;
		boardState._kingMoved[boardState._turn] = true;
		boardState._qRookMoved[boardState._turn] = true;
		return;
	}

	if (move.shortCastle)
	{
		int y = boardState._turn ? BOARD_LENGTH - 1 : 0;
		boardState._pieces[y * BOARD_LENGTH + 6] = boardState._pieces[y * BOARD_LENGTH + 4];
		boardState._pieces[y * BOARD_LENGTH + 5] = boardState._pieces[y * BOARD_LENGTH + 7];
		boardState._pieces[y * BOARD_LENGTH + 4] = PieceCode::EMPTY;
		boardState._pieces[y * BOARD_LENGTH + 7] = PieceCode::EMPTY;
		boardState._kingMoved[boardState._turn] = true;
		boardState._kRookMoved[boardState._turn] = true;
		return;
	}

	PieceCode piece = boardState._pieces[move.yStart * BOARD_LENGTH + move.xStart];
	if (piece == PieceCode::W_KING || piece == PieceCode::B_KING)
		boardState._kingMoved[boardState._turn] = true;
	else if (piece == PieceCode::W_ROOK || piece == PieceCode::B_ROOK)
	{
		if (move.xStart == 0)
			boardState._qRookMoved[boardState._turn] = true;
		if (move.xStart == 7)
			boardState._kRookMoved[boardState._turn] = true;
	}

	boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd] = boardState._pieces[move.yStart * BOARD_LENGTH + move.xStart];
	boardState._pieces[move.yStart * BOARD_LENGTH + move.xStart] = PieceCode::EMPTY;
	if (move.upgrade != PieceCode::EMPTY)
		boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd] = move.upgrade;
	if (move.enPassant)
		boardState._pieces[move.yStart * BOARD_LENGTH + move.xEnd] = PieceCode::EMPTY;
	if (move.doublePawnMove)
		boardState._enPassant = move.xStart;
	else
		boardState._enPassant = -1;
	boardState._turn = (boardState._turn + 1) % 2;
}

void nvc::Chess::playMove(Game& game, const MoveData& move)
{
	BoardState& boardState = game._boardState;
	game._moves.push_back(move);
	if (move.longCastle)
	{
		int y = boardState._turn ? BOARD_LENGTH - 1 : 0;
		boardState._pieces[y * BOARD_LENGTH + 2] = boardState._pieces[y * BOARD_LENGTH + 4];
		boardState._pieces[y * BOARD_LENGTH + 3] = boardState._pieces[y * BOARD_LENGTH];
		boardState._pieces[y * BOARD_LENGTH + 4] = PieceCode::EMPTY;
		boardState._pieces[y * BOARD_LENGTH] = PieceCode::EMPTY;
		boardState._kingMoved[boardState._turn] = true;
		boardState._qRookMoved[boardState._turn] = true;
		return;
	}

	if (move.shortCastle)
	{
		int y = boardState._turn ? BOARD_LENGTH - 1 : 0;
		boardState._pieces[y * BOARD_LENGTH + 6] = boardState._pieces[y * BOARD_LENGTH + 4];
		boardState._pieces[y * BOARD_LENGTH + 5] = boardState._pieces[y * BOARD_LENGTH + 7];
		boardState._pieces[y * BOARD_LENGTH + 4] = PieceCode::EMPTY;
		boardState._pieces[y * BOARD_LENGTH + 7] = PieceCode::EMPTY;
		boardState._kingMoved[boardState._turn] = true;
		boardState._kRookMoved[boardState._turn] = true;
		return;
	}

	PieceCode piece = boardState._pieces[move.yStart * BOARD_LENGTH + move.xStart];
	if (piece == PieceCode::W_KING || piece == PieceCode::B_KING)
		boardState._kingMoved[boardState._turn] = true;
	else if (piece == PieceCode::W_ROOK || piece == PieceCode::B_ROOK)
	{
		if (move.xStart == 0)
			boardState._qRookMoved[boardState._turn] = true;
		if (move.xStart == 7)
			boardState._kRookMoved[boardState._turn] = true;
	}

	boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd] = boardState._pieces[move.yStart * BOARD_LENGTH + move.xStart];
	boardState._pieces[move.yStart * BOARD_LENGTH + move.xStart] = PieceCode::EMPTY;
	if (move.upgrade != PieceCode::EMPTY)
		boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd] = move.upgrade;
	if (move.enPassant)
		boardState._pieces[move.yStart * BOARD_LENGTH + move.xEnd] = PieceCode::EMPTY;
	if (move.doublePawnMove)
		boardState._enPassant = move.xStart;
	else
		boardState._enPassant = -1;
	boardState._turn = (boardState._turn + 1) % 2;
}

void nvc::Chess::checkWinner(Game& game)
{
	PieceCode king = game._boardState._turn ? PieceCode::B_KING : PieceCode::W_KING;
	int kingCoord[2];
	findKing(game._boardState._pieces, game._boardState._turn, kingCoord);
	std::vector<MoveData> moves = genRawMoves(game._boardState);
	filterMoves(game._boardState, moves);
	if (moves.size() == 0)
	{
		if (game._boardState._turn == 0)
			game._blackWin = true;
		else
			game._whiteWin = true;
	}
}

void nvc::Chess::findKing(const PieceCode pieces[], int turn, int* coord)
{
	PieceCode king = turn ? PieceCode::B_KING : PieceCode::W_KING;
	for (int y = 0; y < BOARD_LENGTH; ++y)
	{
		for (int x = 0; x < BOARD_LENGTH; ++x)
		{
			if (pieces[y * BOARD_LENGTH + x] == king)
			{
				coord[0] = x;
				coord[1] = y;
				return;
			}
		}
	}
}

void nvc::Chess::filterMoves(const BoardState& boardState, std::vector<MoveData>& moves)
{
	BoardState temp;
	PieceCode kingCode = boardState._turn ? PieceCode::B_KING : PieceCode::W_KING;
	bool kingThreatened;
	int kingPos[2] = { 0, 0 };
	int kingPosMoved[2] = { 0, 0 };
	findKing(boardState._pieces, boardState._turn, kingPos);
	for (auto it = moves.begin(); it != moves.end();)
	{
		temp.copy(boardState);
		playMove(temp, *it);
		if (temp._pieces[kingPos[1] * BOARD_LENGTH + kingPos[0]] == kingCode)
			kingThreatened = squareThreatened(temp._pieces, boardState._turn, kingPos[0], kingPos[1]);
		else
		{
			findKing(temp._pieces, boardState._turn, kingPosMoved);
			kingThreatened = squareThreatened(temp._pieces, boardState._turn, kingPosMoved[0], kingPosMoved[1]);
		}
		if (kingThreatened)
			it = moves.erase(it);
		else
			++it;
	}
}
