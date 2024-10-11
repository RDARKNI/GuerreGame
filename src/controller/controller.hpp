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
  go_back,
};

/*
Handles User Input and navigation between different Menu States.
*/
inline const char *turn_string[1]{"Turn %3d"};

constexpr inline std::array<const char *const, 6> piece_attrs{
    "HP    %d/%d", "FAT   %d", "DEPL  %d", "PUIS  %d", "RANGE %d", "PROD  %d",
};
#define ystart 3
#define xstart 20
#define igwin_w 20
#define infowin_h 7
#define board_yoffs 1
#define board_xoffs 26

class Controller {
 public:
  Controller(const std::vector<UserInput> &prev_history, Role role);
  Controller(const Controller &other) = delete;
  Controller &operator=(const Controller &other) = delete;
  Controller(Controller &&other) = delete;
  Controller &operator=(Controller &&other) = default;
  std::vector<UserInput> game_loop();

 private:
  DataHandler dh;
  std::vector<UserInput> history;
  UserInput inputs;
  coordval B_H;
  coordval B_W;
  Role role;
  int player_count;
  int player_index;
  BoardWindow boardwin{{Coord(B_H + 2, B_W + 2)},
                       {Coord(yoffs + board_yoffs, xoffs + board_xoffs)},
                       dh.get_curr_targets(),
                       dh.get_board(),
                       dh.get_curr_player_data().y};
  TurnInfoWindow turnwin{{1, igwin_w},
                         boardwin,
                         rel_pos::right,
                         turn_string,
                         &dh.get_curr_turn(),
                         1,
                         1};
  PlayerInfoWindow playerwin{{2, igwin_w},
                             turnwin,
                             rel_pos::below,
                             player_names,
                             &dh.get_curr_player_data(),
                             1};
  PieceInfoWindow sourcewin{{infowin_h, igwin_w},  playerwin,
                            rel_pos::below,        piece_attrs,
                            &dh.get_curr_source(), 1};
  PieceInfoWindow targetwin{{infowin_h, igwin_w},
                            sourcewin,
                            rel_pos::right,
                            piece_attrs,
                            &dh.get_curr_target()};
  ActionWindow actionwin{{5, igwin_w},
                         sourcewin,
                         rel_pos::below,
                         action_names,
                         &(dh.get_curr_actions()),
                         1};
  OptionWindow optionwin{{5, igwin_w},
                         sourcewin,
                         rel_pos::below,
                         option_names,
                         &(dh.get_curr_options()),
                         1};
  std::stack<MenuState> menu_stack;
  bool can_act;
  void init_random_overworld(unsigned int forest_gen, unsigned int gold_gen);
  int send_command();
  void redraw_boardwin();
  MenuAction menu_handler();
  MenuAction start_selection();
  MenuAction start_placement();
  MenuAction src_selection();
  MenuAction act_selection();
  MenuAction opt_selection();
  MenuAction dst_selection();
  ParsedInput conditional_input();
  int mov_action(UserInput inp);
  MenuAction eval_rcvd();
};
#undef ystart
#endif
