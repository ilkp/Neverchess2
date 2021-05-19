#include "Chess.h"

int pieceCodeToSide(const PieceCode& pieceCode)
{
	return (int)pieceCode >> (PIECE_CODE_LENGTH - 1);
}

std::vector<MoveData> genRawMoves(const BoardState& boardStateData)
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

void genRawPieceMoves		(const BoardState& boardState, std::vector<MoveData>& moves, int x, int y)
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

void genRawMovesKing		(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
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

void genRawMovesQueen		(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
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

void genRawMovesBishop		(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	genMovesDir(moves, boardState, xPiece, yPiece, -1, -1);
	genMovesDir(moves, boardState, xPiece, yPiece, -1, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, -1);
}

void genRawMovesRook		(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	genMovesDir(moves, boardState, xPiece, yPiece, -1, 0);
	genMovesDir(moves, boardState, xPiece, yPiece, 0, 1);
	genMovesDir(moves, boardState, xPiece, yPiece, 1, 0);
	genMovesDir(moves, boardState, xPiece, yPiece, 0, -1);
}

void genRawMovesKnight		(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
{
	int x = xPiece - 2;
	int y = yPiece - 1;
	const PieceCode* pieces = boardState._pieces;
	const int turn = boardState._turn;
	if (x > -1 && y > -1 && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || (int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	y = yPiece + 1;
	if (x > -1 && y < BOARD_LENGTH && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || (int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	x = xPiece + 2;
	if (x < BOARD_LENGTH && y < BOARD_LENGTH && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || (int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	y = yPiece - 1;
	if (x < BOARD_LENGTH && y < -1 && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || (int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn))
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
	if (x > -1 && y > -1 && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || (int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	x = xPiece + 1;
	if (x < BOARD_LENGTH && y > -1 && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || (int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	y = yPiece + 2;
	if (x < BOARD_LENGTH && y < BOARD_LENGTH && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || (int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
	x = xPiece - 1;
	if (x > -1 && y < BOARD_LENGTH && (pieces[y * BOARD_LENGTH + x] == PieceCode::EMPTY || (int)pieces[y * BOARD_LENGTH + x] >> (PIECE_CODE_LENGTH - 1) != turn))
	{
		MoveData move;
		move.xStart = xPiece;
		move.yStart = yPiece;
		move.xEnd = x;
		move.yEnd = y;
		moves.push_back(std::move(move));
	}
}

void genRawMovesPawn		(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece)
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

void genMovesDir			(std::vector<MoveData>& moves, const BoardState& boardState, int xPiece, int yPiece, int xDir, int yDir)
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

bool shortCastleAvailable	(const BoardState& boardState)
{
	int row = boardState._turn ? 7 : 0;
	return !boardState._kingMoved[boardState._turn]
		&& !boardState._kRookMoved[boardState._turn]
		&& boardState._pieces[row * BOARD_LENGTH + 5] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 6] == PieceCode::EMPTY
		&& !squareThreatened(boardState, 4, row)
		&& !squareThreatened(boardState, 5, row)
		&& !squareThreatened(boardState, 6, row);
}

bool longCastleAvailable	(const BoardState& boardState)
{
	int row = boardState._turn ? 7 : 0;
	return !boardState._kingMoved[boardState._turn]
		&& !boardState._qRookMoved[boardState._turn]
		&& boardState._pieces[row * BOARD_LENGTH + 1] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 2] == PieceCode::EMPTY
		&& boardState._pieces[row * BOARD_LENGTH + 3] == PieceCode::EMPTY
		&& !squareThreatened(boardState, 2, row)
		&& !squareThreatened(boardState, 3, row)
		&& !squareThreatened(boardState, 4, row);
}

bool squareThreatened(const BoardState& boardState, int xTarget, int yTarget)
{
	for (int yBoard = 0; yBoard < BOARD_LENGTH; ++yBoard)
	{
		for (int xBoard = 0; xBoard < BOARD_LENGTH; ++xBoard)
		{
			if (boardState._pieces[yBoard * BOARD_LENGTH + xBoard] == PieceCode::EMPTY || pieceCodeToSide(boardState._pieces[yBoard * BOARD_LENGTH + xBoard]) == boardState._turn)
				continue;
			if (pieceCanThreatenSquare(boardState, xBoard, yBoard, xTarget, yTarget))
				return true;
		}
	}
	return false;
}

bool pieceCanThreatenSquare(const BoardState& boardState, int xPiece, int yPiece, int xTarget, int yTarget)
{
	switch (boardState._pieces[yPiece * BOARD_LENGTH + xPiece])
	{
	case PieceCode::W_KING: case PieceCode::B_KING:
		return kingCanThreatenSquare(xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_QUEEN: case PieceCode::B_QUEEN:
		return queenCanThreatenSquare(boardState._pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_ROOK: case PieceCode::B_ROOK:
		return rookCanThreatenSquare(boardState._pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_BISHOP: case PieceCode::B_BISHOP:
		return bishopCanThreatenSquare(boardState._pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_KNIGHT: case PieceCode::B_KNIGHT:
		return knightCanThreatenSquare(boardState._pieces, xPiece, yPiece, xTarget, yTarget);
		break;
	case PieceCode::W_PAWN: case PieceCode::B_PAWN:
		return pawnCanThreatenSquare(boardState._turn, xPiece, yPiece, xTarget, yTarget);
		break;
	}
	return false;
}

bool kingCanThreatenSquare(int xPiece, int yPiece, int xTarget, int yTarget)
{
	return abs(xTarget - xPiece) <= 1 && abs(yTarget - yPiece) <= 1;
}

bool queenCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
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

bool bishopCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
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

bool knightCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
{
	if (abs(xTarget - xPiece) == 2 && abs(yTarget - yPiece) == 1 || abs(xTarget - xPiece) == 1 && abs(yTarget - yPiece) == 2)
		return true;
	return false;
}

bool rookCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget)
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

bool pawnCanThreatenSquare(int turn, int xPiece, int yPiece, int xTarget, int yTarget)
{
	int yDir = turn ? 1 : -1;
	if (yPiece + yDir == yTarget && abs(xTarget - xPiece) == 1)
		return true;
	return false;
}

bool moveIsLegal(const BoardState& boardState, const MoveData& move)
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

bool moveIsLegalKing(const BoardState& boardState, const MoveData& move)
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
		&& !squareThreatened(boardState, 4, row)
		&& !squareThreatened(boardState, 5, row)
		&& !squareThreatened(boardState, 6, row))
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
		&& !squareThreatened(boardState, 2, row)
		&& !squareThreatened(boardState, 3, row)
		&& !squareThreatened(boardState, 4, row))
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

bool moveIsLegalQueen(const BoardState& boardState, const MoveData& move)
{
	const PieceCode& target = boardState._pieces[move.yEnd * BOARD_LENGTH + move.xEnd];
	if (target != PieceCode::EMPTY && pieceCodeToSide(target) == boardState._turn)
		return false;
	if (((move.xStart == move.xEnd && move.yStart != move.yEnd) || (move.xStart != move.xEnd && move.yStart == move.yEnd)) || (abs(move.xEnd - move.xStart) == abs(move.yEnd - move.yStart)))
		return squaresAreEmpty(boardState._pieces, move.xStart, move.yStart, move.xEnd, move.yEnd);
	return false;
}

bool moveIsLegalRook(const BoardState& boardState, const MoveData& move)
{
	return false;
}

bool moveIsLegalBishop(const BoardState& boardState, const MoveData& move)
{
	return false;
}

bool moveIsLegalKnight(const BoardState& boardState, const MoveData& move)
{
	return false;
}

bool moveIsLegalPawn(const BoardState& boardState, const MoveData& move)
{
	return false;
}

bool squaresAreEmpty(const PieceCode* pieces, int xStart, int yStart, int xEnd, int yEnd)
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

