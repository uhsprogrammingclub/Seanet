//
//  neuralnet.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/31/16.
//
//

#include "movegenerator.hpp"
#include "neuralnet.hpp"
#include <iostream>

void trainTwoNets(State &state, Move bestMove, NeuralNet *midgameNet,
                  NeuralNet *endgameNet, float alpha) {
  float phaseFactor = getPhaseFactor(state);
  midgameNet->_alpha = alpha * phaseFactor;
  endgameNet->_alpha = alpha * (1 - phaseFactor);

  std::vector<int> pseudoMoves = generatePseudoMoves(state);
  std::vector<int> legalMoves;
  bool bestMoveIsLegal = false;

  for (auto it = pseudoMoves.begin(); it != pseudoMoves.end(); ++it) {
    if (state.isLegalMove(*it)) {
      legalMoves.push_back(*it);
      if (M_EQUALS(bestMove, *it)) {
        bestMoveIsLegal = true;
      }
    }
  }

  if (!bestMoveIsLegal) {
    std::cout << "ERROR: Best move is not a legal move!" << std::endl;
    state.printBoard();
    std::cout << boardToFEN(state) << std::endl;
    std::cout << "Move: " << moveToUCI(bestMove) << std::endl;
  }

  state.makeMove(bestMove);

  FList initialFeatures = getInitialFeatures(state);
  std::vector<FList> correctMidgameFeaturesList =
      midgameNet->calculateFeatures(state, initialFeatures);
  std::vector<FList> correctEndgameFeaturesList =
      endgameNet->calculateFeatures(state, initialFeatures);
  state.takeMove();

  Move bestCalculatedMove = NO_MOVE;
  float bestMoveEval = INT_MIN;
  std::vector<FList> calculatedMidgameFeaturesList;
  std::vector<FList> calculatedEndgameFeaturesList;

  // Iterate through state action pairs
  for (auto move = legalMoves.begin(); move != legalMoves.end(); ++move) {

    state.makeMove(*move);

    initialFeatures = getInitialFeatures(state);
    std::vector<FList> midgameFeaturesList =
        midgameNet->calculateFeatures(state, initialFeatures);
    std::vector<FList> endgameFeaturesList =
        endgameNet->calculateFeatures(state, initialFeatures);
    state.takeMove();

    // check if the move is evaluated higher than move already picked
    float stateEval = midgameFeaturesList.back()[0] * phaseFactor +
                      endgameFeaturesList.back()[0] * (1 - phaseFactor);
    if (stateEval > bestMoveEval) {
      bestCalculatedMove = *move;
      bestMoveEval = stateEval;
      calculatedMidgameFeaturesList = midgameFeaturesList;
      calculatedEndgameFeaturesList = endgameFeaturesList;
    }
  }

  if (!M_EQUALS(bestMove, bestCalculatedMove)) {

    midgameNet->updatePerceptrons(correctMidgameFeaturesList,
                                  calculatedMidgameFeaturesList);
    endgameNet->updatePerceptrons(correctEndgameFeaturesList,
                                  calculatedEndgameFeaturesList);
  }
}

std::vector<FList> NeuralNet::calculateFeatures(State &state,
                                                FList initialFeatures) {
  std::vector<FList> features;
  features[0] = initialFeatures;

  // Iterate through all layers
  for (int layer = 0; layer < _numOfLayers; layer++) {

    // Initialize features of the next layer (perceptron activation values)
    features[layer + 1] = FList();

    // Iterate through each perceptron in the layer (pn = perceptron number)
    for (int pn = 0; pn < _perceptrons[layer].size(); pn++) {

      // update the perceptron activation value of the perceptron
      features[layer + 1][pn] =
          _perceptrons[layer][pn].calculateActivation(features[layer]);
    }
  }

  return features;
}

void NeuralNet::updatePerceptrons(std::vector<FList> correctFeatures,
                                  std::vector<FList> wrongFeatures) {

  // Iterate through all layers
  for (int layer = 0; layer < _numOfLayers; layer++) {

    // Iterate through each perceptron in the layer (pn = perceptron number)
    for (int pn = 0; pn < _perceptrons[layer].size(); pn++) {

      // update the perceptron weight of each feature
      _perceptrons[layer][pn].updateWeights(correctFeatures[pn],
                                            wrongFeatures[pn], _alpha);
    }
  }
}
