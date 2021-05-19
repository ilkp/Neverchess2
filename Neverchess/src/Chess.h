#pragma once
#include <vector>
#include "BoardState.h"

int pieceCodeToSide(const PieceCode& pieceCode);

std::vector<MoveData> genRawMoves(const BoardState& boardStateData);
void genRawPieceMoves			(const BoardState& boardStateData, std::vector<MoveData>& moves, int x, int y);
void genRawMovesKing			(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
void genRawMovesQueen			(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
void genRawMovesBishop			(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
void genRawMovesRook			(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
void genRawMovesKnight			(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
void genRawMovesPawn			(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece);
void genMovesDir				(std::vector<MoveData>& moves, const BoardState& boardStateData, int xPiece, int yPiece, int xDir, int yDir);

bool shortCastleAvailable		(const BoardState& boardState);
bool longCastleAvailable		(const BoardState& boardState);

bool squareThreatened			(const BoardState& boardState, int xTarget, int yTarget);
bool pieceCanThreatenSquare		(const BoardState& boardState, int xPiece, int yPiece, int xTarget, int yTarget);
bool kingCanThreatenSquare		(int xPiece, int yPiece, int xTarget, int yTarget);
bool queenCanThreatenSquare		(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
bool bishopCanThreatenSquare	(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
bool knightCanThreatenSquare	(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
bool rookCanThreatenSquare		(const PieceCode pieces[], int xPiece, int yPiece, int xTarget, int yTarget);
bool pawnCanThreatenSquare		(int turn, int xPiece, int yPiece, int xTarget, int yTarget);

bool moveIsLegal				(const BoardState& boardState, const MoveData& move);
bool moveIsLegalKing			(const BoardState& boardState, const MoveData& move);
bool moveIsLegalQueen			(const BoardState& boardState, const MoveData& move);
bool moveIsLegalRook			(const BoardState& boardState, const MoveData& move);
bool moveIsLegalBishop			(const BoardState& boardState, const MoveData& move);
bool moveIsLegalKnight			(const BoardState& boardState, const MoveData& move);
bool moveIsLegalPawn			(const BoardState& boardState, const MoveData& move);
bool squaresAreEmpty			(const PieceCode* pieces, int xStart, int yStart, int xEnd, int yEnd);