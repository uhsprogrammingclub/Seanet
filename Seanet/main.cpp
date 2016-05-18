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
  //FEN = "7k/8/8/p7/1P6/8/8/7K w - - 0 1";

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
  for (std::vector<int>::iterator it = pseudoMoves.begin();
       it != pseudoMoves.end(); ++it) {
    std::cout << moveToUCI(*it) << ", ";
    if (gameState.isLegalMove(*it)) {
      legalMoves.push_back(*it);
    }
  }
  std::cout << '\n';

  printf("Legal moves (%lu):", legalMoves.size());
  for (std::vector<int>::iterator it = legalMoves.begin();
       it != legalMoves.end(); ++it) {
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