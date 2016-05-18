//
//  searchcontroller.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/17/16.
//
//

#include "searchcontroller.hpp"

void SearchController::checkTimeLimit() {
  if ((time(NULL) - _startTime) >= _timeLimit) {
    _stopSearch = true;
  }
}