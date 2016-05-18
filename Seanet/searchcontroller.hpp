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

class SearchController {
public:
  int _depthLimit;
  int _timeLimit;
  int _totalNodes;
  double _startTime = time(NULL);
  bool _stopSearch;
  int _bestRootMove;
  int _bestRootScore = INT_MIN;
  int _bestPreviousMove = 0;
  void checkTimeLimit();
};

#endif /* defined(__Seanet__searchcontroller__) */
