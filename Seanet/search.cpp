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
void search(State &state, SearchController &sControl) {
  for (int depth = 1; depth <= sControl._depthLimit; depth++) {
    S_PVLINE newLine;
    negamax(INT_MIN + 1, INT_MAX - 1, depth, state, sControl, newLine);
    if (sControl._stopSearch) {
      break;
    };
    state.bestLine = newLine;
    // sControl._bestPreviousMove = sControl._bestRootMove;

    std::cout << "Depth " << depth << "completed with an evaluation of "
              << state.bestLine.moves[0].eval << " and a best move of "
              << moveToUCI(state.bestLine.moves[0].move) << "\n";
    std::cout << "Principal Variation Line: " << pvLineToString(state.bestLine)
              << std::endl;
  }
}

int negamax(int alpha, int beta, int depth, State &state,
            SearchController &sControl, S_PVLINE &pvLine) {
  sControl._totalNodes++;

  if ((sControl._totalNodes & 1024) == 0) {
    sControl.checkTimeLimit();
  }
  if (depth <= 0) {
    pvLine.moveCount = 0;
    // return quisce(alpha, beta) -- to be implemented with quiescence search
    int eval = evaluate(state);
    eval *= state._sideToMove == WHITE ? -1 : 1;
    // printf("EVAL %i", eval);
    return eval;
  }
  S_PVLINE line;

  std::vector<int> moves = generatePseudoMoves(state);

  /*for (std::vector<int>::iterator it = moves.begin(); it != moves.end(); ++it)
  {
    if (*it == state.bestLine.moves[state._ply].move) {
      Move x = *it;
      moves.erase(it);
      moves.insert(moves.begin(), x);
      break;
    }
  }*/

  for (Move move : moves) {
    state.makeMove(move);

    if (!state.isPositionLegal()) {
      state.takeMove();
      continue;
    }
    state._ply++;
    int score = -negamax(-beta, -alpha, depth - 1, state, sControl, line);
    state._ply--;

    state.takeMove();
    if (sControl._stopSearch) {
      return 0;
    }

    if (score >= beta) {
      // printf("BETA CUTOFF: score %i, beta %i, alpha %i\n", score, beta,
      // alpha);
      return beta; // Fail hard beta-cutoff
    }
    if (score > alpha) {
      alpha = score; // Update value of "best path so far for maximizer"
      pvLine.moves[0] = S_MOVE{move, score};
      memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(S_MOVE));
      pvLine.moveCount = line.moveCount + 1;
      if (depth > 4) {
        //printf("score: %i\n", score);
      }
    }
  }
  return alpha;
}
