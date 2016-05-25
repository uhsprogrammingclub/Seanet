//
//  hash.hpp
//  Seanet
//
//  Created by Douglas Corley on 5/23/16.
//
//

#ifndef hash_hpp
#define hash_hpp

#include "board.hpp"
#include "defs.h"
#include "util.hpp"
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include <unordered_map>

enum NodeType { UNSET, EXACT, ALPHA, BETA };

const int TABLE_SIZE = 0x100000 * 2;

class TranspositionTable {};

struct ZobristKeys {
  U64 zArray[64][12]; // Squares + piece types (2 colors * 6 pieces * 64
                      // squares)
  U64 zSide;
  U64 zEP[8];
  U64 zCastle[4]; // Castling

  void init();
  int getIndex(U64 zHash);
  U64 getZobristHash(State s);
  U64 rand64();
  void updateHash(U64 &zKey, State s, Move m);
};

ZobristKeys z;

typedef struct {
  U64 zobrist;
  int depth;
  int score;
  bool ancient;
  NodeType type;

} S_HASHENTRY;

typedef struct {
  S_HASHENTRY *pTable;
  int numEntries;
} S_HASHTABLE;

const S_HASHENTRY NULL_HASH_ENTRY{0, 0, 0, 0, UNSET};

void initHashTable(S_HASHTABLE *table);
void clearHashTable(S_HASHTABLE *table);

void storeHashEntry(U64 zobrist, int depth, int score, bool ancient,
                    NodeType type, S_HASHTABLE *table);
S_HASHENTRY probeHashTable(S_HASHTABLE *table, U64 zobrist);

#endif /* hash_hpp */
