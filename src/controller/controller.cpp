#include "controller.hpp"

#include <random>
#include <utility>

#include "../network/network_helper.hpp"
using enum ParsedInput;

MenuAction Controller::eval_rcvd() {
  switch (inputs.act) {
    case END_TURN:
      return MenuAction::new_turn;
    case CREATE_UNIT:
      return MenuAction::create_unit;
    default:
      return MenuAction::do_action;
  }
}
/*
We always have a history
- If we don't load anything, it's just the init settings
- If we do, then it's loaded normally
In either case, the client needs to receive the history before Controller
In connection, we always just send the settings (both put it in history)
Then in controller, the Host goes through history (except 0th element) and plays
out each move This copies history
*/

Controller::Controller(const std::vector<UserInput>& prev_history, Role role)
    : role{role},
      dh{prev_history[0]},
      B_H{prev_history[0].src.y},
      B_W{prev_history[0].src.x},
      player_count{prev_history[0].dst.y},
      player_index{prev_history[0].dst.x} {
  history = {prev_history[0]};
  UserInput init_opts{prev_history[0]};
  if (role == LOCAL || role == HOST) {
    if (prev_history.size() == 1) {
      init_random_overworld(prev_history[0].act, goldgeneration);
    } else {
      for (size_t i{1}; i < prev_history.size(); ++i) {
        inputs = prev_history[i];
        dh.send_command(inputs);
        history.push_back(inputs);
        if (role == HOST) {
          broadcast_inputs(inputs);
        }
      }
    }
    broadcast_inputs({{-1, -1}, -1, -1, {-1, -1}});
  } else {
    while (!receive_inputs(0, inputs)) {
      if (inputs.dst.y == -1) {
        break;
      }
      dh.send_command(inputs);
      history.push_back(inputs);
    }
  }
  dh.refresh_player_data();  // all types of actions could change player data
  playerwin.show();
  dh.refresh_curr_turn();

  turnwin.show();
  redraw_boardwin();
  inputs = {};
  if (dh.get_curr_turn()) {
    menu_stack = std::stack<MenuState>{{MenuState::src_sel}};
  } else {
    menu_stack = std::stack<MenuState>{{MenuState::start_sel}};
  }
  can_act = role == LOCAL || dh.get_curr_player_data().y == player_index;
}

// std::mt19937 gen(seed ? std::mt19937{seed}
//                       : std::mt19937{std::random_device{}()});
void Controller::init_random_overworld(unsigned int forest_gen,
                                       unsigned int gold_gen) {
  std::bernoulli_distribution forest_dist(forest_gen / 100.0f);
  std::bernoulli_distribution gold_dist(gold_gen);
  std::mt19937 gen{std::random_device{}()};
  inputs = {{0, 0}, CREATE_UNIT, 0, {0, 0}};
  Coord target{0, 0};
  for (target.y = 0; target.y < B_H; ++target.y) {
    for (target.x = 0; target.x < B_W; ++target.x) {
      if (forest_dist(gen)) {
        inputs.act = CREATE_UNIT;
        inputs.opt = CREATE_FORET;
        inputs.dst = target;
        send_command();
      } else if (gold_dist(gen)) {
        inputs.act = CREATE_UNIT;
        inputs.opt = CREATE_MINE;
        inputs.dst = target;
        send_command();
      }
    }
  }
  inputs = {{0, 0}, END_TURN, 0, {0, 0}};  // after init we always end the turn
  send_command();
}

int Controller::mov_action(UserInput inp) {
  if (dh.get_curr_actions()[inp.act].y == MOVE) {
    dh.select_source(inp.dst);
    for (const auto& act : dh.get_curr_actions()) {
      if (act.x) {
        return 1;
      }
    }
  }
  return 0;
}

