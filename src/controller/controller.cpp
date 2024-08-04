#include "controller.hpp"

#include <cassert>
#include <random>

#include "../game_model/game.hpp"
#include "../input_parser.hpp"
#include "../save/save.hpp"
#include "data_handler.hpp"
#include "network_helper.hpp"

Controller::Controller(const char* savefile) {
  switch (role) {
    case LOCAL:
      load_save(savefile);
      if (!dh.get_history().size()) {
        init_random_overworld();
      }
      break;
    case HOST:
      load_save(savefile);
      if (!dh.get_history().size()) {
        init_random_overworld();
      }
      for (const UserInput& input : dh.get_history()) {
        broadcast(input);  // todo add error handling
      }
      broadcast(UserInput{{-1, -1}, -1, -1, {-1, -1}});
      break;
    case CLIENT:
      while (inputs.option >= 0) {
        receive(inputs);
        dh.send_command();
      }
      inputs = {};
      break;
  }
}

int Controller::quit() {
  menu_stack.push(QUIT_GAME);
  return 1;
}

void Controller::load_save(const std::string& savefile) {
  for (const UserInput& input : load_game(savefile)) {
    inputs = input;
    dh.send_command();
  }
}

void Controller::init_random_overworld() {
  std::bernoulli_distribution forest_dist(FORESTATION);
  std::bernoulli_distribution gold_dist(GOLDDIST);
  std::random_device rd;
  std::mt19937 gen(rd());
  inputs = {{0, 0}, CREATE_UNIT, 0, {0, 0}};
  for (int i = 0; i < B_H; ++i) {
    for (int j = 0; j < B_W; ++j) {
      inputs.target = {i, j};
      if (forest_dist(gen)) {
        inputs.option = CREATE_FORET;
        dh.send_command();
      } else if (gold_dist(gen)) {
        inputs.option = CREATE_MINE;
        dh.send_command();
      }
      boardwin.redraw_square(inputs.target);
    }
  }
  inputs = {{0, 0}, END_TURN, 0, {0, 0}};
  dh.send_command();
}

void Controller::game_loop() {
  menu_stack.push(dh.get_current_turn() <= 0 ? START_UNIT_SELECT : ACTEUR_SEL);
  boardwin.render();
  while (1) {
    switch (menu_stack.top()) {
      case START_UNIT_SELECT:
        start_unit_selection();
        break;
      case START_UNIT_PLACEMENT:
        start_unit_placement();
        break;
      case ACTEUR_SEL:
        square_selection();
        break;
      case ACTION_SEL:
        action_selection();
        break;
      case OPTION_SEL:
        if (action_takes_options[dh.get_curr_action().type]) {
          option_selection();
          break;
        }
        menu_stack.pop();
        menu_stack.push(TARGET_SEL);
        break;
      case TARGET_SEL:
        if (action_takes_squares[dh.get_curr_action().type]) {
          target_selection();
          break;
        }
        send_action();
        break;
      case QUIT_GAME:
        save_game(dh.get_history());
        return;
      default:
        assert(0);
    }
  }
}

void Controller::start_unit_selection() {
  inputs.action = 0;
  dh.start_selection();
  playerwin.show();
  optionwin.move_cursor();
  if (c >= PLAYER_COUNT) {
    for (int i = 0; i < dh.get_curr_options().size(); ++i) {
      if (dh.get_curr_options()[i].value) {
        inputs.option = i;
        menu_stack.push(START_UNIT_PLACEMENT);
        return;
      }
    }
  }
  while (1) {
    switch (input_handler(dh.get_current_player_data().index)) {
      case UP:
        inputs.option = (inputs.option - 1 + 2) % 2;
        optionwin.move_cursor();
        break;
      case DOWN:
        inputs.option = (inputs.option + 1) % 2;
        optionwin.move_cursor();
        break;
      case CONF:
        menu_stack.push(START_UNIT_PLACEMENT);
        return;
      case QUIT:
        quit();
        return;
      default:
        break;
    }
  }
}

