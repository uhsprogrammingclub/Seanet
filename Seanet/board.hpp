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
#include <stdio.h>
#include <string>

class State;
class Move;
class Undo;

class Move {
public:
  int to = 64;
  int from = 64;
  Piece promotion = EMPTY;
  Piece capturedPiece = EMPTY;
  bool castling = false;
  bool enPassant = false;

  Move();
  Move(int from, int to, Piece promotion);
  Move(std::string uci);

  std::string uci();
  bool equals(Move *other);
};

class Undo {
public:
  Move *move;
  int castleRights;
  int EPTarget;
  int halfMoveClock;

  Undo();
  Undo(Move *move, int castleRights, int EPTarget, int halfMoveClock);
  Undo(Move *move, State *state);
};

class State {
public:
  U64 pieceBitboards[8];
  Piece pieces[64];
  int castleRights = 0;
  int EPTarget = 64;
  int halfMoveClock;
  int fullMoveCounter;
  Side sideToMove;
  Undo history[2048];

  void printBoard();
  State();
};

#endif /* board_hpp */
