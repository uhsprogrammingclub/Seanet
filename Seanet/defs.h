//
//  defs.h
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#ifndef defs_h
#define defs_h

#include <map>

#define DEBUG true

typedef unsigned long long U64;
typedef int Move;
typedef std::map<std::string, std::string> KeyInfoMap;

extern U64 setMask[64];
extern U64 clearMask[64];
extern U64 kingAttacks[64];
extern U64 knightAttacks[64];
extern U64 pawnAttacks[2][64];
extern U64 bbBlockers8Way[64][8];
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
const int MATERIAL_WORTH[13] = {0,   100,  -100, 325,   -325,   330,    -330,
                                550, -550, 1000, -1000, 100000, -100000};

const int MATERIAL_PHASE[13] = {0, 0, 0, 1, 1, 1, 1, 2, 2, 4, 4, 0, 0};
const int TOTAL_PHASE = 24;
const double e = 2.71828;

// enum { MATERIAL_WHITE, MATERIAL_BLACK };

enum { BLACK = -1, NONE, WHITE };

enum { WKCA = 0b0001, WQCA = 0b0010, BKCA = 0b0100, BQCA = 0b1000 };

const int MAX_DEPTH = 64;
const int ASP_WINDOW = 30;
const int CHECKMATE = 1000000;

typedef struct {
  Move _move;
  int _castleRights;
  int _EPTarget;
  int _halfMoveClock;
  U64 _zHash;
} S_UNDO;

typedef struct {
  Move move;
  int score;
} S_MOVE_AND_SCORE;

typedef struct {
  int moveCount = 0;
  Move moves[MAX_DEPTH];
} S_PVLINE;

const int NUM_OF_FEATURES = 10;
enum {
  PV_REORDERING,
  SEE_REORDERING,
  KH_REORDERING,
  HH_REORDERING,
  NULL_MOVE,
  TT_EVAL,
  TT_REORDERING,
  PV_SEARCH,
  ASPIRATION_WINDOWS,
  QS_REORDERING
};

#define CLRBIT(bb, i) (bb &= clearMask[i])
#define SETBIT(bb, i) (bb |= setMask[i])
#define UP(bb) (bb << 8)
#define DOWN(bb) (bb >> 8)
#define RIGHT(bb) (bb << 1 & ~FILE_BB[0])
#define LEFT(bb) (bb >> 1 & ~FILE_BB[7])

// Date definitions
#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21

// Move bits

/*
  0000 0000 0000 0000 0011 1111 -> from M & 0x3F
  0000 0000 0000 1111 1100 0000 -> to (M >> 6) & 0x3F
  0000 0000 1111 0000 0000 0000 -> captured piece (M >> 12) & 0xF
  0000 1111 0000 0000 0000 0000 -> promotion piece (M >> 16) & 0xF
  0001 0000 0000 0000 0000 0000 -> castle M & 0x100000
  0010 0000 0000 0000 0000 0000 -> en passant M & 0x200000

 */

#define M_FROMSQ(m) (m & 0x3F)
#define M_TOSQ(m) ((m >> 6) & 0x3F)
#define M_CAPTUREDP(m) ((m >> 12) & 0xF)
#define M_PROMOTIONP(m) ((m >> 16) & 0xF)
#define M_CASTLE(m) (m & 0x100000)
#define M_EP(m) (m & 0x200000)

#define M_ISCAPTURE(m) (m & 0xF000)
#define M_ISPROMOTION(m) (m & 0xF0000)
#define NEW_MOVE(from, to) (from | (to << 6))
#define M_SETCAP(m, piece) (m |= (piece << 12))
#define M_SETPROM(m, piece) (m |= (piece << 16))
#define M_SETCASTLE(m, cas) (cas ? m |= 0x100000 : m &= 0xEFFFFF)
#define M_SETEP(m, ep) (ep ? m |= 0x200000 : m &= 0xDFFFFF)
#define M_EQUALS(m1, m2) ((0xF0FFF & m1) == (0xF0FFF & m2))

const Move NO_MOVE = 0;

