#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <string>
#include <vector>
#include <array>
#include <cstdlib>
#include <random>
#include <chrono>
#include <thread>
#include <filesystem>
#include "player.h"
#include "checkers.h"

// Move object for passing ease, may eventually expand
struct Move {
  std::pair<int, int> position;
  int piece_id;
};

struct LeaderBoardRecord {
    std::string playerName;
    int playerWins;
    bool operator==(const LeaderBoardRecord &lhs) const {
        return (this->playerName == lhs.playerName);
    }
    bool operator<(LeaderBoardRecord &lhs) {
        return (this->playerWins > lhs.playerWins);
    }
};

struct LossLeaderBoardRecord {
    std::string playerName;
    int playerLosses;
    bool operator==(const LossLeaderBoardRecord &lhs) const {
        return (this->playerName == lhs.playerName);
    }
    bool operator<(LossLeaderBoardRecord &lhs) {
        return (this->playerLosses > lhs.playerLosses);
    }
};

// Game Board
struct Board {
    std::vector<std::array<std::array<int, 8>,8>> board_vec;
};

// Computer move pathing Node
struct Node {
    int piece_id {0};
    std::vector<std::pair<int,int>> single_moves;
    std::vector<std::vector<std::pair<int,int>>> jump_moves;
};

// Proto start here
std::string clean_string(std::string string_to_validate);
int validate_num (int min, int max, std::string string_of_num);
bool saveGame(Player player1, Player player2, Player turn_of_current_player, Board board);
void addLeaderBoardRecord(Player &winning_player);
void addLosingBoardRecord(Player &losing_player);
bool renderLeaderBoard();
bool renderLoserBoard();
bool renderRules();
bool renderPauseMenuOperation(Player player1, Player player2, Player current_player, Board board);
bool loadGame(Board &board);
bool startNewGame(Board &board);
struct Move playerPositionPromptValidation(Player &player_moving, Player &player_not_moving, Board &board);
void renderBoard(const Player &player1, const Player &player2, Board &board);
bool isOnBoard(const std::pair<int, int> check_location);
bool reverseMoveCheck(std::vector<std::pair<int,int>> vec, std::vector<Node> &possible_moves, int piece_id);
bool playerPieceCheck(Player &player, std::pair<int,int> position);
bool playerPieceRankUpCheck(Player &player_moving, int piece_id);
bool playerSingleMoveValidator(Player &player, Player &player_not_moving, int piece_id, std::pair<int,int> position);
bool playerJumpMoveValidator(Player &player, Player &player_not_moving, const int piece_id, const std::pair<int,int> position, const std::pair<int,int> current_position);
void playerMove(Player &player, int piece_id, std::pair<int,int> position);
void playerJumpMove(Player &player, Player &player_captured, int piece_id, std::pair<int,int> position, std::pair<int,int> current_position);
bool playerMovementValidation(Player &player_moving, Player &player_not_moving, const int piece_id, const std::pair<int,int> position, const std::pair<int,int> current_position);
void gameReplay(Board &board);
int rand_num(int min, int max);
void comp_check_smove_location(Player &player_human, Player &player_comp, Checkers &piece, std::vector<Node> &possible_moves, std::pair<int,int> &current_position, int shift_row, int shift_col);
void comp_check_jmove_location(Player &player_human, Player &player_comp, Checkers &piece, std::vector<Node> &possible_moves, std::pair<int,int> &current_position, int shift_row, int shift_col);
bool computerTurn(Player &player_human, Player &player_comp, Board &board);
bool playerAdditionalJumpTurn(Player &player_not_moving, Player &player_moving, int piece_id);
void playerTurnPrompt(Player &player_moving, Player &player_not_moving, Board &board);
bool winConditionCheck (Player &player1, Player &player2, Board &board);
// Proto end here
int main() {
  // Initialize game board
  Board board {};
  // Display and validate the menu selection
  bool entry_valid {false};
  while(!entry_valid) {
    std::string menu_selection {"0"};
    std::cout << std::setw(50) << std::left << "Checkers Main Menu" << std::endl;
    std::cout << std::setw(50) << std::left << "1 - Start New Game" << std::endl;
    std::cout << std::setw(50) << std::left << "2 - Game Rules" << std::endl;
    std::cout << std::setw(50) << std::left << "3 - Leaderboard" << std::endl;
    std::cout << std::setw(50) << std::left << "4 - Loserboard" << std::endl;
    std::cout << std::setw(50) << std::left << "5 - Load Game" << std::endl;
    std::cout << std::setw(50) << std::left << "6 - Exit Game" << std::endl;
    std::cout << std::setw(50) << "---------------------------" << std::endl;
    std::cout << "Select an option from above using the number: ";  
    std::getline(std::cin, menu_selection);
    int validated_menu_selection{0};
    validated_menu_selection = validate_num(0, 7, menu_selection);
    
    if(validated_menu_selection == 0 || validated_menu_selection > 6 ) {
      entry_valid = false;
    } else {
      entry_valid = true;
    }
  // Switch on user input
  switch(validated_menu_selection) {
    case 1: {
      entry_valid = startNewGame(board);
      break;
    }
    case 2: {
      entry_valid = renderRules();
      std::cout << std::endl;
      break;
    }
    case 3: {
      entry_valid = renderLeaderBoard();
      std::cout << std::endl;
      break;
    }
    case 4: {
      entry_valid = renderLoserBoard();
      std::cout << std::endl;
      break;
    }
    case 5: {
      entry_valid = loadGame(board);
      break;
    }
    case 6: {
      // Quit game
      entry_valid = true;
      break;
    }
    default: {
      // Default is to re-display menu
      entry_valid = false;
      break;
    }
  }
}
    
return 0;
}

// Validates a string and cleans it for sanitized input usage
std::string clean_string(std::string string_to_validate) {
    std::transform(string_to_validate.begin(), string_to_validate.end(), string_to_validate.begin(), [](char c){return std::tolower(c);});
    std::transform(string_to_validate.begin(), string_to_validate.end(), string_to_validate.begin(), [](char c){if(isalnum(c)) {return c;} else {return 'x';}});
    
    return string_to_validate;
}

// Validates a number is an integer and sits between a min & max number
int validate_num(int min, int max, std::string string_of_num) {
    std::stringstream num(string_of_num);
    int new_num{0};
    
    if(num >> new_num && (new_num > min) && (new_num < max)) {
        return new_num;
    } else {
        return 0;
    }
}

