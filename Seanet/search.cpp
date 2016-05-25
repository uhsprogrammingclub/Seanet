//
//  search.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#include "search.hpp"

int quiescencePly = 0;
Move killerMoves[64 * 3]; // 64 mAX plys with 3 moves each.
/**
Takes input of a state, time limit, and (optional) max depth, returns the best
move for the state.sideToMove() as an int.
 **/
void search(State &state, SearchController &sControl) {
  sControl.resetStats();
  for (int i = 0; i < 192; i++) {
    killerMoves[i] = 0;
  }
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
      std::cout << "; " << timeElapsed << " ms; ";
      std::cout << (int)(sControl._totalNodes / (timeElapsed));
      std::cout << " kn/s";
      std::cout << "; "
                << (float)(100.0 * sControl._fhfNodes / sControl._fhNodes)
                << "% fhf";
      std::cout << "; "
                << (float)(100.0 * sControl._fhNodes / sControl._totalNodes)
                << "% fh";
      std::cout << "; " << (sControl._totalNodes / 1000) << "K nodes";
      std::cout << "; seldepth ";
      std::cout << sControl._maxDepth;
      std::cout << std::endl;
    }
  }
}

void addKillerMove(int ply, Move move) {
  int firstMoveIndex = ply * 3;
  if (M_EQUALS(move, killerMoves[firstMoveIndex])) {
  } else if (M_EQUALS(move, killerMoves[firstMoveIndex + 1])) {
    std::swap(killerMoves[firstMoveIndex], killerMoves[firstMoveIndex + 1]);
  } else {
    std::swap(killerMoves[firstMoveIndex + 1], killerMoves[firstMoveIndex + 2]);
    std::swap(killerMoves[firstMoveIndex], killerMoves[firstMoveIndex + 1]);
    killerMoves[firstMoveIndex] = move;
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
  int insertNextMoveAt = 0;
  int insertBadMoveAt = (int)moves.size() - 1;
  //  std::cout << *(moves.begin() + insertBadMoveAt +1);
  //	std::cout << std::endl;
  //  std::cout << *(moves.end());
  //  std::cout << std::endl << std::endl;

  // PV-move reorder
  for (std::vector<int>::iterator it = moves.begin() + insertNextMoveAt;
       it != moves.end(); ++it) {
    if (M_EQUALS(*it, state.bestLine.moves[state._ply].move)) {
      std::swap(moves[insertNextMoveAt], moves[it - moves.begin()]);
      insertNextMoveAt++;
      break;
    }
  }
  // Hash move reorder
  // TODO

  //   Reorder based on SEE
  for (std::vector<int>::iterator it = moves.begin() + insertNextMoveAt;
       it <= moves.begin() + insertBadMoveAt; ++it) {
    int seeEval = see(*it, state);
    if (seeEval > 0) {
      std::swap(moves[insertNextMoveAt], moves[it - moves.begin()]);
      insertNextMoveAt++;
    } else if (seeEval < 0) {
      std::swap(moves[insertBadMoveAt], moves[it - moves.begin()]);
      insertBadMoveAt--;
      it--;
    }
  }

  // Killer moves
  Move killers[3] = {killerMoves[state._ply * 3],
                     killerMoves[state._ply * 3 + 1],
                     killerMoves[state._ply * 3 + 2]};
  int killerIndex[3] = {-1, -1, -1};

  for (std::vector<int>::iterator it = moves.begin() + insertNextMoveAt;
       it <= moves.begin() + insertBadMoveAt; ++it) {
    if (M_EQUALS(*it, killers[0])) {
      killerIndex[0] = (int)(it - moves.begin());
    } else if (M_EQUALS(*it, killers[1])) {
      killerIndex[1] = (int)(it - moves.begin());
    } else if (M_EQUALS(*it, killers[2])) {
      killerIndex[2] = (int)(it - moves.begin());
    }
  }
  for (int i = 0; i < 3; i++) {
    if (killerIndex[i] != -1) {
      std::swap(moves[insertNextMoveAt], moves[killerIndex[i]]);
      insertNextMoveAt++;
    }
  }

  // Non captures sorted by history heuristic
  // TODO

  //  printf("Reordered moves: ");
  //  for (auto it = moves.begin(); it != moves.end(); ++it) {
  //    std::cout << moveToUCI(*it) << "(" << see(*it, state) << "), ";
  //  }
  //  std::cout << '\n';

  int moveNum = 0;
  bool gameOver = true; // set to false if there's a legal move
  for (Move move : moves) {
    state.makeMove(move);

    if (!state.isPositionLegal()) {
      state.takeMove();
      continue;
    }
    moveNum++;
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
      if (moveNum == 1) {
        sControl._fhfNodes++;
      }
      sControl._fhNodes++;
      if (!M_ISCAPTURE(move)) {
        addKillerMove(state._ply, move);
      }
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
    int moveNum = 0;
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
      moveNum++;
      gameOver = false;
      state._ply++;
      quiescencePly++;
      int score = -qSearch(-beta, -alpha, state, sControl);
      state._ply--;
      quiescencePly--;

      state.takeMove();

      if (score >= beta) {
        if (moveNum == 1) {
          sControl._fhfNodes++;
        }
        sControl._fhNodes++;
        return beta;
      }
      if (score > alpha) {
        alpha = score;
      }
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
