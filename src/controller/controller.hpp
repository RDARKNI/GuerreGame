#ifndef CONTROL_HPP
#define CONTROL_HPP
#include <stack>

#include "../input/input_parser.hpp"
#include "../ui/cursewindow.hpp"
#include "data_handler.hpp"
enum class MenuState {
  start_sel,
  start_pla,
  src_sel,
  act_sel,
  opt_sel,
  dst_sel,
};
enum class MenuAction {
  reg,
  new_turn,
  do_action,
  create_unit,
  save_and_quit,
  recd_input,
  go_back,
};

/*
Handles User Input and navigation between different Menu States.
*/
class Controller {
 public:
  Controller(const std::vector<UserInput>& prev_history, Role role);

  std::vector<UserInput> game_loop();

 private:
  DataHandler dh;
  std::vector<UserInput> history;
  UserInput inputs;
  char B_H;
  char B_W;
  Role role;
  int player_count;
  int player_index;

/*
Boardwin starts at {3,20}
Sourcewin at {B_W+4}

*/
#define ystart 3
#define xstart 20
#define igwin_w 20
#define infowin_h 7

  BoardWindow boardwin{{char(B_H + 2), char(B_W + 2)},
                       {char(ystart), char(xstart + xo)},
                       dh.get_curr_targets()};
  InfoWindow<int> turnwin{{1, igwin_w},       boardwin, rel_pos::right,
                          dh.get_curr_turn(), 1,        1};
  InfoWindow<Coord> playerwin{
      {2, igwin_w}, turnwin, rel_pos::below, dh.get_curr_player_data(), 1};
  InfoWindow<SquareData> sourcewin{
      {infowin_h, igwin_w}, playerwin, rel_pos::below, dh.get_curr_source(), 1};
  InfoWindow<SquareData> targetwin{
      {infowin_h, igwin_w}, sourcewin, rel_pos::right, dh.get_curr_target()};

  MenuWindow<std::vector<Coord>, action_names.size()> actionwin{
      {5, igwin_w},          sourcewin, rel_pos::below, action_names,
      dh.get_curr_actions(), 1};
  MenuWindow<std::vector<Coord>, option_names.size()> optionwin{
      {5, igwin_w},          sourcewin, rel_pos::below, option_names,
      dh.get_curr_options(), 1};
  std::stack<MenuState> menu_stack;
  bool can_act;
  void init_random_overworld(unsigned int forest_gen, unsigned int gold_gen);
  MenuAction menu_handler();
  MenuAction start_selection();
  MenuAction start_placement();
  MenuAction src_selection();
  MenuAction act_selection();
  MenuAction opt_selection();
  MenuAction dst_selection();
  void redraw_boardwin();
  // to transmit actual changes in the game state
  int send_command();
  ParsedInput conditional_input();
  int mov_action(UserInput inp);
  MenuAction eval_rcvd();
};
#undef ystart
#endif
