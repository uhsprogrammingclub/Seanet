//
//  evaluator.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#include "evaluator.hpp"

int evaluate(State &state) {
  int *allPieces = new int[65];
  getSetBits(state.allPieces(), allPieces);

  int score = 0;
  for (int i = 0; i < 65; i++) {
    if (allPieces[i] == -1) {
      break;
    }
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
  return score;
}