bool saveGame(Player player1, Player player2, Player turn_of_current_player, Board board) {
    std::string save_game_filename{""};
    std::string save_game_filename_complete{""};
    std::string save_board_filename{""};
    std::cout << "Enter the name of the your game save: ";
    std::getline(std::cin, save_game_filename);
    std::filesystem::path save_game_folder {"./Checkers_Game/src/savegames/"};
    
    // Checking for existing save file and prompt user to overwrite or rename
    bool overwrite{false};
    while(!overwrite) {
      save_game_filename = clean_string(save_game_filename);
      if(std::filesystem::is_empty(save_game_folder)) {
          overwrite = true;
      } else {
        bool new_filename {false};
        for(auto saves: std::filesystem::directory_iterator(save_game_folder)) {
            if(save_game_filename == saves.path().stem().string()) {
                std::string user_choice {"n"};
                std::cout << "A save game with this name already exists. Would you like to save over it? Y/N: ";
                std::getline(std::cin, user_choice);
                user_choice = clean_string(user_choice);
                
                while(user_choice.empty()) {
                    std::cout << "A save game with this name already exists. Would you like to save over it? Y/N: ";
                    std::getline(std::cin, user_choice);
                    user_choice = clean_string(user_choice);
                }
                        
                if(user_choice.at(0) == 'y') {
                  new_filename = false;
                } else {
                  std::cout << "Please choose a new name: ";
                  std::getline(std::cin, save_game_filename);
                  overwrite = false;
                  new_filename = true;
                }
            }
        }
        if(!new_filename) {
            overwrite = true;
        }
      }
    }
    
    // Outputs the players, player piece count, player turn bool, and piece positions
    save_game_filename_complete = "./Checkers_Game/src/savegames/" + save_game_filename + ".txt";
    std::ofstream out_file {save_game_filename_complete};
    if(!out_file.is_open()){
      std::cerr << "Could not open or create file to save game.";
      return false;
    } else {
      out_file << 0 << " ";
      if(turn_of_current_player.getColor()) {
        out_file << 1 << " ";
      } else {
        out_file << 0 << " ";
      }
      out_file << player1.myPieces.size() << " " << player1.getName() << " " << player1.getGender() << " "
        << player1.getColor() << " " << player1.getWins() << " " << player1.getLosses() << std::endl;
      
      if(player2.getCompPlayer()) {
        out_file << 1 << " ";
      } else {
        out_file << 0 << " ";
      }
      if(!turn_of_current_player.getColor()) {
        out_file << 1 << " ";
      } else {
        out_file << 0 << " ";
      }
      out_file << player2.myPieces.size() << " " << player2.getName() << " " << player2.getGender() << " "
        << player2.getColor() << " " << player2.getWins() << " " << player2.getLosses() << std::endl;
        
      for(auto piece: player1.getMyPieces()) {
          out_file << piece.getId() << " " 
          << piece.getPositions().begin()->first << " " << piece.getPositions().begin()->second << " "
          << piece.getRank() << std::endl;
      }
      for(auto piece: player2.getMyPieces()) {
          out_file << piece.getId() << " " 
          << piece.getPositions().begin()->first << " " << piece.getPositions().begin()->second << " "
          << piece.getRank() << std::endl;
      }
    }
    out_file.close();
    
    // Saves the board states in Board vector to a save file in boards folder
    save_board_filename = "./Checkers_Game/src/boards/" + save_game_filename + ".txt";
    std::ofstream out_file_2 {save_board_filename};
    if(!out_file_2.is_open()) {
      std::cerr << "Could not open or create file to save board.";
      return false;
    } else {
      // Outputs states in save file
      for(size_t k=0; k<board.board_vec.size()-1; ++k ) {
        for(size_t i=0; i<8; ++i) {
          for(size_t j=0; j<8; ++j) {
            if(i==7 && j==7) {
              out_file_2 << board.board_vec.at(k)[i][j];
            } else {
              out_file_2 << board.board_vec.at(k)[i][j] << ",";
            }
          } 
        }
        out_file_2 << std::endl;
      }
    }
    out_file_2.close();
    
    return true;
}

void addLeaderBoardRecord(Player &winning_player) {
  winning_player.setWins(winning_player.getWins() + 1);  
  LeaderBoardRecord new_addition{winning_player.getName(),winning_player.getWins()};
  
  // Open and load in leaderboard file
  std::vector<LeaderBoardRecord> existing_leaders {};
  std::ifstream in_file {"./Checkers_Game/leaderboard.txt"};
  if(!in_file.is_open()) {
    std::cerr << "File could not be found or opened.";
  } else {
    while(!in_file.eof()) {
        std::string line{};
        std::getline(in_file, line);
        if(!line.empty()) {
          std::istringstream temp_record_in{line};
          std::string name;
          int wins;
          temp_record_in >> name >> wins;
          LeaderBoardRecord temp {name, wins};
          existing_leaders.push_back(temp);    
        } else {}
    }
    in_file.close();
  }
  // Search for existing player name
  auto it = std::find(existing_leaders.begin(), existing_leaders.end(), LeaderBoardRecord {new_addition.playerName} );
  
  // Add new record or increment existing record
  std::ofstream out_file {"./Checkers_Game/leaderboard.txt"};
  if(!out_file.is_open()){
    std::cerr << "File could not be found or opened.";
  } else {
    if(it == existing_leaders.end()) {
      existing_leaders.push_back(new_addition);
      for(auto record: existing_leaders) {
        out_file << record.playerName << " " << record.playerWins << std::endl;
      }
    } else {
      it->playerWins = it->playerWins+1;
      for(auto record: existing_leaders) {
        out_file << record.playerName << " " << record.playerWins << std::endl;
      }
    }
  }
  out_file.close();
}

void addLosingBoardRecord(Player &losing_player) {
  losing_player.setWins(losing_player.getWins() + 1);  
  LeaderBoardRecord new_addition{losing_player.getName(),losing_player.getWins()};
  
  // Open and load in loserboard file
  std::vector<LeaderBoardRecord> existing_leaders {};
  std::ifstream in_file {"./Checkers_Game/loserboard.txt"};
  if(!in_file.is_open()) {
    std::cerr << "File could not be found or opened.";
  } else {
    while(!in_file.eof()) {
        std::string line{};
        std::getline(in_file, line);
        if(!line.empty()) {
          std::istringstream temp_record_in{line};
          std::string name;
          int wins;
          temp_record_in >> name >> wins;
          LeaderBoardRecord temp {name, wins};
          existing_leaders.push_back(temp);    
        } else {}
    }
    in_file.close();
  }
  // Search for existing player name
  auto it = std::find(existing_leaders.begin(), existing_leaders.end(), LeaderBoardRecord {new_addition.playerName} );
  
  // Add new record or increment existing record
  std::ofstream out_file {"./Checkers_Game/leaderboard.txt"};
  if(!out_file.is_open()){
    std::cerr << "File could not be found or opened.";
  } else {
    if(it == existing_leaders.end()) {
      existing_leaders.push_back(new_addition);
      for(auto record: existing_leaders) {
        out_file << record.playerName << " " << record.playerWins << std::endl;
      }
    } else {
      it->playerWins = it->playerWins+1;
      for(auto record: existing_leaders) {
        out_file << record.playerName << " " << record.playerWins << std::endl;
      }
    }
  }
  out_file.close();
}

bool renderLeaderBoard() {
  // Load leader board into vec
  std::vector<LeaderBoardRecord> leader_board_records {};
  std::ifstream in_file {"./Checkers_Game/leaderboard.txt"};
  if(!in_file.is_open()) {
    std::cerr << "File could not be found or opened.";
  } else {
    while(!in_file.eof()) {
        std::string line{};
        std::getline(in_file, line);
        if(!line.empty()) {
          std::istringstream temp_record_in{line};
          std::string name;
          int wins;
          temp_record_in >> name >> wins;
          LeaderBoardRecord temp {name, wins};
          leader_board_records.push_back(temp);    
        } else {}
    }
    in_file.close();
  }
  // Sort for display high to low wins leveraging overload< in struct LeaderBoardRecord
  std::sort(leader_board_records.begin(), leader_board_records.end());
  // Display
  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << std::setw(50) << "Race To The Top! Checkers Leading Winners." << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;
  for(auto record: leader_board_records) {
    std::cout << std::setw(30) << record.playerName << " Wins " << record.playerWins << std::endl;
  }
  std::string temp_input;
  std::cout << "Type anything and press enter to return to main menu." << std::endl;
  std::getline(std::cin, temp_input);
  temp_input = clean_string(temp_input);
  return false;
}

bool renderLoserBoard() {
  // Load loss leader board into vec
  std::vector<LossLeaderBoardRecord> leader_board_records {};
  std::ifstream in_file {"./Checkers_Game/loserboard.txt"};
  if(!in_file.is_open()) {
    std::cerr << "File could not be found or opened.";
    return false;
  } else {
    while(!in_file.eof()) {
        std::string line{};
        std::getline(in_file, line);
        if(!line.empty()) {
          std::istringstream temp_record_in{line};
          std::string name;
          int losses;
          temp_record_in >> name >> losses;
          LossLeaderBoardRecord temp {name, losses};
          leader_board_records.push_back(temp);    
        } else {}
    }
    in_file.close();
  }
  // Sort for display high to low losses leveraging overload< in struct LossLeaderBoardRecord
  std::sort(leader_board_records.begin(), leader_board_records.end());
  // Display
  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << std::setw(50) << "Race To The Bottom! Checkers Leading Losers." << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;
  for(auto record: leader_board_records) {
    std::cout << std::setw(30) << record.playerName << " Losses " << record.playerLosses << std::endl;
  }
  std::string temp_input;
  std::cout << "Type anything and press enter to return to main menu." << std::endl;
  std::getline(std::cin, temp_input);
  temp_input = clean_string(temp_input);
  return false;
}

