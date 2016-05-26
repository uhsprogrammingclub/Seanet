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
      score += whitePawnPS[pcIndex];
      break;
    case bP:
      score -= whitePawnPS[63 - pcIndex];
      break;
    case wN:
      score += whiteKnightPS[pcIndex];
      break;
    case bN:
      score -= whiteKnightPS[63 - pcIndex];
      break;
    case wB:
      score += whiteBishopPS[pcIndex];
      break;
    case bB:
      score -= whiteBishopPS[63 - pcIndex];
      break;
    case wR:
      score += whiteRookPS[pcIndex];
      break;
    case bR:
      score -= whiteRookPS[63 - pcIndex];
      break;
    case wQ:
      score += whiteQueenPS[pcIndex];
      break;
    case bQ:
      score -= whiteQueenPS[63 - pcIndex];
      break;
    case wK:
      score += whiteKingMiddlePS[pcIndex];
      break;
    case bK:
      score -= whiteKingMiddlePS[63 - pcIndex];
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

// void generateFlippedTables() {
//  for (int r = 0; r < 8; r++) {
//    for (int c = 0; c < 8; c++) {
//      blackPawnPS[r][c] = whitePawnPS[7 - r][7 - c] * -1;
//      blackKnightPS[r][c] = whiteKnightPS[7 - r][7 - c] * -1;
//      blackBishopPS[r][c] = whiteBishopPS[7 - r][7 - c] * -1;
//      blackRookPS[r][c] = whiteRookPS[7 - r][7 - c] * -1;
//      blackQueenPS[r][c] = whiteQueenPS[7 - r][7 - c] * -1;
//      blackKingMiddlePS[r][c] = whiteKingMiddlePS[7 - r][7 - c] * -1;
//      blackKingEndPS[r][c] = whiteKingEndPS[7 - r][7 - c] * -1;
//    }
//  }
//}