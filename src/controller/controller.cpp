#include "controller.hpp"

#include <random>
#include <utility>

#include "../network/network_helper.hpp"

using enum ParsedInput;

/*

Going forward: Everything starts as sentinel value -> First valid one
When back: We have a valid value.                  -> Keep same
When tab                                           -> Next one

*/
template <typename T, scoped_enum V>
size_t get_next_cycle(const std::vector<T>& vec, V type) {
  bool has_found = type == sentinelof<V>();
  for (int j{}; j < 2; ++j) {
    for (size_t i{}; i < vec.size(); ++i) {
      if (has_found) {
        if (vec[i].mods) { return i; }
      }
      if (vec[i].type == type) { has_found = true; }
    }
  }
  return 0;
  do {
    for (size_t i{}; i < vec.size(); ++i) {
      if (has_found) {
        if (vec[i].mods) { return i; }
      }
      if (vec[i].type == type) { has_found = true; }
    }
  } while (!has_found);
  return 0;  // might cause an infinite loop if we pass an invalid value

  // bool has_found = type == sentinelof<V>();
  // while (true) {
  //   for (size_t i{}; i < vec.size(); ++i) {
  //     if (has_found) {
  //       if (vec[i].mods) { return i; }
  //     }
  //     if (vec[i].type == type) {
  //       if (has_found) { return 0; }  // means we've cycled all the way through
  //       has_found = true;
  //     }
  //     // if (vec[i].mods && has_found) { return i; }
  //     // if (vec[i].type == type) {
  //     //   if (!has_found) {
  //     //     has_found = true;
  //     //   } else {
  //     //     if (!vec[i].mods) { return 0; }  // means we've cycled all the way through
  //     //   }
  //     // }
  //   }
  //   has_found = true;  // in case we have selected an invalid value
  // }
  // return 0;
}

/*
I DON'T KNOW WHY THIS DOESNT WORK
*/

