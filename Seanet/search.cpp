//
//  search.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#include "search.hpp"

int quiescencePly = 0;
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
    /*
  std::cout << "Depth " << depth << " completed with an evaluation of "
            << state.bestLine.moves[0].eval << " and a best move of "
            << moveToUCI(state.bestLine.moves[0].move) << "\n";
  std::cout << "Principal Variation Line: " << pvLineToString(state.bestLine)
            << std::endl;*/
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
    return qSearch(alpha, beta, state, sControl);
    // return evaluate(state) * state._sideToMove == WHITE ? 1 : -1;
  }
  S_PVLINE line;

  std::vector<int> moves = generatePseudoMoves(state);

  for (std::vector<int>::iterator it = moves.begin(); it != moves.end(); ++it) {
    if (M_EQUALS(*it, state.bestLine.moves[state._ply].move)) {
      Move x = *it;
      moves.erase(it);
      moves.insert(moves.begin(), x);
      break;
    }
  }
  bool gameOver = true; // set to false if there's a legal move
  for (Move move : moves) {
    state.makeMove(move);

    if (!state.isPositionLegal()) {
      state.takeMove();
      continue;
    }
    gameOver = false;
    state._ply++;
    int score = -negamax(-beta, -alpha, depth - 1, state, sControl, line);
    state._ply--;

    state.takeMove();
    if (sControl._stopSearch) {
      return 0;
    }

    if (score >= beta) {
      return beta; // Fail hard beta-cutoff
    }
    if (score > alpha) {
      alpha = score; // Update value of "best path so far for maximizer"
      pvLine.moves[0] = S_MOVE{move, score};
      memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(S_MOVE));
      pvLine.moveCount = line.moveCount + 1;
      if (depth > 4) {
        // printf("score: %i\n", score);
      }
    }
  }
  if (gameOver) {
    return evaluateGameOver(state);
  }
  return alpha;
}

int qSearch(int alpha, int beta, State &state, SearchController &sControl) {
  // printf("Quiescence Ply: %i\n", quiescencePly);
  sControl._totalNodes++;

  bool inCheck = state.isInCheck(state._sideToMove);
  std::vector<int> moves = generatePseudoMoves(state, inCheck);
  bool gameOver = true; // assume game is over until we find a legal move

  int stand_pat = evaluate(state) * (state._sideToMove == WHITE ? 1 : -1);
  if (stand_pat >= beta) {
    alpha = beta;
  } else {

    if (alpha < stand_pat) {
      alpha = stand_pat;
    }

    for (Move move : moves) {
      if (!inCheck) {
        if ((state._pieces[M_TOSQ(move)] == EMPTY && !M_ISPROMOTION(move)) ||
            see(move, state) < 0) {
          continue;
        }
      }

      state.makeMove(move);

      if (!state.isPositionLegal()) {
        state.takeMove();
        continue;
      }
      gameOver = false;
      state._ply++;
      quiescencePly++;
      int score = -qSearch(-beta, -alpha, state, sControl);
      state._ply--;
      quiescencePly--;

      state.takeMove();

      if (score >= beta)
        return beta;
      if (score > alpha)
        alpha = score;
    }
  }
  if (alpha == beta) { // check game over for beta cutoff
    if (isGameOver(state, moves)) {
      return evaluateGameOver(state);
    }
  } else {
    if (gameOver && (inCheck || isGameOver(state, moves))) {
      return evaluateGameOver(state);
    }
  }
  return alpha;
}
