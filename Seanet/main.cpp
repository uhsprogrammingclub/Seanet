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
  State *state =
      boardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  state->printBoard();
  return 0;
}
