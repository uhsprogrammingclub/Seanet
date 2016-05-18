//
//  search.h
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#ifndef __Seanet__search__
#define __Seanet__search__

#include "evaluator.hpp"
#include "movegenerator.hpp"
#include "searchcontroller.hpp"
#include "util.hpp"
#include <stdio.h>

int alphaBeta(int alpha, int beta, int depth, State state,
              SearchController sControl);
int search(State state, double timeLimit, int maxDepth = INT_MAX);

#endif /* defined(__Seanet__search__) */
