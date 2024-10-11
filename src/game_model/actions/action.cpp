
#include "action.hpp"

#include <queue>
#include <unordered_set>

inline bool can_afford(const Player& player, PieceType type) {
  return player.get_gold() >= pieces_data[index(type)].cout;
}

ActionStatus Action::helper_create_new_unit(Coord src, PieceType type,
                                            Coord dst) {
  Pion& actor{board[src]};
  Pion& target{board[dst]};
  Player& player{get_owner(actor)};
  target = Pion{type, player.get_colour()};
  if (src != dst) {
    player.add_piece(dst);
  }
  player.change_gold(-pieces_data[index(type)].cout);
  actor.change_fatigue(+2);
  return ActionStatus::created_unit;
}

std::vector<Coord> Action::helper_get_free_neighbours(Coord src) const {
  return board.get_neighbours(src, 1,
                              [](const Pion& target) { return !target; });
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
bool Action::helper_can_afford(const Pion& actor, PieceType type) const {
  return !actor.get_fat() && can_afford(get_owner(actor), type);
}
std::vector<OptionData> ResearchUpgrade::get_opts(const Pion& actor) const {
  return {{OptionID::res_fortress, get_owner(actor).get_gold() >= 15}};
}
ActionStatus ResearchUpgrade::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  Player& player{get_owner(actor)};
  if (in.opt == OptionID::res_fortress) {
    player.change_gold(-15);
    player.add_research(0);
    actor.change_fatigue(+2);
  }
  return ActionStatus::ok;
}

template <>
struct std::hash<Coord> {
  std::size_t operator()(const Coord& c) const {
    return (c.y << max_coord_bits) + c.x;
  }
};

std::vector<Coord> Move::get_dsts(Coord source) const {
  const Pion& actor{board[source]};
  unsigned char index{actor.get_player_i()};
  auto mov{actor.get_depl()};
  auto not_foe = [index](const Pion& target) {
    return !target || target.get_player_i() == index;
  };
  std::vector<Coord> squares;
  std::queue<Coord> visited{{source}};  // squares we can move through
  std::unordered_set<Coord> bord{source};
  while (mov) {
    for (size_t i{visited.size()}; i; --i) {
      for (Coord sq : board.get_neighbours(visited.front(), 1, not_foe)) {
        if (bord.count(sq)) {
          continue;
        }
        bord.insert(sq);
        visited.push(sq);
        if (!board[sq]) {
          squares.push_back(sq);
        }
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
  return ActionStatus::ok;
}

ActionStatus ToMonk::perform(const UserInput& in) {
  return helper_create_new_unit(in.src, PieceType::Moine, in.src);
}

ActionStatus ToCastle::perform(const UserInput& in) {
  return helper_create_new_unit(in.src, PieceType::Chateau, in.src);
}
ActionStatus Harvest::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  get_owner(actor).change_gold(+actor.get_prod());
  actor.change_fatigue(+2);
  return ActionStatus::ok;
}
ActionStatus Idle::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  actor.change_pv(+2);
  actor.change_fatigue(+2);
  return ActionStatus::ok;
}
std::vector<Coord> Convert::get_dsts(Coord source) const {
  return helper_get_foe_neighbours(source);
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
    if (game.kill_player_if(target_player.get_colour())) {
      return ActionStatus::defeated_player;
    }
  }
  return ActionStatus::ok;
}
ActionStatus Attack::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  Pion& target{board[in.dst]};
  Player& target_player{get_owner(target)};
  actor.change_fatigue(+2);
  target.change_pv(-actor.get_puiss());
  if (!target.get_pv()) {
    if (target.get_type() == PieceType::Bombardier) {
      return Explode{game, board}.perform({in.dst});
    }
    target_player.remove_piece(in.dst);
    PieceType type{target.get_type()};
    target = Pion{};
    if (type == PieceType::Chateau) {
      if (game.kill_player_if(target_player.get_colour())) {
        return ActionStatus::defeated_player;
      }
    }
  }
  return ActionStatus::ok;
}

