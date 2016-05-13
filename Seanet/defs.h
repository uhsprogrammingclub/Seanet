//
//  defs.h
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#ifndef defs_h
#define defs_h

typedef unsigned long long U64;

enum { WHITES, BLACKS, PAWNS, KNIGHTS, BISHOPS, ROOKS, QUEENS, KINGS };
enum Piece { EMPTY, wP, bP, wN, bN, wB, bB, wR, bR, wQ, bQ, wK, bK };

enum Side { BLACK = -1, NONE, WHITE };

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

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

#endif /* defs_h */
