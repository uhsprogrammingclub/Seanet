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
#include "util.hpp"
#include <stdio.h>
#include <sys/time.h>

const int NUM_OF_FEATURES = 4;
enum { PV_REORDERING, SEE_REORDERING, KH_REORDERING, HH_REORDERING };

class SearchController {
public:
  int _depthLimit = 12;
  int _timeLimit = 60;
  int _uciOutput = false;

  bool features[NUM_OF_FEATURES] = {true, true, true, true};

  bool _stopSearch = false;
  timeval _startTime;
  int _totalNodes = 0;
  int _fhNodes = 0;
  int _fhfNodes = 0;
  int _currDepth = 0;
  int _maxDepth = 0;
  Move _currMove = 0;
  int _currMoveNumber = 0;

  void checkTimeLimit();
  void resetStats();
};

#endif /* defined(__Seanet__searchcontroller__) */
