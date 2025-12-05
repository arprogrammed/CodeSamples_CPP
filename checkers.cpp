#include "checkers.h"

#include <string>
#include <vector>


Checkers::Checkers(int id, bool rank, std::string piece_type, std::vector<std::pair<int,int>> positions) : id(id), rank(rank), piece_type(piece_type), positions(positions) {}

void Checkers::setRank(bool rank) {
  this->rank = rank;
};

void Checkers::setPiece_type(std::string gender) {
  if(gender == "male") {
    if(this->getRank()) {
      this->piece_type = "King";
    } else {
      this->piece_type = "man";
    }
  } else if(gender == "female") {
    if(this->getRank()) {
      this->piece_type = "Queen";
    } else {
      this->piece_type = "woman";
    }
  } else {
    if(this->getRank()) {
      this->piece_type = "Royal";
    } else {
      this->piece_type = "pawn";
    }
  }
}

bool Checkers::comparePostion(std::pair<int,int> new_position) {
  if (this->positions.begin()->first == new_position.first && this->positions.begin()->second == new_position.second) {
    return true;
  } else {
    return false;
  }
}
