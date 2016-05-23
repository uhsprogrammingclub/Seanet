//
//  main.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "board.hpp"
#include "evaluator.hpp"
#include "movegenerator.hpp"
#include "search.hpp"
#include "searchcontroller.hpp"
#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

void takePlayerMove();
State gameState;

int main(int argc, const char *argv[]) {
  // insert code here...

  std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  FEN = "3k4/1Q6/8/4K3/8/8/8/8 b - - 14 11";
  FEN = "4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1";

  std::string EPD =
      "1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - - bm Qd1+; id BK.01;";

  KeyInfoMap test = splitEDP(EPD);

  for (auto elem : test) {
    std::cout << elem.first << "\n";
    std::cout << elem.second << "\n";
  }
  std::cout << "FEN: " << test["fen"] << "\n";
  std::cout << "BM: " << test["bm"] << "\n";
  std::cout << "ID:" << test["id"] << "\n";

  initPresets();
  gameState = boardFromFEN(FEN);
  gameState.printBoard();

  SearchController sControl;
  search(gameState, sControl);
  printf("Best move: %s (%i)",
         moveToUCI(gameState.bestLine.moves[0].move).c_str(),
         gameState.bestLine.moves[0].eval);

  // takePlayerMove();

  return 0;
}

void takePlayerMove() {
  gameState.printBoard();
  std::cout << "FEN: " << boardToFEN(gameState) << std::endl;

  std::vector<int> pseudoMoves = generatePseudoMoves(gameState);
  std::vector<int> legalMoves;

  printf("Pseudo moves (%lu):", pseudoMoves.size());
  for (auto it = pseudoMoves.begin(); it != pseudoMoves.end(); ++it) {
    std::cout << moveToUCI(*it) << ", ";
    if (gameState.isLegalMove(*it)) {
      legalMoves.push_back(*it);
    }
  }
  std::cout << '\n';

  printf("Legal moves (%lu):", legalMoves.size());
  for (auto it = legalMoves.begin(); it != legalMoves.end(); ++it) {
    std::cout << moveToUCI(*it) << ", ";
  }
  std::cout << '\n';

  std::string userMove;
  std::cout << "Enter the next move!\n";
  std::getline(std::cin, userMove);
  if (userMove == "undo") {
    gameState.takeMove();
  } else {
    int move = moveFromUCI(userMove);
    if (std::find(legalMoves.begin(), legalMoves.end(), move) !=
        legalMoves.end()) {
      gameState.makeMove(move);
    } else {
      std::cout << "Illegal Move!\n";
    }
  }
  takePlayerMove();
}