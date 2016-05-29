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

int calculatePhase(State &state) {
  int currentPhase =
      countSetBits(state._pieceBitboards[PAWNS]) * MATERIAL_PHASE[wP] +
      countSetBits(state._pieceBitboards[KNIGHTS]) * MATERIAL_PHASE[wK] +
      countSetBits(state._pieceBitboards[BISHOPS]) * MATERIAL_PHASE[wB] +
      countSetBits(state._pieceBitboards[ROOKS]) * MATERIAL_PHASE[wR] +
      countSetBits(state._pieceBitboards[QUEENS]) * MATERIAL_PHASE[wQ];
	return ((TOTAL_PHASE - currentPhase) * 256 + (TOTAL_PHASE / 2)) / TOTAL_PHASE;
}

int calculateMaterial(State &state) {
  allPieces[0] = -1;
  getSetBits(state.allPieces(), allPieces);

  int score = 0;
  for (int i = 0; allPieces[i] != -1; i++) {
    int pcIndex = allPieces[i];
    Piece piece = state._pieces[pcIndex];

    score += MATERIAL_WORTH[piece];
  }

  return score;
}

int evaluate(State &state) {
  allPieces[0] = -1;
  getSetBits(state.allPieces(), allPieces);

  int score = state._material;
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
      score += ((whiteKingMiddlePS[pcIndex] * (256 - state._phase)) +
                (whiteKingEndPS[pcIndex] * state._phase)) /
               256;
      // score += whiteKingMiddlePS[pcIndex];
      break;
    case bK:
      score -= ((whiteKingMiddlePS[63 - pcIndex] * (256 - state._phase)) +
                (whiteKingEndPS[63 - pcIndex] * state._phase)) /
               256;
      // score -= whiteKingMiddlePS[63 - pcIndex];
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
