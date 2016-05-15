//
//  board.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "board.hpp"
#include "util.hpp"
#include <cmath>
#include <iostream>
#include <ostream>

void State::printBoard() {
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

Undo::Undo(Move *move, int castleRights, int EPTarget, int halfMoveClock) {
  _move = move;
  _castleRights = castleRights;
  _EPTarget = EPTarget;
  _halfMoveClock = halfMoveClock;
}
Undo::Undo(Move *move, const State &state)
    : Undo(move, state._castleRights, state._EPTarget, state._halfMoveClock) {}

Move::Move(int from, int to, Piece promotion) {
  _from = from;
  _to = to;
  _promotion = promotion;
}

Move::Move(std::string uci) {
  if (uci.length() != 4 && uci.length() != 5) {
    return;
  }
  int from = uciToIndex(uci.substr(0, 2));
  int to = uciToIndex(uci.substr(2, 2));
  Piece promotion = EMPTY;
  if (uci.length() == 5) {
    promotion = charToPiece(uci[4]);
  }
  _from = from;
  _to = to;
  _promotion = promotion;
}

std::string Move::uci() {
  std::string uci = indexToUCI(_from) + indexToUCI(_to);
  if (_promotion != EMPTY) {
    uci += pieceToChar(_promotion);
  }
  return uci;
}
bool Move::equals(const Move &other) {
  if (_to != other._to) {
    return false;
  } else if (_from != other._from) {
    return false;
  } else if (_promotion != other._promotion) {
    return false;
  } else {
    return true;
  }
}

void State::makeMove(Move *move) {
  _history.emplace(move, *this);
  _halfMoveClock++;
  Piece movingP = _pieces[move->_from];
  move->_capturedPiece = _pieces[move->_to];
  movePiece(move->_from, move->_to);
  if (move->_promotion != EMPTY) {
    if (_sideToMove == WHITE) {
      addPiece(move->_promotion, move->_to);
    } else {
      int blackP = move->_promotion;
      blackP++;
      addPiece((Piece)blackP, move->_to);
    }
  }

  if (move->_capturedPiece != EMPTY) {
    _halfMoveClock = 0;
  }

  if (movingP == wP || movingP == bP) {
    _halfMoveClock = 0;
    // Handle en passant
    if (move->_from % 8 != move->_to % 8 && move->_capturedPiece == EMPTY) {
      clearSquare(_EPTarget);
      move->_enPassant = true;
    }
    if (std::abs(move->_from / 8 - move->_to / 8) > 1) {
      _EPTarget = move->_to;
    } else {
      _EPTarget = -1;
    }
  }

  // handle castling
  if (movingP == wK || movingP == bK) {
    _castleRights &= _sideToMove == WHITE ? ~(WKCA | WQCA) : ~(BKCA | BQCA);
    for (int y = 0; y < 8; y += 7) {
      if (move->_from == y * 8 + 4) {
        if (move->_to == y * 8 + 6) {
          movePiece(y * 8 + 7, y * 8 + 5);
          move->_castling = true;
        } else if (move->_to == y * 8 + 2) {
          movePiece(y * 8 + 0, y * 8 + 3);
          move->_castling = true;
        }
      }
    }
  }
  if (movingP == wR) {
    if (move->_from == 0) {
      _castleRights &= ~WQCA;
    } else if (move->_from == 7) {
      _castleRights &= ~WKCA;
    }
  } else if (movingP == bR) {
    if (move->_from == 56) {
      _castleRights &= ~BQCA;
    } else if (move->_from == 63) {
      _castleRights &= ~BKCA;
    }
  }

  if (move->_capturedPiece == wR) {
    if (move->_to == 0) {
      _castleRights &= ~WQCA;
    } else if (move->_to == 7) {
      _castleRights &= ~WKCA;
    }
  } else if (move->_capturedPiece == bR) {
    if (move->_to == 56) {
      _castleRights &= ~BQCA;
    } else if (move->_to == 63) {
      _castleRights &= ~BKCA;
    }
  }

  _sideToMove = -_sideToMove;
}
void State::takeMove() {
  if (_history.empty()) {
    return;
  }
  Undo undo = _history.top();
  _history.pop();
  Move *move = undo._move;
  _halfMoveClock = undo._halfMoveClock;
  _EPTarget = undo._EPTarget;
  _castleRights = undo._castleRights;
  _sideToMove = -_sideToMove;
  movePiece(move->_to, move->_from);
  if (move->_promotion != EMPTY) {
    _sideToMove == WHITE ? addPiece(wP, move->_from)
                         : addPiece(bP, move->_from);
  }
  if (move->_capturedPiece != EMPTY) {
    addPiece(move->_capturedPiece, move->_to);
  }
  if (move->_enPassant) {
    _sideToMove == WHITE ? addPiece(bP, _EPTarget) : addPiece(wP, _EPTarget);
  }
  if (move->_castling) {
    for (int y = 0; y < 8; y += 7) {
      if (move->_from == y * 8 + 4) {
        if (move->_to == y * 8 + 6) {
          movePiece(y * 8 + 5, y * 8 + 7);
        } else if (move->_to == y * 8 + 2) {
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
U64 State::allPieces() {
  return _pieceBitboards[WHITES] | _pieceBitboards[BLACKS];
}
int State::kingPos(int side) {
  U64 friendlyBB =
      side == WHITE ? _pieceBitboards[WHITES] : _pieceBitboards[BLACKS];
  return LS1B(friendlyBB & _pieceBitboards[KINGS]);
}
bool State::canCastle(int side, bool kSide) { return false; }
bool State::isInCheck(int side) { return false; }
bool State::isPositionLegal() { return true; }
bool State::isLegalMove(Move *move) {
  makeMove(move);
  bool isLegal = isPositionLegal();
  takeMove();
  return isLegal;
}