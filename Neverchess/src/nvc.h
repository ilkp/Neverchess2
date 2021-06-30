#pragma once
#include <set>
#include <map>

namespace nvc
{
#define WHITE 0
#define BLACK 1
#define PIECE_CODE_LENGTH 7
#define BOARD_LENGTH 8
#define N_UNIQUE_PIECES 12

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

	static const std::map<PieceCode, float> pieceValues
	{
		{ PieceCode::W_QUEEN, -9.0f },
		{ PieceCode::W_ROOK, -5.0f },
		{ PieceCode::W_BISHOP, -3.0f },
		{ PieceCode::W_KNIGHT, -3.0f },
		{ PieceCode::W_PAWN, -1.0f },

		{ PieceCode::B_QUEEN, 9.0f },
		{ PieceCode::B_ROOK, 5.0f },
		{ PieceCode::B_BISHOP, 3.0f },
		{ PieceCode::B_KNIGHT, 3.0f },
		{ PieceCode::B_PAWN, 1.0f }
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
		int _turn = WHITE;
		bool _kingMoved[2] = { false, false };
		bool _kRookMoved[2] = { false, false };
		bool _qRookMoved[2] = { false, false };
		int _enPassant = -1;

		BoardState() {}

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

		void copy(const BoardState& other)
		{
			for (int y = 0; y < BOARD_LENGTH; ++y)
				for (int x = 0; x < BOARD_LENGTH; ++x)
					_pieces[y * BOARD_LENGTH + x] = other._pieces[y * BOARD_LENGTH + x];
			_turn = other._turn;
			_enPassant = other._enPassant;
			_kingMoved[WHITE] = other._kingMoved[WHITE];
			_kingMoved[BLACK] = other._kingMoved[BLACK];
			_kRookMoved[WHITE] = other._kRookMoved[WHITE];
			_kRookMoved[BLACK] = other._kRookMoved[BLACK];
			_qRookMoved[WHITE] = other._qRookMoved[WHITE];
			_qRookMoved[BLACK] = other._qRookMoved[BLACK];
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

		void resetBoardState()
		{
			int bPawnRow = 6 * BOARD_LENGTH;
			int bPieceRow = 7 * BOARD_LENGTH;

			for (int y = 2; y < BOARD_LENGTH - 2; ++y)
				for (int x = 0; x < BOARD_LENGTH; ++x)
					_pieces[y * BOARD_LENGTH + x] = PieceCode::EMPTY;

			for (int x = 0; x < BOARD_LENGTH; ++x)
			{
				_pieces[BOARD_LENGTH + x] = PieceCode::W_PAWN;
				_pieces[bPawnRow + x] = PieceCode::B_PAWN;
			}

			_pieces[0] = PieceCode::W_ROOK;
			_pieces[1] = PieceCode::W_KNIGHT;
			_pieces[2] = PieceCode::W_BISHOP;
			_pieces[3] = PieceCode::W_QUEEN;
			_pieces[4] = PieceCode::W_KING;
			_pieces[5] = PieceCode::W_BISHOP;
			_pieces[6] = PieceCode::W_KNIGHT;
			_pieces[7] = PieceCode::W_ROOK;

			_pieces[bPieceRow + 0] = PieceCode::B_ROOK;
			_pieces[bPieceRow + 1] = PieceCode::B_KNIGHT;
			_pieces[bPieceRow + 2] = PieceCode::B_BISHOP;
			_pieces[bPieceRow + 3] = PieceCode::B_QUEEN;
			_pieces[bPieceRow + 4] = PieceCode::B_KING;
			_pieces[bPieceRow + 5] = PieceCode::B_BISHOP;
			_pieces[bPieceRow + 6] = PieceCode::B_KNIGHT;
			_pieces[bPieceRow + 7] = PieceCode::B_ROOK;

			_turn = WHITE;
			_kingMoved[WHITE] = false;
			_kingMoved[BLACK] = false;
			_kRookMoved[WHITE] = false;
			_kRookMoved[BLACK] = false;
			_qRookMoved[WHITE] = false;
			_qRookMoved[BLACK] = false;
			_enPassant = -1;
		}
	};

	struct Game
	{
		bool _whiteWin = false;
		bool _blackWin = false;
		bool _draw = false;
		BoardState _boardState;
		std::vector<MoveData> _moves;
		std::set<uint32_t> _pastBoardHashes;

		Game()
		{
			_boardState.resetBoardState();
		}

		void reset()
		{
			_whiteWin = false;
			_blackWin = false;
			_draw = false;
			_boardState.resetBoardState();
			_moves.clear();
			_pastBoardHashes.clear();
		}
	};
}