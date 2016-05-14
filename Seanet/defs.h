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

#endif /* defs_h */
