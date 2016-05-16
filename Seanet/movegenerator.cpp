//
//  movegenerator.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "movegenerator.hpp"
#include "util.hpp"

U64 pawnPush(U64 bb, int pawnSide, const State &s);
U64 pawnAttack(int index, int pawnSide, const State &s);
U64 rookAttacks(int index, U64 consideredPieces);
U64 rookAttacks(int index, const State &s) {
  return rookAttacks(index, s.allPieces());
};
U64 bishopAttacks(int index, U64 consideredPieces);
U64 bishopAttacks(int index, const State &s) {
  return bishopAttacks(index, s.allPieces());
};
U64 magicMovesRook[64][4096];

U64 magicMovesBishop[64][4096];
U64 occupancyVariation[64][4096];

std::vector<int> generatePseudoMoves(const State &s) {
  std::vector<int> moves;
  moves.reserve(64);
  U64 friendlyBB = s._sideToMove == WHITE ? s._pieceBitboards[WHITES]
                                          : s._pieceBitboards[BLACKS];

  int *pieceSquares = new int[65];
  int *pieceMoves = new int[65];
  // King Moves
  int kingIndex = LS1B(friendlyBB & s._pieceBitboards[KINGS]);
  getSetBits(kingAttacks[kingIndex] & ~friendlyBB, pieceMoves);
  for (int to = 0; pieceMoves[to] != -1; to++) {
    moves.push_back(NEW_MOVE(kingIndex, pieceMoves[to]));
  }

  // Castling moves
  if (s.canCastle(s._sideToMove, true)) {
    moves.push_back(NEW_MOVE(kingIndex, (kingIndex + 2)));
  }
  if (s.canCastle(s._sideToMove, false)) {
    moves.push_back(NEW_MOVE(kingIndex, (kingIndex - 2)));
  }

  // Knight Moves
  getSetBits(friendlyBB & s._pieceBitboards[KNIGHTS], pieceSquares);

  for (int from = 0; pieceSquares[from] != -1; from++) {
    getSetBits(knightAttacks[pieceSquares[from]] & ~friendlyBB, pieceMoves);
    for (int to = 0; pieceMoves[to] != -1; to++) {
      moves.push_back(NEW_MOVE(pieceSquares[from], pieceMoves[to]));
    }
  }

  // Pawn moves
  getSetBits(friendlyBB & s._pieceBitboards[PAWNS], pieceSquares);

  for (int from = 0; pieceSquares[from] != -1; from++) {
    getSetBits(pawnPush(setMask[pieceSquares[from]], s._sideToMove, s) |
                   pawnAttack(pieceSquares[from], s._sideToMove, s),
               pieceMoves);
    for (int to = 0; pieceMoves[to] != -1; to++) {

      if (pieceMoves[to] / 8 < 1 || pieceMoves[to] / 8 >= 7) {
        int move1 = NEW_MOVE(pieceSquares[from], pieceMoves[to]);
        int move2 = NEW_MOVE(pieceSquares[from], pieceMoves[to]);
        int move3 = NEW_MOVE(pieceSquares[from], pieceMoves[to]);
        int move4 = NEW_MOVE(pieceSquares[from], pieceMoves[to]);
        M_SETPROM(move1, bQ);
        M_SETPROM(move2, bN);
        M_SETPROM(move3, bR);
        M_SETPROM(move4, bB);
        moves.push_back(move1);
        moves.push_back(move2);
        moves.push_back(move3);
        moves.push_back(move4);
      } else {
        moves.push_back(NEW_MOVE(pieceSquares[from], pieceMoves[to]));
      }
    }
  }

  // Rook Moves
  getSetBits(friendlyBB & s._pieceBitboards[ROOKS], pieceSquares);

  for (int from = 0; pieceSquares[from] != -1; from++) {
    getSetBits(rookAttacks(pieceSquares[from], s) & ~friendlyBB, pieceMoves);
    for (int to = 0; pieceMoves[to] != -1; to++) {
      moves.push_back(NEW_MOVE(pieceSquares[from], pieceMoves[to]));
    }
  }

  // Bishop Moves
  getSetBits(friendlyBB & s._pieceBitboards[BISHOPS], pieceSquares);

  for (int from = 0; pieceSquares[from] != -1; from++) {
    getSetBits(bishopAttacks(pieceSquares[from], s) & ~friendlyBB, pieceMoves);
    for (int to = 0; pieceMoves[to] != -1; to++) {
      moves.push_back(NEW_MOVE(pieceSquares[from], pieceMoves[to]));
    }
  }

  // Queen Moves
  getSetBits(friendlyBB & s._pieceBitboards[QUEENS], pieceSquares);

  for (int from = 0; pieceSquares[from] != -1; from++) {
    getSetBits((rookAttacks(pieceSquares[from], s) |
                bishopAttacks(pieceSquares[from], s)) &
                   ~friendlyBB,
               pieceMoves);
    for (int to = 0; pieceMoves[to] != -1; to++) {
      moves.push_back(NEW_MOVE(pieceSquares[from], pieceMoves[to]));
    }
  }

  delete[] pieceSquares;
  delete[] pieceMoves;
  return moves;
}

