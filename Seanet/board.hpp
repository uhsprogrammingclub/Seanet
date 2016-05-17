//
//  board.hpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#ifndef board_hpp
#define board_hpp

#include "defs.h"
#include <stack>
#include <stdio.h>
#include <string>

class State;
class Undo;

class Undo {
public:
  Move _move;
  int _castleRights;
  int _EPTarget;
  int _halfMoveClock;

  inline Undo(){};
  Undo(Move move, int castleRights, int EPTarget, int halfMoveClock);
  Undo(Move move, const State &state);
};

class State {
public:
  U64 _pieceBitboards[8] = {0LL};
  Piece _pieces[64] = {EMPTY};
  int _castleRights = 0;
  int _EPTarget = -1;
  int _halfMoveClock;
  int _fullMoveCounter;
  int _sideToMove;
  std::stack<Undo> _history;

  void printBoard() const;
  void makeMove(Move move);
  void takeMove();
  void clearSquare(int index);
  void addPiece(Piece piece, int index);
  void movePiece(int from, int to);
  U64 allPieces() const;
  int kingPos(int side) const;
  bool canCastle(int side, bool kSide) const;
  bool isInCheck(int side) const;
  bool isPositionLegal() const;
  bool isLegalMove(Move move);
  bool isLegalCheckEvasion(Move move);
  bool isAbsolutePin(int pinnedSq, int attackedSq, int defendingSide);
};

#endif /* board_hpp */
