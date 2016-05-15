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
class Move;
class Undo;

class Move {
public:
  int _to = -1;
  int _from = -1;
  Piece _promotion = EMPTY;
  Piece _capturedPiece = EMPTY;
  bool _castling = false;
  bool _enPassant = false;

  Move(int from, int to, Piece promotion = EMPTY);
  Move(std::string uci);

  std::string uci();
  bool equals(const Move &other) const;
};

class Undo {
public:
  Move *_move;
  int _castleRights;
  int _EPTarget = -1;
  int _halfMoveClock;

  inline Undo(){};
  Undo(Move *move, int castleRights, int EPTarget, int halfMoveClock);
  Undo(Move *move, const State &state);
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

  void printBoard();
  void makeMove(Move *move);
  void takeMove();
  void clearSquare(int index);
  void addPiece(Piece piece, int index);
  void movePiece(int from, int to);
  U64 allPieces() const;
  int kingPos(int side) const;
  bool canCastle(int side, bool kSide) const;
  bool isInCheck(int side) const;
  bool isPositionLegal() const;
  bool isLegalMove(Move *move);
};

static bool operator==(const Move &m1, const Move &m2) { return m1.equals(m2); }

#endif /* board_hpp */
