
#include "action.hpp"

#include <queue>
#include <unordered_set>

inline bool can_afford(const Player& player, PieceType type) {
  return player.get_gold() >= pieces_data[index(type)].cout;
}
/*
todo:

get_owner could be simplified since we always add to current player

*/
ActionStatus Action::helper_create_new_unit(Coord src, PieceType type, Coord dst) {
  Pion& actor{board[src]};
  Pion& target{board[dst]};
  Player& player{get_owner(actor)};
  target = Pion{type, player.get_colour()};
  player.add_piece(dst);
  player.change_gold(-pieces_data[index(type)].cout);
  actor.change_fatigue(+2);
  return ActionStatus::Ok;
}
ActionStatus Action::helper_turn_into_unit(Coord src, PieceType type) {
  Pion& actor{board[src]};
  Player& player{get_owner(actor)};
  actor = Pion{type, player.get_colour()};
  player.change_gold(-pieces_data[index(type)].cout);
  actor.change_fatigue(+2);
  return ActionStatus::Ok;
}
std::vector<Coord> Action::h_get_free_neighbours(Coord src) const {
  return board.get_neighbours(src, 1, [](const Pion& target) { return !target; });
}
std::vector<Coord> Action::helper_get_foe_neighbours(Coord src) const {
  const Pion& actor{board[src]};
  unsigned char index{actor.get_player_i()};
  auto is_foe = [index](const Pion& target) {
    unsigned char target_index{target.get_player_i()};
    return target_index && target_index != index;
  };
  return board.get_neighbours(src, actor.get_atkran(), is_foe);
}
bool Action::h_can_afford(const Pion& actor, PieceType type) const {
  return !actor.get_fat() && can_afford(get_owner(actor), type);
}
std::vector<OptionData> ResearchUpgrade::get_opts(const Pion& actor) const {
  return {{OptionID::ResearchFortress, get_owner(actor).get_gold() >= 15}};
}
ActionStatus ResearchUpgrade::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  Player& player{get_owner(actor)};
  if (in.opt == OptionID::ResearchFortress) {
    player.change_gold(-15);
    player.add_research(0);
    actor.change_fatigue(+2);
  }
  return ActionStatus::Ok;
}

template <>
struct std::hash<Coord> {
  std::size_t operator()(const Coord& c) const { return (c.y << Coord::max_bits) + c.x; }
};

std::vector<Coord> Move::get_dsts(Coord source) const {
  const Pion& actor{board[source]};
  unsigned char index{actor.get_player_i()};
  auto mov{actor.get_depl()};
  auto not_foe = [index](const Pion& target) { return !target || target.get_player_i() == index; };
  std::vector<Coord> squares;
  std::queue<Coord> visited{{source}};  // squares we can move through
  std::unordered_set<Coord> bord{source};
  while (mov) {
    for (size_t i{visited.size()}; i; --i) {
      for (Coord sq : board.get_neighbours(visited.front(), 1, not_foe)) {
        if (bord.count(sq)) { continue; }
        bord.insert(sq);
        visited.push(sq);
        if (!board[sq]) { squares.push_back(sq); }
      }
      visited.pop();
    }
    --mov;
  }
  return squares;
}
ActionStatus Move::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  Player& player{get_owner(actor)};
  Pion& target{board[in.dst]};
  player.remove_piece(in.src);
  player.add_piece(in.dst);
  actor.change_fatigue(+1);
  std::swap(actor, target);
  return ActionStatus::Ok;
}

ActionStatus Harvest::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  get_owner(actor).change_gold(+actor.get_prod());
  actor.change_fatigue(+2);
  return ActionStatus::Ok;
}
ActionStatus Idle::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  actor.change_pv(+2);
  actor.change_fatigue(+2);
  return ActionStatus::Ok;
}

ActionStatus Convert::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  Player& player{get_owner(actor)};
  Pion& target{board[in.dst]};
  Player& target_player{get_owner(target)};
  target.set_player(player.get_colour());
  target_player.remove_piece(in.dst);
  player.add_piece(in.dst);
  actor.change_fatigue(+4);
  target.change_fatigue(+2);
  if (target.get_type() == PieceType::Chateau) {
    if (game.kill_player_if(target_player.get_colour())) { return ActionStatus::KilledPlayer; }
  }
  return ActionStatus::Ok;
}
ActionStatus Attack::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  Pion& target{board[in.dst]};
  Player& target_player{get_owner(target)};
  actor.change_fatigue(+2);
  target.change_pv(-actor.get_puiss());
  if (!target.get_pv()) {
    if (target.get_type() == PieceType::Bombardier) { return Explode{game, board}.perform({in.dst}); }
    target_player.remove_piece(in.dst);
    PieceType type{target.get_type()};
    target = Pion{};
    if (type == PieceType::Chateau) {
      if (game.kill_player_if(target_player.get_colour())) { return ActionStatus::KilledPlayer; }
    }
  }
  return ActionStatus::Ok;
}

