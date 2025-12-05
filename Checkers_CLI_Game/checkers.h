#ifndef CHECKERS_H
#define CHECKERS_H

#include <string>
#include <vector>

class Player;

class Checkers{
public:
  int id;
  bool rank {false}; //True is a king piece in checkers
  std::string piece_type; //Piece_type setby gender of player
  std::vector<std::pair<int,int>> positions;

  Checkers();
  Checkers(int id = 0, bool rank = false, std::string piece_type = "pawn", std::vector<std::pair<int,int>> positions = {});

  void setId();
  int getId() const {return this->id;};
  void setRank(bool);
  bool getRank() const {return this->rank;};
  void setPiece_type(std::string gender);
  std::string getPiece_type() const {return this->piece_type;};
  std::vector<std::pair<int,int>> getPositions() const {return this->positions;};
  bool comparePostion(std::pair<int,int> new_position);

};

#endif
