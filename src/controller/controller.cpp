#include "controller.hpp"

#include <random>
#include <utility>

#include "../network/network_helper.hpp"

using enum ParsedInput;
MenuAction Controller::eval_rcvd() {
  switch (inputs.act) {
    case ActionID::end_turn:
      return MenuAction::new_turn;
    case ActionID::create_unit:
      return MenuAction::create_unit;
    default:
      return MenuAction::do_action;
  }
}
/*
We always have a history
If we don't load anything, it's just the init settings
If we do, then it's loaded normally
In either case, the client needs to receive the history before Controller
In connection, we always just send the settings (both put it in history)
Then in controller, the Role::host goes through history (except 0th element) and
plays out each move This copies history
*/

Controller::Controller(const std::vector<UserInput> &prev_history, Role role)
    : dh{prev_history[0]},
      B_H{prev_history[0].src.y},
      B_W{prev_history[0].src.x},
      role{role},
      player_count{prev_history[0].dst.y},
      player_index{prev_history[0].dst.x} {
  history = {prev_history[0]};
  if (role == Role::local || role == Role::host) {
    if (prev_history.size() == 1) {
      init_random_overworld(char(prev_history[0].act), goldgeneration);
    } else {
      for (size_t i{1}; i < prev_history.size(); ++i) {
        inputs = prev_history[i];
        dh.send_command(inputs);
        history.push_back(inputs);
        if (role == Role::host) {
          broadcast_inputs(inputs);
        }
      }
    }
    broadcast_inputs({{coord_senitel, coord_senitel},
                      ActionID::action,
                      OptionID::option,
                      {coord_senitel, coord_senitel}});
  } else {
    while (!receive_inputs(0, inputs)) {
      if (inputs.dst.y == coord_senitel) {
        break;
      }
      dh.send_command(inputs);
      history.push_back(inputs);
    }
  }
  dh.refresh_player_data();  // all types of actions could change player data
  dh.refresh_curr_turn();
  redraw_boardwin();
  menu_stack = std::stack<MenuState>{{MenuState::start_sel}};
  can_act = role == Role::local || dh.get_curr_player_data().y == player_index;
}

// std::mt19937 gen(seed ? std::mt19937{seed}
//                       : std::mt19937{std::random_device{}()});
void Controller::init_random_overworld(unsigned int fgen,
                                       unsigned int gold_gen) {
  int total_squares = B_H * B_W;
  int occ_squares = 0;
  std::bernoulli_distribution forest_dist(static_cast<float>(fgen) / 100.0f);
  std::bernoulli_distribution gold_dist(gold_gen);
  std::mt19937 gen{std::random_device{}()};
  inputs = {{0, 0}, ActionID::create_unit, OptionID::option, {0, 0}};
  Coord target{0, 0};
  for (target.y = 0; target.y < B_H; ++target.y) {
    for (target.x = 0; target.x < B_W; ++target.x) {
      if (total_squares - occ_squares <= player_count * 2) {
        break;
      }
      if (forest_dist(gen)) {
        inputs.act = ActionID::create_unit;
        inputs.opt = OptionID::create_arbre;
        inputs.dst = target;
        send_command();
        ++occ_squares;
      } else if (gold_dist(gen)) {
        inputs.act = ActionID::create_unit;
        inputs.opt = OptionID::create_mine;
        inputs.dst = target;
        send_command();
        ++occ_squares;
      }
    }
  }
  inputs = {{0, 0}, ActionID::end_turn, OptionID::option, {0, 0}};
  // after init we always end the turn
  send_command();
}

int Controller::mov_action(UserInput inp) {
  if (inp.act == ActionID::move) {
    dh.select_source(inp.dst);
    for (auto act : dh.get_curr_actions()) {
      if (act.mods) {
        return 1;
      }
    }
  }
  return 0;
}

