//
//  main.cpp
//  Learning
//
//  Created by Stiven Deleur on 5/31/16.
//
//

#include "datum.hpp"
#include "neuralnet.hpp"
#include "perceptron.hpp"
#include <fstream>
#include <iostream>

float alpha = 0.5;
NeuralNet *midgameNeuralNet;
NeuralNet *endgameNeuralNet;
void loadExistingWeights(NeuralNet *net, std::string name);
void recordWeights(NeuralNet *net, std::string name);
void loadRandomWeights(NeuralNet *net);
std::vector<Datum> loadData(std::vector<std::string> pgnFiles);

int main(int argc, const char *argv[]) {

  initPresets();

  // Randomly seed rand() function
  timeval currTime;
  gettimeofday(&currTime, 0);
  int now = (int)(timeToMS(currTime));
  srand(now);

  std::vector<int> layers = {5, 5, 1}; // layers[0] is initial features
  midgameNeuralNet = new NeuralNet(layers);
  endgameNeuralNet = new NeuralNet(layers);

  loadRandomWeights(midgameNeuralNet);
  std::cout << "Loaded wieghts." << std::endl;
  recordWeights(midgameNeuralNet, "midgame");
  std::cout << "Recorded weights." << std::endl;

  delete midgameNeuralNet;
  delete endgameNeuralNet;

  std::vector<std::string> pgnGames;
  pgnGames.push_back("Carlsen.pgn");

  std::vector<Datum> data = loadData(pgnGames);
  std::cout << data.size() << std::endl;

  // Counter for number of examples tested in current training session
  int numExamples = 0;

  while (true) {
    for (Datum datum : data) {
      // Increment counter
      numExamples++;

      // Perform training
      trainTwoNets(datum._state, datum._bestMove, midgameNeuralNet,
                   endgameNeuralNet, alpha);

      // Record the current weights every n training positions
      if (numExamples % 2000 == 0) {
        recordWeights(midgameNeuralNet, "Midgame");
        recordWeights(endgameNeuralNet, "Endgame");
      }
    }
  }

  return 0;
}

void loadRandomWeights(NeuralNet *net) {
  std::vector<std::vector<FList>> weights = net->getWeights();
  for (int layer = 0; layer < weights.size(); layer++) {
    for (int pn = 0; pn < weights[layer].size(); pn++) {
      for (int f = 0; f < weights[layer][pn].size(); f++) {
        net->_perceptrons[layer][pn]._weights[f] =
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      }
    }
  }
}

void loadExistingWeights(NeuralNet *net, std::string name) {}

void recordWeights(NeuralNet *net, std::string name) {
  char buff[DTTMSZ];
  std::fstream records;
  records.open("weight-records.text", std::fstream::out | std::fstream::app);
  records << "<network name=" << name << ">\n"
          << "<info>Weights as of " << getDtTm(buff) << "</info>\n<weights>\n";
  std::vector<std::vector<FList>> weights = net->getWeights();
  for (int layer = 0; layer < weights.size(); layer++) {
    records << "<layer>\n";
    for (int pn = 0; pn < weights[layer].size(); pn++) {
      records << "(";
      for (int f = 0; f < weights[layer][pn].size(); f++) {
        records << weights[layer][pn][f];
        if (f < weights[layer][pn].size() - 1) {
          records << ", ";
        }
      }
      records << ")";
    }
    records << "\n</layer>\n";
  }
  records << "</weights>\n";
  records << "</network>\n";
  records.close();
}

std::vector<Datum> loadData(std::vector<std::string> pgnFiles) {
  std::vector<Datum> data;

  for (std::string fileName : pgnFiles) {
    std::vector<std::string> games =
        exportGamesFromPGN(std::ifstream(fileName));
    for (std::string game : games) {

      std::vector<Move> moveLine = getGameMoveLine(game);
      int winner = getPGNGameWinner(game);
      State state = boardFromFEN("startpos");
      for (Move move : moveLine) {
        if (winner == NONE || state._sideToMove == winner) {
          if (isPseudoQuiet(state)) {
            data.emplace_back(state, move, 0);
          }
        }
        state.makeMove(move);
      }
    }
  }

  return data;
}
