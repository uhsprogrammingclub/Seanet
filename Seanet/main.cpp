//
//  main.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#include "board.hpp"
#include "evaluator.hpp"
#include "hash.hpp"
#include "movegenerator.hpp"
#include "search.hpp"
#include "searchcontroller.hpp"
#include "util.hpp"
#include <algorithm>
#include <ctime>
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
  FEN = "8/8/4K3/4P3/8/4k3/8/8 w - - 0 1";
  // mirrored positions
  //  FEN = "K7/8/8/3Q4/4q3/8/8/7k b - - 0 1";
  //   FEN = "1rb1k2q/1p2r1bp/3n1p2/n1p4P/p4P1N/2P1N3/PB1R2P1/Q2K1BR1 b - - 0
  //   1";
  //  FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR w - - 0 1";
  FEN = "2r1k2r/2pn1pp1/1p3n1p/p3PP2/4q2B/P1P5/2Q1N1PP/R4RK1 w q -";
  // FEN = "4k2r/3n1pp1/1pr2n1p/p1p1PP2/4QR1B/P1P5/4N1PP/R5K1 w q - 2 2";

  initPresets();

  gameState = boardFromFEN(FEN);
  // gameState.printBoard();
  /*std::cout << "Static board evaluation: " << evaluate(gameState) <<
  std::endl;

  SearchController sControl;
  sControl._depthLimit = 9;
  sControl._timeLimit = 90;
  search(gameState, sControl);*/

  startUCI();

  // clock_t begin = clock();
  /*
    std::cout << "\nEvaluation:\n["
              << gameState._lineEval * (gameState._sideToMove == WHITE ? 1 : -1)
              << "] " << pvLineToString(gameState._bestLine) << "\n" <<
    std::endl;
    std::cout << "TRANSPOSITIONS: " << sControl._transpositions << std::endl;*/
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
}

void startUCI() {
  std::cout << "id name Seanet 0.1b\n";
  std::cout << "id author Stiven Deleur, Nathaniel Corley\n";

  std::cout << "uciok\n" << std::endl;
  takeUCIInput();
}

void takeUCIInput() {
  std::string input;

  SearchController uciStateControl;
  State uciGameState;
  std::thread searchThread;

  bool searching = false;

  uciStateControl._uciOutput = true;
  while (std::getline(std::cin, input)) {

    if (uciStateControl._stopSearch == true) {
      if (searchThread.joinable()) {
        searchThread.join();
      }
    }

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
          uciStateControl._timeLimit = INT_MAX;
          uciStateControl._depthLimit = INT_MAX;
        } else if (inputParts.at(i) == "wtime") {
          uciStateControl._wTime = std::stoi(inputParts.at(i + 1));

        } else if (inputParts.at(i) == "btime") {
          uciStateControl._bTime = std::stoi(inputParts.at(i + 1));

        } else if (inputParts.at(i) == "winc") {
          uciStateControl._wInc = std::stoi(inputParts.at(i + 1));

        } else if (inputParts.at(i) == "binc") {
          uciStateControl._bInc = std::stoi(inputParts.at(i + 1));

        } else if (inputParts.at(i) == "movestogo") {
          uciStateControl._moveToGo = std::stoi(inputParts.at(i + 1));

        } else if (inputParts.at(i) == "depth") {
          uciStateControl._maxDepth = std::stoi(inputParts.at(i + 1));

        } else if (inputParts.at(i) == "nodes") {
          uciStateControl._nodeLimit = std::stoi(inputParts.at(i + 1));

        } else if (inputParts.at(i) == "mate") {
        } else if (inputParts.at(i) == "movetime") {

          uciStateControl._timeLimit = std::stoi(inputParts.at(i + 1)) / 1000;

        } else if (inputParts.at(i) == "infinite") {
          uciStateControl._timeLimit = INT_MAX;
          uciStateControl._depthLimit = INT_MAX;
        }
      }
      searching = true;
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
    } else if (commandName == "uci") {
      // Using UCI protocol...
    } else {
      std::cout << "Unrecognized command: " << input << "\n";
    }
  }
}