Coord get_next_dst_coords(const std::vector<Coord>& vec, Coord dst) {
  bool has_found{false};
  for (auto square : vec) {
    if (has_found) { return square; }
    if (square == dst) { has_found = true; }
  }
  return vec[0];
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

Controller::Controller(const std::vector<UserInput>& prev_history, Role role)
    : dh{prev_history[0]},
      history{{prev_history[0]}},
      B_H{prev_history[0].src.y},
      B_W{prev_history[0].src.x},
      role{role},
      player_count{prev_history[0].dst.y},
      player_index{prev_history[0].dst.x} {
  switch (role) {
    case Role::local:
    case Role::host:
    case Role::preview:
      if (prev_history.size() == 1) {
        init_random_overworld(char(prev_history[0].act), goldgeneration);
      } else {
        for (size_t i{1}; i < prev_history.size(); ++i) {
          inputs = prev_history[i];
          dh.transmit_command(inputs);
          history.push_back(inputs);
        }
      }
      if (role == Role::host) {
        for (size_t i{1}; i < history.size(); ++i) {
          broadcast_inputs(inputs);
        }
        broadcast_inputs({Coord::sentinel, ActionID::Sentinel, OptionID::Sentinel, Coord::sentinel});
      }
      break;
    case Role::client:
      while (!receive_inputs(0, inputs) && dh.validate_input(inputs) && inputs.act != ActionID::Sentinel) {
        dh.transmit_command(inputs);
        history.push_back(inputs);
      }
  }
  dh.refresh_player_data();
  dh.refresh_curr_turn();
  boardwin.redraw();
  can_act = role == Role::local || dh.get_curr_player_data().y == player_index;
}
std::vector<UserInput> Controller::game_loop() {
  inputs = {Coord::sentinel, ActionID::Sentinel, OptionID::Sentinel, Coord::sentinel};
  playerwin.show();
  turnwin.show();
  menustack = std::stack<MenuState>{{MenuState::src_sel}};
  while (true) {
    switch (menustack.top()) {
      case MenuState::go_back      : menustack.pop(), menustack.pop(); break;
      case MenuState::src_sel      : menustack.push(src_selection()); break;
      case MenuState::act_sel      : menustack.push(act_selection()); break;
      case MenuState::opt_sel      : menustack.push(opt_selection()); break;
      case MenuState::dst_sel      : menustack.push(dst_selection()); break;
      case MenuState::do_action    : menustack = {}, menustack.push(do_action()); break;
      case MenuState::end_game     : history.pop_back(), winscr(winner); [[fallthrough]];
      case MenuState::save_and_quit: return history;
    }
  }
}

MenuState Controller::do_action() {
  if (inputs.act == ActionID::Attack) { flash_screen(); }
  if (inputs.act == ActionID::Convert) { convert_anim(dh.get_curr_player_data().y); }
  return send_command();
}

MenuState Controller::send_command() {
  if (role != Role::local && (can_act || role == Role::host)) {
    broadcast_inputs(inputs, dh.get_curr_player_data().y);
  }
  ActionStatus res = dh.transmit_command(inputs);
  history.push_back(inputs);
  dh.refresh_player_data();
  playerwin.show();
  switch (res) {
    case ActionStatus::Ok:
      boardwin.redraw_square(inputs.src);
      boardwin.redraw_square(inputs.dst);
      if (inputs.act == ActionID::Move &&
          (dh.select_source(inputs.dst),
           std::ranges::any_of(dh.get_curr_actions(), [](auto act) { return act.mods; }))) {
        inputs = {inputs.dst, ActionID::Sentinel, OptionID::Sentinel, {}};
        menustack.push(MenuState::src_sel);
        return MenuState::act_sel;
      }
      break;
    case ActionStatus::NewTurn: dh.refresh_curr_turn(), turnwin.show(), boardwin.redraw(); break;
    case ActionStatus::KilledPlayer:
      boardwin.redraw();
      if ((winner = dh.get_game().get_winner()) >= 0) { return MenuState::end_game; }
      break;
  }
  can_act = role == Role::local || player_index == dh.get_curr_player_data().y;
  inputs = {Coord::sentinel, ActionID::Sentinel, OptionID::Sentinel, Coord::sentinel};
  return MenuState::src_sel;
}

inline void Controller::move_cursor(Coord& coord, ParsedInput direction) {
  switch (direction) {
    case up   : coord.y = (coord.y - 1 + B_H) % B_H; return;
    case down : coord.y = (coord.y + 1) % B_H; return;
    case left : coord.x = (coord.x - 1 + B_W) % B_W; return;
    case right: coord.x = (coord.x + 1) % B_W; return;
    default   : std::unreachable();
  }
}

MenuState Controller::src_selection() {
  if (inputs.src == Coord::sentinel) { inputs.src = dh.get_next_src_coords(inputs.src); }
  boardwin.unmark_squares();
  optionwin.hide();
  targetwin.hide();
  auto mov{[this]() {
    dh.select_source(inputs.src);
    sourcewin.show();
    actionwin.show();
    boardwin.move_cursor(inputs.src);
  }};
  mov();
  while (true) {
    ParsedInput input{conditional_input()};
    switch (input) {
      case up:
      case down:
      case left:
      case right     : move_cursor(inputs.src, input), mov(); break;
      case next_valid: inputs.src = dh.get_next_src_coords(inputs.src), mov(); break;
      case confirm:
        if (can_act && std::ranges::any_of(dh.get_curr_actions(), [](auto act) { return act.mods; })) {
          return MenuState::act_sel;
        }
        break;
      case next_turn:
        if (dh.get_curr_turn() > 0) {
          inputs.act = ActionID::EndTurn;
          return MenuState::do_action;
        };
        break;
      case quit     : return MenuState::save_and_quit;
      case interrupt: return MenuState::do_action;
      default       : break;
    }
  }
}

MenuState Controller::act_selection() {
  const auto& curr_actions{dh.get_curr_actions()};
  const size_t acount{curr_actions.size()};
  size_t act_i{};
  if (inputs.act == ActionID::Sentinel) {
    act_i = get_next_cycle(curr_actions, inputs.act);
  } else {
    for (size_t i{0}; i < acount; ++i) {
      if (curr_actions[i].type == inputs.act) {
        act_i = i;
        break;
      }
    }
  }
  optionwin.hide();
  targetwin.hide();
  actionwin.show();
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
      case next_valid:
        act_i = get_next_cycle(dh.get_curr_actions(), curr_actions[act_i].type);
        mov();
        break;
      case confirm:
        if (curr_actions[act_i].mods) {
          inputs.act = curr_actions[act_i].type;
          return MenuState::opt_sel;
        }
        break;
      case back: inputs.act = ActionID::Sentinel; return MenuState::go_back;
      case next_turn:
        if (dh.get_curr_turn() > 0) {
          inputs.act = ActionID::EndTurn;
          return MenuState::do_action;
        }
        break;
      case quit     : return MenuState::save_and_quit;
      case interrupt: return MenuState::do_action;
      default       : break;
    }
  }
}

