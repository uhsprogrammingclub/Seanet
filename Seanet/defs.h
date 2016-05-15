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
extern U64 kingAttacks[64];
extern U64 knightAttacks[64];
extern U64 pawnAttacks[2][64];
const U64 RANK_BB[8] = {0x00000000000000FFULL, 0x000000000000FF00ULL,
                        0x0000000000FF0000ULL, 0x00000000FF000000ULL,
                        0x000000FF00000000ULL, 0x0000FF0000000000ULL,
                        0x00FF000000000000ULL, 0xFF00000000000000ULL};
const U64 FILE_BB[8] = {0x0101010101010101ULL, 0x0202020202020202ULL,
                        0x0404040404040404ULL, 0x0808080808080808ULL,
                        0x1010101010101010ULL, 0x2020202020202020ULL,
                        0x4040404040404040ULL, 0x8080808080808080ULL};

enum { WHITES, BLACKS, PAWNS, KNIGHTS, BISHOPS, ROOKS, QUEENS, KINGS };
enum Piece { EMPTY, wP, bP, wN, bN, wB, bB, wR, bR, wQ, bQ, wK, bK };

enum { BLACK = -1, NONE, WHITE };

enum { WKCA = 0b0001, WQCA = 0b0010, BKCA = 0b0100, BQCA = 0b1000 };

#define CLRBIT(bb, i) (bb &= clearMask[i])
#define SETBIT(bb, i) (bb |= setMask[i])
#define UP(bb) (bb << 8)
#define DOWN(bb) (bb >> 8)
#define RIGHT(bb) (bb << 1 & ~FILE_BB[0])
#define LEFT(bb) (bb >> 1 & ~FILE_BB[7])
#endif /* defs_h */
