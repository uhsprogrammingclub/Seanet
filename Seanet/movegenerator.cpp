//
//  movegenerator.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "movegenerator.hpp"
#include "util.hpp"

std::vector<Move> generatePseudoMoves(const State &s) {
  std::vector<Move> moves;
  U64 friendlyBB = s._sideToMove == WHITE ? s._pieceBitboards[WHITES]
                                          : s._pieceBitboards[BLACKS];

  // King Moves
  int kingIndex = LS1B(friendlyBB & s._pieceBitboards[KINGS]);
  std::vector<int> kingMoves = getSetBits(kingAttacks[kingIndex] & ~friendlyBB);
  for (std::vector<int>::iterator it = kingMoves.begin(); it != kingMoves.end();
       ++it) {
    moves.push_back(Move(kingIndex, *it));
  }

  return moves;
}