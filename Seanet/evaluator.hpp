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
int calculateMaterial(State &state);
int evaluate(State &state);
int evaluateGameOver(State &state, int contempt);
bool isGameOver(State &state);
bool isGameOver(State &state, std::vector<Move> moves);
bool isThreeFoldRepetition(State &state);
int evaluateDraw(State &state, int contempt);

int countIsolatedPawns(State &state);
int countIsolaniPawns(State &state);
int countDoubledPawns(State &state);
int countConnectedPawns(State &state);
int countBackwardPawns(State &state);
int countBlockedPawns(State &state);
int countPassedPawns(State &state);
int countHoles(State &state);
int countKingPawnShields(State &state);

FList getInitialFeatures(State &state);
float getPhaseFactor(State &state);

#endif /* defined(__Seanet__evaluator__) */
