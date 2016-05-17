#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include "board.hpp"
#include "catch.hpp"
#include "movegenerator.hpp"
#include "util.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

int leafNodes = 0;
void perftTest(State &b, int depth);
void divide(std::string FEN, int depth);

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
  int TEST_LIMIT = 20;
  int maxDepth = 6;
  int perftStart = 1;
  std::string divideFEN = "";
  const clock_t startTime = clock();
  // divideFEN = "1K6/8/8/3Q4/4q3/8/7k/8 w - - 2 1";
  // 1";

  if (divideFEN != "") {
    divide(divideFEN, 2);
    return;
  }

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
    int depths[6] = {0};
    for (int i = 1; i < subLine.size(); i++) {
      depths[i - 1] = std::stoi(subLine[i].substr(3));
    }
    state = boardFromFEN(FEN);
    printf("\n### Running Test #%d ###\n", testNum);
    for (int i = 0; i < 6 && depths[i] != 0; i++) {
      const clock_t iStartTime = clock();
      if (i >= maxDepth) {
        break;
      }
      state.printBoard();
      printf("Starting test to depth %d on FEN %s\n", i + 1, FEN.c_str());
      leafNodes = 0;
      std::vector<int> moves = generatePseudoMoves(state);
      printf("%lu root moves:\n", moves.size());
      int moveNum = 0;
      for (std::vector<int>::iterator moveIt = moves.begin();
           moveIt != moves.end(); ++moveIt) {
        int oldNodes = leafNodes;
        state.makeMove(*moveIt);
        if (state.isPositionLegal()) {
          moveNum++;
          perftTest(state, i);
          printf("Move %d: %s %i\n", (moveNum), moveToUCI(*moveIt).c_str(),
                 leafNodes - oldNodes);
        }
        state.takeMove();
      }
      printf("Leaf nodes: %d, expected: %d; Finished in %f seconds\n",
             leafNodes, depths[i],
             float(clock() - iStartTime) / CLOCKS_PER_SEC);
      REQUIRE((int)depths[i] == leafNodes);
    }
  }
  printf("PERFT test finished successfully in %f minutes\n",
         (float(clock() - startTime) / (CLOCKS_PER_SEC * 60.0)));
}

void perftTest(State &state, int depth) {

  if (depth == 0) {
    leafNodes++;
    return;
  }

  std::vector<int> moves = generatePseudoMoves(state);

  for (std::vector<int>::iterator moveIt = moves.begin(); moveIt != moves.end();
       ++moveIt) {
    state.makeMove(*moveIt);
    if (state.isPositionLegal()) {
      perftTest(state, depth - 1);
    }
    state.takeMove();
  }
}

void divide(std::string FEN, int depth) {

  State state = boardFromFEN(FEN);
  state.printBoard();
  printf("Starting divide to depth %d on FEN %s\n", depth, FEN.c_str());
  leafNodes = 0;
  std::vector<int> moves = generatePseudoMoves(state);
  printf("%lu root moves:\n", moves.size());
  int moveNum = 0;
  for (std::vector<int>::iterator moveIt = moves.begin(); moveIt != moves.end();
       ++moveIt) {
    int oldNodes = leafNodes;
    state.makeMove(*moveIt);
    if (state.isPositionLegal()) {
      moveNum++;
      perftTest(state, depth - 1);
      printf("Move %d: %s %i\n", (moveNum), moveToUCI(*moveIt).c_str(),
             leafNodes - oldNodes);
    }

    state.takeMove();
  }
  printf("Leaf nodes: %d\n", leafNodes);
}
