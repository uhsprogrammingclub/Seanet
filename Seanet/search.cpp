//
//  search.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#include "search.hpp"
#include <algorithm>

int quiescencePly = 0;
Move killerMoves[64 * 3];     // 64 mAX plys with 3 moves each.
int historyHeuristic[64][64]; // historyHeuristic[from][to]
S_PVLINE NULL_LINE;

/**
Takes input of a state, time limit, and (optional) max depth, returns the best
move for the state.sideToMove() as an int.
 **/
void search(State &state, SearchController &sControl) {
  sControl.resetStats();
  initHashTable(&sControl.table);

  // reset heuristics
  for (int i = 0; i < 192; i++) {
    killerMoves[i] = 0;
  }
  for (int from = 0; from < 64; from++) {
    for (int to = 0; to < 64; to++) {
      historyHeuristic[from][to] = 0;
    }
  }

  for (int depth = 1; depth <= sControl._depthLimit; depth++) {
    sControl._currDepth = depth;
    sControl._maxDepth = depth;
    negamax(INT_MIN + 1, INT_MAX - 1, depth, state, sControl, state._bestLine);
    if (sControl._stopSearch) {
      break;
    };

    timeval currTime;
    gettimeofday(&currTime, 0);
    int timeElapsed =
        (int)(timeToMS(currTime) - timeToMS(sControl._startTime)) + 1;
    if (sControl._output) {
      if (sControl._uciOutput) {
        std::cout << "info"
                  << " depth " << sControl._currDepth << " seldepth "
                  << sControl._maxDepth << " time " << timeElapsed << " nodes "
                  << sControl._totalNodes << " score cp " << state._lineEval
                  << " nps "
                  << (int)(sControl._totalNodes / (timeElapsed / 1000.0))
                  << " pv " << pvLineToString(state._bestLine) << std::endl;
      } else {
        std::cout << sControl._currDepth << " ["
                  << state._lineEval * (state._sideToMove == WHITE ? 1 : -1)
                  << "] " << pvLineToString(state._bestLine) << "; "
                  << timeElapsed << " ms; "
                  << (int)(sControl._totalNodes / (timeElapsed)) << " kn/s"
                  << "; "
                  << (float)(100.0 * sControl._fhfNodes / sControl._fhNodes)
                  << "% fhf"
                  << "; "
                  << (float)(100.0 * sControl._fhNodes / sControl._totalNodes)
                  << "% fh"
                  << "; " << (sControl._totalNodes / 1000) << "K nodes"
                  << "; seldepth " << sControl._maxDepth << std::endl;
      }
    }
  }

  if (sControl._uciOutput) {
    std::cout << "bestmove " << moveToUCI(state._bestLine.moves[0])
              << " ponder " << moveToUCI(state._bestLine.moves[1]) << std::endl;
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

bool reorderByHH(Move i, Move j) {
  return historyHeuristic[M_FROMSQ(i)][M_TOSQ(i)] >
         historyHeuristic[M_FROMSQ(j)][M_TOSQ(j)];
}

bool sortScoredMoves(const S_MOVE_AND_SCORE &i, const S_MOVE_AND_SCORE &j) {
  return i.score > j.score;
}

void pickMove(int moveNum, std::vector<S_MOVE_AND_SCORE> &scoredMoves) {
  int bestScore = INT_MIN;
  int bestNum = moveNum;
  for (std::vector<S_MOVE_AND_SCORE>::iterator it =
           scoredMoves.begin() + moveNum;
       it != scoredMoves.end(); it++) {
    if (it->score > bestScore) {
      bestScore = it->score;
      bestNum = (int)(it - scoredMoves.begin());
    }
  }
  std::swap(scoredMoves[moveNum], scoredMoves[bestNum]);
}

int negamax(int alpha, int beta, int depth, State &state,
            SearchController &sControl, S_PVLINE &pvLine) {
  sControl._totalNodes++;
  sControl._mainNodes++;
  if ((sControl._totalNodes & 10240) == 0) {
    sControl.checkTimeLimit();
  }

  // Check if TT entry exists for given state, and return stored score

  Move bestTTMove = NO_MOVE;
  if (sControl._features[TT_EVAL]) {
    S_HASHENTRY oldEntry = probeHashTable(sControl.table, state._zHash);
    if (oldEntry != NULL_ENTRY && oldEntry.zobrist == state._zHash) {
      bestTTMove = oldEntry.move;
      if (oldEntry.depth >= depth) {
        if (oldEntry.type == EXACT) {
          sControl._transpositions++;
          sControl._exactNodes++;
          return oldEntry.score;
        }
        if (oldEntry.type == ALPHA && oldEntry.score <= alpha) {
          sControl._transpositions++;
          sControl._alphaNodes++;
          return alpha;
        }
        if (oldEntry.type == BETA && oldEntry.score >= beta) {
          sControl._transpositions++;
          sControl._betaNodes++;
          return beta;
        }
      }
    }
  }

  // Check if search is at final depth
  if (depth <= 0) {
    pvLine.moveCount = 0;
    int score = qSearch(alpha, beta, state, sControl);
    if (sControl._features[TT_EVAL]) {
      // storeHashEntry(state._zHash, depth, score, NO_MOVE, EXACT,
      // sControl.table);
    }
    return score;
    // return evaluate(state) * state._sideToMove == WHITE ? 1 : -1;
  }

  if (sControl._features[NULL_MOVE] && !state.isInCheck(state._sideToMove)) {

    state.makeNullMove();
    state._ply++;
    int score =
        -negamax(-beta, -beta + 1, depth - 3, state, sControl, NULL_LINE);
    state._ply--;
    state.takeNullMove();
    if (score >= beta) {
      if (sControl._features[TT_EVAL]) {
        storeHashEntry(state._zHash, depth, beta, NO_MOVE, BETA,
                       sControl.table);
      }
      return beta;
    }
  }

  S_PVLINE line;

  NodeType flag = ALPHA;

  std::vector<Move> moves = generatePseudoMoves(state);
  std::vector<S_MOVE_AND_SCORE> scoredMoves;
  scoredMoves.reserve(moves.size());

  for (std::vector<Move>::iterator it = moves.begin(); it != moves.end();
       ++it) {

    // PV-move reorder
    if (sControl._features[PV_REORDERING] &&
        M_EQUALS(*it, state._bestLine.moves[state._ply])) {
      scoredMoves.push_back(S_MOVE_AND_SCORE{*it, 1000000});
      continue;
    }
    /*
        if (sControl._features[TT_EVAL] && M_EQUALS(*it, bestTTMove)) {
          scoredMoves.push_back(S_MOVE_AND_SCORE{*it, 990000});
          continue;
        }*/

    //   Reorder based on SEE
    if (sControl._features[SEE_REORDERING] &&
        state._pieces[M_TOSQ(*it)] != EMPTY) {
      int seeEval = see(*it, state);
      if (seeEval >= 0) {
        scoredMoves.push_back(S_MOVE_AND_SCORE{*it, 950000 + seeEval});
        continue;
      } else if (seeEval < 0) {
        scoredMoves.push_back(S_MOVE_AND_SCORE{*it, seeEval});
        continue;
      }
    }

    // Killer moves
    if (sControl._features[KH_REORDERING]) {
      if (M_EQUALS(*it, killerMoves[state._ply * 3])) {
        scoredMoves.push_back(S_MOVE_AND_SCORE{*it, 900000});
        continue;
      } else if (M_EQUALS(*it, killerMoves[state._ply * 3 + 1])) {
        scoredMoves.push_back(S_MOVE_AND_SCORE{*it, 850000});
        continue;
      } else if (M_EQUALS(*it, killerMoves[state._ply * 3 + 2])) {
        scoredMoves.push_back(S_MOVE_AND_SCORE{*it, 800000});
        continue;
      }
    }

    // Non captures sorted by history heuristic
    if (sControl._features[HH_REORDERING]) {
      int hhScore = historyHeuristic[M_FROMSQ(*it)][M_TOSQ(*it)];
      if (hhScore > 700000) {
        hhScore = 700000;
      }
      scoredMoves.push_back(S_MOVE_AND_SCORE{*it, hhScore});
      continue;
    }
    scoredMoves.push_back(S_MOVE_AND_SCORE{*it, 0});
  }

  int legal = 0;
  for (int moveNum = 0; moveNum < scoredMoves.size(); moveNum++) {
    pickMove(moveNum, scoredMoves);
    Move move = scoredMoves[moveNum].move;
    state.makeMove(move);

    if (!state.isPositionLegal()) {
      state.takeMove();
      continue;
    }
    legal++;
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
      if (sControl._features[TT_EVAL]) {
        storeHashEntry(state._zHash, depth, beta, move, BETA, sControl.table);
      }
      if (legal == 1) {
        sControl._fhfNodes++;
      }
      sControl._fhNodes++;
      if (!M_ISCAPTURE(move)) {

        if (sControl._features[KH_REORDERING]) {
          addKillerMove(state._ply, move);
        }
        if (sControl._features[HH_REORDERING]) {
          historyHeuristic[M_FROMSQ(move)][M_TOSQ(move)] += depth * depth;
        }
      }
      return beta; // Fail hard beta-cutoff
    }
    if (score > alpha) {
      flag = EXACT;
      alpha = score; // Update value of "best path so far for maximizer"
      pvLine.moves[0] = move;
      memcpy(pvLine.moves + 1, line.moves, line.moveCount * sizeof(Move));
      pvLine.moveCount = line.moveCount + 1;
      if (state._ply == 0) {
        state._lineEval = score;
      }
    }
  }
  if (!legal) {
    return evaluateGameOver(state);
  }
  if (sControl._features[TT_EVAL]) {
    storeHashEntry(state._zHash, depth, alpha, pvLine.moves[0], flag,
                   sControl.table);
  }
  return alpha;
}

int qSearch(int alpha, int beta, State &state, SearchController &sControl) {
  // printf("Quiescence Ply: %i\n", quiescencePly);
  if (state._ply > sControl._maxDepth) {
    sControl._maxDepth = state._ply;
  }
  sControl._totalNodes++;

  sControl._qNodes++;
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