std::vector<Coord> Attack::get_dsts(Coord source) const {
  return helper_get_foe_neighbours(source);
}
ActionStatus Cut::perform(const UserInput& in) {
  Pion& actor{board[in.src]};
  Player& player{get_owner(actor)};
  Pion& target{board[in.dst]};
  target.change_pv(-actor.get_puiss());
  player.change_gold(+target.get_prod());
  if (!target.get_pv()) {
    player.change_gold(+target.get_prod());
    game.get_player(0).remove_piece(in.dst);
    target = Pion{};
  }
  actor.change_fatigue(+2);
  return ActionStatus::ok;
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
        if (target_type == PieceType::Chateau) {
          game.kill_player_if(target_player.get_colour());
        }
      }
    }
  }
  if (player.get_status() == PlayerStatus::alive) {
    actor = Pion{};
    player.remove_piece(in.src);
  }
  return ActionStatus::defeated_player;
}
ActionStatus BuildBombardier::perform(const UserInput& in) {
  return helper_create_new_unit(in.src, PieceType::Bombardier, in.dst);
}
std::vector<Coord> BuildBombardier::get_dsts(Coord source) const {
  return helper_get_free_neighbours(source);
}

ActionStatus BuildTower::perform(const UserInput& in) {
  return helper_create_new_unit(in.src, PieceType::Tour, in.dst);
}
std::vector<Coord> BuildTower::get_dsts(Coord source) const {
  return helper_get_free_neighbours(source);
}

ActionStatus BuildUnit::perform(const UserInput& in) {
  using enum PieceType;
  PieceType type;
  switch (in.opt) {
    case OptionID::create_paysan:
      type = Paysan;
      break;
    case OptionID::create_seigneur:
      type = Seigneur;
      break;
    case OptionID::create_guerrier:
      type = Guerrier;
      break;
    case OptionID::create_archer:
      type = Archer;
      break;
    default:
      std::unreachable();
  }
  return helper_create_new_unit(in.src, type, in.dst);
}
std::vector<Coord> BuildUnit::get_dsts(Coord source) const {
  return helper_get_free_neighbours(source);
}
std::vector<OptionData> BuildUnit::get_opts(const Pion& actor) const {
  int gold{get_owner(actor).get_gold()};
  using enum OptionID;
  return {
      {create_paysan, gold >= pieces_data[index(PieceType::Paysan)].cout},
      {create_seigneur, gold >= pieces_data[index(PieceType::Seigneur)].cout},
      {create_guerrier, gold >= pieces_data[index(PieceType::Guerrier)].cout},
      {create_archer, gold >= pieces_data[index(PieceType::Archer)].cout}};
}
ActionStatus CreateUnit::perform(const UserInput& in) {
  PieceType type;
  switch (in.opt) {
    case OptionID::create_arbre:
      type = PieceType::Arbre;
      break;
    case OptionID::create_mine:
      type = PieceType::Mine;
      break;
    case OptionID::create_chateau:
      type = PieceType::Chateau;
      break;
    case OptionID::create_paysan:
      type = PieceType::Paysan;
      break;
    default:
      std::unreachable();
      // assert(0);
  }
  Player& player{game.get_player(in.src.y)};
  Pion& target{board[in.dst]};
  target = Pion{type, player.get_colour()};
  player.add_piece(in.dst);
  if (player.get_status() == PlayerStatus::alive) {
    game.next_player();
    if (player.get_colour() == game.get_initial_player_count() &&
        player.get_pieces().size() == 2) {
      game.next_turn();
      return ActionStatus::new_turn;
    }
  }
  return ActionStatus::created_unit;
}

std::vector<OptionData> CreateUnit::get_opts(const Pion&) const {
  const auto& pieces{game.get_curr_player().get_pieces()};
  if (pieces.size() == 2) {
    return {};
  }
  std::vector<OptionData> curr_options = {{OptionID::create_chateau, 1},
                                          {OptionID::create_paysan, 1}};
  if (pieces.empty()) {
    return curr_options;
  }
  curr_options[board[pieces[0]].get_type() != PieceType::Chateau].mods = 0;
  return curr_options;
}
ActionStatus EndTurn::perform(const UserInput&) {
  game.next_player();
  game.next_turn();
  return ActionStatus::new_turn;
}
