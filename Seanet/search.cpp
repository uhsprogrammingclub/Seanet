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
  sControl.resetStats();
  for (int depth = 1; depth <= sControl._depthLimit; depth++) {
    sControl._currDepth = depth;
    sControl._maxDepth = depth;
    S_PVLINE newLine;
    negamax(INT_MIN + 1, INT_MAX - 1, depth, state, sControl, newLine);
    if (sControl._stopSearch) {
      break;
    };
    state.bestLine = newLine;

    timeval currTime;
    gettimeofday(&currTime, 0);
    int timeElapsed =
        (int)(timeToMS(currTime) - timeToMS(sControl._startTime)) + 1;
    if (sControl._uciOutput) {
      std::cout << "info";
      std::cout << " depth " << sControl._currDepth;
      std::cout << " seldepth " << sControl._maxDepth;
      std::cout << " time " << timeElapsed;
      std::cout << " nodes " << sControl._totalNodes;
      std::cout << " score cp " << state.bestLine.moves[0].eval;
      std::cout << " nps "
                << (int)(sControl._totalNodes / (timeElapsed / 1000.0));
      std::cout << " pv " << pvLineToString(state.bestLine);
      std::cout << std::endl;
    } else {
      std::cout << sControl._currDepth;
      std::cout << " [";
      std::cout << state.bestLine.moves[0].eval;
      std::cout << "] ";
      std::cout << pvLineToString(state.bestLine);
      std::cout << "(" << timeElapsed << " ms)";
      std::cout << (int)(sControl._totalNodes / (timeElapsed));
      std::cout << " kn/s\n";
    }
  }
}

int negamax(int alpha, int beta, int depth, State &state,
            SearchController &sControl, S_PVLINE &pvLine) {
  sControl._totalNodes++;

  if ((sControl._totalNodes & 10240) == 0) {
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
    if (state._ply == 0) {
      sControl._currMove = move;
      sControl._currMoveNumber++;
    }
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
      pvLine.moves[0] =
          S_MOVE{move, score * (state._sideToMove == WHITE ? 1 : -1)};
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
  if (state._ply > sControl._maxDepth) {
    sControl._maxDepth = state._ply;
  }
  sControl._totalNodes++;

  if ((sControl._totalNodes & 10240) == 0) {
    sControl.checkTimeLimit();
  }

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
