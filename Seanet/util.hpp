//
//  util.hpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#ifndef util_hpp
#define util_hpp

#include "board.hpp"
#include "defs.h"
#include "util.hpp"
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

inline std::vector<std::string> &split(const std::string &s, char delim,
                                       std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

inline std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

inline char pieceToChar(Piece p) {
  switch (p) {
  case wP:
    return 'P';
  case bP:
    return 'p';
  case wN:
    return 'N';
  case bN:
    return 'n';
  case wB:
    return 'B';
  case bB:
    return 'b';
  case wR:
    return 'R';
  case bR:
    return 'r';
  case wQ:
    return 'Q';
  case bQ:
    return 'q';
  case wK:
    return 'K';
  case bK:
    return 'k';
  default:
    return ' ';
  }
}

inline Piece charToPiece(char p) {
  switch (p) {
  case 'P':
    return wP;
  case 'p':
    return bP;
  case 'N':
    return wN;
  case 'n':
    return bN;
  case 'B':
    return wB;
  case 'b':
    return bB;
  case 'R':
    return wR;
  case 'r':
    return bR;
  case 'Q':
    return wQ;
  case 'q':
    return bQ;
  case 'K':
    return wK;
  case 'k':
    return bK;
  default:
    return EMPTY;
  }
}

inline int bitboardForPiece(Piece p) {
  switch (p) {
  case wP:
    return PAWNS;
  case bP:
    return PAWNS;
  case wN:
    return KNIGHTS;
  case bN:
    return KNIGHTS;
  case wB:
    return BISHOPS;
  case bB:
    return BISHOPS;
  case wR:
    return ROOKS;
  case bR:
    return ROOKS;
  case wQ:
    return QUEENS;
  case bQ:
    return QUEENS;
  case wK:
    return KINGS;
  case bK:
    return KINGS;
  default:
    return 8;
  }
}

inline int sideBitboardForPiece(Piece p) {
  if (p % 2 == 0) {
    return BLACKS;
  } else {
    return WHITES;
  }
}

inline int uciToIndex(std::string uci) {
  if (uci.length() == 2) {
    int x = uci[0] - 97;
    int y = uci[1] - '0' - 1;
    return y * 8 + x;
  } else {
    return 64;
  }
}

inline std::string indexToUCI(int index) {
  int x = index % 8;
  int y = (index - x) / 8;
  std::string uci;
  uci.push_back(x + 97);
  uci.push_back(y + '0' + 1);
  return uci;
}

inline State *boardFromFEN(std::string FEN) {
  State *b = new State();
  std::vector<std::string> subFEN = split(FEN, ' ');

  std::vector<std::string> piecesByRow = split(subFEN[0], '/');
  std::string sideToMove = subFEN[1];
  std::string castlingRights = subFEN[2];
  std::string enPassantTarget = subFEN[3];
  int halfMoveClock = std::stoi(subFEN[4]);
  int fullMoveCounter = std::stoi(subFEN[5]);

  int y = 7;
  for (std::vector<std::string>::iterator it = piecesByRow.begin();
       it != piecesByRow.end(); ++it) {
    int x = 0;
    for (std::string::iterator piece = it->begin(); piece != it->end();
         ++piece) {
      if (std::isdigit(*piece)) {
        x += *piece - '0';
      } else {
        int index = y * 8 + x;
        Piece p = charToPiece(*piece);
        b->pieces[index] = p;
        b->pieceBitboards[bitboardForPiece(p)] |= 1L << index;
        b->pieceBitboards[sideBitboardForPiece(p)] |= 1L << index;
        x++;
      }
    }
    y--;
  }
  b->sideToMove = sideToMove == "w" ? WHITE : BLACK;
  for (std::string::iterator it = castlingRights.begin();
       it != castlingRights.end(); ++it) {
    switch (*it) {
    case 'Q':
      b->castleRights |= WQCA;
      break;
    case 'K':
      b->castleRights |= WKCA;
      break;
    case 'q':
      b->castleRights |= BQCA;
      break;
    case 'k':
      b->castleRights |= BKCA;
      break;
    }
  }
  b->EPTarget = uciToIndex(enPassantTarget);
  b->halfMoveClock = halfMoveClock;
  b->fullMoveCounter = fullMoveCounter;
  return b;
}

#endif /* util_hpp */
