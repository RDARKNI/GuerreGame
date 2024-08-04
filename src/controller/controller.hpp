#ifndef CONTROL_HPP
#define CONTROL_HPP
#include <stack>

#include "../helpers/user_input.hpp"
#include "../input_parser.hpp"
#include "cursewindow.hpp"
#include "data_handler.hpp"
#include "network_helper.hpp"
enum MenuID {
  START_UNIT_SELECT,
  START_UNIT_PLACEMENT,
  ACTEUR_SEL,
  ACTION_SEL,
  OPTION_SEL,
  TARGET_SEL,
  QUIT_GAME,
};

#define ystart 20

/*
Handles User Input and navigation between different Menu States.
*/
class Controller {
 public:
  Controller(const char* other);
  void game_loop();

 private:
  UserInput inputs;
  DataHandler dh{inputs};
  BoardWindow boardwin{dh, {3, ystart}};
  InfoWindow sourcewin{dh, {9, ystart + (int)B_W + 4}, inputs.acteur};
  InfoWindow targetwin{dh, {9, ystart + (int)B_W + 25}, inputs.target};
  PlayerWindow playerwin{dh, {6, ystart + (int)B_W + 4}};
  ActionWindow actionwin{dh, {17, ystart + (int)B_W + 4}, inputs.action};
  OptionWindow optionwin{dh, {17, ystart + (int)B_W + 4}, inputs.option};
  TurnWindow turnwin{dh, {4, ystart + (int)B_W + 4}};
  std::stack<MenuID> menu_stack;
  int c{0};
  void load_save(const std::string& filename);

  void init_random_overworld();
  void start_unit_selection();
  void start_unit_placement();
  void square_selection();
  void action_selection();
  void option_selection();
  void target_selection();

  // to transmit actual changes in the game state
  void send_action();
  void end_turn();
  int quit();
};
#undef ystart
#endif
