// #ifndef ACTION_HPP
// #define ACTION_HPP
// #include <vector>
//
// #include "../../helpers/action_data.hpp"
// #include "../../helpers/coords.hpp"
// #include "../../helpers/piece_data.hpp"
// #include "../board.hpp"
// #include "../game.hpp"
// #include "../pion.hpp"
// #include "../player.hpp"
//
// class Action {
//  public:
//   Action(Game& game, Board& board) : game{game}, board{board} {}
//   Game& game;
//   Board& board;
//   constexpr virtual ActionID get_type() const = 0;
//   constexpr bool takes_options() const { return
//   act_takes_options(get_type()); } constexpr bool takes_squares() const {
//   return act_takes_squares(get_type()); } virtual bool
//   get_fatigue_condition(const Pion& actor) const {
//     return actor.get_fatigue() < 1;
//   }
//   virtual std::vector<Coord> get_opts(const Pion& actor) const { return {}; }
//   virtual std::vector<Coord> get_dsts(Coord source) const { return {}; }
//   virtual ActionStatus perform(const UserInput& in) = 0;
//   virtual ~Action() = default;
//
//  protected:
//   const Player& get_owner(const Pion& piece) const;
//   Player& get_owner(const Pion& piece);
//   ActionStatus helper_create_new_unit(Coord src, PieceType type, Coord dst);
//   std::vector<Coord> helper_get_free_neighbours(Coord src) const;
//   std::vector<Coord> helper_get_foe_neighbours(Coord src) const;
//   bool helper_can_afford(const Pion& actor, PieceType type) const;
// };
//
// class EndTurn : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::end_turn; }
//   bool get_fatigue_condition(const Pion& actor) const override { return true;
//   } ActionStatus perform(const UserInput&) override;
// };
//
///*
// For error checking, it would be better to store the things in the actions
// itself
//-> Instead of having to recheck, simply check if in
//*/
///*
// Failure conditions:
// player already has unit
// opt not valid unit
// dst out of bounds
// dst occupied
//*/
// class CreateUnit : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return
//   ActionID::create_unit; } std::vector<Coord> get_opts(const Pion& actor)
//   const override {
//     const auto& pieces{game.get_curr_player().get_pieces()};
//     if (pieces.size() == 2) {
//       return {};
//     }
//     std::vector<Coord> curr_options{{{to_index(OptionID::create_chateau), 1},
//                                      {to_index(OptionID::create_paysan),
//                                      1}}};
//     if (pieces.empty()) {
//       return curr_options;
//     }
//     curr_options[board[pieces[0]].get_type() != PieceType::Chateau].x = 0;
//     return curr_options;
//   }
//   bool get_fatigue_condition(const Pion&) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
///*
// Failure conditions:
// src out of bounds
// src not valid piece
// src not current player
// not enough gold
// opt not valid unit
// dst out of bounds
// dst occupied
//*/
// class BuildUnit final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::build_unit;
//   } std::vector<Coord> get_opts(const Pion& actor) const override;
//   std::vector<Coord> get_dsts(Coord source) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class BuildTower final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return
//   ActionID::build_tower; } bool get_fatigue_condition(const Pion& actor)
//   const override; std::vector<Coord> get_dsts(Coord source) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class BuildBombardier final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::build_bomb;
//   } bool get_fatigue_condition(const Pion& actor) const override;
//   std::vector<Coord> get_dsts(Coord source) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class Explode final : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::explode; }
//   ActionStatus perform(const UserInput& in) override;
// };
///*
// Maybe separate get_dsts from whether it takes squares
//-> CreateUnits has valid squares but we do not want to show them
//-> Explode targets other squares even though it has no target
//
//*/
// class Cut final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::cut; }
//   bool get_fatigue_condition(const Pion& actor) const override;
//   std::vector<Coord> get_dsts(Coord source) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class Attack final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::attack; }
//   bool get_fatigue_condition(const Pion& actor) const override;
//   std::vector<Coord> get_dsts(Coord source) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class Convert final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::convert; }
//   bool get_fatigue_condition(const Pion& actor) const override;
//   std::vector<Coord> get_dsts(Coord source) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class Idle final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::idle; }
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class Harvest final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::farm; }
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class ToCastle final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::to_castle;
//   } bool get_fatigue_condition(const Pion& actor) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class ToMonk final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::to_monk; }
//   bool get_fatigue_condition(const Pion& actor) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class ResearchUpgrade final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override {
//     return ActionID::research_upgrade;
//   }
//   bool get_fatigue_condition(const Pion& actor) const override;
//   virtual std::vector<Coord> get_opts(const Pion& actor) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class Move final : public Action {
//  public:
//   using Action::Action;
//   constexpr ActionID get_type() const override { return ActionID::move; }
//   std::vector<Coord> get_dsts(Coord source) const override;
//   ActionStatus perform(const UserInput& in) override;
// };
// #endif