U64 rookAttacks(int index, U64 consideredPieces) {
  U64 bbBlockers = consideredPieces & occupancyMaskRook[index];
  int databaseIndex = (int)((bbBlockers * magicNumberRook[index]) >>
                            magicNumberShiftsRook[index]);
  return magicMovesRook[index][databaseIndex];
}

U64 bishopAttacks(int index, U64 consideredPieces) {
  U64 bbBlockers = consideredPieces & occupancyMaskBishop[index];
  int databaseIndex = (int)((bbBlockers * magicNumberBishop[index]) >>
                            magicNumberShiftsBishop[index]);
  return magicMovesBishop[index][databaseIndex];
}

U64 pawnPush(U64 bb, int pawnSide, const State &s) {
  U64 moves = 0ULL;
  U64 rank4 = pawnSide == WHITE ? RANK_BB[3] : RANK_BB[4];
  U64 pawns = (pawnSide == WHITE ? UP(bb) : DOWN(bb)) & ~s.allPieces();
  moves |= pawns;
  pawns = (pawnSide == WHITE ? UP(pawns) : DOWN(pawns)) & ~s.allPieces();
  moves |= pawns &
           rank4; // only add double push moves if the destination is on rank 4
  return moves;
}

U64 pawnAttack(int index, int pawnSide, const State &s) {
  U64 EPTarget = 0ULL;
  U64 attack = 0ULL;
  U64 enemy = 0ULL;
  if (s._EPTarget != -1) {
    EPTarget = setMask[s._EPTarget];
  }
  if (pawnSide == WHITE) {
    enemy = s._pieceBitboards[BLACKS];
    attack = pawnAttacks[0][index];
    EPTarget = UP(EPTarget);
  } else {
    enemy = s._pieceBitboards[WHITES];
    attack = pawnAttacks[1][index];
    EPTarget = DOWN(EPTarget);
  }
  enemy |= EPTarget;

  return attack & enemy;
}

U64 attacksTo(int index, const State &s, int defendingSide,
              U64 consideredPieces) {
  if (consideredPieces == 0) {
    consideredPieces = s.allPieces();
  }
  U64 enemyBB = defendingSide == BLACK ? s._pieceBitboards[WHITES]
                                       : s._pieceBitboards[BLACKS];
  enemyBB &= consideredPieces;
  U64 knights = knightAttacks[index] & s._pieceBitboards[KNIGHTS];
  U64 kings = kingAttacks[index] & s._pieceBitboards[KINGS];
  U64 bishopsQueens = bishopAttacks(index, consideredPieces) &
                      (s._pieceBitboards[BISHOPS] | s._pieceBitboards[QUEENS]);
  U64 rooksQueens = rookAttacks(index, consideredPieces) &
                    (s._pieceBitboards[ROOKS] | s._pieceBitboards[QUEENS]);
  U64 pawns = pawnAttack(index, defendingSide, s) & s._pieceBitboards[PAWNS];

  return (knights | kings | bishopsQueens | rooksQueens | pawns) & enemyBB;
}

