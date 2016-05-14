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
      string += pieceToChar(pieces[y * 8 + x]);
    }
    string += "|\n";
    std::cout << y + 1 << string;
  }
  std::cout << "  A B C D E F G H \n";
}

Undo::Undo() {}

Undo::Undo(Move *move, int castleRights, int EPTarget, int halfMoveClock) {
  this->move = move;
  this->castleRights = castleRights;
  this->EPTarget = EPTarget;
  this->halfMoveClock = halfMoveClock;
}
Undo::Undo(Move *move, State *state) {
  this->move = move;
  this->castleRights = state->castleRights;
  this->EPTarget = state->EPTarget;
  this->halfMoveClock = state->halfMoveClock;
}

Move::Move() {}

Move::Move(int from, int to, Piece promotion) {
  this->from = from;
  this->to = to;
  this->promotion = promotion;
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
  this->from = from;
  this->to = to;
  this->promotion = promotion;
}

std::string Move::uci() {
  std::string uci = indexToUCI(from) + indexToUCI(to);
  if (this->promotion != EMPTY) {
    uci += pieceToChar(this->promotion);
  }
  return uci;
}
bool Move::equals(Move *other) {
  if (this->to != other->to) {
    return false;
  } else if (this->from != other->from) {
    return false;
  } else if (this->promotion != other->promotion) {
    return false;
  } else {
    return true;
  }
}

State::State() {}

void State::makeMove(Move *move) {}
void State::takeMove() {}
void State::clearSquare(int index) {}
void State::addPiece(Piece piece, int index) {}
void State::movePiece(int from, int to) {}
U64 State::allPieces() {
  return this->pieceBitboards[WHITES] | this->pieceBitboards[BLACKS];
}
int State::kingPos(Side side) {
  U64 friendlyBB = side == WHITE ? this->pieceBitboards[WHITES]
                                 : this->pieceBitboards[BLACKS];
  return LS1B(friendlyBB & this->pieceBitboards[KINGS]);
}
bool State::canCastle(Side side, bool kSide) { return false; }
bool State::isInCheck(Side side) { return false; }
bool State::isPositionLegal() { return true; }