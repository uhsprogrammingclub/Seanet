//
//  search.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#include "search.hpp"

/**
Takes input of a state, time limit, and (optional) max depth, returns the best
move for the state.sideToMove() as an int.
 **/
int search(State state, double timeLimit, int depthLimit) {
  int alpha = INT_MIN;
  int beta = INT_MAX;
  SearchController sControl;
  for (int depth = 1; depth <= depthLimit; depth++) {
    std::vector<int> moves = generatePseudoMoves(state);
    for (std::vector<int>::iterator it = moves.begin(); it != moves.end();
         ++it) {
      state.makeMove(*it);
      if (!state.isPositionLegal()) {
        state.takeMove();
        continue;
      }
      int score = alphaBeta(alpha, beta, depth - 1, state, sControl);
      state.takeMove();

      if (sControl._stopSearch) {
        break;
      };

      if (score > sControl._bestRootScore) {
        sControl._bestRootScore = score;
        sControl._bestRootMove = *it;
      }
    }

    sControl._bestPreviousMove = sControl._bestRootMove;

    std::cout << "Depth " << depth << "completed with an evaluation of "
              << sControl._bestRootScore << " and a best move of "
              << moveToUCI(sControl._bestRootMove) << "\n";
  }
  return sControl._bestRootMove;
}

int alphaBeta(int alpha, int beta, int depth, State state,
              SearchController sControl) {

  sControl._totalNodes++;

  if ((sControl._totalNodes & 1024) == 0) {
    sControl.checkTimeLimit();
  }

  if (sControl._stopSearch) {
    return 0;
  }

  if (depth <= 0) {
    // return quisce(alpha, beta) -- to be implemented with quiescence search
    int eval = evaluate(state);
    eval *= state._sideToMove == WHITE ? -1 : 1;
    return eval;
  }
  std::vector<int> moves = generatePseudoMoves(state);

  for (auto it = moves.begin(); it != moves.end(); ++it) {
    if (*it == sControl._bestPreviousMove) {
      auto x = *it;
      moves.erase(it);
      moves.insert(moves.begin(), x);
      break;
    }
  }

  for (std::vector<int>::iterator it = moves.begin(); it != moves.end(); ++it) {
    state.makeMove(*it);
    if (!state.isPositionLegal()) {
      state.takeMove();
      continue;
    }
    int score = -alphaBeta(-beta, -alpha, depth - 1, state, sControl);
    state.takeMove();
    if (score >= beta) {
      return beta; // Fail hard beta-cutoff
    }
    if (score > alpha) {
      alpha = score; // Update value of "best path so far for maximizer"
    }
  }
  return alpha;
}
