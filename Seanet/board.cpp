//
//  board.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "board.hpp"
#include "movegenerator.hpp"
#include "util.hpp"
#include <cmath>
#include <iostream>
#include <ostream>

void State::printBoard() const {
  for (int y = 7; y >= 0; y--) {
    std::string string = "";
    for (int x = 0; x < 8; x++) {
      string += "|";
      string += pieceToChar(_pieces[y * 8 + x]);
    }
    string += "|\n";
    std::cout << y + 1 << string;
  }
  std::cout << "  A B C D E F G H \n";
}

Undo::Undo(Move move, int castleRights, int EPTarget, int halfMoveClock) {
  _move = move;
  _castleRights = castleRights;
  _EPTarget = EPTarget;
  _halfMoveClock = halfMoveClock;
}
Undo::Undo(Move move, const State &state)
    : Undo(move, state._castleRights, state._EPTarget, state._halfMoveClock) {}

void State::makeMove(Move move) {
  _history.emplace(move, *this);
  _halfMoveClock++;
  int from = M_FROMSQ(move);
  int to = M_TOSQ(move);
  Piece movingP = _pieces[from];
  M_SETCAP(move, _pieces[to]);
  movePiece(from, to);
  if (M_ISPROMOTION(move)) {
    if (_sideToMove == BLACK) {
      addPiece((Piece)M_PROMOTIONP(move), to);
    } else {
      int whiteP = (Piece)M_PROMOTIONP(move);
      whiteP--;
      addPiece((Piece)whiteP, to);
    }
  }

  if (M_ISCAPTURE(move)) {
    _halfMoveClock = 0;
  }

  if (movingP == wP || movingP == bP) {
    _halfMoveClock = 0;
    // Handle en passant
    int fromX = from % 8;
    int toX = to % 8;
    if (fromX != toX && !M_ISCAPTURE(move)) {
      if (_EPTarget == -1) {
        printf("ERROR: EN PASSANT ON INVALID SQUARE! Move: %s (%i); captured "
               "Piece: %i\n",
               moveToUCI(move).c_str(), move, _pieces[to]);
      }
      clearSquare(_EPTarget);
      M_SETEP(move, true);
    }
    if (std::abs((from - fromX) / 8 - (to - toX) / 8) > 1) {
      _EPTarget = to;
    } else {
      _EPTarget = -1;
    }
  } else {
    _EPTarget = -1;
  }

  // handle castling
  if (movingP == wK || movingP == bK) {
    _castleRights &= _sideToMove == WHITE ? ~(WKCA | WQCA) : ~(BKCA | BQCA);
    for (int y = 0; y < 8; y += 7) {
      if (from == y * 8 + 4) {
        if (to == y * 8 + 6) {
          movePiece(y * 8 + 7, y * 8 + 5);
          M_SETCASTLE(move, true);
        } else if (to == y * 8 + 2) {
          movePiece(y * 8 + 0, y * 8 + 3);
          M_SETCASTLE(move, true);
        }
      }
    }
  }
  if (movingP == wR) {
    if (from == 0) {
      _castleRights &= ~WQCA;
    } else if (from == 7) {
      _castleRights &= ~WKCA;
    }
  } else if (movingP == bR) {
    if (from == 56) {
      _castleRights &= ~BQCA;
    } else if (from == 63) {
      _castleRights &= ~BKCA;
    }
  }

  if (M_CAPTUREDP(move) == wR) {
    if (to == 0) {
      _castleRights &= ~WQCA;
    } else if (to == 7) {
      _castleRights &= ~WKCA;
    }
  } else if (M_CAPTUREDP(move) == bR) {
    if (to == 56) {
      _castleRights &= ~BQCA;
    } else if (to == 63) {
      _castleRights &= ~BKCA;
    }
  }

  _sideToMove = -_sideToMove;
  _history.top()._move = move;
}
void State::takeMove() {
  if (_history.empty()) {
    return;
  }
  Undo undo = _history.top();
  _history.pop();
  int move = undo._move;
  _halfMoveClock = undo._halfMoveClock;
  _EPTarget = undo._EPTarget;
  _castleRights = undo._castleRights;
  _sideToMove = -_sideToMove;
  int from = M_FROMSQ(move);
  int to = M_TOSQ(move);
  movePiece(to, from);
  if (M_ISPROMOTION(move)) {
    _sideToMove == WHITE ? addPiece(wP, from) : addPiece(bP, from);
  }
  if (M_ISCAPTURE(move)) {
    addPiece((Piece)M_CAPTUREDP(move), to);
  }
  if (M_EP(move)) {
    _sideToMove == WHITE ? addPiece(bP, _EPTarget) : addPiece(wP, _EPTarget);
  }
  if (M_CASTLE(move)) {
    for (int y = 0; y < 8; y += 7) {
      if (from == y * 8 + 4) {
        if (to == y * 8 + 6) {
          movePiece(y * 8 + 5, y * 8 + 7);
        } else if (to == y * 8 + 2) {
          movePiece(y * 8 + 3, y * 8 + 0);
        }
      }
    }
  }
}
void State::clearSquare(int index) {
  Piece p = _pieces[index];
  if (p != EMPTY) {
    CLRBIT(_pieceBitboards[bitboardForPiece(p)], index);
    CLRBIT(_pieceBitboards[sideBitboardForPiece(p)], index);
    _pieces[index] = EMPTY;
  }
}
void State::addPiece(Piece p, int index) {
  clearSquare(index);
  SETBIT(_pieceBitboards[bitboardForPiece(p)], index);
  SETBIT(_pieceBitboards[sideBitboardForPiece(p)], index);
  _pieces[index] = p;
}
void State::movePiece(int from, int to) {
  Piece p = _pieces[from];
  clearSquare(from);
  addPiece(p, to);
}
U64 State::allPieces() const {
  return _pieceBitboards[WHITES] | _pieceBitboards[BLACKS];
}
int State::kingPos(int side) const {
  U64 friendlyBB =
      side == WHITE ? _pieceBitboards[WHITES] : _pieceBitboards[BLACKS];
  int index = LS1B(friendlyBB & _pieceBitboards[KINGS]);
  if (index == -1) {
    printf("NO KING DETECTED!\n");
    printBoard();
  }
  return index;
}
bool State::canCastle(int side, bool kSide) const {
  if (isInCheck(side)) {
    return false;
  }

  U64 allBB = allPieces();

  if (side == WHITE && (_castleRights & WKCA) != 0 && kSide) {
    if ((allBB & 0x60ULL) != 0 || attacksTo(5, *this, side) != 0 ||
        attacksTo(6, *this, side) != 0) {
      return false;
    }
    return true;
  } else if (side == BLACK && (_castleRights & BKCA) != 0 && kSide) {
    if ((allBB & 0x6000000000000000ULL) != 0 ||
        attacksTo(61, *this, side) != 0 || attacksTo(62, *this, side) != 0) {
      return false;
    }
    return true;
  } else if (side == WHITE && (_castleRights & WQCA) != 0 && !kSide) {
    if ((allBB & 0xEULL) != 0 || attacksTo(3, *this, side) != 0 ||
        attacksTo(2, *this, side) != 0) {
      return false;
    }
    return true;
  } else if (side == BLACK && (_castleRights & BQCA) != 0 && !kSide) {
    if ((allBB & 0xE00000000000000ULL) != 0 ||
        attacksTo(59, *this, side) != 0 || attacksTo(58, *this, side) != 0) {
      return false;
    }
    return true;
  }
  return false;
}
bool State::isInCheck(int side) const {
  if (attacksTo(kingPos(side), *this, side) == 0) {
    return false;
  } else {
    return true;
  }
}

bool State::isPositionLegal() const {
  if (isInCheck(-_sideToMove)) {
    return false;
  } else {
    return true;
  }
}

bool State::isLegalMove(Move move) {
  makeMove(move);
  bool isLegal = isPositionLegal();
  takeMove();
  return isLegal;
}

bool State::isAbsolutePin(int pinnedSq, int attackedSq, int defendingSide) {
  int numOfAttacksWithPin =
      countSetBits(attacksTo(attackedSq, *this, defendingSide));
  U64 bbWithoutPin = allPieces();
  CLRBIT(bbWithoutPin, pinnedSq);
  int numOfAttacksWithoutPin =
      countSetBits(attacksTo(attackedSq, *this, defendingSide, bbWithoutPin));
  if (numOfAttacksWithPin < numOfAttacksWithoutPin) {
    return true;
  } else {
    return false;
  }
}