std::vector<UserInput> Controller::game_loop() {
  while (true) {
    switch (menu_handler()) {
      case MenuAction::new_turn: {
        inputs = {{0, 0}, END_TURN, 0, {0, 0}};
        send_command();
        dh.refresh_curr_turn();
        turnwin.show();
        redraw_boardwin();
        menu_stack = std::stack<MenuState>{{MenuState::src_sel}};
        break;
      }
      case MenuAction::do_action: {
        UserInput inp = inputs;
        boardwin.unmark_squares();
        if (dh.get_curr_actions()[inp.act].y == ATTACK) {
          flash_screen();
        } else if (dh.get_curr_actions()[inp.act].y == CONVERT) {
          convert_anim(dh.get_curr_player_data().y);
        }
        switch (send_command()) {
          case 2: {
            winscr(dh.get_curr_player_data().y);
            history.pop_back();
            return history;
          }
          case 1: {
            redraw_boardwin();
            menu_stack = std::stack<MenuState>{{MenuState::src_sel}};
            break;
          }
          case 0: {
            boardwin.redraw_square(inp.src, dh.get_square_data(inp.src));
            boardwin.redraw_square(inp.dst, dh.get_square_data(inp.dst));
            if (!mov_action(inp)) {
              menu_stack = std::stack<MenuState>{{MenuState::src_sel}};
            } else {
              inputs = {inp.dst, 0, 0, {0, 0}};
              menu_stack = std::stack<MenuState>{
                  {MenuState::src_sel, MenuState::act_sel}};
            }
            break;
          }
        }
        break;
      }
      case MenuAction::save_and_quit: {
        return history;
      }
      case MenuAction::create_unit: {
        inputs.act = CREATE_UNIT;
        inputs.src.y = dh.get_curr_player_data().y;
        UserInput inp = inputs;
        send_command();
        boardwin.redraw_square(inp.dst, dh.get_square_data(inp.dst));
        menu_stack = std::stack<MenuState>{{MenuState::start_sel}};
        dh.refresh_curr_turn();
        break;
      }
      case MenuAction::go_back: {
        menu_stack.pop();
        break;
      }
      default:
        break;
    }
  }
}
MenuAction Controller::menu_handler() {
  using enum MenuState;
  while (true) {
    switch (menu_stack.top()) {
      case start_sel:
        return start_selection();
      case start_pla:
        return start_placement();
      case src_sel:
        return src_selection();
      case act_sel:
        return act_selection();
      case opt_sel:
        return opt_selection();
      case dst_sel:
        return dst_selection();
      default:
        std::unreachable();
    }
  }
}

MenuAction Controller::start_selection() {
  if (dh.get_curr_turn()) {
    redraw_boardwin();
    dh.refresh_curr_turn();
    turnwin.show();
    menu_stack = std::stack<MenuState>{{MenuState::src_sel}};
    return MenuAction::reg;
  }
  targetwin.hide();
  dh.start_selection();
  if (!dh.get_curr_options()[inputs.opt].x) {
    inputs.opt = 1 - inputs.opt;
  }
  optionwin.move_cursor(inputs.opt);
  while (true) {
    switch (conditional_input()) {
      case up:
      case down:
        inputs.opt = 1 - inputs.opt;
        optionwin.move_cursor(inputs.opt);
        break;
      case conf:
        if (can_act && dh.get_curr_options()[inputs.opt].x) {
          menu_stack.push(MenuState::start_pla);
          return MenuAction::reg;
        }
        break;
      case quit:
        return MenuAction::save_and_quit;
      case interrupt:
        return eval_rcvd();
      default:
        break;
    }
  }
}

MenuAction Controller::start_placement() {
  auto mov{[this]() {
    dh.select_target(inputs.dst);
    targetwin.show();
    boardwin.move_cursor(inputs.dst);
  }};
  mov();
  while (true) {
    switch (conditional_input()) {
      case up:
        inputs.dst.y = (inputs.dst.y - 1 + B_H) % B_H;
        mov();
        break;
      case down:
        inputs.dst.y = (inputs.dst.y + 1) % B_H;
        mov();
        break;
      case right:
        inputs.dst.x = (inputs.dst.x + 1) % B_W;
        mov();
        break;
      case left:
        inputs.dst.x = (inputs.dst.x - 1 + B_W) % B_W;
        mov();
        break;
      case conf:
        if (!dh.get_curr_target().data) {
          return MenuAction::create_unit;
        }
        break;
      case back:
        return MenuAction::go_back;
      case quit:
        return MenuAction::save_and_quit;
      case interrupt:
        return eval_rcvd();
      default:
        break;
    }
  }
}

