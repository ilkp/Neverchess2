#pragma once
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <optional>
#include <future>
#include "nvc.h"
#include "SimpleANN.h"

#define HIGH_LABEL 1.0f
#define LOW_LABEL 0.0f

namespace nvc
{
	struct TrainingManagerCreateInfo
	{
		int _nGames;
		int _nRandomMoves;
		int _nMaxMoves;
		int _trainSampleRate;
	};

	class TrainingManager
	{
	public:
		TrainingManager(const TrainingManagerCreateInfo createInfo)
		{
			initializeZobristValues();
			_games.resize(createInfo._nGames);
			_nRandomMoves = createInfo._nRandomMoves;
			_nMaxMoves = createInfo._nMaxMoves;
			_trainSampleRate = createInfo._trainSampleRate;
		}
		void generateGames(simpleANN::ANNetwork& ann);
		void train(simpleANN::ANNetwork& ann);
		void placePiece(PieceCode pieces[], PieceCode pieceCode, int x, int y);
		void exportANN(simpleANN::ANNetwork& network, std::string fileName);
		void printBoard(const BoardState& boardState) const;
		//AnnUtilities::Network importANN			(std::string fileName);
		std::vector<Game> _games;

	private:
		std::mutex _hashTableLock;
		int _nRandomMoves;
		int _nMaxMoves;
		int _trainSampleRate;
		int _abDepth;
		uint32_t _zobristPieceValues[BOARD_LENGTH * BOARD_LENGTH * N_UNIQUE_PIECES] = { 0 };
		uint32_t _zobristTurnValues[2] = { 0 };
		uint32_t _zobristKingMovedValues[2] = { 0 };
		uint32_t _zobristQRookMovedValues[2] = { 0 };
		uint32_t _zobristKRookMovedValues[2] = { 0 };
		uint32_t _zobristEnPassantValues[9] = { 0 };
		std::unordered_map<uint32_t, float> _hashedEvaluations;

		void processGame(Game& game, simpleANN::ANNetwork& ann);
		void initializeZobristValues();
		bool zobristValueExists(uint32_t value);
		uint32_t hashBoard(const BoardState& boardState);
		void boardStateToNetworkInput(const BoardState& boardState, float* inputArray);
		std::optional<float> findHashedBoardState(uint32_t hash);
		void insertHashedBoardState(uint32_t hash, float value);
		void playRandomStartMoves(nvc::Game& game);
		float evaluate(const BoardState& boardState, simpleANN::ANNetwork& network, float* networkInputArray);
	};
}