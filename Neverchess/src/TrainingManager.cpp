#include <fstream>
#include <iostream>
#include "TrainingManager.h"
#include "Chess.h"

void nvc::TrainingManager::initializeZobristValues()
{
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_int_distribution<uint32_t> dist(0, ULONG_MAX);
	uint32_t zobristValue;
	for (int i = 0; i < BOARD_LENGTH * BOARD_LENGTH * N_UNIQUE_PIECES; ++i)
	{
		do
			zobristValue = dist(e2);
		while (zobristValueExists(zobristValue));
		_zobristPieceValues[i] = zobristValue;
	}
	for (int i = 0; i < BOARD_LENGTH + 1; ++i)
	{
		do
			zobristValue = dist(e2);
		while (zobristValueExists(zobristValue));
		_zobristEnPassantValues[i] = zobristValue;
	}
	for (int i = 0; i < 2; ++i)
	{
		do
			zobristValue = dist(e2);
		while (zobristValueExists(zobristValue));
		_zobristTurnValues[i] = zobristValue;
	}
	for (int i = 0; i < 2; ++i)
	{
		do
			zobristValue = dist(e2);
		while (zobristValueExists(zobristValue));
		_zobristKingMovedValues[i] = zobristValue;
	}
	for (int i = 0; i < 2; ++i)
	{
		do
			zobristValue = dist(e2);
		while (zobristValueExists(zobristValue));
		_zobristQRookMovedValues[i] = zobristValue;
	}
	for (int i = 0; i < 2; ++i)
	{
		do
			zobristValue = dist(e2);
		while (zobristValueExists(zobristValue));
		_zobristKRookMovedValues[i] = zobristValue;
	}
}

bool nvc::TrainingManager::zobristValueExists(uint32_t value)
{
	for (int i = 0; i < BOARD_LENGTH * BOARD_LENGTH * N_UNIQUE_PIECES; ++i)
		if (_zobristPieceValues[i] == value)
			return true;
	for (int i = 0; i < BOARD_LENGTH + 1; ++i)
		if (_zobristEnPassantValues[i] == value)
			return true;
	if (_zobristTurnValues[WHITE] == value || _zobristTurnValues[BLACK] == value)
		return true;
	if (_zobristKingMovedValues[WHITE] == value || _zobristKingMovedValues[BLACK] == value)
		return true;
	if (_zobristQRookMovedValues[WHITE] == value || _zobristQRookMovedValues[BLACK] == value)
		return true;
	if (_zobristKRookMovedValues[WHITE] == value || _zobristKRookMovedValues[BLACK] == value)
		return true;
	return false;
}

uint32_t nvc::TrainingManager::hashBoard(const BoardState& boardState)
{
	uint32_t hash = 0;
	PieceCode piece;
	int pieceIndex = 0;
	hash = hash ^ _zobristTurnValues[boardState._turn];
	for (int i = 0; i < BOARD_LENGTH * BOARD_LENGTH; ++i)
	{
		if (boardState._pieces[i] != PieceCode::EMPTY)
		{
			piece = boardState._pieces[i];
			switch (piece)
			{
			case PieceCode::W_KING:
				pieceIndex = 0;
				break;
			case PieceCode::W_QUEEN:
				pieceIndex = 1;
				break;
			case PieceCode::W_PAWN:
				pieceIndex = 2;
				break;
			case PieceCode::W_KNIGHT:
				pieceIndex = 3;
				break;
			case PieceCode::W_BISHOP:
				pieceIndex = 4;
				break;
			case PieceCode::W_ROOK:
				pieceIndex = 5;
				break;
			case PieceCode::B_KING:
				pieceIndex = 6;
				break;
			case PieceCode::B_QUEEN:
				pieceIndex = 7;
				break;
			case PieceCode::B_PAWN:
				pieceIndex = 8;
				break;
			case PieceCode::B_KNIGHT:
				pieceIndex = 9;
				break;
			case PieceCode::B_BISHOP:
				pieceIndex = 10;
				break;
			case PieceCode::B_ROOK:
				pieceIndex = 11;
				break;
			}
			hash = hash ^ _zobristPieceValues[(i * N_UNIQUE_PIECES) + pieceIndex];
		}
	}
	for (int i = 0; i < BOARD_LENGTH + 1; ++i)
		hash = hash ^ _zobristEnPassantValues[boardState._enPassant + 1];
	hash = hash ^ _zobristKingMovedValues[WHITE];
	hash = hash ^ _zobristKingMovedValues[BLACK];
	hash = hash ^ _zobristQRookMovedValues[WHITE];
	hash = hash ^ _zobristQRookMovedValues[BLACK];
	hash = hash ^ _zobristQRookMovedValues[WHITE];
	hash = hash ^ _zobristQRookMovedValues[BLACK];
	return hash;
}

