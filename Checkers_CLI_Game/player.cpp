#include "player.h"
#include "checkers.h"

#include <string>
#include <vector>
#include <algorithm>


Player::Player(std::string name, std::string gender, bool color, bool compPlayer, int wins, int losses, std::vector<Checkers> myPieces) : name(name), gender(gender), color(color), compPlayer(compPlayer), wins(wins), losses(losses), myPieces(myPieces) {
  this->makePieces();  
}

void Player::setName(std::string name) {
  this->name = name;
}

void Player::setGender(std::string gender,std::vector<Checkers> &myPieces) {
  std::transform(gender.begin(), gender.end(), gender.begin(), ::tolower);
  this->gender = gender;
  for(size_t i = 0; i < myPieces.size(); ++i) {
    myPieces.at(i).setPiece_type(this->gender);
  }
}

void Player::setColor(bool color) {
  this->color = color;
}

void Player::setCompPlayer(bool compPlayer) {
  this->compPlayer = compPlayer;
}

void Player::setWins(int wins) {
  this->wins = wins;
}

void Player::setLosses(int losses) {
  this->losses = losses;
}

void Player::makePieces() {
  // Create the Checkers pieces for the player2
  int id = 30;
  if(this->color == false) {
    for(int i=1; i < 4; ++i) {
      for(int j=1; j < 9; j += 2) {
        if(i == 2) {
          int k = j + 1;
          Checkers piece{id,false,"",{{i,k}}};
          myPieces.push_back(piece);
        } else {
          Checkers piece{id,false,"",{{i,j}}};
          myPieces.push_back(piece);
        }
        ++id;
      }
    }
    this->setGender(this->gender,this->myPieces);
  } else {
    // Create the Checkers pieces for the player1
    id = 10;
    for(int i=8; i > 5; --i) {
      for(int j=2; j < 9; j += 2) {
        if(i == 7) {
          int k = j - 1;
          Checkers piece{id,false,"",{{i,k}}};
          myPieces.push_back(piece);
        } else {
          Checkers piece{id,false,"",{{i,j}}};
          myPieces.push_back(piece);
        }
        ++id;
      }
    }
    this->setGender(this->gender,this->myPieces);
  }
}
