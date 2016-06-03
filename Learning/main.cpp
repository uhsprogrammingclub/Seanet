//
//  main.cpp
//  Learning
//
//  Created by Stiven Deleur on 5/31/16.
//
//

#include "datum.hpp"
#include "neuralnet.hpp"
#include "perceptron.hpp"
#include <fstream>
#include <iostream>

float alpha = 0.5;
NeuralNet *midgameNeuralNet;
NeuralNet *endgameNeuralNet;
void loadExistingWeights(NeuralNet *neuralnet);

int main(int argc, const char *argv[]) {
  midgameNeuralNet = new NeuralNet();
  endgameNeuralNet = new NeuralNet();

  //  for (state, bestMove) {
  //	  trainTwoNets(state, bestMove, midgameNeuralNet, endgameNeuralNet,
  // alpha);
  //  }

  // save nets
  delete midgameNeuralNet;
  delete endgameNeuralNet;

  State state;
  initPresets();
  std::ifstream file("LCT-II.text");
  std::string line;
  while (std::getline(file, line)) {
    KeyInfoMap info = splitEDP(line);
    state = boardFromFEN(info["fen"]);
    if (isPseudoQuiet(state)) {
      // std::cout << "Pseduo Quiet:" << std::endl;
    } else {
      std::cout << "NOT Pseduo Quiet:" << std::endl;
      state.printBoard();
      std::cout << info["fen"] << std::endl;
    }
  }

  std::vector<std::string> games =
      exportGamesFromPGN(std::ifstream("Carlsen.pgn"));
  for (std::string game : games) {
    std::cout << "Game: " << game << std::endl;
    std::cout << "PV line: " << pvLineToString(getGameMoveLine(game))
              << std::endl;
    std::cout << "Game winner: " << getPGNGameWinner(game) << std::endl;
  }
  return 0;
}
