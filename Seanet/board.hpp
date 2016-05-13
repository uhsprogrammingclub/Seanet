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

class Move {
public:
  int toSqr;
  int fromSqr;
  Piece promotion = EMPTY;
  Piece capturedPiece = EMPTY;
  bool castling = false;
  bool enPassant = false;
};

class Undo {
public:
  Move move;
  int castleRights;
  int EPTarget;
  int halfMoveClock;
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
};

#endif /* board_hpp */
