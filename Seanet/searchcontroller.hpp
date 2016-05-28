//
//  searchcontroller.hpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/17/16.
//
//

#ifndef __Seanet__searchcontroller__
#define __Seanet__searchcontroller__

#include "board.hpp"
#include "hash.hpp"
#include "util.hpp"
#include <stdio.h>
#include <sys/time.h>

class SearchController {
public:
  int _depthLimit = 60;

  int _uciOutput = false;
  bool _output = true;

  bool _features[NUM_OF_FEATURES] = {true, true, true, true, true, true, true};

  bool _stopSearch = false;
  timeval _startTime;
  int _totalNodes = 0;
  int _qNodes = 0;
  int _mainNodes = 0;
  int _fhNodes = 0;
  int _fhfNodes = 0;
  int _currDepth = 0;
  int _transpositions = 0;
  int _exactNodes = 0;
  int _alphaNodes = 0;
  int _betaNodes = 0;
  int _maxDepth = 0;
  Move _currMove = 0;
  int _currMoveNumber = 0;
  HashTable table;

  void checkTimeLimit();
  void resetStats();
  std::string featuresToString();
};

#endif /* defined(__Seanet__searchcontroller__) */
