//
//  searchcontroller.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/17/16.
//
//

#include "searchcontroller.hpp"

void SearchController::checkTimeLimit() {
  if ((time(NULL) - _startTime.tv_sec) >= _timeLimit) {
    _stopSearch = true;
  }
}

void SearchController::resetStats() {
  gettimeofday(&_startTime, 0);
  _totalNodes = 0;
  _fhNodes = 0;
  _fhfNodes = 0;
  _currDepth = 0;
  _maxDepth = 0;
  _currMove = 0;
  _currMoveNumber = 0;
  _stopSearch = false;
}