MenuAction Controller::src_selection() {
  optionwin.hide();
  targetwin.hide();
  if (inputs.src == Coord{0, 0}) {  // this is buggy
    inputs.src = dh.get_next_piece_coords({-1, -1});
  }
  auto mov{[this]() {
    dh.select_source(inputs.src);
    sourcewin.show();
    actionwin.show();
    boardwin.move_cursor(inputs.src);
  }};
  mov();
  while (true) {
    switch (conditional_input()) {
      case up:
        inputs.src.y = (inputs.src.y - 1 + B_H) % B_H;
        mov();
        break;
      case down:
        inputs.src.y = (inputs.src.y + 1) % B_H;
        mov();
        break;
      case right:
        inputs.src.x = (inputs.src.x + 1) % B_W;
        mov();
        break;
      case left:
        inputs.src.x = (inputs.src.x - 1 + B_W) % B_W;
        mov();
        break;
      case next_valid:
        inputs.src = dh.get_next_piece_coords(inputs.src);
        mov();
        break;
      case conf:
        if (can_act) {
          for (Coord act : dh.get_curr_actions()) {
            if (act.x) {
              menu_stack.push(MenuState::act_sel);
              return MenuAction::reg;
            }
          }
        }
        break;
      case next_turn:
        if (can_act) {
          return MenuAction::new_turn;
        }
        break;
      case quit:
        return MenuAction::save_and_quit;
      case interrupt:
        return eval_rcvd();
      default:
        break;
    }
  }
}

MenuAction Controller::act_selection() {
  optionwin.hide();
  targetwin.hide();
  const std::vector<Coord>& curr_actions{dh.get_curr_actions()};
  fprintf(stderr, "%d, %lu\n", inputs.act, curr_actions.size());
  while (!curr_actions[inputs.act].x) {
    ++inputs.act;
  }
  const int acount{static_cast<int>(curr_actions.size())};
  auto mov{[this]() {
    dh.select_action(inputs.act);
    actionwin.move_cursor(inputs.act);
    boardwin.mark_squares(dh.get_squares_data(dh.get_curr_targets()));
    boardwin.move_cursor(inputs.src);  // not sure about this
  }};
  mov();
  while (true) {
    switch (conditional_input()) {
      case up:
        inputs.act = (inputs.act - 1 + acount) % acount;
        mov();
        break;
      case down:
        inputs.act = (inputs.act + 1) % acount;
        mov();
        break;
      case conf:
        if (curr_actions[inputs.act].x) {
          menu_stack.push(MenuState::opt_sel);
          return MenuAction::reg;
        }
        break;
      case back:
        inputs.act = 0;
        boardwin.unmark_squares();
        return MenuAction::go_back;
      case next_turn:
        return MenuAction::new_turn;
      case quit:
        return MenuAction::save_and_quit;
      case interrupt:
        return eval_rcvd();
      default:
        break;
    }
  }
}

MenuAction Controller::opt_selection() {
  if (!action_takes_options(dh.get_curr_actions()[inputs.act].y)) {
    menu_stack.pop();
    menu_stack.push(MenuState::dst_sel);
    return MenuAction::reg;
  }
  actionwin.hide();
  targetwin.hide();
  auto mov{[this]() { optionwin.move_cursor(inputs.opt); }};
  mov();
  const auto& curr_options{dh.get_curr_options()};
  const int ocount{static_cast<int>(curr_options.size())};
  while (true) {
    switch (conditional_input()) {
      case up:
        inputs.opt = (inputs.opt - 1 + ocount) % ocount;
        mov();
        break;
      case down:
        inputs.opt = (inputs.opt + 1) % ocount;
        mov();
        break;
      case conf:
        if (curr_options[inputs.opt].x) {
          menu_stack.push(MenuState::dst_sel);
          return MenuAction::reg;
        }
        break;
      case back:
        inputs.opt = 0;
        return MenuAction::go_back;
      case next_turn:
        return MenuAction::new_turn;
      case quit:
        return MenuAction::save_and_quit;
      case interrupt:
        return eval_rcvd();
      default:
        break;
    }
  }
}

