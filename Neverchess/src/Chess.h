#pragma once
#include <vector>
#include "nvc.h"

namespace nvc
{
	class Chess
	{
	public:
		static int pieceCodeToSide(const PieceCode& pieceCode);
		static std::vector<MoveData> genRawMoves(const BoardState& boardStateData);
		static bool squareThreatened(const PieceCode pieces[], int myTurn, int xTarget, int yTarget);
		static void playMove(BoardState& boardState, const MoveData& move);
		static void playMove(Game& game, const MoveData& move);
		static void filterMoves(const BoardState& boardState, std::vector<MoveData>& moves);
		static void checkWinner(Game& game);
		static bool moveIsLegal(const BoardState& boardState, const MoveData& move);
		static float boardValue(const BoardState& boardState);

	private:
		static void genRawPieceMoves(const BoardState& boardStateData, std::vector<MoveData>& moves, int x, int y);
		static void genRawMovesKing(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
		static void genRawMovesQueen(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
		static void genRawMovesBishop(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
		static void genRawMovesRook(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
		static void genRawMovesKnight(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
		static void genRawMovesPawn(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
		static void genMovesDir(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece, int xDir, int yDir);

		static bool shortCastleAvailable(const BoardState& boardState);
		static bool longCastleAvailable(const BoardState& boardState);

		static bool pieceCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
		static bool kingCanThreatenSquare(int xPiece, int yPiece, int xTarget, int yTarget);
		static bool queenCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
		static bool bishopCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
		static bool knightCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
		static bool rookCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
		static bool pawnCanThreatenSquare(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);

		static bool moveIsLegalKing(const BoardState& boardState, const MoveData& move);
		static bool moveIsLegalQueen(const BoardState& boardState, const MoveData& move);
		static bool moveIsLegalRook(const BoardState& boardState, const MoveData& move);
		static bool moveIsLegalBishop(const BoardState& boardState, const MoveData& move);
		static bool moveIsLegalKnight(const BoardState& boardState, const MoveData& move);
		static bool moveIsLegalPawn(const BoardState& boardState, const MoveData& move);
		static bool squaresAreEmpty(const PieceCode* pieces, int xStart, int yStart, int xEnd, int yEnd);

		static void findKing(const PieceCode pieces[], int turn, int* coord);
	};

}