void generateOccupancyVariations(bool isRook) {
  int i, j, bitRef;
  long mask;
  int variationCount;
  int *setBitsInMask, *setBitsInIndex;
  int bitCount[64];

  for (bitRef = 0; bitRef <= 63; bitRef++) {
    mask = isRook ? occupancyMaskRook[bitRef] : occupancyMaskBishop[bitRef];
    setBitsInMask = getSetBits(mask);
    bitCount[bitRef] = countSetBits(mask);
    variationCount = (int)(1ULL << bitCount[bitRef]);
    for (i = 0; i < variationCount; i++) {
      occupancyVariation[bitRef][i] = 0;

      // find bits set in index "i" and map them to bits in the 64 bit
      // "occupancyVariation"

      setBitsInIndex =
          getSetBits(i); // an array of integers showing which bits are set
      for (j = 0; setBitsInIndex[j] != -1; j++) {
        occupancyVariation[bitRef][i] |=
            (1ULL << setBitsInMask[setBitsInIndex[j]]);
      }
      delete setBitsInIndex;
    }
    delete setBitsInMask;
  }
}

void generateMoveDatabase(bool isRook) {
  long validMoves;
  int variations, bitCount;
  int bitRef, i, j, magicIndex;

  for (bitRef = 0; bitRef <= 63; bitRef++) {
    bitCount = isRook ? countSetBits(occupancyMaskRook[bitRef])
                      : countSetBits(occupancyMaskBishop[bitRef]);
    variations = (int)(1ULL << bitCount);

    for (i = 0; i < variations; i++) {
      validMoves = 0;
      if (isRook) {
        magicIndex =
            (int)((occupancyVariation[bitRef][i] * magicNumberRook[bitRef]) >>
                  magicNumberShiftsRook[bitRef]);

        for (j = bitRef + 8; j <= 63; j += 8) {
          validMoves |= (1ULL << j);
          if ((occupancyVariation[bitRef][i] & (1ULL << j)) != 0)
            break;
        }
        for (j = bitRef - 8; j >= 0; j -= 8) {
          validMoves |= (1ULL << j);
          if ((occupancyVariation[bitRef][i] & (1ULL << j)) != 0)
            break;
        }
        for (j = bitRef + 1; j % 8 != 0; j++) {
          validMoves |= (1ULL << j);
          if ((occupancyVariation[bitRef][i] & (1ULL << j)) != 0)
            break;
        }
        for (j = bitRef - 1; j % 8 != 7 && j >= 0; j--) {
          validMoves |= (1ULL << j);
          if ((occupancyVariation[bitRef][i] & (1ULL << j)) != 0)
            break;
        }

        magicMovesRook[bitRef][magicIndex] = validMoves;
      } else {
        magicIndex =
            (int)((occupancyVariation[bitRef][i] * magicNumberBishop[bitRef]) >>
                  magicNumberShiftsBishop[bitRef]);

        for (j = bitRef + 9; j % 8 != 0 && j <= 63; j += 9) {
          validMoves |= (1ULL << j);
          if ((occupancyVariation[bitRef][i] & (1ULL << j)) != 0)
            break;
        }
        for (j = bitRef - 9; j % 8 != 7 && j >= 0; j -= 9) {
          validMoves |= (1ULL << j);
          if ((occupancyVariation[bitRef][i] & (1ULL << j)) != 0)
            break;
        }
        for (j = bitRef + 7; j % 8 != 7 && j <= 63; j += 7) {
          validMoves |= (1ULL << j);
          if ((occupancyVariation[bitRef][i] & (1ULL << j)) != 0)
            break;
        }
        for (j = bitRef - 7; j % 8 != 0 && j >= 0; j -= 7) {
          validMoves |= (1ULL << j);
          if ((occupancyVariation[bitRef][i] & (1ULL << j)) != 0)
            break;
        }

        magicMovesBishop[bitRef][magicIndex] = validMoves;
      }
    }
  }
}
