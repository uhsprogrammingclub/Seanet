//
//  util.cpp
//  Seanet
//
//  Created by Stiven Deleur on 5/12/16.
//
//
#include "movegenerator.hpp"
#include "util.hpp"

const int index64[64] = {0,  47, 1,  56, 48, 27, 2,  60, 57, 49, 41, 37, 28,
                         16, 3,  61, 54, 58, 35, 52, 50, 42, 21, 44, 38, 32,
                         29, 23, 17, 11, 4,  62, 46, 55, 26, 59, 40, 36, 15,
                         53, 34, 51, 20, 43, 31, 22, 10, 45, 25, 39, 14, 33,
                         19, 30, 9,  24, 13, 18, 8,  12, 7,  6,  5,  63};

unsigned char popCountOfByte256[256];

U64 setMask[64];
U64 clearMask[64];
U64 kingAttacks[64];
U64 knightAttacks[64];
U64 pawnAttacks[2][64];

std::vector<std::string> &split(const std::string &s, char delim,
                                std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

char pieceToChar(Piece p) {
  switch (p) {
  case wP:
    return 'P';
  case bP:
    return 'p';
  case wN:
    return 'N';
  case bN:
    return 'n';
  case wB:
    return 'B';
  case bB:
    return 'b';
  case wR:
    return 'R';
  case bR:
    return 'r';
  case wQ:
    return 'Q';
  case bQ:
    return 'q';
  case wK:
    return 'K';
  case bK:
    return 'k';
  default:
    return ' ';
  }
}

Piece charToPiece(char p) {
  switch (p) {
  case 'P':
    return wP;
  case 'p':
    return bP;
  case 'N':
    return wN;
  case 'n':
    return bN;
  case 'B':
    return wB;
  case 'b':
    return bB;
  case 'R':
    return wR;
  case 'r':
    return bR;
  case 'Q':
    return wQ;
  case 'q':
    return bQ;
  case 'K':
    return wK;
  case 'k':
    return bK;
  default:
    return EMPTY;
  }
}

int bitboardForPiece(Piece p) {
  switch (p) {
  case wP:
    return PAWNS;
  case bP:
    return PAWNS;
  case wN:
    return KNIGHTS;
  case bN:
    return KNIGHTS;
  case wB:
    return BISHOPS;
  case bB:
    return BISHOPS;
  case wR:
    return ROOKS;
  case bR:
    return ROOKS;
  case wQ:
    return QUEENS;
  case bQ:
    return QUEENS;
  case wK:
    return KINGS;
  case bK:
    return KINGS;
  default:
    return 8;
  }
}

int sideBitboardForPiece(Piece p) {
  if (p % 2 == 0) {
    return BLACKS;
  } else {
    return WHITES;
  }
}

int uciToIndex(std::string uci) {
  if (uci.length() == 2) {
    int x = uci[0] - 97;
    int y = uci[1] - '0' - 1;
    return y * 8 + x;
  } else {
    return -1;
  }
}

std::string indexToUCI(int index) {
  int x = index % 8;
  int y = (index - x) / 8;
  std::string uci;
  uci.push_back(x + 97);
  uci.push_back(y + '0' + 1);
  return uci;
}

std::string boardToFEN(const State &b) {
  std::string FEN = "";
  int indices[64];
  int *pindex = indices;
  for (int i = 0; i < 8;
       i++) { // Making a list of locations to loop (start top left, etc.)
    for (int j = 56 - i * 8; j < 64 - i * 8; j++) {
      *(pindex++) = j;
    }
  }
  int adjEmpty = 0;
  for (int i = 0; i < 64; i++) { // Adding piece locations including spaces
    int index = indices[i];
    Piece p = b._pieces[index]; // Use getPieceAtIndex function here...
    if (p != EMPTY) {
      FEN += adjEmpty > 0 ? std::to_string(adjEmpty) : "";
      adjEmpty = 0;
      FEN += std::string(1, pieceToChar(p));
    } else {
      adjEmpty++;
    }
    if (index % 8 == 7) {
      FEN += adjEmpty > 0 ? std::to_string(adjEmpty) : "";
      adjEmpty = 0;
      FEN += index != 7 ? "/" : "";
    }
  }
  FEN += b._sideToMove == WHITE ? " w " : " b "; // Side to move

  // Castling rights
  int castling[] = {WKCA, WQCA, BKCA, BQCA};
  std::string castlingStrings[] = {"K", "Q", "k", "q"};
  for (auto &&right : castling) {
    U64 isolated = b._castleRights & right;
    FEN += isolated != 0 ? castlingStrings[LS1B(isolated)] : "";
  }
  FEN += b._castleRights == 0 ? "-" : "";

  // EP Target
  FEN += b._EPTarget != -1 ? " " + std::to_string(b._EPTarget) + " " : " - ";
  // Move counters
  FEN += std::to_string(b._halfMoveClock) + " " +
         std::to_string(b._fullMoveCounter);

  return FEN;
}

State boardFromFEN(std::string FEN) {
  State b;
  std::vector<std::string> subFEN = split(FEN, ' ');

  std::vector<std::string> piecesByRow = split(subFEN[0], '/');
  std::string sideToMove = subFEN[1];
  std::string castlingRights = subFEN[2];
  std::string enPassantTarget = subFEN[3];
  int halfMoveClock = std::stoi(subFEN[4]);
  int fullMoveCounter = std::stoi(subFEN[5]);

  int y = 7;
  for (std::vector<std::string>::iterator it = piecesByRow.begin();
       it != piecesByRow.end(); ++it) {
    int x = 0;
    for (std::string::iterator piece = it->begin(); piece != it->end();
         ++piece) {
      if (std::isdigit(*piece)) {
        x += *piece - '0';
      } else {
        int index = y * 8 + x;
        Piece p = charToPiece(*piece);
        b._pieces[index] = p;
        b._pieceBitboards[bitboardForPiece(p)] |= 1L << index;
        b._pieceBitboards[sideBitboardForPiece(p)] |= 1L << index;
        x++;
      }
    }
    y--;
  }
  b._sideToMove = sideToMove == "w" ? WHITE : BLACK;
  for (std::string::iterator it = castlingRights.begin();
       it != castlingRights.end(); ++it) {
    switch (*it) {
    case 'Q':
      b._castleRights |= WQCA;
      break;
    case 'K':
      b._castleRights |= WKCA;
      break;
    case 'q':
      b._castleRights |= BQCA;
      break;
    case 'k':
      b._castleRights |= BKCA;
      break;
    }
  }
  b._EPTarget = uciToIndex(enPassantTarget);
  b._halfMoveClock = halfMoveClock;
  b._fullMoveCounter = fullMoveCounter;
  return b;
}

int LS1B(U64 bb) {
  const U64 debruijn64 = 0x03f79d71b4cb0a89ULL;
  if (bb == 0) {
    return -1;
  }
  return index64[((bb ^ (bb - 1)) * debruijn64) >> 58];
}

int popBit(U64 *bb) {
  int index = LS1B(*bb);
  *bb &= (*bb - 1);
  return index;
}

void initpopCountOfByte256() {
  popCountOfByte256[0] = 0;
  for (int i = 1; i < 256; i++)
    popCountOfByte256[i] = popCountOfByte256[i / 2] + (i & 1);
}

int countSetBits(U64 bb) {
  return popCountOfByte256[bb & 0xff] + popCountOfByte256[(bb >> 8) & 0xff] +
         popCountOfByte256[(bb >> 16) & 0xff] +
         popCountOfByte256[(bb >> 24) & 0xff] +
         popCountOfByte256[(bb >> 32) & 0xff] +
         popCountOfByte256[(bb >> 40) & 0xff] +
         popCountOfByte256[(bb >> 48) & 0xff] + popCountOfByte256[bb >> 56];
}

std::vector<int> getSetBits(U64 bb) {
  std::vector<int> setBits;
  while (bb) {
    int index = LS1B(bb);
    setBits.push_back(index);
    CLRBIT(bb, index);
  }
  return setBits;
}

void initPresets() {

  initpopCountOfByte256();
  for (int i = 0; i < 64; i++) {
    setMask[i] = 1ULL << i;
    U64 bit = setMask[i];
    clearMask[i] = ~bit;
    U64 kingAttack = bit | RIGHT(bit) | LEFT(bit);
    kingAttack |= UP(kingAttack) | DOWN(kingAttack);
    kingAttacks[i] = CLRBIT(kingAttack, i);

    U64 l1 = LEFT(bit);
    U64 l2 = LEFT(LEFT(bit));
    U64 r1 = RIGHT(bit);
    U64 r2 = RIGHT(RIGHT(bit));

    U64 h1 = l2 | r2;
    U64 h2 = l1 | r1;

    knightAttacks[i] = UP(h1) | DOWN(h1) | UP(UP(h2)) | DOWN(DOWN(h2));

    U64 wRightAttack = UP(RIGHT(bit));
    U64 wLeftAttack = UP(LEFT(bit));
    U64 bRightAttack = DOWN(RIGHT(bit));
    U64 bLeftAttack = DOWN(LEFT(bit));
    pawnAttacks[0][i] = (wRightAttack | wLeftAttack);
    pawnAttacks[1][i] = (bRightAttack | bLeftAttack);
  }

  generateOccupancyVariations(true);
  generateMoveDatabase(true);
  generateOccupancyVariations(false);
  generateMoveDatabase(false);
}