bool renderRules() {
  std::cout << "------------------------------------------------------------------" << std::endl;
  std::cout << "Checkers Game Rules" << std::endl;
  std::cout << "------------------------------------------------------------------" << std::endl;
  std::cout << "1.  There are only two players, each with 12 pieces." << std::endl;
  std::cout << "2.  Player 1 will go first." << std::endl;
  std::cout << "3.  Players will take turns one after the other." << std::endl;
  std::cout << "4.  Basic pieces move only forwards in diagonal directions." << std::endl;
  std::cout << "5.  The goal is to capture each others pieces." << std::endl;
  std::cout << "6.  To capture, you must jump over another player's piece." << std::endl;
  std::cout << "7.  This game does will force double and triple jumps." << std::endl;
  std::cout << "8.  If your piece makes it to the other side it's leveled up." << std::endl;
  std::cout << "9.  Leveled pieces can move forwards or backwards diagonally." << std::endl;
  std::cout << "10. First person with no pieces left loses or draw if both have one piece left." << std::endl;
  
  std::cout << "Note: This game does not force initial jump moves. Play with honor!" << std::endl;
  std::string temp_input;
  std::cout << "Type anything and press enter to return to main menu." << std::endl;
  std::getline(std::cin, temp_input);
  temp_input = clean_string(temp_input);
  return false;
}

bool loadGame(Board &board) {
  std::string users_saved_game{""};
  std::filesystem::path save_game_folder {"./Checkers_Game/src/savegames/"}; // use ../Checkers_Game in debug with Codelite IDE
  
  std::cout << "-----------------------------" << std::endl;
  std::cout << "Choose A Save Game" << std::endl;
  std::cout << "-----------------------------" << std::endl;
  if(std::filesystem::is_empty(save_game_folder)) {
    std::string tmp_input{""};
    std::cout << "There are no saved games as of yet! Start a new game from the menu." << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Hit enter to return to the main menu:";
    std::getline(std::cin, tmp_input);
    tmp_input = clean_string(tmp_input);
    return false;
  }
  for(auto saves: std::filesystem::directory_iterator(save_game_folder)) {
      std::string fs_name {saves.path().stem().string()};
      std::cout << fs_name << std::endl;
  }
  std::cout << "-----------------------------" << std::endl;
  std::cout << "Enter the file name to load: ";
  std::getline(std::cin, users_saved_game);
  users_saved_game = clean_string(users_saved_game);
  
  // Check for game name user selected and loop till valid file selected
  bool game_exists {false};
  while(!game_exists) {
  
    for(auto saves: std::filesystem::directory_iterator(save_game_folder)) {
      if(users_saved_game == saves.path().stem().string()) {
        game_exists = true;
      } else {}
    }

    if(!game_exists) {
      std::cout << "Game save does not exist, please enter another game to load: ";
      std::getline(std::cin, users_saved_game);
      users_saved_game = clean_string(users_saved_game);
      game_exists = false;
    } else {}
  }
  // Load variables and objects for use
  users_saved_game = users_saved_game + ".txt";
  Player player1 {"P1","male",true};
  Player player2 {"P2","other",false};
  player1.myPieces.clear();
  player2.myPieces.clear();
  bool player_turn{false};
  bool next_player{false};
  int line_num{0};
  int player1_piece_count{0};
  int player2_piece_count{0};
  
  // Load in save game file
  std::ifstream in_file {save_game_folder.string() + users_saved_game};
  if(!in_file.is_open()) {
    std::cerr << "File could not be found or opened.";
    return false;
  } else {
    while(!in_file.eof()) {
      std::string line{};
      std::getline(in_file, line);
      std::istringstream temp_record_in{line};
        
      std::string name{""};
      std::string gender_p1{""};
      std::string gender_p2{""};
      bool compPlayer{false};
      bool color{false};
      int wins{0};
      int losses{0};
      int piece_id{0};
      int row{0};
      int column{0};
      bool rank{false};
      
      // Line_num dictates which player is being loaded in
      if( line_num == 0 ) {
        temp_record_in >> compPlayer >> player_turn >> player1_piece_count >> name >> gender_p1 >> color >> wins >> losses;
        player1.setCompPlayer(compPlayer);
        player1.setName(name);
        player1.setColor(color);
        player1.setWins(wins);
        player1.setLosses(losses);
          if(player_turn) {
              next_player = true;
          } else {}
      } else if ( line_num == 1 ) {
        temp_record_in >> compPlayer >> player_turn >> player2_piece_count >> name >> gender_p2 >> color >> wins >> losses;
        player2.setCompPlayer(compPlayer);
        player2.setName(name);
        player2.setColor(color);
        player2.setWins(wins);
        player2.setLosses(losses);
          if(player_turn) {
              next_player = false;
          } else {}
      } else {}
      
      // Load in player's pieces based on the XXXX_piece_count
      if ( line_num > 1 && player1_piece_count != 0 ) {
        temp_record_in >> piece_id >> row >> column >> rank;
        std::pair<int, int> new_position(row, column);
        std::vector<std::pair<int,int>> post;
        post.push_back(new_position);
        Checkers temp_checker{piece_id,rank,"pawn", post};
        player1.myPieces.push_back(temp_checker);
        --player1_piece_count;
      } else if ( line_num > 1 && player2_piece_count != 0 ) {
        temp_record_in >> piece_id >> row >> column >> rank;
        std::pair<int, int> new_position(row, column);
        std::vector<std::pair<int,int>> post;
        post.push_back(new_position);
        Checkers temp_checker{piece_id,rank,"pawn", post};
        player2.myPieces.push_back(temp_checker);
        --player2_piece_count;
      }
      ++line_num;
      // Set the pieces genders based on loaded Gender
      if(player1_piece_count == 0 && player2_piece_count == 0 ) {
          // setGender() leverages setPiece_Type() for each checker
          player1.setGender(gender_p1, player1.myPieces);
          player2.setGender(gender_p2, player2.myPieces);
      } else {}
    }
  }
  in_file.close();
  
  // Load in the board state arrays into the board vector from saved board
  std::filesystem::path save_board_folder {"./Checkers_Game/src/boards/"};
  std::ifstream in_file_2 {save_board_folder.string() + users_saved_game};
  if(!in_file_2.is_open()) {
    std::cerr << "File could not be found or opened.";
    return false;
  } else {
    std::array<std::array<int, 8>,8> temp_arr;
    board.board_vec.clear();
    while(!in_file_2.eof()) {
      size_t row{0};
      size_t col{0};
      std::string line {""};
      std::getline(in_file_2, line);
      std::istringstream parsable_line {line};
      for(std::string sub_line; std::getline(parsable_line, sub_line, ',');) {
          if(col == 8) {
              col = 0;
              ++row;
          }
          int temp_num = validate_num(-1, 444, sub_line);
          temp_arr[row][col] = temp_num;
          ++col;
      }
      board.board_vec.push_back(temp_arr);
    }
  }
  in_file_2.close();
  // Confirm output to pick up the game where it left off
  std::cout << "\n---------------------------------------------------------------\n" << std::endl;
  if(next_player) {
      std::cout << "Game Loaded! It's " << player1.getName() << "'s turn!" << std::endl; 
  } else {
      std::cout << "Game Loaded! It's " << player2.getName() << "'s turn!" << std::endl; 
  }
  std::cout <<std::endl;
  renderBoard(player1,player2,board);
  // Basic play loop toggled on player turn after load
  bool endgame {false};
  while(!endgame) {
    if(next_player) {
      playerTurnPrompt(player1,player2,board);
      renderBoard(player1,player2,board);      
      endgame = winConditionCheck(player1,player2,board);
      if(!endgame) {
          playerTurnPrompt(player2,player1,board);
          renderBoard(player1,player2,board);
          endgame = winConditionCheck(player1,player2,board);
      }
    } else {
      playerTurnPrompt(player2,player1,board);
      renderBoard(player1,player2,board);
      endgame = winConditionCheck(player1,player2,board);
      if(!endgame) {
        playerTurnPrompt(player1,player2,board);
        renderBoard(player1,player2,board);
        endgame = winConditionCheck(player1,player2,board);
      }
    }
  }
  
  return false;
}

