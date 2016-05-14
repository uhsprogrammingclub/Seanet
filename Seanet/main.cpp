//
//  main.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "board.hpp"
#include "util.hpp"
#include <iostream>

int main(int argc, const char *argv[]) {
  // insert code here...
  initPresets();
  State state =
      boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  state.clearSquare(0);
  state.printBoard();
  initpopCountOfByte256();
  // U64 num = 0b1111100000110110110001;
  // printf("Set bits of %llu is %i\n", num, countSetBits(num));
  return 0;
}
