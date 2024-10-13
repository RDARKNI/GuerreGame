#include "game.hpp"

#include "actions/action.hpp"
#include "player.hpp"

Game::Game(UserInput settings)
    : board{settings.src},
      actions{{std::make_unique<Move>(*this, board), std::make_unique<CreateUnit>(*this, board),
               std::make_unique<EndTurn>(*this, board), std::make_unique<Move>(*this, board),
               std::make_unique<Attack>(*this, board), std::make_unique<BuildUnit>(*this, board),
               std::make_unique<Harvest>(*this, board), std::make_unique<ToCastle>(*this, board),
               std::make_unique<Idle>(*this, board), std::make_unique<Convert>(*this, board),
               std::make_unique<ToMonk>(*this, board), std::make_unique<Cut>(*this, board),
               std::make_unique<Explode>(*this, board), std::make_unique<BuildTower>(*this, board),
               std::make_unique<ResearchUpgrade>(*this, board),
               std::make_unique<BuildBombardier>(*this, board), std::make_unique<CreateChateau>(*this, board),
               std::make_unique<CreatePaysan>(*this, board)}},
      initial_player_count(settings.dst.y),
      curr_player_count(settings.dst.y) {
  players[0] = Player{0, PlayerStatus::Gaia};
  for (unsigned char i{1}; i < initial_player_count + 1; ++i) {
    players[i] = Player{i, PlayerStatus::Alive};
  }
}
void Game::next_turn() {
  ++turn;
  Player& player{get_curr_player()};
  for (Coord sq : player.get_pieces()) {
    Pion& piece{board[sq]};
    if (piece.get_type() == PieceType::Chateau) { player.change_gold(+piece.get_prod()); }
    piece.start_turn();
  }
}
bool Game::kill_player_if(unsigned char index) {
  int castle_count{0};
  for (auto piece : players[index].get_pieces()) {
    if (board[piece].get_type() == PieceType::Chateau) { castle_count++; }
  }
  if (!castle_count) {
    for (auto piece : players[index].get_pieces()) {
      board[piece] = Pion();
    }
    players[index] = Player(index, PlayerStatus::Dead);
    --curr_player_count;
    return true;
  }
  return false;
}

ActionStatus Game::receive_command(const UserInput& inputs) {
  // fprintf(stderr, "(%d,%d),%d,%d,(%d,%d)\n", inputs.src.y, inputs.src.x,
  //         (int)inputs.act, (int)inputs.opt, inputs.dst.y, inputs.dst.x);
  return actions[index(inputs.act)]->perform(inputs);
}
