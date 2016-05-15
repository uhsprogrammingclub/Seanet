//
//  movegenerator.hpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//

#ifndef movegenerator_hpp
#define movegenerator_hpp

#include "board.hpp"
#include "defs.h"
#include <stdio.h>
#include <vector>

std::vector<Move> generatePseudoMoves(const State &s);

#endif /* movegenerator_hpp */
