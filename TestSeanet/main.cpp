#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include "board.hpp"
#include "catch.hpp"
#include "movegenerator.hpp"
#include "util.hpp"
#include <fstream>
#include <iostream>
#include <string>

TEST_CASE("Checking boardToFEN() and boardFromFEN()", "[fenFunctions]") {
  State state;

  std::ifstream file("perfttests.text");
  std::string line;
  while (std::getline(file, line)) {
    std::string correctFEN;
    std::istringstream is(line);
    std::getline(is, correctFEN, ';');
    correctFEN.erase(correctFEN.find_last_not_of(" \n\r\t") + 1);
    state = boardFromFEN(correctFEN);
    std::string testFEN = boardToFEN(state);
    REQUIRE(testFEN == correctFEN);
  }
}

TEST_CASE("Running PERFT tests", "[perft]") {
  State state;
  initPresets();
  int leafNodes = 0;
  int TEST_LIMIT = 20;
  int maxDepth = 6;
  int perftStart = 1;

  std::ifstream file("perfttests.text");
  std::string line;
  std::vector<std::string> data;
  while (std::getline(file, line)) {
    data.push_back(line);
  }
  data.erase(data.begin(), data.begin() + (perftStart - 1));
  int testNum = perftStart - 1;
  for (std::vector<std::string>::iterator it = data.begin(); it != data.end();
       ++it) {
    testNum++;
    if (testNum > TEST_LIMIT) {
      break;
    }
    std::vector<std::string> subLine = split(*it, ';');
    std::string FEN = subLine[0];
    FEN.erase(FEN.find_last_not_of(" \n\r\t") + 1);
    int depths[6];
    for (int i = 1; i < subLine.size(); i++) {
      depths[i] = std::stoi(subLine[i].substr(3));
      // printf("Test Number: %d, Depth %d: %d ", testNum, i,
      // depths[testNum][i]);
    }
    state = boardFromFEN(FEN);
    printf("\n### Running Test #%d ###\n", testNum);
    for (int i = 0; i < 6; i++) {
      if (i > maxDepth) {
        break;
      }
      state.printBoard();
      printf("Starting test to depth %d on FEN %s", maxDepth, FEN.c_str());
      leafNodes = 0;
      std::vector<Move> moves = generatePseudoMoves(state);
      int moveNum = 0;
      for (std::vector<Move>::iterator moveIt = moves.begin();
           moveIt != moves.end(); ++moveIt) {
        int oldNodes = leafNodes;
        state.makeMove(&(*moveIt));
        if (state.isPositionLegal()) {
          moveNum++;
        }
      }
    }
  }
}
