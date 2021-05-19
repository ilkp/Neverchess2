#pragma once
#include "BoardState.h"
#include <queue>
#include <unordered_map>
#include "src/SimpleANN.h"

struct AlphaBetaEvaluation
{
	MoveData move;
	float evaluatedValue;
};

class TrainingManager
{
private:
	unsigned long int zobristPieceValues[BOARD_LENGTH * BOARD_LENGTH * 12] = { 0 };
	unsigned long int zobristTurnValues[2] = { 0 };
	unsigned long int zobristKingMovedValues[2] = { 0 };
	unsigned long int zobristQRookMovedValues[2] = { 0 };
	unsigned long int zobristKRookMovedValues[2] = { 0 };
	unsigned long int zobristEnPassantValues[9] = { 0 };
	std::queue<AlphaBetaEvaluation> alphaBetaHistory;
	std::unordered_map<unsigned long int, int> hashPositions;
	std::unordered_map<unsigned long int, AlphaBetaEvaluation> boardEvaluations;
	int availableThreads = 0;
	bool whiteWin = false;
	bool blackWin = false;

	void setANNInput(const BoardState& BoardState, simpleANN::Layer* inputLayer);
	int positionAppeared(const BoardState& BoardState);
	void findKing(const PieceCode pieces[], bool turn, int* pos);
	void playMove(BoardState& BoardState, const MoveData& move);
	std::vector<BoardState> filterMoves(const BoardState& BoardState, std::vector<MoveData>& moves);
	unsigned long int zobristHash(const BoardState& BoardState);
	bool zobristValueExists(unsigned long int v);
	void checkWinner(const BoardState& BoardState);
	void printBoard(const BoardState& BoardState) const;

public:
	void train(simpleANN::ANNetwork& ann);
	void process(BoardState& BoardState, simpleANN::ANNetwork& network, int evaluationDepth, int maxTurns);
	void evaluate(const BoardState& BoardState, simpleANN::ANNetwork& network, AlphaBetaEvaluation& eval, bool noMoves);
	void initBoardStatePieces(PieceCode pieces[]);
	void placePiece(PieceCode pieces[], PieceCode pieceCode, int x, int y);
	AlphaBetaEvaluation alphaBeta(BoardState& BoardState, simpleANN::ANNetwork& network, int depth, float alpha, float beta);
	void reset();
	void resetBoardState(BoardState& boardStateDate);
	void calculateZobristValues();
	void exportANN(simpleANN::ANNetwork& network, std::string fileName);
	//AnnUtilities::Network importANN			(std::string fileName);
};

