//
//  main.cpp
//  Learning
//
//  Created by Stiven Deleur on 5/31/16.
//
//

#include "neuralnet.hpp"
#include "perceptron.hpp"
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
  return 0;
}