std::vector<UserInput> Controller::game_loop() {
  playerwin.show();
  turnwin.show();
  while (true) {
    switch (menu_handler()) {
      case MenuAction::new_turn: {
        inputs = {{0, 0}, ActionID::end_turn, OptionID::option, {0, 0}};
        send_command();
        break;
      }
      case MenuAction::create_unit: {
        inputs = {{dh.get_curr_player_data().y, 0},
                  ActionID::create_unit,
                  inputs.opt,
                  inputs.dst};
        send_command();
        break;
      }
      case MenuAction::do_action: {
        boardwin.unmark_squares();
        if (inputs.act == ActionID::attack) {
          flash_screen();
        }
        if (inputs.act == ActionID::convert) {
          convert_anim(dh.get_curr_player_data().y);
        }
        if (send_command()) {
          return history;
        };
        break;
      }
      case MenuAction::save_and_quit: {
        return history;
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
  while (true) {
    switch (menu_stack.top()) {
      case MenuState::start_sel:
        return start_selection();
      case MenuState::start_pla:
        return start_placement();
      case MenuState::src_sel:
        return src_selection();
      case MenuState::act_sel:
        return act_selection();
      case MenuState::opt_sel:
        return opt_selection();
      case MenuState::dst_sel:
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
  static int opt_i = 0;
  const auto &curr_opts{dh.get_curr_options()};
  optionwin.show();
  if (!curr_opts[opt_i].mods) {
    opt_i = 1 - opt_i;
  }
  inputs.opt = curr_opts[opt_i].type;
  optionwin.move_cursor(opt_i);
  while (true) {
    switch (conditional_input()) {
      case up:
      case down:
        opt_i = 1 - opt_i;
        inputs.opt = curr_opts[opt_i].type;
        optionwin.move_cursor(opt_i);
        break;
      case conf:
        if (can_act && curr_opts[opt_i].mods) {
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
        if (!dh.get_curr_target()) {
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
    inputs.src = dh.get_next_piece_coords({coord_senitel, coord_senitel});
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
          for (auto act : dh.get_curr_actions()) {
            if (act.mods) {
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
  actionwin.show();
  const auto &curr_actions{dh.get_curr_actions()};
  size_t act_i = dh.get_next_action_index(inputs.act);  // to_index(inputs.act);
  while (!curr_actions[act_i].mods) {
    ++act_i;
  }
  const size_t acount{curr_actions.size()};
  auto mov{[this, &act_i]() {
    dh.select_action(act_i);
    actionwin.move_cursor(act_i);
    boardwin.mark_squares();
    boardwin.move_cursor(inputs.src);  // not sure about this
  }};
  mov();
  while (true) {
    switch (conditional_input()) {
      case up:
        act_i = (act_i - 1 + acount) % acount;
        mov();
        break;
      case down:
        act_i = (act_i + 1) % acount;
        mov();
        break;
      case conf:
        if (curr_actions[act_i].mods) {
          menu_stack.push(MenuState::opt_sel);
          inputs.act = curr_actions[act_i].type;
          return MenuAction::reg;
        }
        break;
      case back:
        inputs.act = ActionID::action;
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
  if (!act_takes_options(inputs.act)) {
    menu_stack.pop();
    menu_stack.push(MenuState::dst_sel);
    return MenuAction::reg;
  }
  actionwin.hide();
  targetwin.hide();
  optionwin.show();
  size_t opt_i = dh.get_next_option_index(inputs.opt);  // to_index(inputs.opt);
  const auto &curr_options{dh.get_curr_options()};
  auto mov{[this, &opt_i]() { optionwin.move_cursor(opt_i); }};
  mov();
  const size_t ocount{curr_options.size()};
  while (true) {
    switch (conditional_input()) {
      case up:
        opt_i = (opt_i - 1 + ocount) % ocount;
        mov();
        break;
      case down:
        opt_i = (opt_i + 1) % ocount;
        mov();
        break;
      case conf:
        if (curr_options[opt_i].mods) {
          inputs.opt = curr_options[opt_i].type;
          menu_stack.push(MenuState::dst_sel);
          return MenuAction::reg;
        }
        break;
      case back:
        inputs.opt = OptionID::option;
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
  if (!act_takes_squares(inputs.act)) {
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
      case next_valid:
        inputs.dst = dh.get_next_target_coords(inputs.dst);
        mov();
        break;
      case conf:
        for (auto coord : targets) {
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
Role::host sends to 2-8 and has player_index as 1
*/
ParsedInput Controller::conditional_input() {
  if (can_act) {
    nodelay(stdscr, false);
    return input_handler();
  } else {
    UserInput received_input;
    int sending_player = role == Role::host ? dh.get_curr_player_data().y : 0;
    size_t ret = receive_inputs(sending_player, received_input, 1);
    if (ret == 0) {
      nodelay(stdscr, true);
      return input_handler();
    } else if (ret < 0) {
      return quit;  // todo error handling
    }
    if (ret > 0 && !dh.validate_input(received_input)) {
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
      boardwin.refresh_square(c);
    }
  }
  boardwin.refresh();
}

int Controller::send_command() {
  if (role != Role::local) {
    if (can_act || role == Role::host) {
      broadcast_inputs(inputs, dh.get_curr_player_data().y);
    }
  }
  ActionStatus res = dh.send_command(inputs);
  history.push_back(inputs);
  dh.refresh_player_data();  // all types of actions could change player data
  playerwin.show();
  switch (res) {
    case ActionStatus::ok: {
      boardwin.redraw_square(inputs.src);
      boardwin.redraw_square(inputs.dst);
      if (mov_action(inputs)) {
        inputs = {inputs.dst, ActionID::action, OptionID::option, {0, 0}};
        menu_stack =
            std::stack<MenuState>{{MenuState::src_sel, MenuState::act_sel}};
        return 0;
      }
      menu_stack = std::stack<MenuState>{{MenuState::src_sel}};
      break;
    }
    case ActionStatus::new_turn: {
      dh.refresh_curr_turn();
      turnwin.show();
      redraw_boardwin();
      menu_stack = std::stack<MenuState>{{MenuState::src_sel}};
      break;
    }
    case ActionStatus::created_unit: {
      dh.refresh_curr_turn();
      boardwin.redraw_square(inputs.dst);
      menu_stack = std::stack<MenuState>{{MenuState::start_sel}};
      break;
    }
    case ActionStatus::defeated_player: {
      redraw_boardwin();
      int winner = dh.get_game().get_winner();
      if (winner >= 0) {
        history.pop_back();
        winscr(winner);
        return 1;  // TODO win condition
      }
      menu_stack = std::stack<MenuState>{{MenuState::src_sel}};
    } break;
    case ActionStatus::error_input: {
      std::unreachable();
    }
  }
  can_act = role == Role::local || player_index == dh.get_curr_player_data().y;
  inputs = {};
  return 0;
}
