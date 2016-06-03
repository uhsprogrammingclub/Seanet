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
const int NUMBER_NETS = 2;
NeuralNet *midgameNeuralNet;
NeuralNet *endgameNeuralNet;
void loadExistingWeights(
    std::array<std::vector<std::vector<int>>, NUMBER_NETS> *weights);

int main(int argc, const char *argv[]) {
  midgameNeuralNet = new NeuralNet();
  endgameNeuralNet = new NeuralNet();

  // Array of current weights (updated after each training example)
  std::array<std::vector<std::vector<int>>, NUMBER_NETS> _currentWeights;

  NeuralNet nets[NUMBER_NETS] = {midgameNeuralNet, endgameNeuralNet};

  // Load existing weights for each net
  /*** To be implemented ***/

  // Counter for number of examples tested in current training session
  int numExamples = 0;

  for (state, bestMove) {
    // Increment counter
    numExamples++;

    // Perform training
    trainTwoNets(state, bestMove, midgameNeuralNet, endgameNeuralNet, alpha);

    // Extract updated weights from neural nets
    for (int i = 0; i < sizeof(nets) / sizeof(*nets); i++) {
      for (int layer = 0; layer < nets[i]->_numOfLayers; layer++) {
        for (int pn = 0; pn < nets[i]->getSizeOfLayer(layer); pn++) {
          _currentWeights[i][layer][pn] =
              nets[i]->getPerceptronWeights(layer, pn);
        }
      }
    }
    // Record the current weights every n training positions
    if (numExamples % 2000 == 0) {
      char buff[DTTMSZ];
      std::fstream records;
      records.open("weight-records.text",
                   std::fstream::out | std::fstream::app);
      records << "[ Weights as of " << getDtTm(buff) << " ] \n";
      for (int i = 0; i < sizeof(nets) / sizeof(*nets); i++) {
        records << "< Neural Network # " << i << " >\n";
        for (int layer = 0; layer < nets[i]->_numOfLayers; layer++) {
          for (int pn = 0; pn < nets[i]->getSizeOfLayer(layer); pn++) {
            records << _currentWeights[i][layer][pn] << " ";
          }
          records << "| ";
        }
      }
      records.close();
    }
  }

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
