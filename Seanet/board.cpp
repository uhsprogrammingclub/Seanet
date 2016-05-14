//
//  board.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "board.hpp"
#include "util.hpp"
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

void State::makeMove(Move *move) {}
void State::takeMove() {}
void State::clearSquare(int index) {
  Piece p = _pieces[index];
  if (p != EMPTY) {
    CLRBIT(_pieceBitboards[bitboardForPiece(p)], index);
    CLRBIT(_pieceBitboards[sideBitboardForPiece(p)], index);
    _pieces[index] = EMPTY;
  }
}
void State::addPiece(Piece piece, int index) {}
void State::movePiece(int from, int to) {}
U64 State::allPieces() {
  return _pieceBitboards[WHITES] | _pieceBitboards[BLACKS];
}
int State::kingPos(Side side) {
  U64 friendlyBB =
      side == WHITE ? _pieceBitboards[WHITES] : _pieceBitboards[BLACKS];
  return LS1B(friendlyBB & _pieceBitboards[KINGS]);
}
bool State::canCastle(Side side, bool kSide) { return false; }
bool State::isInCheck(Side side) { return false; }
bool State::isPositionLegal() { return true; }