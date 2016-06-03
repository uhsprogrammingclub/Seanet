//
//  perceptron.hpp
//  Seanet
//
//  Created by Stiven Deleur on 5/31/16.
//
//

#ifndef perceptron_hpp
#define perceptron_hpp

#include "defs.h"
#include <stdio.h>
#include <vector>

class Perceptron {
  FList _weights;

public:
  FList getWeights();
  float calculateActivation(FList features);
  void updateWeights(FList correctFeatures, FList wrongFeatures, float alpha);
};

float activationFunction(float sum);

#endif /* perceptron_hpp */
