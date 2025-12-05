#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "checkers.h"

class Player { 
public:
  std::string name;
  std::string gender;
  bool color; // True equals black and player1
  bool compPlayer; // True equals computer player or non-human player
  int wins;
  int losses;
  std::vector<Checkers> myPieces;

  Player();
  Player(std::string name = "", std::string gender = "other", bool color = true, bool comp_player = false, int wins = 0, int losses = 0, std::vector<Checkers> myPieces = {});

  void setName(std::string);
  std::string getName() const {return this->name;};
  void setGender(std::string,std::vector<Checkers> &myPieces);
  std::string getGender() const {return this->gender;};
  void setColor(bool);
  bool getColor() const {return this->color;};
  void setCompPlayer(bool);
  bool getCompPlayer() const {return this->compPlayer;};
  void setWins(int);
  int getWins() const {return this->wins;};
  void setLosses(int);
  int getLosses() const {return this->losses;};
  std::vector<Checkers> getMyPieces() const {return this->myPieces;}
  void makePieces();
  
};

#endif