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
#include <thread>
#include <vector>

void startUCI();
void takeUCIInput();

void takePlayerMove();
State gameState;

int main(int argc, const char *argv[]) {
  // insert code here...

  std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  FEN = "4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1";

  initPresets();

  gameState = boardFromFEN(FEN);
  gameState.printBoard();

  // SearchController sControl;;
  // search(gameState, sControl);

  //  printf("Best move: %s (%i)",
  //         moveToUCI(gameState.bestLine.moves[0].move).c_str(),
  //         gameState.bestLine.moves[0].eval);

  takePlayerMove();

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
  } else if (userMove == "uci") {
    startUCI();
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

void startUCI() {
  std::cout << "id name Seanet 0.1b\n";
  std::cout << "id author Stiven Deleur, Nathaniel Corley\n";

  std::cout << "uciok\n";
  takeUCIInput();
}

void uciProtocol() {
  std::string Line; // to read the command given by the GUI
  SearchController uciStateControl;
  State uciGameState;

  while (getline(std::cin, Line)) {
    if (Line == "uci") {
      std::cout << "id name Seanet 0.1b\n";
      std::cout << "id author Stiven Deleur, Nathaniel Corley\n";
      std::cout << "uciok\n";
    } else if (Line == "quit") {
      std::cout << "Bye Bye" << std::endl;
      break;
    } else if (Line == "isready") {
      std::cout << "readyok" << std::endl;
    } else if (Line == "ucinewgame") {
      ; // nothing to do
    }

    if (Line.substr(0, 23) == "position startpos moves ") {
      ; // nothing to do
    } else if (Line == "stop") {
      ; // nothing to do
    } else if (Line.substr(0, 3) == "go ") {
    }
  }

  return;
}

void takeUCIInput() {
  std::string input;

  SearchController uciStateControl;
  State uciGameState;

  while (std::getline(std::cin, input)) {
    std::vector<std::string> inputParts;
    inputParts = split(input, ' ');
    std::string commandName = inputParts.at(0);
    std::thread searchThread;
    if (commandName == "isready") {
      std::cout << "readyok\n";
    } else if (commandName == "ucinewgame") {
    } else if (commandName == "position") {
      std::string FEN = inputParts.at(1);
      if (FEN == "startpos") {
        uciGameState = boardFromFEN(
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      } else {
        uciGameState = boardFromFEN(FEN);
      }
      for (int i = 3; i < inputParts.size(); i++) {
        Move move = moveFromUCI(inputParts.at(i));
        uciGameState.makeMove(move);
      }
    } else if (commandName == "go") {
      for (int i = 1; i < inputParts.size(); i++) {
        if (inputParts.at(i) == "searchmoves") {
        } else if (inputParts.at(i) == "ponder") {
        } else if (inputParts.at(i) == "wtime") {
        } else if (inputParts.at(i) == "btime") {
        } else if (inputParts.at(i) == "winc") {
        } else if (inputParts.at(i) == "binc") {
        } else if (inputParts.at(i) == "movestogo") {
        } else if (inputParts.at(i) == "depth") {
        } else if (inputParts.at(i) == "nodes") {
        } else if (inputParts.at(i) == "mate") {
        } else if (inputParts.at(i) == "movetime") {
        } else if (inputParts.at(i) == "infinite") {
        }
        search(uciGameState, uciStateControl);
      }
    } else if (commandName == "stop") {
      // stop search
    } else if (commandName == "ponderhit") {
    } else if (commandName == "quit") {
      exit(0);
    } else {
      std::cout << "Unrecognized command: " << input << "\n";
    }
  }
}