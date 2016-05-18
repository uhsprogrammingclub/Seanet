//
//  searchcontroller.cpp
//  Seanet
//
//  Created by Nathaniel Corley on 5/17/16.
//
//

#include "searchcontroller.hpp"

void SearchController::checkTimeLimit() {
  if ((time(NULL) - _startTime) > _timeLimit) {
    std::cout << "STOP THE SEARCH";
    _stopSearch = true;
  }
  std::cout << "ZERO: " << time(NULL) << " " << _startTime << "\n";
  std::cout << (time(NULL) - _startTime) << " " << _timeLimit << "\n";
  std::cout << "\n TIME LIMIT: " << _timeLimit << " \n";
}