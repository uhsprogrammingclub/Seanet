//
//  evaluator.hpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#ifndef __Seanet__evaluator__
#define __Seanet__evaluator__

#include "board.hpp"
#include "util.hpp"
#include <stdio.h>
#include <vector>

int calculatePhase(State &state);
int evaluate(State &state);
int evaluateGameOver(State &state);
bool isGameOver(State &state);
bool isGameOver(State &state, std::vector<Move> moves);

const int PAWN_PHASE = 0;
const int KNIGHT_PHASE = 1;
const int BISHOP_PHASE = 1;
const int ROOK_PHASE = 2;
const int QUEEN_PHASE = 4;

#endif /* defined(__Seanet__evaluator__) */