ActionStatus Cut::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  Player& player{get_owner(actor)};
  Pion& target{board[in.dst]};
  target.change_pv(-actor.get_puiss());
  player.change_gold(+target.get_prod());
  if (!target.get_pv()) {
    player.change_gold(+target.get_prod());
    // game.get_player(0).remove_piece(in.dst);
    target = Pion{};
  }
  actor.change_fatigue(+2);
  return ActionStatus::Ok;
}
std::vector<Coord> Cut::get_dsts(Coord source) const {
  constexpr auto is_res = [](const Pion& target) {
    PieceType type{target.get_type()};
    return type == PieceType::Arbre || type == PieceType::Mine;
  };
  return board.get_neighbours(source, board[source].get_atkran(), is_res);
}

ActionStatus Explode::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  int puiss{actor.get_puiss()};
  Player& player{get_owner(actor)};
  for (Coord square : board.get_neighbours(in.src, actor.get_atkran())) {
    Pion& target{board[square]};
    if (target) {
      Player& target_player{get_owner(target)};
      PieceType target_type{target.get_type()};
      target.change_pv(-puiss);
      if (!target.get_pv()) {
        target = Pion{};
        target_player.remove_piece(square);
        if (target_type == PieceType::Chateau) { game.kill_player_if(target_player.get_colour()); }
      }
    }
  }
  if (player.get_status() == PlayerStatus::Alive) {
    actor = Pion{};
    player.remove_piece(in.src);
  }
  return ActionStatus::KilledPlayer;
}

ActionStatus BuildUnit::perform(const UserInput& in) {
  PieceType type;
  switch (in.opt) {
    case OptionID::CreatePaysan  : type = PieceType::Paysan; break;
    case OptionID::CreateSeigneur: type = PieceType::Seigneur; break;
    case OptionID::CreateGuerrier: type = PieceType::Guerrier; break;
    case OptionID::CreateArcher  : type = PieceType::Archer; break;
    default                      : std::unreachable();
  }
  // todo remove
  return helper_create_new_unit(in.src, type, in.dst);
}

std::vector<OptionData> BuildUnit::get_opts(const Pion& actor) const {
  int gold{get_owner(actor).get_gold()};
  return {{OptionID::CreatePaysan, gold >= pieces_data[index(PieceType::Paysan)].cout},
          {OptionID::CreateSeigneur, gold >= pieces_data[index(PieceType::Seigneur)].cout},
          {OptionID::CreateGuerrier, gold >= pieces_data[index(PieceType::Guerrier)].cout},
          {OptionID::CreateArcher, gold >= pieces_data[index(PieceType::Archer)].cout}};
}

// Pion& actor{board[src]};
// Pion& target{board[dst]};
// Player& player{get_owner(actor)};
// target = Pion{type, player.get_colour()};
// if (src != dst) {
//   player.add_piece(dst);
// }
// player.change_gold(-pieces_data[index(type)].cout);
// actor.change_fatigue(+2);
// return ActionStatus::created_unit;

/*
Player doesn't need to be encoded since it's the current player anyways

*/
ActionStatus CreateUnit::perform(const UserInput& in) {
  PieceType type;
  switch (in.opt) {
    case OptionID::CreateArbre: type = PieceType::Arbre; break;
    case OptionID::CreateMine : type = PieceType::Mine; break;
    default                   : std::unreachable();
  }
  board[in.dst] = Pion(type, 0);
  // player.add_piece(in.dst);
  return ActionStatus::Ok;
}

std::vector<OptionData> CreateUnit::get_opts(const Pion&) const {
  return {{OptionID::CreateArbre, 1}, {OptionID::CreateMine, 1}};
}
ActionStatus EndTurn::perform(const UserInput&) {
  game.next_player();
  game.next_turn();
  return ActionStatus::NewTurn;
}