const U64 magicNumberRook[64] = {
    0xa180022080400230ULL, 0x40100040022000ULL,   0x80088020001002ULL,
    0x80080280841000ULL,   0x4200042010460008ULL, 0x4800a0003040080ULL,
    0x400110082041008ULL,  0x8000a041000880ULL,   0x10138001a080c010ULL,
    0x804008200480ULL,     0x10011012000c0ULL,    0x22004128102200ULL,
    0x200081201200cULL,    0x202a001048460004ULL, 0x81000100420004ULL,
    0x4000800380004500ULL, 0x208002904001ULL,     0x90004040026008ULL,
    0x208808010002001ULL,  0x2002020020704940ULL, 0x8048010008110005ULL,
    0x6820808004002200ULL, 0xa80040008023011ULL,  0xb1460000811044ULL,
    0x4204400080008ea0ULL, 0xb002400180200184ULL, 0x2020200080100380ULL,
    0x10080080100080ULL,   0x2204080080800400ULL, 0xa40080360080ULL,
    0x2040604002810b1ULL,  0x8c218600004104ULL,   0x8180004000402000ULL,
    0x488c402000401001ULL, 0x4018a00080801004ULL, 0x1230002105001008ULL,
    0x8904800800800400ULL, 0x42000c42003810ULL,   0x8408110400b012ULL,
    0x18086182000401ULL,   0x2240088020c28000ULL, 0x1001201040c004ULL,
    0xa02008010420020ULL,  0x10003009010060ULL,   0x4008008008014ULL,
    0x80020004008080ULL,   0x282020001008080ULL,  0x50000181204a0004ULL,
    0x102042111804200ULL,  0x40002010004001c0ULL, 0x19220045508200ULL,
    0x20030010060a900ULL,  0x8018028040080ULL,    0x88240002008080ULL,
    0x10301802830400ULL,   0x332a4081140200ULL,   0x8080010a601241ULL,
    0x1008010400021ULL,    0x4082001007241ULL,    0x211009001200509ULL,
    0x8015001002441801ULL, 0x801000804000603ULL,  0xc0900220024a401ULL,
    0x1000200608243ULL};
const U64 magicNumberBishop[64] = {
    0x2910054208004104ULL, 0x2100630a7020180ULL,  0x5822022042000000ULL,
    0x2ca804a100200020ULL, 0x204042200000900ULL,  0x2002121024000002ULL,
    0x80404104202000e8ULL, 0x812a020205010840ULL, 0x8005181184080048ULL,
    0x1001c20208010101ULL, 0x1001080204002100ULL, 0x1810080489021800ULL,
    0x62040420010a00ULL,   0x5028043004300020ULL, 0xc0080a4402605002ULL,
    0x8a00a0104220200ULL,  0x940000410821212ULL,  0x1808024a280210ULL,
    0x40c0422080a0598ULL,  0x4228020082004050ULL, 0x200800400e00100ULL,
    0x20b001230021040ULL,  0x90a0201900c00ULL,    0x4940120a0a0108ULL,
    0x20208050a42180ULL,   0x1004804b280200ULL,   0x2048020024040010ULL,
    0x102c04004010200ULL,  0x20408204c002010ULL,  0x2411100020080c1ULL,
    0x102a008084042100ULL, 0x941030000a09846ULL,  0x244100800400200ULL,
    0x4000901010080696ULL, 0x280404180020ULL,     0x800042008240100ULL,
    0x220008400088020ULL,  0x4020182000904c9ULL,  0x23010400020600ULL,
    0x41040020110302ULL,   0x412101004020818ULL,  0x8022080a09404208ULL,
    0x1401210240484800ULL, 0x22244208010080ULL,   0x1105040104000210ULL,
    0x2040088800c40081ULL, 0x8184810252000400ULL, 0x4004610041002200ULL,
    0x40201a444400810ULL,  0x4611010802020008ULL, 0x80000b0401040402ULL,
    0x20004821880a00ULL,   0x8200002022440100ULL, 0x9431801010068ULL,
    0x1040c20806108040ULL, 0x804901403022a40ULL,  0x2400202602104000ULL,
    0x208520209440204ULL,  0x40c000022013020ULL,  0x2000104000420600ULL,
    0x400000260142410ULL,  0x800633408100500ULL,  0x2404080a1410ULL,
    0x138200122002900ULL};

