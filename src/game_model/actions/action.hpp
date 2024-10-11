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

class Action {
 public:
  Action(Game& game, Board& board) : game{game}, board{board} {}
  Game& game;
  Board& board;
  constexpr virtual ActionID get_type() const = 0;
  constexpr bool takes_options() const { return act_takes_options(get_type()); }
  constexpr bool takes_squares() const { return act_takes_squares(get_type()); }
  virtual bool other_cond(const Pion& actor) const {
    return actor.get_fat() < 1;
  }
  virtual std::vector<OptionData> get_opts(const Pion&) const { return {}; }
  virtual std::vector<Coord> get_dsts(Coord) const { return {}; }
  virtual ActionStatus perform(const UserInput& in) = 0;
  virtual ~Action() = default;

 protected:
  Player& get_owner(const Pion& piece) {
    return game.get_player(piece.get_player_i());
  }
  const Player& get_owner(const Pion& piece) const {
    return game.get_player(piece.get_player_i());
  }
  ActionStatus helper_create_new_unit(Coord src, PieceType type, Coord dst);
  std::vector<Coord> helper_get_free_neighbours(Coord src) const;
  std::vector<Coord> helper_get_foe_neighbours(Coord src) const;
  bool helper_can_afford(const Pion& actor, PieceType type) const;
};

class EndTurn : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::end_turn; }
  bool other_cond(const Pion&) const override { return true; }
  ActionStatus perform(const UserInput&) override;
};

class CreateUnit : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::create_unit; }
  std::vector<OptionData> get_opts(const Pion& actor) const override;
  bool other_cond(const Pion&) const override { return game.get_turn() < 1; }
  ActionStatus perform(const UserInput& in) override;
};

class BuildUnit final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::build_unit; }
  std::vector<OptionData> get_opts(const Pion& actor) const override;
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};

class BuildTower final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::build_tower; }
  bool other_cond(const Pion& actor) const override {
    return helper_can_afford(actor, PieceType::Tour);
  }
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};

class BuildBombardier final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::build_bomb; }
  bool other_cond(const Pion& actor) const override {
    return helper_can_afford(actor, PieceType::Bombardier);
  }
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};

class Explode final : public Action {
 public:
  using Action::Action;
  ActionID get_type() const override { return ActionID::explode; }
  ActionStatus perform(const UserInput& in) override;
};

class Cut final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::cut; }
  bool other_cond(const Pion& actor) const override {
    return actor.get_fat() < 2;
  }
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};

class Attack final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::attack; }
  bool other_cond(const Pion& actor) const override {
    if (actor.get_type() == PieceType::Chateau &&
        !get_owner(actor).has_research(0)) {
      return false;
    }
    return actor.get_fat() < 2;
  }
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};

class Convert final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::convert; }
  bool other_cond(const Pion& actor) const override {
    return actor.get_fat() < 2;
  }
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};

class Idle final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::idle; }
  ActionStatus perform(const UserInput& in) override;
};

class Harvest final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::farm; }
  ActionStatus perform(const UserInput& in) override;
};

class ToCastle final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::to_castle; }
  bool other_cond(const Pion& actor) const override {
    return helper_can_afford(actor, PieceType::Chateau);
  }
  ActionStatus perform(const UserInput& in) override;
};

class ToMonk final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::to_monk; }
  bool other_cond(const Pion& actor) const override {
    return helper_can_afford(actor, PieceType::Moine);
  }
  ActionStatus perform(const UserInput& in) override;
};

class ResearchUpgrade final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override {
    return ActionID::research_upgrade;
  }
  bool other_cond(const Pion& actor) const override {
    return !actor.get_fat() && !get_owner(actor).has_research(0);
  }
  virtual std::vector<OptionData> get_opts(const Pion& actor) const override;
  ActionStatus perform(const UserInput& in) override;
};

class Move final : public Action {
 public:
  using Action::Action;
  constexpr ActionID get_type() const override { return ActionID::move; }
  std::vector<Coord> get_dsts(Coord source) const override;
  ActionStatus perform(const UserInput& in) override;
};
#endif
