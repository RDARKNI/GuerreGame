#include "game.hpp"

#include <memory>
#include <vector>

#include "../game_config.hpp"
#include "../helpers/action_data.hpp"
#include "../helpers/error_data.hpp"
#include "../helpers/piece_data.hpp"
#include "board.hpp"
#include "pion.hpp"

Game::Game() {
  Action::board = &board;
  Pion::board = &board;
  Player::board = &board;
  players.emplace_back(std::make_unique<Player>(0));
  for (size_t i = 1; i < PLAYER_COUNT + 1; ++i) {
    player_indices.emplace_back(i);
    players.emplace_back(std::make_unique<Player>(i));
  }
}

int Game::create_unit(int unit_type, Coord target) {
  Piece_ID type;
  switch (unit_type) {
    case CREATE_FORET:
      type = FORET;
      break;
    case CREATE_MINE:
      type = MINE;
      break;
    case CREATE_CHATEAU:
      type = CHATEAU;
      break;
    case CREATE_PAYSAN:
      type = PAYSAN;
      break;
    default:
      assert(0);
      return EX_WRONG_OPTION;  // todo
  }
  players[curr_player_index]->add_piece(
      std::make_unique<Pion>(*players[curr_player_index], type, target));
  if (curr_player_index) {
    if (!(curr_player_index == PLAYER_COUNT &&
          players[curr_player_index]->get_pieces().size() == 2)) {
      curr_player_index = (curr_player_index) % player_indices.size() + 1;
    }
  }
  return 0;
}

int Game::action(const UserInput& input) {
  actions[board[input.acteur]->get_actions()[input.action]]->perform(
      input.acteur, input.option, input.target);
  if (kill_players()) {
    if (player_indices.size() == 1) {
      return 100;
    }
  }
  return 0;
}

int Game::end_turn() {
  players[curr_player_index]->end_turn();
  turn++;
  curr_player_index = (curr_player_index) % player_indices.size() + 1;
  players[curr_player_index]->start_turn();
  return 0;
}

int Game::kill_players() {
  for (int i = 1; i < players.size(); ++i) {
    int count = 0;
    for (const auto& piece : players[i]->get_pieces()) {
      if (piece->get_type() == CHATEAU) {
        count++;
      }
    }
    if (!count) {
      players.erase(players.begin() + i);
      if (curr_player_index >= i) {
        curr_player_index--;
      }
      player_indices.erase(player_indices.begin() + i - 1);
      return 1;
    }
  }
  return 0;
}
