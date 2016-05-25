#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include "board.hpp"
#include "catch.hpp"
#include "movegenerator.hpp"
#include "search.hpp"
#include "searchcontroller.hpp"
#include "util.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

int leafNodes = 0;
void perftTest(State &b, int depth);
void divide(std::string FEN, int depth);
void engineTest(std::string testPath, std::string tag, std::string comments,
                int secondsPerPosition = 25);

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

TEST_CASE("Running Bratko-Kopec Tests", "[Bratko-Kopec]") {
  engineTest("Bratko-Kopec.text", "[Bratko-Kopec]",
             "Stiven - SEE/killer move reordering"); // Add descriptive comments
                                                     // with each
                                                     // full run
}
TEST_CASE("Running LCT-II Tests", "[LCT-II]") {
  engineTest("LCT-II.text", "[LCT-II]",
             "Stiven - SEE/killer move reordering"); // Add descriptive comments
                                                     // with each full run
}

TEST_CASE("Running PERFT tests", "[perft]") {
  State state;
  initPresets();
  int TEST_LIMIT = 200;
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

void engineTest(std::string testPath, std::string tag, std::string comments,
                int secondsPerPosition) {
  State state;
  initPresets();
  std::ifstream file(testPath);
  std::string line;
  int numCorrect = 0;
  while (std::getline(file, line)) {
    KeyInfoMap info = splitEDP(line);
    state = boardFromFEN(info["fen"]);
    SearchController sControl;
    sControl._timeLimit = secondsPerPosition;
    sControl._depthLimit = INT_MAX;
    search(state, sControl);
    std::string result = moveToSAN(state._bestLine.moves[0], state).c_str();
    printf("FEN: %s, Engine BM (eval): %s (%i), Given BM: %s\n",
           info["fen"].c_str(), result.c_str(), state._lineEval,
           info["bm"].c_str());
    CHECK(result == info["bm"].c_str());
    if (result == info["bm"]) {
      numCorrect++;
    }
  }
  char buff[DTTMSZ];
  std::fstream records;
  records.open("records.text", std::fstream::out | std::fstream::app);
  records << "test = " << tag << "; date = " << getDtTm(buff)
          << "; correct = " << numCorrect
          << "; seconds/position = " << secondsPerPosition
          << "; comments = " << comments << ";\n";
  records.close();
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
