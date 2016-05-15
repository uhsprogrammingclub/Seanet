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
int sideBitboardForPiece(Piece p);
int uciToIndex(std::string uci);
std::string indexToUCI(int index);
std::string boardToFEN(const State &b);
State boardFromFEN(std::string FEN);
int LS1B(U64 bb);
void initpopCountOfByte256();
int countSetBits(U64 bb);
void initPresets();
std::vector<int> getSetBits(U64 bb);
std::string bbToString(U64 bb);

#endif /* util_hpp */