const U64 occupancyMaskRook[64] = {
    0x101010101017eULL,    0x202020202027cULL,    0x404040404047aULL,
    0x8080808080876ULL,    0x1010101010106eULL,   0x2020202020205eULL,
    0x4040404040403eULL,   0x8080808080807eULL,   0x1010101017e00ULL,
    0x2020202027c00ULL,    0x4040404047a00ULL,    0x8080808087600ULL,
    0x10101010106e00ULL,   0x20202020205e00ULL,   0x40404040403e00ULL,
    0x80808080807e00ULL,   0x10101017e0100ULL,    0x20202027c0200ULL,
    0x40404047a0400ULL,    0x8080808760800ULL,    0x101010106e1000ULL,
    0x202020205e2000ULL,   0x404040403e4000ULL,   0x808080807e8000ULL,
    0x101017e010100ULL,    0x202027c020200ULL,    0x404047a040400ULL,
    0x8080876080800ULL,    0x1010106e101000ULL,   0x2020205e202000ULL,
    0x4040403e404000ULL,   0x8080807e808000ULL,   0x1017e01010100ULL,
    0x2027c02020200ULL,    0x4047a04040400ULL,    0x8087608080800ULL,
    0x10106e10101000ULL,   0x20205e20202000ULL,   0x40403e40404000ULL,
    0x80807e80808000ULL,   0x17e0101010100ULL,    0x27c0202020200ULL,
    0x47a0404040400ULL,    0x8760808080800ULL,    0x106e1010101000ULL,
    0x205e2020202000ULL,   0x403e4040404000ULL,   0x807e8080808000ULL,
    0x7e010101010100ULL,   0x7c020202020200ULL,   0x7a040404040400ULL,
    0x76080808080800ULL,   0x6e101010101000ULL,   0x5e202020202000ULL,
    0x3e404040404000ULL,   0x7e808080808000ULL,   0x7e01010101010100ULL,
    0x7c02020202020200ULL, 0x7a04040404040400ULL, 0x7608080808080800ULL,
    0x6e10101010101000ULL, 0x5e20202020202000ULL, 0x3e40404040404000ULL,
    0x7e80808080808000ULL};

const U64 occupancyMaskBishop[64] = {
    0x40201008040200ULL, 0x402010080400ULL,   0x4020100a00ULL,
    0x40221400ULL,       0x2442800ULL,        0x204085000ULL,
    0x20408102000ULL,    0x2040810204000ULL,  0x20100804020000ULL,
    0x40201008040000ULL, 0x4020100a0000ULL,   0x4022140000ULL,
    0x244280000ULL,      0x20408500000ULL,    0x2040810200000ULL,
    0x4081020400000ULL,  0x10080402000200ULL, 0x20100804000400ULL,
    0x4020100a000a00ULL, 0x402214001400ULL,   0x24428002800ULL,
    0x2040850005000ULL,  0x4081020002000ULL,  0x8102040004000ULL,
    0x8040200020400ULL,  0x10080400040800ULL, 0x20100a000a1000ULL,
    0x40221400142200ULL, 0x2442800284400ULL,  0x4085000500800ULL,
    0x8102000201000ULL,  0x10204000402000ULL, 0x4020002040800ULL,
    0x8040004081000ULL,  0x100a000a102000ULL, 0x22140014224000ULL,
    0x44280028440200ULL, 0x8500050080400ULL,  0x10200020100800ULL,
    0x20400040201000ULL, 0x2000204081000ULL,  0x4000408102000ULL,
    0xa000a10204000ULL,  0x14001422400000ULL, 0x28002844020000ULL,
    0x50005008040200ULL, 0x20002010080400ULL, 0x40004020100800ULL,
    0x20408102000ULL,    0x40810204000ULL,    0xa1020400000ULL,
    0x142240000000ULL,   0x284402000000ULL,   0x500804020000ULL,
    0x201008040200ULL,   0x402010080400ULL,   0x2040810204000ULL,
    0x4081020400000ULL,  0xa102040000000ULL,  0x14224000000000ULL,
    0x28440200000000ULL, 0x50080402000000ULL, 0x20100804020000ULL,
    0x40201008040200ULL};

const int magicNumberShiftsRook[64] = {
    52, 53, 53, 53, 53, 53, 53, 52, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 52, 53, 53, 53, 53, 53, 53, 52};

const int magicNumberShiftsBishop[64] = {
    58, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59, 59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59, 59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 58};

const U64 border = 0xFF818181818181FFULL;

const int index64[64] = {0,  47, 1,  56, 48, 27, 2,  60, 57, 49, 41, 37, 28,
                         16, 3,  61, 54, 58, 35, 52, 50, 42, 21, 44, 38, 32,
                         29, 23, 17, 11, 4,  62, 46, 55, 26, 59, 40, 36, 15,
                         53, 34, 51, 20, 43, 31, 22, 10, 45, 25, 39, 14, 33,
                         19, 30, 9,  24, 13, 18, 8,  12, 7,  6,  5,  63};

extern unsigned char popCountOfByte256[256];

#endif /* defs_h */
