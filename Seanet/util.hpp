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

std::vector<std::string> &split(const std::string &s, char delim,
                                std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

char pieceToChar(Piece p);
Piece charToPiece(char p);
int bitboardForPiece(Piece p);
int uciToIndex(std::string uci);
std::string indexToUCI(int index);
std::string boardToFEN(const State &b);
State boardFromFEN(std::string FEN);
void initpopCountOfByte256();
void initPresets();
void getSetBits(U64 bb, int *setBits);
int *getSetBits(U64 bb);
std::string bbToString(U64 bb);
std::string moveToUCI(int move);
int moveFromUCI(std::string uci);
std::string pvLineToString(S_PVLINE line);

// inline functions

inline int LS1B(U64 bb) {
  return bb ? index64[((bb ^ (bb - 1)) * 0x03f79d71b4cb0a89ULL) >> 58] : -1;
}

inline int countSetBits(U64 bb) {
  return popCountOfByte256[bb & 0xff] + popCountOfByte256[(bb >> 8) & 0xff] +
         popCountOfByte256[(bb >> 16) & 0xff] +
         popCountOfByte256[(bb >> 24) & 0xff] +
         popCountOfByte256[(bb >> 32) & 0xff] +
         popCountOfByte256[(bb >> 40) & 0xff] +
         popCountOfByte256[(bb >> 48) & 0xff] + popCountOfByte256[bb >> 56];
}

inline int sideBitboardForPiece(Piece p) {
  return p % 2 == 0 ? BLACKS : WHITES;
}

#endif /* util_hpp */