MenuState Controller::opt_selection() {
  if (!act_takes_options(inputs.act)) {
    menustack.pop();
    return MenuState::dst_sel;
  }
  const auto& curr_options{dh.get_curr_options()};
  size_t ocount{curr_options.size()};
  // size_t opt_i{get_next_cycle(dh.get_curr_options(), inputs.opt)};
  size_t opt_i{};
  if (inputs.act == ActionID::Sentinel) {
    opt_i = get_next_cycle(curr_options, inputs.opt);
  } else {
    for (size_t i{0}; i < ocount; ++i) {
      if (curr_options[i].type == inputs.opt) {
        opt_i = i;
        break;
      }
    }
  }
  actionwin.hide();
  targetwin.hide();
  optionwin.show();
  auto mov{[this, &opt_i]() { optionwin.move_cursor(opt_i); }};
  mov();
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
      case next_valid:
        opt_i = get_next_cycle(dh.get_curr_options(), curr_options[opt_i].type);
        mov();
        break;
      case confirm:
        if (curr_options[opt_i].mods) {
          inputs.opt = curr_options[opt_i].type;
          return MenuState::dst_sel;
        }
        break;
      case back     : inputs.opt = OptionID::Sentinel; return MenuState::go_back;
      case next_turn: inputs.act = ActionID::EndTurn; return MenuState::do_action;
      case quit     : return MenuState::save_and_quit;
      case interrupt: return MenuState::do_action;
      default       : break;
    }
  }
}

MenuState Controller::dst_selection() {
  if (!act_takes_dst(inputs.act)) {
    inputs.dst = {};
    return MenuState::do_action;
  }
  const auto& targets{dh.get_curr_targets()};
  inputs.dst = targets[0];
  auto mov{[this]() {
    dh.select_target(inputs.dst);
    targetwin.show();
    boardwin.move_cursor(inputs.dst);
  }};
  mov();
  while (true) {
    ParsedInput input{conditional_input()};
    switch (input) {
      case up:
      case down:
      case left:
      case right:
        move_cursor(inputs.dst, input);
        mov();
        break;
      case next_valid:
        inputs.dst = get_next_dst_coords(targets, inputs.dst);
        mov();
        break;
      case confirm:
        if (std::ranges::contains(targets, inputs.dst)) { return MenuState::do_action; }
        break;
      case back     : inputs.dst = Coord::sentinel; return MenuState::go_back;
      case next_turn: inputs.act = ActionID::EndTurn; return MenuState::do_action;
      case quit     : return MenuState::save_and_quit;
      case interrupt: return MenuState::do_action;
      default       : break;
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
    int sending_player{role == Role::host ? dh.get_curr_player_data().y : 0};
    size_t ret{receive_inputs(sending_player, received_input, 1)};
    if (ret == 0) {
      nodelay(stdscr, true);
      return input_handler();
    } else if (ret < 0) {
      return quit;  // todo error handling
    }
    if (ret > 0 && !dh.validate_input(received_input)) {
      nodelay(stdscr, true);
      ParsedInput in{input_handler()};
      if (in == ParsedInput::next_turn) { return ParsedInput::COUNT; }
      return in;
    }
    inputs = received_input;
    return interrupt;
  }
}

void Controller::init_random_overworld(unsigned int fgen, unsigned int gold_gen) {
  int total_squares{B_H * B_W};
  int occ_squares{0};
  std::bernoulli_distribution forest_dist(static_cast<float>(fgen) / 100.0f);
  std::bernoulli_distribution gold_dist(gold_gen);
  std::mt19937 gen{std::random_device{}()};
  inputs = {{}, ActionID::CreateUnit, OptionID::Sentinel, {}};
  Coord target{0, 0};
  for (target.y = 0; target.y < B_H; ++target.y) {
    for (target.x = 0; target.x < B_W; ++target.x) {
      if (total_squares - occ_squares <= player_count * 2) { break; }
      if (forest_dist(gen)) {
        inputs = {{}, ActionID::CreateUnit, OptionID::CreateArbre, target};
        dh.transmit_command(inputs);
        history.push_back(inputs);
        ++occ_squares;
      } else if (gold_dist(gen)) {
        inputs = {{}, ActionID::CreateUnit, OptionID::CreateMine, target};
        dh.transmit_command(inputs);
        history.push_back(inputs);
        ++occ_squares;
      }
    }
  }
  inputs = {{}, ActionID::EndTurn, OptionID::Sentinel, {}};
  dh.transmit_command(inputs);
  history.push_back(inputs);
}