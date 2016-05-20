//
//  evaluator.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#include "evaluator.hpp"
#include "movegenerator.hpp"

int evaluate(State &state) {
  int *allPieces = new int[65];
  getSetBits(state.allPieces(), allPieces);

  int score = 0;
  for (int i = 0; allPieces[i] != -1; i++) {
    Piece piece = state._pieces[allPieces[i]];
    switch (piece) {
    case wP:
      score += 1;
      break;
    case bP:
      score -= 1;
      break;
    case wN:
      score += 3;
      break;
    case bN:
      score -= 3;
      break;
    case wB:
      score += 3;
      break;
    case bB:
      score -= 3;
      break;
    case wR:
      score += 5;
      break;
    case bR:
      score -= 5;
      break;
    case wQ:
      score += 9;
      break;
    case bQ:
      score -= 9;
      break;
    case wK:
      score += 10000;
      break;
    case bK:
      score -= 10000;
      break;
    default:
      break;
    }
  }
  delete[] allPieces;
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