void nvc::TrainingManager::printBoard(const BoardState& boardState) const
{
	for (int y = BOARD_LENGTH - 1; y > -1; --y)
	{
		for (int x = 0; x < BOARD_LENGTH; ++x)
		{
			std::cout << "[";
			switch (boardState._pieces[y * BOARD_LENGTH + x])
			{
			case PieceCode::EMPTY:
				break;
			case PieceCode::B_KING:
				std::cout << "BK";
				break;
			case PieceCode::B_QUEEN:
				std::cout << "BQ";
				break;
			case PieceCode::B_ROOK:
				std::cout << "BR";
				break;
			case PieceCode::B_BISHOP:
				std::cout << "BB";
				break;
			case PieceCode::B_KNIGHT:
				std::cout << "Bk";
				break;
			case PieceCode::B_PAWN:
				std::cout << "BP";
				break;
			case PieceCode::W_KING:
				std::cout << "WK";
				break;
			case PieceCode::W_QUEEN:
				std::cout << "WQ";
				break;
			case PieceCode::W_ROOK:
				std::cout << "WR";
				break;
			case PieceCode::W_BISHOP:
				std::cout << "WB";
				break;
			case PieceCode::W_KNIGHT:
				std::cout << "Wk";
				break;
			case PieceCode::W_PAWN:
				std::cout << "WP";
				break;
			}
			std::cout << "]\t";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

void nvc::TrainingManager::boardStateToNetworkInput(const BoardState& boardState, float* inputArray)
{
	int loc = 0;
	// Turn
	inputArray[loc++] = (float)boardState._turn;
	// White castles
	if (boardState._kingMoved[WHITE])
	{
		inputArray[loc++] = 0.0f; // king side castle
		inputArray[loc++] = 0.0f; // queen side castle
	}
	else
	{
		if (boardState._kRookMoved[WHITE])
			inputArray[loc++] = 0.0f;
		else
			inputArray[loc++] = 1.0f;
		if (boardState._qRookMoved[WHITE])
			inputArray[loc++] = 0.0f;
		else
			inputArray[loc++] = 1.0f;
	}
	// Black castles
	if (boardState._kingMoved[BLACK])
	{
		inputArray[loc++] = 0.0f; // king side castle
		inputArray[loc++] = 0.0f; // queen side castle
	}
	else
	{
		if (boardState._kRookMoved[BLACK])
			inputArray[loc++] = 0.0f;
		else
			inputArray[loc++] = 1.0f;
		if (boardState._qRookMoved[BLACK])
			inputArray[loc++] = 0.0f;
		else
			inputArray[loc++] = 1.0f;
	}
	// En passant
	for (int i = 0; i < BOARD_LENGTH; ++i)
	{
		if (i == boardState._enPassant)
			inputArray[loc++] = 1.0f;
		else
			inputArray[loc++] = 0.0f;
	}
	// Pieces
	for (int y = 0; y < BOARD_LENGTH; ++y)
		for (int x = 0; x < BOARD_LENGTH; ++x)
			for (int i = 0; i < PIECE_CODE_LENGTH; ++i)
				inputArray[loc++] = float(((int)boardState._pieces[y * BOARD_LENGTH + x] >> i) & 1);
	int a = loc;
}

std::optional<float> nvc::TrainingManager::findHashedBoardState(uint32_t hash)
{
	std::lock_guard<std::mutex> lock(_hashTableLock);
	if (_hashedEvaluations.find(hash) != _hashedEvaluations.end())
		return _hashedEvaluations[hash];
	return {};
}

void nvc::TrainingManager::insertHashedBoardState(uint32_t hash, float value)
{
	std::lock_guard<std::mutex> lock(_hashTableLock);
	_hashedEvaluations.insert({ hash, value });
}

void nvc::TrainingManager::generateGames(simpleANN::ANNetwork& ann)
{
	std::vector<std::thread> threads;
	_hashedEvaluations.clear();
	for (Game game : _games)
	{
		game.reset();
		threads.push_back(std::thread([&]() { processGame(game, ann); }));
	}
	for (std::thread& thread : threads) thread.join();
}

void nvc::TrainingManager::processGame(Game& game, simpleANN::ANNetwork& ann)
{
	std::vector<MoveData> moves;
	BoardState temp;
	uint32_t boardHash;
	std::optional<float> hashedEvaluation;
	float annInput[ANN_INPUT_LENGTH];
	float evaluation;
	float bestEvaluation;
	MoveData* bestMove;
	int nMove = 0;

	playRandomStartMoves(game);
	while (!game._whiteWin && !game._blackWin && !game._draw)
	{
		moves = Chess::genRawMoves(game._boardState);
		Chess::filterMoves(game._boardState, moves);
		if (moves.empty())
			nvc::Chess::checkWinner(game);
		else
		{
			bestEvaluation = game._boardState._turn == 0 ? 10000.0f : -10000.0f;
			bestMove = &moves[0];
			for (MoveData move : moves)
			{
				temp.copy(game._boardState);
				nvc::Chess::playMove(temp, move);
				boardHash = hashBoard(temp);
				hashedEvaluation = findHashedBoardState(boardHash);
				if (hashedEvaluation.has_value())
					evaluation = hashedEvaluation.value();
				else
				{
					evaluation = evaluate(temp, ann, annInput);
					insertHashedBoardState(boardHash, evaluation);
				}
				if (game._boardState._turn == WHITE && evaluation < bestEvaluation || game._boardState._turn == BLACK && evaluation > bestEvaluation)
				{
					bestEvaluation = evaluation;
					bestMove = &move;
				}
			}
			nvc::Chess::playMove(game, *bestMove);

			// End game as a draw if dublicate of a previous position or maximum move count is reached
			boardHash = hashBoard(game._boardState);
			if (game._pastBoardHashes.find(boardHash) != game._pastBoardHashes.end() || ++nMove > _nMaxMoves)
				game._draw = true;
			else
				game._pastBoardHashes.insert(boardHash);
		}
	}
}

void nvc::TrainingManager::playRandomStartMoves(nvc::Game& game)
{
	for (int i = 0; i < _nRandomMoves; ++i)
	{
		std::vector<MoveData> moves = nvc::Chess::genRawMoves(game._boardState);
		nvc::Chess::filterMoves(game._boardState, moves);
		int moveIndex = std::rand() % moves.size();
		nvc::Chess::playMove(game, moves[moveIndex]);
	}
}

void nvc::TrainingManager::train(simpleANN::ANNetwork& ann)
{
	float input[ANN_INPUT_LENGTH];
	float average = (HIGH_LABEL + LOW_LABEL) * 0.5f;
	int totalBatchSize = 0;
	float label;
	float slope;
	for (Game game : _games)
	{
		totalBatchSize += game._moves.size();
		if (game._whiteWin)
			label = LOW_LABEL;
		else if (game._blackWin)
			label = HIGH_LABEL;
		else
			label = average;
		slope = (label - average) / game._moves.size();

		game._boardState.resetBoardState();
		for (unsigned int i = 0; i < game._moves.size(); ++i)
		{
			label = slope * i + average;
			nvc::Chess::playMove(game._boardState, game._moves[i]);
			boardStateToNetworkInput(game._boardState, input);
			ann.propagateForward(input);
			ann.outputWasRead();
			ann.propagateBackward(&label);
		}
	}
	ann.update(totalBatchSize);
}

void nvc::TrainingManager::exportANN(simpleANN::ANNetwork& network, std::string fileName)
{
	int hiddenLayers = 0;
	simpleANN::Layer* l = network._inputLayer->_nextLayer;
	while (l != network._outputLayer)
	{
		++hiddenLayers;
		l = l->_nextLayer;
	}
	std::ofstream file;
	file.open(fileName, std::ios_base::app);
	file << network._inputLayer->_layerSize << "\n";
	file << network._inputLayer->_nextLayer->_layerSize << "\n";
	file << network._outputLayer->_layerSize << "\n";
	file << hiddenLayers << "\n";
	// WRITE WEIGHTS
	l = network._inputLayer->_nextLayer;
	while (l != nullptr)
	{
		for (int i = 0; i < l->_layerSize; ++i)
			for (int j = 0; j < l->_prevLayer->_layerSize; ++j)
				file << l->_weights[i * l->_prevLayer->_layerSize + j] << "\n";
		l = l->_nextLayer;
	}
	// WRITE BIASES
	l = network._inputLayer->_nextLayer;
	while (l != nullptr)
	{
		for (int i = 0; i < l->_layerSize; ++i)
			file << l->_biases[i] << "\n";
		l = l->_nextLayer;
	}
	file.close();
}

float nvc::TrainingManager::evaluate(const BoardState& boardState, simpleANN::ANNetwork& network, float* networkInputArray)
{
	boardStateToNetworkInput(boardState, networkInputArray);
	network.propagateForward(networkInputArray);
	float output = network._outputLayer->_outputs[0];
	network.outputWasRead();
	return output;
}
