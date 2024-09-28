#include "game.hpp"

#include <memory>
#include <utility>

Game::Game(UserInput settings)
    : board{settings.src},
      initial_player_count{settings.dst.y},
      curr_player_count{initial_player_count} {
  Action::board = &board;
  Player::board = &board;
  Pion::players = &players;
  players.emplace_back(std::make_unique<Player>(0));
  for (size_t i{1}; i < initial_player_count + 1; ++i) {
    players.emplace_back(std::make_unique<Player>(i));
  }
}
int Game::kill_players() {
  for (size_t i{1}; i < players.size(); ++i) {
    int castle_count = 0;
    for (const auto& piece : players[i]->get_pieces()) {
      if (board[piece].get_type() == PieceType::Chateau) {
        castle_count++;
      }
    }
    if (!castle_count) {
      players.erase(players.begin() + i);
      if (curr_player_i >= i) {
        --curr_player_i;
      }
      --curr_player_count;
      return 1;
    }
  }
  return 0;
}

int Game::receive_command(const UserInput& inputs) {
  if (inputs.act >= 0) {
    return act(inputs);
  } else if (inputs.act == END_TURN) {
    return end_turn();
  } else if (inputs.act == CREATE_UNIT) {
    return create_unit(inputs);
  }
  std::unreachable();
}

int Game::act(const UserInput& inputs) {
  actions[board[inputs.src].get_actions()[inputs.act]]->perform(
      inputs.src, inputs.opt, inputs.dst);
  if (kill_players()) {
    if (curr_player_count == 1) {
      return 2;
    }
    return 1;
  }
  return 0;
}

int Game::end_turn() {
  turn++;
  curr_player_i = curr_player_i < players.size() - 1 ? curr_player_i + 1 : 1;
  players[curr_player_i]->start_turn();
  return 0;
}

int Game::create_unit(const UserInput& inputs) {
  PieceType type;
  switch (inputs.opt) {
    case CREATE_FORET:
      type = PieceType::Foret;
      break;
    case CREATE_MINE:
      type = PieceType::Mine;
      break;
    case CREATE_CHATEAU:
      type = PieceType::Chateau;
      break;
    case CREATE_PAYSAN:
      type = PieceType::Paysan;
      break;
    default:
      fprintf(stderr, "%d\n", (int)type);
      assert(0);
      std::unreachable();
  }
  assert(inputs.src.y < players.size());
  Player& pl = *players[inputs.src.y];
  board[inputs.dst] = Pion(type, pl, inputs.dst);
  pl.add_piece(inputs.dst);
  if (curr_player_i) {
    if (curr_player_i != initial_player_count || pl.get_pieces().size() != 2) {
      curr_player_i =
          curr_player_i < players.size() - 1 ? curr_player_i + 1 : 1;
    } else {
      end_turn();
      return 1;
    }
  }
  return 0;
}