#define STARTMOV()                       \
  {                                      \
    sourcewin.show();                    \
    targetwin.show();                    \
    boardwin.move_cursor(inputs.target); \
  }

void Controller::start_unit_placement() {
  STARTMOV()
  while (1) {
    switch (input_handler(dh.get_current_player_data().index)) {
      case UP:
        inputs.target.y = (inputs.target.y - 1 + B_H) % B_H;
        STARTMOV()
        break;
      case DOWN:
        inputs.target.y = (inputs.target.y + 1) % B_H;
        STARTMOV()
        break;
      case RIGHT:
        inputs.target.x = (inputs.target.x + 1) % B_W;
        STARTMOV()
        break;
      case LEFT:
        inputs.target.x = (inputs.target.x - 1 + B_W) % B_W;
        STARTMOV()
        break;
      case CONF:
        if (dh.get_piece_data(inputs.target).data) {
          break;
        }
        inputs.action = CREATE_UNIT;
        dh.send_command();
        boardwin.redraw_square(inputs.target);
        if (++c < PLAYER_COUNT * 2) {
          menu_stack = std::stack<MenuID>{{START_UNIT_SELECT}};
          return;
        }
        end_turn();
        optionwin.hide();
        targetwin.hide();
        return;
      case BACK:
        menu_stack.pop();
        return;
      case QUIT:
        quit();
        return;
      default:
        break;
    }
  }
}

#define SQUARESELMOV()                   \
  {                                      \
    dh.request_action_data();            \
    boardwin.unmark_squares();           \
    sourcewin.show();                    \
    actionwin.show();                    \
    boardwin.move_cursor(inputs.acteur); \
  }

void Controller::square_selection() {
  if (inputs.acteur == Coord{0, 0}) {
    inputs.acteur = dh.get_next_piece_coords();
  }
  SQUARESELMOV()
  while (1) {
    switch (input_handler(dh.get_current_player_data().index)) {
      case UP:
        inputs.acteur.y = (inputs.acteur.y - 1 + B_H) % B_H;
        SQUARESELMOV()
        break;
      case DOWN:
        inputs.acteur.y = (inputs.acteur.y + 1) % B_H;
        SQUARESELMOV()
        break;
      case RIGHT:
        inputs.acteur.x = (inputs.acteur.x + 1) % B_W;
        SQUARESELMOV()
        break;
      case LEFT:
        inputs.acteur.x = (inputs.acteur.x - 1 + B_W) % B_W;
        SQUARESELMOV()
        break;
      case CONF:
        curs_set(2);
        for (ActionDat act : dh.get_curr_actions()) {
          if (act.value) {
            menu_stack.push(ACTION_SEL);
            return;
          }
        }
        break;
      case NEXT_VALID:
        inputs.acteur = dh.get_next_piece_coords();
        SQUARESELMOV()
        break;
      case NEXT_TURN:
        end_turn();
        return;
      case QUIT:
        quit();
        return;
      default:
        break;
    }
  }
}

#define ACTIONSELMOV()       \
  {                          \
    actionwin.move_cursor(); \
    boardwin.mark_squares(); \
  }

void Controller::action_selection() {
  const std::vector<ActionDat>& curr_actions{dh.get_curr_actions()};
  const int acount = curr_actions.size();
  actionwin.move_cursor();
  ACTIONSELMOV()
  while (1) {
    switch (input_handler(dh.get_current_player_data().index)) {
      case UP:
        inputs.action = (inputs.action - 1 + acount) % acount;
        ACTIONSELMOV()
        break;
      case DOWN:
        inputs.action = (inputs.action + 1) % acount;
        ACTIONSELMOV()
        break;
      case CONF:
        if (!curr_actions[inputs.action].value) {
          break;
        }
        menu_stack.push(OPTION_SEL);
        return;
      case BACK:
        inputs.action = 0;
        menu_stack.pop();
        return;
      case NEXT_TURN:
        end_turn();
        return;
      case QUIT:
        quit();
        return;
      default:
        break;
    }
  }
}

