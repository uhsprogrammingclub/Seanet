//
//  main.cpp
//  Learning
//
//  Created by Stiven Deleur on 5/31/16.
//
//

#include "defs.h"
#include "neuralnet.hpp"
#include "perceptron.hpp"
#include <ctime>
#include <fstream>
#include <iostream>

float alpha = 0.5;
NeuralNet *midgameNeuralNet;
NeuralNet *endgameNeuralNet;
void loadExistingWeights(NeuralNet *net, std::string name);
void recordWeights(NeuralNet *net, std::string name);
void loadRandomWeights(NeuralNet *net);

int main(int argc, const char *argv[]) {

  // Randomly seed rand() function
  timeval currTime;
  gettimeofday(&currTime, 0);
  int now = (int)(timeToMS(currTime));
  srand(now);

  std::vector<int> layers = {5, 5, 1}; // layers[0] is initial features
  midgameNeuralNet = new NeuralNet(layers);
  endgameNeuralNet = new NeuralNet(layers);

  loadRandomWeights(midgameNeuralNet);
  std::cout << "Loaded wieghts." << std::endl;
  recordWeights(midgameNeuralNet, "Midgame");

  // Counter for number of examples tested in current training session
  int numExamples = 0;

  /*for (state, bestMove) {
    // Increment counter
    numExamples++;

    // Perform training
    trainTwoNets(state, bestMove, midgameNeuralNet, endgameNeuralNet, alpha);

    // Record the current weights every n training positions
    if (numExamples % 2000 == 0) {
      recordWeights(&midgameNeuralNet, "Midgame");
      recordWeights(&endgameNeuralNet, "Endgame");
    }
  }*/

  delete midgameNeuralNet;
  delete endgameNeuralNet;
  return 0;
}

void loadRandomWeights(NeuralNet *net) {
  std::vector<std::vector<FList>> weights = net->getWeights();
  for (int layer = 0; layer < weights.size(); layer++) {
    for (int pn = 0; pn < weights[layer].size(); pn++) {
      for (int f = 0; f < weights[layer][pn].size(); f++) {
        net->_perceptrons[layer][pn]._weights[f] =
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      }
    }
  }
}

void loadExistingWeights(NeuralNet *net, std::string name) {}

void recordWeights(NeuralNet *net, std::string name) {
  char buff[DTTMSZ];
  std::fstream records;
  records.open("weight-records.text", std::fstream::out | std::fstream::app);
  records << "<name>" << name << "</name>\n"
          << "<info>Weights as of " << getDtTm(buff) << "</info>\n<weights>\n";
  std::vector<std::vector<FList>> weights = net->getWeights();
  for (int layer = 0; layer < weights.size(); layer++) {
    records << "<layer>\n";
    for (int pn = 0; pn < weights[layer].size(); pn++) {
      records << "<p>";
      for (int f = 0; f < weights[layer][pn].size(); f++) {
        records << "<f>";
        records << weights[layer][pn][f] << " ";
        records << "</f>";
      }
      records << "</p>";
    }
    records << "\n</layer>\n";
  }
  records << "</weights>";
  records.close();
}
