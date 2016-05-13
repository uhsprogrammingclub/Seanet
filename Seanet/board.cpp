//
//  board.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "board.hpp"
#include <iostream>
#include <ostream>
#include <string>

void State::printBoard() {
  for (int y = 7; y >= 0; y--) {
    std::string string = "";
    for (int x = 0; x < 8; x++) {
      string += "|";
      string += pieceToChar(pieces[y * 8 + x]);
    }
    string += "|\n";
    std::cout << y + 1 << string;
  }
  std::cout << "  A B C D E F G H \n";
}