void Controller::option_selection() {
  optionwin.move_cursor();
  const std::vector<OptionDat>& curr_options{dh.get_curr_options()};
  const int ocount = curr_options.size();
  while (1) {
    switch (input_handler(dh.get_current_player_data().index)) {
      case UP:
        inputs.option = (inputs.option - 1 + ocount) % ocount;
        optionwin.move_cursor();
        break;
      case DOWN:
        inputs.option = (inputs.option + 1) % ocount;
        optionwin.move_cursor();
        break;
      case CONF:
        if (!curr_options[inputs.option].value) {
          break;
        }
        menu_stack.push(TARGET_SEL);
        return;
      case BACK:
        inputs.option = 0;
        menu_stack.pop();
        return;
      case NEXT_TURN:
        end_turn();
        return;
      case QUIT:
        quit();
        return;
      default:
        break;
    }
  }
}

#define TARGETSELMOV()                   \
  {                                      \
    targetwin.show();                    \
    boardwin.move_cursor(inputs.target); \
  }

void Controller::target_selection() {
  const std::vector<Coord> targets = dh.get_curr_targets();
  inputs.target = targets[0];
  TARGETSELMOV()
  while (1) {
    switch (input_handler(dh.get_current_player_data().index)) {
      case UP:
        inputs.target.y = (inputs.target.y - 1 + B_H) % B_H;
        TARGETSELMOV()
        break;
      case DOWN:
        inputs.target.y = (inputs.target.y + 1) % B_H;
        TARGETSELMOV()
        break;
      case RIGHT:
        inputs.target.x = (inputs.target.x + 1) % B_W;
        TARGETSELMOV()
        break;
      case LEFT:
        inputs.target.x = (inputs.target.x - 1 + B_W) % B_W;
        TARGETSELMOV()
        break;
      case CONF:
        for (const Coord& coords : targets) {
          if (coords == inputs.target) {
            send_action();
            return targetwin.hide();
          }
        }
        break;
      case NEXT_VALID:
        inputs.target = dh.get_next_target_coords();
        TARGETSELMOV()
        break;
      case BACK:
        inputs.target = {};
        menu_stack.pop();
        targetwin.hide();
        return;
      case NEXT_TURN:
        end_turn();
        targetwin.hide();
        return;
      case QUIT:
        quit();
        return;
      default:
        break;
    }
  }
}

void Controller::send_action() {
  if (dh.get_curr_action().type == ATTACK) {
    flash_screen();
  } else if (dh.get_curr_action().type == CONVERT) {
    convert_anim(dh, inputs);
  }
  if (dh.send_command() == 100) {
    WinScreen win{dh, {5, 5}};
    menu_stack.push(QUIT_GAME);
    return;
  }
  playerwin.show();
  boardwin.redraw_square(inputs.acteur);
  boardwin.redraw_square(inputs.target);
  menu_stack = std::stack<MenuID>{{ACTEUR_SEL}};
  if (dh.get_curr_action().type == MOVE) {
    boardwin.unmark_squares();
    inputs.acteur = inputs.target;
    dh.request_action_data();
    for (int i = 0; i < dh.get_curr_actions().size(); ++i) {
      if (dh.get_curr_actions()[i].value) {
        boardwin.move_cursor(inputs.acteur);
        inputs.action = i;
        menu_stack.push(ACTION_SEL);
        return;
      }
    }
  }
  inputs = {dh.get_next_piece_coords()};
}

void Controller::end_turn() {
  inputs.action = END_TURN;
  dh.send_command();
  playerwin.show();
  turnwin.show();
  boardwin.render();
  inputs = {};
  menu_stack = std::stack<MenuID>{{ACTEUR_SEL}};
}
