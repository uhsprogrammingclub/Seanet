//
//  hash.cpp
//  Seanet
//
//  Created by Douglas Corley on 5/23/16.
//
//

#include "hash.hpp"

void initHashTable(S_HASHTABLE *table) {
  table->numEntries = TABLE_SIZE / sizeof(S_HASHENTRY) - 2;
  free(table->pTable);
  table->pTable =
      (S_HASHENTRY *)malloc(table->numEntries * sizeof(S_HASHENTRY));
  clearHashTable(table);
  std::cout << "Hash Table init complete with " << table->numEntries
            << " entries." << std::endl;
}

void clearHashTable(S_HASHTABLE *table) {

  S_HASHENTRY *hashEntry;
  for (hashEntry = table->pTable; hashEntry < table->pTable + table->numEntries;
       hashEntry++) {
    hashEntry->zobrist = 0;
    hashEntry->depth = 0;
    hashEntry->score = 0;
    hashEntry->ancient = 0;
    hashEntry->type = UNSET;
  }
}

void storeHashEntry(U64 zobrist, int depth, int score, bool ancient,
                    NodeType type, S_HASHTABLE *table) {
  int index = zobrist % table->numEntries;
  assert(index > 0 && index < table->numEntries - 1);
  if (table->pTable[index].zobrist > 0) {
    if (table->pTable[index].ancient == false ||
        (table->pTable[index].depth < depth &&
         table->pTable[index].zobrist == zobrist)) {
      table->pTable[index].zobrist = zobrist;
      table->pTable[index].depth = depth;
      table->pTable[index].score = score;
      table->pTable[index].ancient = ancient;
      table->pTable[index].type = type;
    }
  } else {
    table->pTable[index].zobrist = zobrist;
    table->pTable[index].depth = depth;
    table->pTable[index].score = score;
    table->pTable[index].ancient = ancient;
    table->pTable[index].type = type;
  }
}

S_HASHENTRY probeHashTable(S_HASHTABLE *table, U64 zobrist) {
  int index = zobrist % table->numEntries;
  assert(index > 0 && index < table->numEntries - 1);
  if (table->pTable[index].zobrist == zobrist) {
    return table->pTable[index];
  } else {
    return NULL_HASH_ENTRY;
  }
}

void ZobristKeys::init() {
  timeval currTime;
  gettimeofday(&currTime, 0);
  int now = (int)(timeToMS(currTime));

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 12; j++) {
      zArray[i][j] = rand64();
    }
  }
  for (int i = 0; i < 8; i++)
    zEP[i] = rand64();
  zSide = rand64();
  for (int i = 0; i < 4; i++) {
    zCastle[i] = rand64();
  }
}

int ZobristKeys::getIndex(U64 zHash) {
  return (int)(zHash % 100 /*TranspositionTable.hashSize*/);
}

U64 ZobristKeys::rand64() {
  return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30) ^
         ((U64)rand() << 45) ^ ((U64)rand() << 60);
}

void ZobristKeys::updateHash(U64 &zKey, State s, Move m) {
  int from = M_FROMSQ(m);
  int to = M_TOSQ(m);
  int pieceIndex = (int)(s._pieces[from]) - 1;
  int captureIndex = (int)(s._pieces[to]) - 1;
  zKey ^= zArray[from][pieceIndex];
  if (captureIndex > -1) {
    zKey ^= zArray[to][captureIndex];
  }
  zKey ^= zArray[to][pieceIndex];
  /* Function not complete */
}

U64 ZobristKeys::getZobristHash(State s) {
  U64 zKey = 0;
  for (int i = 0; i < 64; i++) {
    switch (s._pieces[i]) {
    case EMPTY:
      break;
    case wP:
      zKey ^= zArray[i][0];
      break;
    case bP:
      zKey ^= zArray[i][1];
      break;
    case wN:
      zKey ^= zArray[i][2];
      break;
    case bN:
      zKey ^= zArray[i][3];
      break;
    case wB:
      zKey ^= zArray[i][4];
      break;
    case bB:
      zKey ^= zArray[i][5];
      break;
    case wR:
      zKey ^= zArray[i][6];
      break;
    case bR:
      zKey ^= zArray[i][7];
      break;
    case wQ:
      zKey ^= zArray[i][8];
      break;
    case bQ:
      zKey ^= zArray[i][9];
      break;
    case wK:
      zKey ^= zArray[i][10];
      break;
    case bK:
      zKey ^= zArray[i][11];
      break;
    default:
      std::cout
          << "Unknown piece uncovered in getZobristHash() function. Exiting...";
      exit(0);
    }
  }
  if (s._sideToMove == WHITE) {
    zKey ^= zSide;
  }
  if ((s._castleRights & WKCA) != 0) {
    zKey ^= zCastle[0];
  }
  if ((s._castleRights & WQCA) != 0) {
    zKey ^= zCastle[1];
  }
  if ((s._castleRights & BKCA) != 0) {
    zKey ^= zCastle[2];
  }
  if ((s._castleRights & BQCA) != 0) {
    zKey ^= zCastle[3];
  }
  return zKey;
}
