//
//  evaluator.hpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/16/16.
//
//

#ifndef __Seanet__evaluator__
#define __Seanet__evaluator__

#include "board.hpp"
#include "util.hpp"
#include <stdio.h>

int evaluate(State &state);
int evaluateGameOver(State &state);

#endif /* defined(__Seanet__evaluator__) */
