#include "data_handler.hpp"

#include <vector>

#include "../game_model/game.hpp"
#include "../helpers/user_input.hpp"

DataHandler::DataHandler(const UserInput& inputs) : inputs{inputs} {}

std::vector<MenuItem> DataHandler::get_action_items() const {
  std::vector<MenuItem> menu_items;
  for (ActionDat action_item : act_data) {
    menu_items.emplace_back(action_names[action_item.type], action_item.value);
  }
  return menu_items;
}

std::vector<MenuItem> DataHandler::get_option_items() const {
  std::vector<MenuItem> menu_items;
  for (OptionDat option_item : act_option_data[inputs.action]) {
    menu_items.emplace_back(option_names[option_item.type], option_item.value);
  }
  return menu_items;
}

chtype DataHandler::get_sqdat(Coord coords) const {
  const Pion* const occ = board[coords];
  if (!occ) {
    return ' ';
  }
  return (piece_constants[occ->get_type()].sign) |
         (COLOR_PAIR(occ->get_player().get_colour())) |
         (WA_DIM * ((occ->get_fatigue() > 0) |
                    (!occ->get_player().get_is_their_turn())));
}

SquareData DataHandler::get_piece_data(Coord coords) const {
  const Pion* const occ = board[coords];
  if (!occ) {
    return 0;
  }
  return (occ->get_pv() << 12) + (occ->get_fatigue() << 8) +
         (occ->get_player().get_colour() << 4) + occ->get_type();
}

const ActionDat& DataHandler::get_curr_action() const {
  return act_data[inputs.action];
}
const std::vector<ActionDat>& DataHandler::get_curr_actions() const {
  return act_data;
}
const std::vector<OptionDat>& DataHandler::get_curr_options() const {
  return act_option_data[inputs.action];
}
const std::vector<Coord>& DataHandler::get_curr_targets() const {
  return act_target_data[inputs.action];
}

int DataHandler::get_current_turn() const { return curr_turn; }

const std::vector<UserInput>& DataHandler::get_history() const {
  return history;
}

PlayerData DataHandler::get_current_player_data() const {
  return {game.get_current_player().get_colour(),
          game.get_current_player().get_gold()};
}

Coord DataHandler::get_next_piece_coords() const {
  bool start_found = false;
  for (const auto& piece : game.get_current_player().get_pieces()) {
    if (piece->get_coords() == inputs.acteur) {
      start_found = true;
    } else if (!piece->get_fatigue() && start_found) {
      return piece->get_coords();
    }
  }
  for (const auto& piece : game.get_current_player().get_pieces()) {
    if (!piece->get_fatigue()) {
      return piece->get_coords();
    }
  }
  return inputs.acteur;
}
Coord DataHandler::get_next_target_coords() const {
  bool start_found = false;
  for (const auto& sq : act_target_data[inputs.action]) {
    if (sq == inputs.target) {
      start_found = true;
    } else if (start_found) {
      return sq;
    }
  }
  return act_target_data[inputs.action][0];
}

int DataHandler::send_command() {
  int res = 0;
  if (inputs.action >= 0) {
    res = game.action(inputs);
  }
  if (inputs.action == END_TURN) {
    game.end_turn();
  }
  if (inputs.action == CREATE_UNIT) {
    game.create_unit(inputs.option, inputs.target);
  }
  history.push_back(inputs);
  return res;
}

void DataHandler::start_selection() {
  const Player& pl = game.get_current_player();
  if (!pl.get_pieces().size()) {
    act_option_data = {{{CREATE_CHATEAU, 1}, {CREATE_PAYSAN, 1}}};
    return;
  }
  if (pl.get_pieces()[0]->get_type() == CHATEAU) {
    act_option_data[0] = {{CREATE_CHATEAU, 0}, {CREATE_PAYSAN, 1}};
    return;
  }
  act_option_data[0] = {{CREATE_CHATEAU, 1}, {CREATE_PAYSAN, 0}};
}

void DataHandler::request_action_data() {
  act_data.clear();
  act_option_data.clear();
  act_target_data.clear();
  const Pion* const occptr = board[inputs.acteur];
  if (!occptr || !(occptr->get_player().get_is_their_turn())) {
    return;
  }
  const Pion& occ = *occptr;
  const std::array<ActionID, 5UL>& ac = occ.get_actions();
  for (int i = 0; i < ac.size() && ac[i] != ACTION; ++i) {
    Action& action = *(game.actions[ac[i]]);
    act_data.emplace_back(action.get_type(),
                          action.get_fatigue_condition(inputs.acteur));
    act_option_data.emplace_back();
    act_target_data.emplace_back();
    if (act_data.back().value && action.takes_options()) {
      std::vector<OptionID> possible_options =
          action.get_poss_options(inputs.acteur);
      if (possible_options.size() == 0) {
        act_data[i].value = 0;
        continue;
      }
      for (OptionID opt : action_options[action.get_type()]) {
        if (opt < 0) {
          break;
        }
        act_option_data[i].push_back({opt, 0});
        for (OptionID possopt : possible_options) {
          if (possopt == opt) {
            act_option_data[i].back().value = 1;
          }
        }
      }
    }
    if (act_data.back().value && action.takes_squares()) {
      act_target_data.back() = action.get_poss_squares(inputs.acteur);
      if (act_target_data.back().size() == 0) {
        act_data.back().value = false;
      }
    }
  }
}
