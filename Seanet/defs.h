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

extern U64 setMask[64];
extern U64 clearMask[64];

enum { WHITES, BLACKS, PAWNS, KNIGHTS, BISHOPS, ROOKS, QUEENS, KINGS };
enum Piece { EMPTY, wP, bP, wN, bN, wB, bB, wR, bR, wQ, bQ, wK, bK };

enum Side { BLACK = -1, NONE, WHITE };

enum { WKCA = 0b0001, WQCA = 0b0010, BKCA = 0b0100, BQCA = 0b1000 };

#define CLRBIT(bb, i) (bb &= ~clearMask[i])
#define SETBIT(bb, i) (bb |= setMask[i])
#endif /* defs_h */
