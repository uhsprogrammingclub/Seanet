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
  // FEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0
  // 1";
  // FEN = "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1";

  initPresets();

  gameState = boardFromFEN(FEN);
  gameState.printBoard();

  SearchController sControl;
  sControl._depthLimit = 9;
  sControl._timeLimit = 90;
  search(gameState, sControl);

  std::cout << "\nEvaluation:\n["
            << gameState.bestLine.moves[0].eval *
                   (gameState._sideToMove == WHITE ? 1 : -1)
            << "] " << pvLineToString(gameState.bestLine) << "\n" << std::endl;

  // takePlayerMove();

  return 0;
}

void takePlayerMove() {
  gameState.printBoard();
  std::cout << "FEN: " << boardToFEN(gameState) << std::endl;

  std::vector<int> pseudoMoves = generatePseudoMoves(gameState);
  std::vector<int> legalMoves;

  for (auto it = pseudoMoves.begin(); it != pseudoMoves.end(); ++it) {
    if (gameState.isLegalMove(*it)) {
      legalMoves.push_back(*it);
    }
  }

  printf("Legal moves (%lu):", legalMoves.size());
  for (auto it = legalMoves.begin(); it != legalMoves.end(); ++it) {
    std::cout << moveToUCI(*it) << "(" << see(*it, gameState) << "), ";
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

void takeUCIInput() {
  std::string input;

  SearchController uciStateControl;
  State uciGameState;
  std::thread searchThread;

  uciStateControl._uciOutput = true;
  while (std::getline(std::cin, input)) {
    std::vector<std::string> inputParts;
    inputParts = split(input, ' ');
    std::string commandName = inputParts.at(0);
    if (commandName == "isready") {
      uciGameState = boardFromFEN(
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      std::cout << "readyok\n";
    } else if (commandName == "ucinewgame") {
      uciGameState = boardFromFEN(
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    } else if (commandName == "position") {
      std::string FEN = inputParts.at(1);
      if (FEN == "startpos") {
        uciGameState = boardFromFEN(
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
      } else {
        FEN = input.substr(13);
        uciGameState = boardFromFEN(FEN);
      }
      int i;

      for (i = 2; i < inputParts.size(); i++) {
        if (inputParts.at(i) == "moves") {
          i++;
          break;
        }
      }
      for (; i < inputParts.size(); i++) {
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
          uciStateControl._timeLimit = INT_MAX;
          uciStateControl._depthLimit = INT_MAX;
        }
      }
      searchThread = std::thread(search, std::ref(uciGameState),
                                 std::ref(uciStateControl));
    } else if (commandName == "stop") {
      uciStateControl._stopSearch = true;
      if (searchThread.joinable()) {
        searchThread.join();
      }
      // stop search
    } else if (commandName == "ponderhit") {
    } else if (commandName == "quit") {
      uciStateControl._stopSearch = true;
      if (searchThread.joinable()) {
        searchThread.join();
      }
      break;
    } else {
      std::cout << "Unrecognized command: " << input << "\n";
    }
  }
}