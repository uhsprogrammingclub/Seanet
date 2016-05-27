//
//  evaluator.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#include "evaluator.hpp"
#include "movegenerator.hpp"

// Piece-Square tables

// int blackPawnPS[8][8];
// int blackKnightPS[8][8];
// int blackBishopPS[8][8];
// int blackRookPS[8][8];
// int blackQueenPS[8][8];
// int blackKingMiddlePS[8][8];
// int blackKingEndPS[8][8];

int whitePawnPS[64] = {0,  0,  0,  0,  0,  0,   0,  0,  5,   10, 10, -20, -20,
                       10, 10, 5,  5,  -5, -10, 0,  0,  -10, -5, 5,  0,   0,
                       0,  20, 20, 0,  0,  0,   5,  5,  10,  25, 25, 10,  5,
                       5,  10, 10, 20, 30, 30,  20, 10, 10,  50, 50, 50,  50,
                       50, 50, 50, 50, 0,  0,   0,  0,  0,   0,  0,  0};

int whiteKnightPS[64] = {-50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,
                         5,   5,   0,   -20, -40, -30, 5,   10,  15,  15,  10,
                         5,   -30, -30, 0,   15,  20,  20,  15,  0,   -30, -30,
                         5,   15,  20,  20,  15,  5,   -30, -30, 0,   10,  15,
                         15,  10,  0,   -30, -40, -20, 0,   0,   0,   0,   -20,
                         -40, -50, -40, -30, -30, -30, -30, -40, -50};
int whiteBishopPS[64] = {-20, -10, -10, -10, -10, -10, -10, -20, -10, 5,   0,
                         0,   0,   0,   5,   -10, -10, 10,  10,  10,  10,  10,
                         10,  -10, -10, 0,   10,  10,  10,  10,  0,   -10, -10,
                         5,   5,   10,  10,  5,   5,   -10, -10, 0,   5,   10,
                         10,  5,   0,   -10, -10, 0,   0,   0,   0,   0,   0,
                         -10, -20, -10, -10, -10, -10, -10, -10, -20};
int whiteRookPS[64] = {0,  0,  0,  5,  5,  0,  0,  0,  -5, 0, 0, 0, 0, 0, 0, -5,
                       -5, 0,  0,  0,  0,  0,  0,  -5, -5, 0, 0, 0, 0, 0, 0, -5,
                       -5, 0,  0,  0,  0,  0,  0,  -5, -5, 0, 0, 0, 0, 0, 0, -5,
                       5,  10, 10, 10, 10, 10, 10, 5,  0,  0, 0, 0, 0, 0, 0, 0};

int whiteQueenPS[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20, -10, 0,   5,   0,  0,  0,   0,   -10,
    -10, 5,   5,   5,  5,  5,   0,   -10, 0,   0,   5,   5,  5,  5,   0,   -5,
    -5,  0,   5,   5,  5,  5,   0,   -5,  -10, 0,   5,   5,  5,  5,   0,   -10,
    -10, 0,   0,   0,  0,  0,   0,   -10, -20, -10, -10, -5, -5, -10, -10, -20};
int whiteKingMiddlePS[64] = {
    20,  30,  10,  0,   0,   10,  30,  20,  20,  20,  0,   0,   0,
    0,   20,  20,  -10, -20, -20, -20, -20, -20, -20, -10, -20, -30,
    -30, -40, -40, -30, -30, -20, -30, -40, -40, -50, -50, -40, -40,
    -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50,
    -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30};
int whiteKingEndPS[64] = {-50, -30, -30, -30, -30, -30, -30, -50, -30, -30, 0,
                          0,   0,   0,   -30, -30, -30, -10, 20,  30,  30,  20,
                          -10, -30, -30, -10, 30,  40,  40,  30,  -10, -30, -30,
                          -10, 30,  40,  40,  30,  -10, -30, -30, -10, 20,  30,
                          30,  20,  -10, -30, -30, -20, -10, 0,   0,   -10, -20,
                          -30, -50, -40, -30, -20, -20, -30, -40, -50};

int allPieces[65];
int evaluate(State &state) {
  allPieces[0] = -1;
  getSetBits(state.allPieces(), allPieces);

  int score = 0;
  for (int i = 0; allPieces[i] != -1; i++) {
    int pcIndex = allPieces[i];
    Piece piece = state._pieces[pcIndex];

    score += MATERIAL_WORTH[piece];

    switch (piece) {
    case wP:
      score += 100;
      // score += whitePawnPS[x][y];
      break;
    case bP:
      score -= 100;
      // score += blackPawnPS[x][y];
      break;
    case wN:
      score += 320;
      // score += whiteKnightPS[x][y];
      break;
    case bN:
      score -= 320;
      // score += blackKnightPS[x][y];
      break;
    case wB:
      score += 330;
      // score += whiteBishopPS[x][y];
      break;
    case bB:
      score -= 330;
      // score += blackBishopPS[x][y];
      break;
    case wR:
      score += 500;
      // score += whiteRookPS[x][y];
      break;
    case bR:
      score -= 500;
      // score += blackRookPS[x][y];
      break;
    case wQ:
      score += 900;
      // score += whiteQueenPS[x][y];
      break;
    case bQ:
      score -= 900;
      // score += blackQueenPS[x][y];
      break;
    case wK:
      score += 20000;
      // score += whiteKingMiddlePS[x][y];
      break;
    case bK:
      score -= 20000;
      // score += blackKingMiddlePS[x][y];
      break;
    default:
      break;
    }
  }
  return score;
}

int evaluateGameOver(State &state) {
  if (state.isInCheck(state._sideToMove)) {
    return -1000000;
  } else {
    // stalemate
    return 0;
  }
}

bool isGameOver(State &state, std::vector<Move> moves) {
  for (Move move : moves) {
    if (state.isLegalMove(move)) {
      return false;
    }
  }
  return true;
}

bool isGameOver(State &state) {
  return isGameOver(state, generatePseudoMoves(state));
}

void generateFlippedTables() {
  int MAXROWS = 8;
  int MAXCOLUMNS = 8;
  for (int r = 0; r < (MAXROWS); r++) {
    for (int c = 0; c != MAXCOLUMNS; ++c) {
      blackPawnPS[r][c] = whitePawnPS[MAXROWS - 1 - r][c] * -1;
      blackKnightPS[r][c] = whiteKnightPS[MAXROWS - 1 - r][c] * -1;
      blackBishopPS[r][c] = whiteBishopPS[MAXROWS - 1 - r][c] * -1;
      blackRookPS[r][c] = whiteRookPS[MAXROWS - 1 - r][c] * -1;
      blackQueenPS[r][c] = whiteQueenPS[MAXROWS - 1 - r][c] * -1;
      blackKingMiddlePS[r][c] = whiteKingMiddlePS[MAXROWS - 1 - r][c] * -1;
      blackKingEndPS[r][c] = whiteKingEndPS[MAXROWS - 1 - r][c] * -1;
    }
  }
}