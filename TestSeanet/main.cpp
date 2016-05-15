#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include "board.hpp"
#include "catch.hpp"
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
