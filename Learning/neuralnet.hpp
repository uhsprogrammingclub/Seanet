//
//  neuralnet.hpp
//  Seanet
//
//  Created by Stiven Deleur on 5/31/16.
//
//

#ifndef neuralnet_hpp
#define neuralnet_hpp

#define MAX_LAYERS 2
#define MAX_PERCEPTRONS 5
#define MAX_FEATURES 5

#include "board.hpp"
#include "defs.h"
#include "evaluator.hpp"
#include "perceptron.hpp"
#include <stdio.h>
#include <vector>

class NeuralNet {

public:
  // Allocate memory for _perceptrons
  std::vector<std::vector<Perceptron>> _perceptrons =
      std::vector<std::vector<Perceptron>>(
          MAX_LAYERS, std::vector<Perceptron>(MAX_PERCEPTRONS));

  int _numOfLayers = 2;
  float _alpha = 1;

  std::vector<std::vector<FList>> getWeights();
  void train(State &state, Move bestMove, float alpha);
  std::vector<FList> calculateFeatures(State &state, FList initialFeatures);
  void updatePerceptrons(std::vector<FList> correctFeatures,
                         std::vector<FList> wrongFeatures);
  NeuralNet(std::vector<int> &layers);
};

void trainTwoNets(State &state, Move bestMove, NeuralNet *midgameNet,
                  NeuralNet *endgameNet, float alpha);

#endif /* neuralnet_hpp */
