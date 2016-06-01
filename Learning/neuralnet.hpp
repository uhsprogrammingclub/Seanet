//
//  neuralnet.hpp
//  Seanet
//
//  Created by Stiven Deleur on 5/31/16.
//
//

#ifndef neuralnet_hpp
#define neuralnet_hpp

#include "board.hpp"
#include "defs.h"
#include "evaluator.hpp"
#include "perceptron.hpp"
#include <stdio.h>
#include <vector>

class NeuralNet {
  std::vector<std::vector<Perceptron>> _perceptrons;

public:
  int _numOfLayers = 2;
  float _alpha = 1;

  void train(State &state, Move bestMove, float alpha);
  std::vector<FList> calculateFeatures(State &state, FList initialFeatures);
  void updatePerceptrons(std::vector<FList> correctFeatures,
                         std::vector<FList> wrongFeatures);
};

void trainTwoNets(State &state, Move bestMove, NeuralNet *midgameNet,
                  NeuralNet *endgameNet, float alpha);
#endif /* neuralnet_hpp */