bool startNewGame(Board &board) {
  // User prompts and input validation
  std::cout << "Player 1 enter your name: ";
  std::string name1;
  std::getline(std::cin, name1);
  name1 = clean_string(name1);
  std::transform(name1.begin(), name1.begin()+1, name1.begin(), [](char c){return std::toupper(c);});
  std::cout << "Player 1 enter your gender (Male, Female, or Other): ";
  std::string gender1;
  std::getline(std::cin, gender1);
  gender1 = clean_string(gender1);
  // Setup Player1 based on inputs
  Player player1{name1,gender1,true};
  bool valid_gender{false};
  while(!valid_gender) {
      if(gender1 == "male" || gender1 == "female" || gender1 == "other") {
          player1.setGender(gender1, player1.myPieces);
          valid_gender = true;
      } else {
          std::cout << "Please enter a valid gender: ";
          std::getline(std::cin, gender1);
          gender1 = clean_string(gender1);
          valid_gender = false;
      }
  }
  
  std::string comp_player;
  // Prompt for computer player or human player for player 2
  bool valid_choice {false};
  while(!valid_choice) {
      std::cout << "Player with the computer? Y/N: ";
      std::getline(std::cin, comp_player);
      comp_player = clean_string(comp_player);
      
      while(comp_player.empty()){
        std::cout << "Player with the computer? Y/N: ";
        std::getline(std::cin, comp_player);
        comp_player = clean_string(comp_player);
      }
      
      if(comp_player.at(0) == 'y' || comp_player.at(0) == 'n') {
        valid_choice = true;
      } else {
        valid_choice = false;
      }
  }
  
  std::string name2;
  std::string gender2;
  bool npc {false};
  // User prompts and input validation
  if(comp_player.at(0) == 'n') {
    std::cout << "Player 2 enter your name: ";
    std::getline(std::cin, name2);
    name2 = clean_string(name2);
    std::transform(name2.begin(), name2.begin()+1, name2.begin(), [](char c){return std::toupper(c);});
    std::cout << "Player 2 enter your gender (Male, Female, or Other): ";
    std::getline(std::cin, gender2);
    gender2 = clean_string(gender2);
  } else {
    name2 = "ComputerPlayer";
    std::transform(name2.begin(), name2.begin()+1, name2.begin(), [](char c){return std::toupper(c);});
    gender2 = "other";
    npc = true;
  }
  
  // Setup Player2 based on inputs
  Player player2{name2,gender2,false,npc};
  valid_gender = false;
  while(!valid_gender) {
      if(gender2 == "male" || gender2 == "female" || gender2 == "other") {
          player2.setGender(gender2, player2.myPieces);
          valid_gender = true;
      } else {
          std::cout << "Please enter a valid gender: ";
          std::getline(std::cin, gender2);
          gender2 = clean_string(gender2);
          valid_gender = false;
      }
  }
  // Game begins
  std::cout << std::endl;
  std::cout << "New Game! Get ready " << player1.getName() << " & " << player2.getName() << "!" << std::endl;
  std::cout << "\n---------------------------------------------------------------\n" << std::endl;
  std::cout << std::endl;

  renderBoard(player1,player2,board);
  // Basic game loop toggled on player turn
  bool endgame {false};
  while(!endgame) {
    playerTurnPrompt(player1,player2,board);
    renderBoard(player1,player2,board);
    endgame = winConditionCheck(player1,player2,board);
    if(!endgame) {
        playerTurnPrompt(player2,player1,board);
        renderBoard(player1,player2,board);
        endgame = winConditionCheck(player1,player2,board);
    }
  }
  return false;
}

bool renderPauseMenuOperation(Player player1, Player player2, Player current_player, Board board) {
  std::string menu_selection {"0"};
  int validated_menu_selection{0};
  // Display and validate the menu selection
  bool entry_valid {false};
  while(!entry_valid) {
    std::cout << std::endl;
    std::cout << std::setw(50) << "---------------------------" << std::endl;
    std::cout << std::setw(50) << std::left << "Checkers Pause Menu" << std::endl;
    std::cout << std::setw(50) << std::left << "1 - Resume Game" << std::endl;
    std::cout << std::setw(50) << std::left << "2 - Save Game" << std::endl;
    std::cout << std::setw(50) << std::left << "3 - Quit Game" << std::endl;
    std::cout << std::setw(50) << "---------------------------" << std::endl;
    std::cout << "Select an option from above using the number: ";  
    std::getline(std::cin, menu_selection);
    validated_menu_selection = 0;
    validated_menu_selection = validate_num(0, 4, menu_selection);
    
    if(validated_menu_selection == 0 || validated_menu_selection > 3) {
      entry_valid = false;
      std::cout << "Inalid menu option. Please try again." << std::endl;
    } else {
      entry_valid = true;
    }
  }
  // Switch on selection and execute functions
  switch(validated_menu_selection) {
    case 1: {
      // Returns to game from pause menu
      return true;
      break;
    }
    case 2: {
      // Saves game from pause menu then returns to game
      while(!saveGame(player1, player2, current_player, board)) {
          std::cout << "Could not save game, retrying save." << std::endl;
      }
      std::cout << "Game saved!" << std::endl;
      return true;
      break;
    }
    case 3: {
      // Exits game entirely and prompts for save game
      std::string exit_save{""};
      std::cout << "Would you like to save before quitting? Y/N: ";
      std::getline(std::cin, exit_save);
      exit_save = clean_string(exit_save);
      
      while(exit_save.empty()) {
        std::cout << "Would you like to save before quitting? Y/N: ";
        std::getline(std::cin, exit_save);
        exit_save = clean_string(exit_save);
      }
      
      if (exit_save.at(0) == 'y') {
        if(saveGame(player1, player2, current_player, board)){
          std::cout << "Game saved!";
          std::exit(EXIT_SUCCESS);
        } else {
          std::exit(EXIT_SUCCESS);
        }
      } else {
        std::exit(EXIT_SUCCESS);
      }
      break;
    }
    default: {
      // Default returns to game
      return true;
      break;
    }
  }
  return false;
}

void renderBoard(const Player &player1, const Player &player2, Board &board) {
  // 2D array to hold the board
  int arr[8][8] {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  };
  // Loop through the players pieces and place them on the board
  for( size_t j = 0; j < player1.myPieces.size(); ++j) {
    int row_p1 = player1.myPieces.at(j).getPositions().begin()->first;
    int col_p1 = player1.myPieces.at(j).getPositions().begin()->second;
    bool is_royal = player1.myPieces.at(j).getRank();
    if(is_royal){
      arr[row_p1-1][col_p1-1] = player1.myPieces.at(j).getId()*10;
    } else {
      arr[row_p1-1][col_p1-1] = player1.myPieces.at(j).getId();
    }
  }

  for( size_t j = 0; j < player2.myPieces.size(); ++j) {
    int row_p2 = player2.myPieces.at(j).getPositions().begin()->first;
    int col_p2 = player2.myPieces.at(j).getPositions().begin()->second;
    bool is_royal = player2.myPieces.at(j).getRank();
    if(is_royal){
      arr[row_p2-1][col_p2-1] = player2.myPieces.at(j).getId()*10;
    } else {
      arr[row_p2-1][col_p2-1] = player2.myPieces.at(j).getId();
    }
  }
  // Display the board
  for(int k = 0; k < 9; ++k) {
    if(k == 0) {
      std::cout << std::setw(1) << k << "|";
    } else {
      std::cout << std::setw(3) << k << "|";
    }
  }
  std::cout << std::endl;
  for(int i = 0; i < 8; ++i) {
    std::cout << i+1 << "|";
    for(int j = 0; j < 8; ++j) {
      if(arr[i][j] == 0 && (i % 2 == 0) && (j % 2 != 0)) {
        std::cout << std::setw(3) << "   " << "|";
      } else if(arr[i][j] == 0 && (i % 2 != 0) && (j % 2 == 0)) {
        std::cout << std::setw(3) << "   " << "|";
      } else if(arr[i][j] == 0) {
        std::cout << std::setw(3) << "   " << "|";
      } else {
        std::cout << std::setw(3) << arr[i][j] << "|";
      }
    }
    std::cout << std::endl;
  }
  // Save the board state to the board vector for replay later
  std::array<std::array<int, 8>, 8> arr_std;
  for (size_t i = 0; i < 8; ++i) {
    for (size_t j = 0; j < 8; ++j) {
      arr_std[i][j] = arr[i][j];
    }
  }
  board.board_vec.push_back(arr_std);
}

bool isOnBoard(const std::pair<int, int> check_location) {
    // Need to test the pair to ensure that not out of range
    if(check_location.first > 8 || check_location.first <= 0) {
        return false;
    } else if (check_location.second > 8 || check_location.second <= 0) {
        return false;
    } else {
        return true;
    }
}