MenuAction Controller::dst_selection() {
  if (!action_takes_squares[dh.get_curr_actions()[inputs.act].y]) {
    return MenuAction::do_action;
  }
  const std::vector<Coord> targets = dh.get_curr_targets();
  inputs.dst = targets[0];
  auto mov{[this]() {
    dh.select_target(inputs.dst);
    targetwin.show();
    boardwin.move_cursor(inputs.dst);
  }};
  mov();
  while (true) {
    switch (conditional_input()) {
      case up:
        inputs.dst.y = (inputs.dst.y - 1 + B_H) % B_H;
        fprintf(stderr, "%d,%d\n", inputs.dst.y, inputs.dst.x);
        mov();
        break;
      case down:
        inputs.dst.y = (inputs.dst.y + 1) % B_H;
        fprintf(stderr, "%d,%d\n", inputs.dst.y, inputs.dst.x);

        mov();
        break;
      case right:
        inputs.dst.x = (inputs.dst.x + 1) % B_W;
        fprintf(stderr, "%d,%d\n", inputs.dst.y, inputs.dst.x);

        mov();
        break;
      case left:
        inputs.dst.x = (inputs.dst.x - 1 + B_W) % B_W;
        fprintf(stderr, "%d,%d\n", inputs.dst.y, inputs.dst.x);

        mov();
        break;
      case next_valid:
        inputs.dst = dh.get_next_target_coords(inputs.dst);
        fprintf(stderr, "%d,%d\n", inputs.dst.y, inputs.dst.x);

        mov();
        break;
      case conf:
        for (const Coord& coord : targets) {
          if (coord == inputs.dst) {
            targetwin.hide();
            return MenuAction::do_action;
          }
        }
        break;
      case back:
        inputs.dst = {};
        targetwin.hide();
        return MenuAction::go_back;
      case next_turn:
        targetwin.hide();
        return MenuAction::new_turn;
      case quit:
        return MenuAction::save_and_quit;
      case interrupt:
        return eval_rcvd();
      default:
        break;
    }
  }
}
/*
Client always sends to 0 and has player_index as 2-8
Host sends to 2-8 and has player_index as 1

*/
ParsedInput Controller::conditional_input() {
  if (can_act) {
    nodelay(stdscr, false);
    return input_handler();
  } else {
    UserInput received_input;
    int sending_player = role == HOST ? dh.get_curr_player_data().y : 0;
    if (receive_inputs(sending_player, received_input, 1)) {
      nodelay(stdscr, true);
      return input_handler();
    }
    inputs = received_input;
    return interrupt;
  }
}
void Controller::redraw_boardwin() {
  boardwin.discard_markings();
  Coord c{0, 0};
  for (c.y = 0; c.y < B_H; ++c.y) {
    for (c.x = 0; c.x < B_W; ++c.x) {
      boardwin.refresh_square(c, dh.get_square_data(c));
    }
  }
  boardwin.refresh_win();
}

int Controller::send_command() {
  if (role != LOCAL) {
    if (can_act || role == HOST) {
      broadcast_inputs(inputs, dh.get_curr_player_data().y);
    }
  }
  int res = dh.send_command(inputs);
  history.push_back(inputs);
  dh.refresh_player_data();  // all types of actions could change player data
  playerwin.show();
  can_act = role == LOCAL || player_index == dh.get_curr_player_data().y;
  inputs = {};
  return res;
}
