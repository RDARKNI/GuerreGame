#ifndef ACTION_HPP
#define ACTION_HPP
#include <vector>

#include "../../helpers/action_data.hpp"
#include "../../helpers/coords.hpp"
#include "../../helpers/piece_data.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../pion.hpp"
#include "../player.hpp"
/*
Todo:
Get rid of options, simplfying the action space
Perhaps have one action for build units ingame and one for creating the out of
nothing
*/

enum class ActionPossible { Possible, Impossible, Invisible };

class Action {
 public:
  Action(Game& game, Board& board) : game{game}, board{board} {}
  constexpr virtual ActionID get_type() const = 0;
  constexpr bool takes_opts() const { return act_takes_options(get_type()); }
  constexpr bool takes_dsts() const { return act_takes_dst(get_type()); }
  virtual bool other_cond(Pion actor) const { return actor.get_fat() < 1; }
  virtual std::vector<OptionData> get_opts(const Pion&) const { return {}; }
  virtual std::vector<Coord> get_dsts(Coord) const { return {}; }
  virtual ActionStatus perform(const UserInput& in) = 0;
  virtual ~Action() = default;

 protected:
  Game& game;
  Board& board;
  Player& get_owner(const Pion& piece) { return game.get_player(piece.get_player_i()); }
  const Player& get_owner(const Pion& piece) const { return game.get_player(piece.get_player_i()); }
  ActionStatus helper_create_new_unit(Coord src, PieceType type, Coord dst);
  ActionStatus helper_turn_into_unit(Coord src, PieceType type);

  std::vector<Coord> h_get_free_neighbours(Coord src) const;
  std::vector<Coord> helper_get_foe_neighbours(Coord src) const;
  bool h_can_afford(const Pion& actor, PieceType type) const;
};

class EndTurn : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::EndTurn; }
  bool other_cond(Pion) const override { return true; }
  ActionStatus perform(const UserInput&) override;
};

class CreateUnit : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::CreateUnit; }
  std::vector<OptionData> get_opts(const Pion& actor) const override;
  bool other_cond(Pion) const override { return game.get_turn() < 0; }
  ActionStatus perform(const UserInput& in) override;
};

class BuildUnit final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildUnit; }
  std::vector<OptionData> get_opts(const Pion& actor) const override;
  std::vector<Coord> get_dsts(Coord source) const override { return h_get_free_neighbours(source); }
  ActionStatus perform(const UserInput& in) override;
};

class BuildTower final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildTower; }
  bool other_cond(Pion actor) const override { return h_can_afford(actor, PieceType::Tour); }
  std::vector<Coord> get_dsts(Coord source) const override { return h_get_free_neighbours(source); }
  ActionStatus perform(const UserInput& in) override {
    return helper_create_new_unit(in.src, PieceType::Tour, in.dst);
  }
};

class BuildBombardier final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildBomb; }
  bool other_cond(Pion src) const override { return h_can_afford(src, PieceType::Bombardier); }
  std::vector<Coord> get_dsts(Coord source) const override { return h_get_free_neighbours(source); }
  ActionStatus perform(const UserInput& in) override {
    return helper_create_new_unit(in.src, PieceType::Bombardier, in.dst);
  }
};

class Explode final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::Explode; }
  ActionStatus perform(const UserInput& in) override;
};

class Cut final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::Cut; }
  bool other_cond(Pion actor) const override { return actor.get_fat() < 2; }
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};

class Attack final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::Attack; }
  bool other_cond(Pion actor) const override {
    if (actor.get_type() == PieceType::Chateau && !get_owner(actor).has_research(0)) { return false; }
    return actor.get_fat() < 2;
  }
  std::vector<Coord> get_dsts(Coord source) const override { return helper_get_foe_neighbours(source); }
  ActionStatus perform(const UserInput& in) override;
};

class Convert final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::Convert; }
  bool other_cond(Pion actor) const override { return actor.get_fat() < 2; }
  std::vector<Coord> get_dsts(Coord source) const override { return helper_get_foe_neighbours(source); }
  ActionStatus perform(const UserInput& in) override;
};

class Idle final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::Idle; }
  ActionStatus perform(const UserInput& in) override;
};

class Harvest final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::Farm; }
  ActionStatus perform(const UserInput& in) override;
};

class ToCastle final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::ToCastle; }
  bool other_cond(Pion actor) const override { return h_can_afford(actor, PieceType::Chateau); }
  ActionStatus perform(const UserInput& in) override {
    return helper_turn_into_unit(in.src, PieceType::Chateau);
  }
};

class ToMonk final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::ToMonk; }
  bool other_cond(Pion actor) const override { return h_can_afford(actor, PieceType::Moine); }
  ActionStatus perform(const UserInput& in) override {
    return helper_turn_into_unit(in.src, PieceType::Moine);
  }
};

class ResearchUpgrade final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::Research; }
  bool other_cond(Pion actor) const override { return !actor.get_fat() && !get_owner(actor).has_research(0); }
  virtual std::vector<OptionData> get_opts(const Pion& actor) const override;
  ActionStatus perform(const UserInput& in) override;
};

class Move final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::Move; }
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};

// change this
class CreatePaysan final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildChateau; }
  virtual bool other_cond(Pion) const override {
    const auto& pieces = game.get_curr_player().get_pieces();
    return game.get_turn() < 1 && (pieces.empty() || board[pieces[0]].get_type() != PieceType::Paysan);
  }
  ActionStatus perform(const UserInput& in) override {
    Pion& actor{board[in.src]};
    Player& player{game.get_curr_player()};
    actor = Pion(PieceType::Paysan, player.get_colour());
    player.add_piece(in.src);
    game.next_player();
    if (player.get_colour() == game.get_initial_player_count() && player.get_pieces().size() == 2) {
      game.next_turn();
      return ActionStatus::NewTurn;
    }
    return ActionStatus::Ok;
  }
};
// change this
class CreateChateau final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildChateau; }
  virtual bool other_cond(Pion) const override {
    const auto& pieces = game.get_curr_player().get_pieces();
    return game.get_turn() < 1 && (pieces.empty() || board[pieces[0]].get_type() != PieceType::Chateau);
  }
  ActionStatus perform(const UserInput& in) override {
    Pion& actor{board[in.src]};
    Player& player{game.get_curr_player()};
    actor = Pion(PieceType::Chateau, player.get_colour());
    player.add_piece(in.src);
    game.next_player();
    if (player.get_colour() == game.get_initial_player_count() && player.get_pieces().size() == 2) {
      game.next_turn();
      return ActionStatus::NewTurn;
    }
    return ActionStatus::Ok;
  }
};

class BuildArbre final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildArbre; }
  std::vector<Coord> get_dsts(Coord source) const override { return h_get_free_neighbours(source); }
  ActionStatus perform(const UserInput& in) override;
};

class BuildMine final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildMine; }
  std::vector<Coord> get_dsts(Coord source) const override { return h_get_free_neighbours(source); }
  ActionStatus perform(const UserInput& in) override;
};

class BuildArcher final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildArcher; }
  std::vector<Coord> get_dsts(Coord source) const override { return h_get_free_neighbours(source); }
  ActionStatus perform(const UserInput& in) override;
};

class BuildGuerrier final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::BuildGuerrier; }
  std::vector<Coord> get_dsts(Coord source) const override { return h_get_free_neighbours(source); }
  ActionStatus perform(const UserInput& in) override;
};

#endif