bool playerPieceCheck(Player &player, std::pair<int,int> position) {
  // Check if the position is already occupied by players' own piece
  bool player_match {false};
  for(size_t i = 0; i < player.myPieces.size(); ++i) {
    if(player.myPieces.at(i).comparePostion(position)) {
      player_match = true;
    } else {}
  }
  return player_match;
}

bool playerPieceRankUpCheck(Player &player_moving, int piece_id) {
  // Check if the piece is a king, queen, or royalty and rank up message
  // Also provide a bool return for setting the rank and set piece_type
  auto it = find_if(player_moving.myPieces.begin(), player_moving.myPieces.end(), [&piece_id](Checkers &piece) { return piece.getId() == piece_id;});
  if(player_moving.getColor() && it->positions.begin()->first == 1) {
    std::cout << player_moving.getName() << " you have promoted your " << it->getPiece_type() << "!" << std::endl;
    return true;
  } else if (!player_moving.getColor() && it->positions.begin()->first == 8) {
    std::cout << player_moving.getName() << " you have promoted your " << it->getPiece_type() << "!" << std::endl;
    return true;
  } else {
    return false;
  }
}

bool playerSingleMoveValidator(Player &player, Player &player_not_moving, const int piece_id, const std::pair<int,int> position) {
  bool valid {false};
  // Find the piece in the player's vector of pieces
  auto it = find_if(player.myPieces.begin(), player.myPieces.end(), [&piece_id](Checkers &piece) { return piece.getId() == piece_id;});
  if(!playerPieceCheck(player, position) && !playerPieceCheck(player_not_moving, position)) {
    if(player.getColor()) {
      // Player 1
      int current_row = it->positions.begin()->first;
      int current_col = it->positions.begin()->second;
      // Check rank of piece, true==royaly,king,queen
      if(!it->getRank()){
        // Below is the basic man move validation for player 1
        // Basic men can only move for player 1 (-1 row, +1 col OR -1 col) for player 2 this is in reverse
        if(current_row-1 != position.first) {
          std::cout << "You can only move one row at a time." << std::endl;
        } else if(position.second == current_col+1 || position.second == current_col-1){
          valid = true;
        } else {
          std::cout << "You can only move one column at a time." << std::endl;
        }
      } else {
        // Royaly, king, queen movement which can move 1 in any diagnol direction
        if(position.first > current_row+1 || position.first < current_row-1) {
          std::cout << "You can only move one row at a time." << std::endl;
        } else if(position.second == current_col+1 || position.second == current_col-1){
          valid = true;
        } else {
          std::cout << "You can only move one column at a time." << std::endl;
        }
      }
    } else {
      // Player 2
      int current_row = it->positions.begin()->first;
      int current_col = it->positions.begin()->second;
  
      if(!it->getRank()) {
        // Below is the basic man move validation for player 2
        if(current_row+1 != position.first) {
          std::cout << "You can only move one row at a time." << std::endl;
        } else if(position.second == current_col+1 || position.second == current_col-1){
          valid = true;
        } else {
          std::cout << "You can only move one column at a time." << std::endl;
        }
      } else {
        // Royaly, king, queen movement which can move 1 in any diagnol direction
        if(position.first > current_row+1 || position.first < current_row-1) {
          std::cout << "You can only move one row at a time." << std::endl;
        } else if(position.second == current_col+1 || position.second == current_col-1){
          valid = true;
        } else {
          std::cout << "You can only move one column at a time." << std::endl;
        }
      }
    }
  } else {
    std::cout << "You can not move to a position that is already occupied." << std::endl;
    valid = false;
  }
  return valid;  
}

bool playerJumpMoveValidator(Player &player, Player &player_not_moving, const int piece_id, const std::pair<int,int> position, const std::pair<int,int> current_position) {
  bool valid {false};
  // Find the piece in the player's vector of pieces
  auto it = find_if(player.myPieces.begin(), player.myPieces.end(), [&piece_id](Checkers &piece) { return piece.getId() == piece_id;});
  if(!playerPieceCheck(player, position) && !playerPieceCheck(player_not_moving, position)) {

    std::pair<int,int> capture_position;
    capture_position.first = (current_position.first + position.first) / 2;
    capture_position.second = (current_position.second + position.second) / 2;

    if(playerPieceCheck(player_not_moving, capture_position)) { // Check if the piece being jump is opposing player's
      if(player.getColor()) {
        // Player 1
        int current_row = current_position.first; // it->positions.begin()->first;
        int current_col = current_position.second; // it->positions.begin()->second;
        // Check rank of piece, true==royaly,king,queen
        if(!it->getRank()){
          // Below is the basic man move validation for player 1
          // Basic men can only move for player 1 (-1 row, +1 col OR -1 col) for player 2 this is in reverse
          if(current_row-2 != position.first) {
            if(!player.getCompPlayer()) { std::cout << "You can only move one row at a time." << std::endl; } else {};
          } else if(position.second == current_col+2 || position.second == current_col-2){
            valid = true;
          } else {
            if(!player.getCompPlayer()) { std::cout << "You can only move one column at a time." << std::endl; } else {};
          }
        } else {
          // Royaly, king, queen movement which can move 1 in any diagnol direction
          if(position.first > current_row+2 || position.first < current_row-2) {
            if(!player.getCompPlayer()) { std::cout << "You can only move one row at a time." << std::endl; } else {};
          } else if(position.second == current_col+2 || position.second == current_col-2){
            valid = true;
          } else {
            if(!player.getCompPlayer()) { std::cout << "You can only move one column at a time." << std::endl; } else {};
          }
        }
      } else {
        // Player 2
        int current_row = current_position.first; // it->positions.begin()->first;
        int current_col = current_position.second; // it->positions.begin()->second;
    
        if(!it->getRank()) {
          // Below is the basic man move validation for player 2
          if(current_row+2 != position.first) {
            if(!player.getCompPlayer()) { std::cout << "You can only move one row at a time." << std::endl; } else {};
          } else if(position.second == current_col+2 || position.second == current_col-2){
            valid = true;
          } else {
            if(!player.getCompPlayer()) { std::cout << "You can only move one column at a time." << std::endl; } else {};
          }
        } else {
          // Royaly, king, queen movement which can move 1 in any diagnol direction
          if(position.first > current_row+2 || position.first < current_row-2) {
            if(!player.getCompPlayer()) { std::cout << "You can only move one row at a time." << std::endl; } else {};
          } else if(position.second == current_col+2 || position.second == current_col-2){
            valid = true;
          } else {
            if(!player.getCompPlayer()) { std::cout << "You can only move one column at a time." << std::endl; } else {};
          }
        }
      }
    } else {
      if(!player.getCompPlayer()) { std::cout << "You can not jump an empty space or your own piece." << std::endl; } else {};
      valid = false;
    }
  } else {
    if(!player.getCompPlayer()) { std::cout << "You can not move to a position that is already occupied." << std::endl; } else {};
    valid = false;
  }
  return valid;  
}

bool playerMovementValidation(Player &player_moving, Player &player_not_moving, const int piece_id, const std::pair<int,int> position, const std::pair<int,int> current_position) {
  // Combines the single and jump move validators into one validation function
  if(position.first == current_position.first+2 || position.first == current_position.first-2) {
    return playerJumpMoveValidator(player_moving, player_not_moving, piece_id, position, current_position);
  } else {
    return playerSingleMoveValidator(player_moving, player_not_moving, piece_id, position);
  }
}

void playerMove(Player &player, int piece_id, std::pair<int,int> position) {
  // Find the piece in the player's vector of pieces
  auto it = find_if(player.myPieces.begin(), player.myPieces.end(), [&piece_id](Checkers &piece) { return piece.getId() == piece_id;});
  // Move the piece to the new position
  it->positions.insert(it->positions.begin(), position);
  // Check to see if piece can be kinged and set the rank if possible
  if(!it->getRank()) {
    it->setRank(playerPieceRankUpCheck(player, it->getId()));
    if(it->getRank()){
      it->setPiece_type(player.getGender());
    }
  }
}

