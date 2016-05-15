//
//  main.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "board.hpp"
#include "movegenerator.hpp"
#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

void takePlayerMove();
State gameState;

int main(int argc, const char *argv[]) {
  // insert code here...

  std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  FEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

  initPresets();
  gameState = boardFromFEN(FEN);
  takePlayerMove();

  return 0;
}

void takePlayerMove() {
  gameState.printBoard();
  std::cout << "FEN: " << boardToFEN(gameState) << std::endl;

  std::vector<Move> pseudoMoves = generatePseudoMoves(gameState);
  std::vector<Move> legalMoves;

  printf("Pseudo moves (%lu):", pseudoMoves.size());
  for (std::vector<Move>::iterator it = pseudoMoves.begin();
       it != pseudoMoves.end(); ++it) {
    std::cout << it->uci() << ", ";
    if (gameState.isLegalMove(&(*it))) {
      legalMoves.push_back(*it);
    }
  }
  std::cout << '\n';

  printf("Legal moves (%lu):", legalMoves.size());
  for (std::vector<Move>::iterator it = legalMoves.begin();
       it != legalMoves.end(); ++it) {
    std::cout << it->uci() << ", ";
  }
  std::cout << '\n';

  std::string userMove;
  std::cout << "Enter the next move!\n";
  std::cin >> userMove;
  if (userMove == "undo") {
    gameState.takeMove();
  } else {
    Move *move = new Move(userMove);
    if (std::find(legalMoves.begin(), legalMoves.end(), *move) !=
        legalMoves.end()) {
      gameState.makeMove(move);
    } else {
      std::cout << "Illegal Move!\n";
      delete move;
    }
  }
  takePlayerMove();
}