void playerJumpMove(Player &player, Player &player_captured, int piece_id, std::pair<int,int> position, std::pair<int,int> current_position) {
  // Find the piece in the player's vector of pieces
  auto it = find_if(player.myPieces.begin(), player.myPieces.end(), [&piece_id](Checkers &piece) { return piece.getId() == piece_id;});
  // Create a pair to hold the position of the piece that is being captured
  std::pair<int,int> capture_position;
  capture_position.first = (current_position.first + position.first) / 2;
  capture_position.second = (current_position.second + position.second) / 2;
  // Find the piece in the player_captured vector of pieces
  int cap_piece_id;    
  for(size_t i = 0; i < player_captured.myPieces.size(); ++i) {
    if(player_captured.myPieces.at(i).comparePostion(capture_position)) {
      cap_piece_id = player_captured.myPieces.at(i).getId();
    } else {}
  }
  // Move the piece to the new position
  it->positions.insert(it->positions.begin(), position);
  // Check to see if piece can be kinged and set the rank if possible
  if(!it->getRank()) {
    it->setRank(playerPieceRankUpCheck(player, it->getId()));
    if(it->getRank()){
      it->setPiece_type(player.getGender());
    }
  }
  // Remove the captured piece from the player_captured vector of pieces
  auto it2 = find_if(player_captured.myPieces.begin(), player_captured.myPieces.end(), [&cap_piece_id](Checkers &piece) { return piece.getId() == cap_piece_id;});
  player_captured.myPieces.erase(it2);
  std::cout << player.getName() << " you have successfully captured " << player_captured.getName() << "'s " << it2->getPiece_type() << "." << std::endl;
}

struct Move playerPositionPromptValidation(Player &player_moving, Player &player_not_moving, Board &board) {
  std::string input_piece_id {"0"};
  std::string input_new_row {"0"};
  std::string input_new_col {"0"};    
  int piece_id {0};
  int new_row {0};
  int new_col {0};
  Move move{std::pair<int, int> {new_row, new_col},piece_id};
  // Validation for movement to a valid position on each turn
  bool valid_position {false};
  while(!valid_position) {
    piece_id = 0;
    new_row = 0;
    new_col = 0;
    
    std::cout << "\n---------------------------------------------------------------\n" << std::endl;
    std::cout << player_moving.getName() << " it is your turn. (Type 9 at any time for the menu.) \n" << std::endl;
    if(player_moving.getColor()) {
        std::cout << "Player 1 your pieces are 10-21 for ids." << std::endl;
    } else {
        std::cout << "Player 2 your pieces are 30-41 for ids." << std::endl;
    }
    std::cout << player_moving.getName() << " enter piece number you want to move: ";
    std::getline(std::cin, input_piece_id);
    // Reduce the royal/kinged/queened piece ids down from hundreds from user input
    piece_id = validate_num(8, 410, input_piece_id);
    if(piece_id > 99) {
        piece_id /= 10;
    } else {}
    
    // Testing for player1
    Player player1{""};
    Player player2{""};
    if(!player_moving.getColor()) {
      player2 = player_moving;
      player1 = player_not_moving;
    } else {
      player2 = player_not_moving;
      player1 = player_moving;
    }
    // Logic for pausing the game
    if(piece_id == 9) {
      bool player_choice = renderPauseMenuOperation(player1, player2, player_moving, board);
      while(!player_choice) {
        player_choice = renderPauseMenuOperation(player1, player2, player_moving, board);
      }
      valid_position = false;
    } else {
      std::cout << player_moving.getName() << " enter row you want to move to: ";
      std::getline(std::cin, input_new_row);
      new_row = validate_num(0, 10, input_new_row);
      if(new_row == 9) {
        bool player_choice = renderPauseMenuOperation(player1, player2, player_moving, board);
        while(!player_choice) {
          player_choice = renderPauseMenuOperation(player1, player2, player_moving, board);
        }
        valid_position = false;
      } else {
        std::cout << player_moving.getName() << " enter column you want to move to: ";
        std::getline(std::cin, input_new_col);
        new_col = validate_num(0, 10, input_new_col);
        if(new_col == 9) {
          bool player_choice = renderPauseMenuOperation(player1, player2, player_moving, board);
          while(!player_choice) {
              player_choice = renderPauseMenuOperation(player1, player2, player_moving, board);
          }   
          valid_position = false;
        } else {
          auto position = std::make_pair(new_row,new_col);
  
          // Input validation for inputs on position from user
          if(position.first > 8 || position.first < 1 || position.second > 8 || position.second < 1) {
            std::cout << "Your move is not within the grid." << std::endl;
            std::cout << "Try again!";
            valid_position = false;
          } else if ((player_moving.getColor() && piece_id > 21) || (player_moving.getColor() && piece_id < 10)) {
            std::cout << "That is not your piece." << std::endl;
            std::cout << "Try again!";
            valid_position = false;
          } else if ((!player_moving.getColor() && piece_id > 41) || (!player_moving.getColor() && piece_id < 30)) {
            std::cout << "That is not your piece." << std::endl;
            std::cout << "Try again!";
            valid_position = false;
          } else {
            move.piece_id = piece_id;
            move.position = position;
            valid_position = true;
          }
        }
      }
    }
  }
  return move;
}

void gameReplay(Board &board) {
  // Print the board state to the console in a loop with a delay of 1 second between each state
  for(auto board_state : board.board_vec){
    std::cout << system("cls") << " Game Replay 0" << std::endl;
    for(int k = 0; k < 9; ++k) {
      if(k == 0) {
        std::cout << std::setw(1) << k << "|";
      } else {
        std::cout << std::setw(3) << k << "|";
      }
    }
    std::cout << std::endl;
    for(int i = 0; i < 8; ++i) {
      std::cout << i+1 << "|";
      for(int j = 0; j < 8; ++j) {
        if(board_state[i][j] == 0) {
          std::cout << std::setw(3) << "  " << "|";
        } else {
          std::cout << std::setw(3) << board_state[i][j] << "|";
        }
      }
      std::cout << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

int rand_num(int min, int max) {
    // Make rand num generator
    std::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void comp_check_smove_location(Player &player_human, Player &player_comp, Checkers &piece, std::vector<Node> &possible_moves, std::pair<int,int> &current_position, int shift_row, int shift_col) {
     std::pair<int,int> check_location {current_position.first+shift_row,current_position.second+shift_col};
     // Checks single move to see if it's occupied and if not then log the move into Node
     if(playerPieceCheck(player_comp, check_location)) {
       return;
     } else if(playerPieceCheck(player_human, check_location)) {
       return;
     } else {
       int current_piece {piece.getId()};
       auto it = std::find_if(possible_moves.begin(), possible_moves.end(), [&current_piece](Node &node) {return node.piece_id == current_piece;});
       if(it != possible_moves.end() && isOnBoard(check_location)) {
         it->single_moves.push_back(check_location);
       } else {
         Node my_node{current_piece};
         if(playerSingleMoveValidator(player_comp, player_human, piece.getId(), check_location) && isOnBoard(check_location)) {
           my_node.single_moves.push_back(check_location);
           possible_moves.push_back(my_node);
         }
       }
       return;
     }
}

void comp_check_jmove_location(Player &player_human, Player &player_comp, Checkers &piece, std::vector<Node> &possible_moves, std::pair<int,int> &current_position, int shift_row, int shift_col) {
     std::pair<int,int> check_location {current_position.first+shift_row,current_position.second+shift_col};
     std::pair<int,int> jump_to_position {current_position.first+shift_row+shift_row,current_position.second+shift_col+shift_col};
     if(playerPieceCheck(player_comp, check_location)) {
       return;
     } else if(playerPieceCheck(player_human, check_location)) {
       // Find the Node associated with the piece ID
       int current_piece = piece.getId();
       auto it = std::find_if(possible_moves.begin(), possible_moves.end(), [&current_piece](Node &node) {return node.piece_id == current_piece;});
       // If found then find a jump path vec in jumps vec that has last position at the current position
       if(it != possible_moves.end()) {
        auto it2 = std::find_if(it->jump_moves.begin(), it->jump_moves.end(), [&current_position](std::vector<std::pair<int,int>> jump_path) {return (jump_path[jump_path.size()-1]) == current_position;});
        if(it2 != it->jump_moves.end()) {
          if(playerJumpMoveValidator(player_comp, player_human, current_piece, jump_to_position, current_position) && isOnBoard(jump_to_position)) {
            // Copy the vector path of jumps in it2 to new vec of jumps
            std::vector<std::pair<int,int>> temp_vec {};
            for(auto pair: *it2) {
                temp_vec.push_back(pair);
            }
            // Add new valid jump to temp vec and push is to the jumps vec of vecs
            // If a longer temp_vec, check the last "virtual" position to test if it is next jump_position
            // This test will prevent the backtracking along a possible double or triple jump
            // Thus averting an infinite loop of jumps between original position and 2nd/3rd jumps end position
            if(temp_vec.size() > 1 && temp_vec.at(temp_vec.size()-2) == jump_to_position) {
                return;
            } else {
                temp_vec.push_back(jump_to_position);
                it->jump_moves.push_back(temp_vec);
            }           
          } else { return; }
        } else {
          // If no vec in jumps vec found with last current position then make new
          if(playerJumpMoveValidator(player_comp, player_human, current_piece, jump_to_position, current_position) && isOnBoard(jump_to_position)) {
           std::vector<std::pair<int,int>> new_jump_path {jump_to_position};
           it->jump_moves.push_back(new_jump_path);
          } else { return; }
        }
       } else {
         // If no Node found with corresponding piece ID then make new and add jump path vec to jumps vec
         if(playerJumpMoveValidator(player_comp, player_human, current_piece, jump_to_position, current_position) && isOnBoard(jump_to_position)) {
           Node my_node{current_piece};
           std::vector<std::pair<int,int>> jump_path {jump_to_position};
           my_node.jump_moves.push_back(jump_path);
           possible_moves.push_back(my_node);
         }
       }
       return;
     } else {
       return;
     }
}

void single_move_look_around(Player &player_human, Player &player_comp, Checkers &piece, std::vector<Node> &possible_moves, std::pair<int,int> &current_position) {
    uint8_t possible_move_count{2};
    if(piece.getRank()) {
      possible_move_count = 4;  
    }
    // After using the rank to determine number of moves work around to find next single move
    while(possible_move_count > 0) {
        switch(possible_move_count) {
          case 1: {
             comp_check_smove_location(player_human, player_comp, piece, possible_moves, current_position, 1, 1);
             --possible_move_count;
             break;
          }
          case 2: {
             comp_check_smove_location(player_human, player_comp, piece, possible_moves, current_position, 1, -1);
             --possible_move_count;
             break;
          }
          case 3: {
             comp_check_smove_location(player_human, player_comp, piece, possible_moves, current_position, -1, -1);
             --possible_move_count;
             break;
          }
          case 4: {
             comp_check_smove_location(player_human, player_comp, piece, possible_moves, current_position, -1, 1);
             --possible_move_count;
             break; 
          }
        }
    }
}

void jump_move_look_around(Player &player_human, Player &player_comp, Checkers &piece, std::vector<Node> &possible_moves, std::pair<int,int> &current_position) {
    uint8_t possible_move_count{2};
    int shift_row {0};
    int shift_column {0};
    if(piece.getRank()) {
      possible_move_count = 4;  
    }
    // After using the rank to determine number of moves work from highest to lowest to find jumps
    while(possible_move_count > 0) {
        switch(possible_move_count) {
          case 1: {
            // Numbers for shift to look at are in reverse for player 1 which is color=true
            if(player_comp.getColor()) { shift_row = -1; shift_column = -1;} else { shift_row = 1; shift_column = 1; }
            comp_check_jmove_location(player_human, player_comp, piece, possible_moves, current_position, shift_row, shift_column);
            --possible_move_count;
            break;
          }
          case 2: {
            if(player_comp.getColor()) { shift_row = -1; shift_column = 1;} else { shift_row = 1; shift_column = -1; }
            comp_check_jmove_location(player_human, player_comp, piece, possible_moves, current_position, shift_row, shift_column);
            --possible_move_count;
            break;
          }
          case 3: {
            if(player_comp.getColor()) { shift_row = 1; shift_column = 1;} else { shift_row = -1; shift_column = -1; }
            comp_check_jmove_location(player_human, player_comp, piece, possible_moves, current_position, shift_row, shift_column);
            --possible_move_count;
            break;
          }
          case 4: {
            if(player_comp.getColor()) { shift_row = 1; shift_column = -1;} else { shift_row = -1; shift_column = 1; }
            comp_check_jmove_location(player_human, player_comp, piece, possible_moves, current_position, shift_row, shift_column);
            --possible_move_count;
            break;
          }
        }
    }
}

bool computerTurn(Player &player_human, Player &player_comp, Board &board) {
    std::vector<Node> possible_moves {};
    std::pair<int,int> current_position {};
    int current_piece_id {};
    
    for(auto &piece: player_comp.myPieces) {
        // Check piece rank to set direction move counter 2 for men, 4 for king
        current_position.first = piece.getPositions().begin()->first;
        current_position.second = piece.getPositions().begin()->second;
        current_piece_id = piece.getId();
        
        // Single move look around
        single_move_look_around(player_human, player_comp, piece, possible_moves, current_position);
        // Jump move look around
        jump_move_look_around(player_human, player_comp, piece, possible_moves, current_position);
        // Set the current location to the last item on the first vector in the vec of jumps
        // Repeat until there is no more vecs in the vec of jumps
        auto it0 = std::find_if(possible_moves.begin(), possible_moves.end(), [&current_piece_id](Node &node) {return node.piece_id == current_piece_id;});
        if(it0 != possible_moves.end() && !it0->jump_moves.empty()) {

          for(size_t i=0; i < it0->jump_moves.size(); ++i) {
            int vec_end = it0->jump_moves[i].size()-1;
            current_position.first = it0->jump_moves.at(i).at(vec_end).first;
            current_position.second = it0->jump_moves.at(i).at(vec_end).second;
            jump_move_look_around(player_human, player_comp, piece, possible_moves, current_position);
          }
        }
    }
        
    // Implementation to randomly pick out the best move based on longest jump path and random single move.
    std::vector<Node> best_moves {};
    for(auto node: possible_moves) {
        int node_id = node.piece_id;
        // JUMP MOVE locator and building of best_moves vec of nodes
        // it4 finds the node id in best_moves that matches the node in possible_moves
        auto it4 = std::find_if(best_moves.begin(), best_moves.end(), [&node_id](Node nodeX){ return nodeX.piece_id == node_id;});
        // IF node in possible_moves is empty of jump_moves skip to SINGLE MOVE else cont
        if(!node.jump_moves.empty()) {
            for(auto vec: node.jump_moves) {
                // Looping through to find the longest jump move to record to best_moves node
                if(it4 != best_moves.end() && vec.size() > it4->jump_moves.begin()->size()) {
                    it4->jump_moves.clear();
                    it4->jump_moves.push_back(vec);
                } else if (it4 == best_moves.end()) {
                    Node temp {};
                    temp.piece_id = node_id;
                    temp.jump_moves.push_back(vec);
                    best_moves.push_back(temp);
                } else {}
            }
        }
        // SINGLE MOVE locator and building of best_moves vec of nodes
        // it5 finds the node id in best_moves that matches the node in possible_moves
        auto it5 = std::find_if(best_moves.begin(), best_moves.end(), [&node_id](Node nodeX){ return nodeX.piece_id == node_id;});
        // IF node in possible_moves is empty of single_moves skip to for loop end else cont
        if(!node.single_moves.empty()) {
            // Randomly select a single move from the node for best_moves vec
            int amount_of_single_moves = node.single_moves.size()-1;
            int comp_modifier = rand_num(0, amount_of_single_moves);
            if(it5 != best_moves.end()) {
                it5->single_moves.push_back(node.single_moves.at(comp_modifier));
            } else if (it5 == best_moves.end()) {
                Node temp {};
                temp.piece_id = node_id;
                temp.single_moves.push_back(node.single_moves.at(comp_modifier));
                best_moves.push_back(temp);
            } else {}
        }
    }
    
    // Pull out Jump Moves any available into a priority_moves vec for using first choice to move
    std::vector<Node> priority_moves {};
    for(auto node_best: best_moves) {
        if(!node_best.jump_moves.empty()) {
            Node temp {};
            temp.piece_id = node_best.piece_id;
            for(auto vec: node_best.jump_moves) {
                std::vector<std::pair<int,int>> temp_vec {};
                for(auto int_pair: vec) {
                    temp_vec.push_back(int_pair);
                }
                temp.jump_moves.push_back(temp_vec);
            }
            priority_moves.push_back(temp);
        } else {}
    }
    
    if(!priority_moves.empty()) {
        // Using proirity_moves vector run a rand_num to chose one and excecute the move
        int priority_moves_size = priority_moves.size()-1;
        int comp_selector_num = rand_num(0, priority_moves_size);
        int comp_chosen_piece_id = priority_moves.at(comp_selector_num).piece_id;
    
        auto it6 = std::find_if(priority_moves.begin(), priority_moves.end(), [&comp_chosen_piece_id](Node move_node){ return move_node.piece_id == comp_chosen_piece_id;});
        auto it7 = std::find_if(player_comp.myPieces.begin(), player_comp.myPieces.end(), [&comp_chosen_piece_id](Checkers move_piece){ return move_piece.getId() == comp_chosen_piece_id;});
            // Implement jump moves in loop
            for(auto move: it6->jump_moves.at(0)) {
                // Set current position
                std::pair<int, int> curr_pos {0,0};
                curr_pos.first = it7->positions.begin()->first;
                curr_pos.second = it7->positions.begin()->second;
                // Execute jump move for move in jump_moves
                playerJumpMove(player_comp, player_human, comp_chosen_piece_id, move, curr_pos);
                // User facing message
                std::cout << "\n" << player_comp.getName() << "'s piece " << comp_chosen_piece_id << " has successfully captured " << player_human.getName() << "'s piece."<< std::endl;
            }
    } else {
        // Using best_moves vector run a rand_num to chose one and excecute the move
        int best_moves_size = best_moves.size()-1;
        int comp_selector_num = rand_num(0, best_moves_size);
        int comp_chosen_piece_id = best_moves.at(comp_selector_num).piece_id;
        
        auto it8 = std::find_if(best_moves.begin(), best_moves.end(), [&comp_chosen_piece_id](Node move_node){ return move_node.piece_id == comp_chosen_piece_id;});
        std::pair<int, int> single_move_location {0,0};
        single_move_location.first = it8->single_moves.begin()->first;
        single_move_location.second = it8->single_moves.begin()->second;
        //Execute the single move chosen in single_moves
        playerMove(player_comp, comp_chosen_piece_id, single_move_location);
        // User facing message
        std::cout << "\n" << player_comp.getName() << "'s piece " << comp_chosen_piece_id << " has successfully moved to "
            << single_move_location.first << "," << single_move_location.second << std::endl;
    }
    return true;
}

bool playerAdditionalJumpTurn(Player &player_not_moving, Player &player_moving, int piece_id) {
    // Finding the moving players piece for getting the new current location and checker piece
    auto it9 = std::find_if(player_moving.myPieces.begin(), player_moving.myPieces.end(), [&piece_id](Checkers pieces){return pieces.getId() == piece_id;});
    std::vector<Node> possible_moves {};
    Checkers &chosen_checker = *it9;
    std::pair<int,int> current_position {it9->positions.begin()->first,it9->positions.begin()->second};
    // Jump move look around for finding additional jumps
    jump_move_look_around(player_not_moving, player_moving, chosen_checker, possible_moves, current_position);
    
    // Check the new possible_moves for any jump moves and present to user
    if(possible_moves.empty()) {
        return false;
    } else {
        int increment {1};
        std::cout << std::endl;
        std::cout << player_moving.getName() << " you can make new additional jump moves below:" << std::endl;
        for(auto vec: possible_moves.at(0).jump_moves) {
            std::cout << "- Option " << increment << " - " << vec.at(0).first << "," << vec.at(0).second << std::endl;
            ++increment;
        }
    }
    
    // User selects next jump move and the move is executed
    std::string user_choice {"0"};
    int validated_user_choice{0};
    bool valid_choice {false};
    while(!valid_choice) {
        std::cout << "Please choose your next jump move by typing the option number: ";
        std::getline(std::cin, user_choice);
        validated_user_choice = validate_num(0, possible_moves.at(0).jump_moves.size()+1, user_choice);
        --validated_user_choice;
        int j_moves_size = possible_moves.at(0).jump_moves.size()-1;
        if(validated_user_choice < 0 || validated_user_choice > j_moves_size) {
            valid_choice = false;
        } else {
            valid_choice = true;
        }
    }
    std::pair<int,int> jump_move {};
    jump_move.first = possible_moves.at(0).jump_moves.at(validated_user_choice).begin()->first;
    jump_move.second = possible_moves.at(0).jump_moves.at(validated_user_choice).begin()->second;
    playerJumpMove(player_moving, player_not_moving, piece_id, jump_move, current_position);
    return true;
}

void playerTurnPrompt(Player &player_moving, Player &player_not_moving, Board &board) {
  bool successful_move {false};
  while(!successful_move) {
    if(player_moving.getCompPlayer()){
        std::cout << player_moving.getName() << "'s turn." << std::endl;
        successful_move = computerTurn(player_not_moving, player_moving, board);
    } else {
        // Prompt the player for their move
        Move move = playerPositionPromptValidation(player_moving, player_not_moving, board);
        std::pair<int,int> position = move.position;
        int piece_id = move.piece_id;
      
        // Establish the current position of the piece
        std::pair<int,int> current_pos;
        auto it_moving = find_if(player_moving.myPieces.begin(), player_moving.myPieces.end(), [&piece_id](Checkers &piece){ return piece.getId() == piece_id;});
        current_pos.first = it_moving->positions.begin()->first;
        current_pos.second = it_moving->positions.begin()->second;
        
        // Validate the player's move against validators for single and jump moves else prompt again
        bool validate = playerMovementValidation(player_moving, player_not_moving, piece_id, position, current_pos);
        if((position.first == current_pos.first+2 || position.first == current_pos.first-2) && validate) {
          playerJumpMove(player_moving, player_not_moving, piece_id, position, current_pos);
          bool additional_jump {true};
          while(additional_jump) {
            additional_jump = playerAdditionalJumpTurn(player_not_moving, player_moving, piece_id);
          }
          successful_move = true;
          std::cout << player_moving.getName() << " you have successfully jumped with your " << it_moving->piece_type << " to a new position." << std::endl;
        } else if (validate) {
          playerMove(player_moving, piece_id, position);
          successful_move = true;
          std::cout << player_moving.getName() << " you have successfully moved your " << it_moving->piece_type << " to a new position." << std::endl;
        } else {
          std::cout << "Try again!\n";
          successful_move = false;
        }
    }
  }
  std::cout << "\n---------------------------------------------------------------\n" << std::endl;
}

bool winConditionCheck (Player &player1, Player &player2, Board &board) {
    
    bool p1_win {false};
    bool p2_win {false};
    // Win conditions test for p1, p2, or draw wins
    if(player1.myPieces.empty() && !player2.myPieces.empty()) {
        p2_win = true;
    } else if (player2.myPieces.empty() && !player1.myPieces.empty()) {
        p1_win = true;
    } else if (player1.myPieces.size() == 1 && player2.myPieces.size() == 1) {
    } else {
        return false;
    }
    
    // Adds the new LeaderBoard record
    if(p1_win) {
        std::cout << player1.getName() << " Won the Game!!" << std::endl;
        addLeaderBoardRecord(player1);
    } else if (p2_win) {
        std::cout << player2.getName() << " Won the Game!!" << std::endl;
        addLeaderBoardRecord(player2);
    } else {
        std::cout << "It's a draw! No winners and no losers." << std::endl;
    }
    // Adds the new LossLeaderBoard record
    if(p2_win){
        int loss_total = player1.getLosses();
        player1.setLosses(loss_total + 1);
        addLosingBoardRecord(player1);
    } else if (p1_win) {
        int loss_total = player2.getLosses();
        player2.setLosses(loss_total + 1);
        addLosingBoardRecord(player2);
    } else {}

    // Prompts user for game replay or skip and if 'y' replays
    std::string replay_choice {""};
    std::cout << "Game Over. Watch the replay? Y/N: ";
    std::getline(std::cin, replay_choice);
    replay_choice = clean_string(replay_choice);

    while(replay_choice.empty()) {
        std::cout << "Game Over. Watch the replay? Y/N: ";
        std::getline(std::cin, replay_choice);
        replay_choice = clean_string(replay_choice);
    }

    if(replay_choice.at(0) == 'y'){
        gameReplay(board);
    } else {
        std::cout << "Game replay skipped" << std::endl;
    }
    return true;
}
