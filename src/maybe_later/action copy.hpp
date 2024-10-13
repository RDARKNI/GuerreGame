// #ifndef ACTION_HPP
// #define ACTION_HPP
// #include <vector>
//
// #include "../../helpers/actiontest.hpp"
// #include "../../helpers/coords.hpp"
// #include "../../helpers/piece_data.hpp"
// #include "../board.hpp"
// #include "../game.hpp"
// #include "../pion.hpp"
// #include "../player.hpp"
///*
// Todo:
// Get rid of options, simplfying the action space
// Perhaps have one action for build units ingame and one for creating the out
// of nothing
//*/
///*
// Options:
// Sometimes a unit is available only at certain points or for certain units to
// build
//-> Have unit store options for each action?
//->  -> Would require a lot
//
//*/
// enum class ActionPossible { possible, impossible, invisible };
//
// class Action {
//  public:
//   Action(Game& game, Board& board) : game{game}, board{board} {}
//   Game& game;
//   Board& board;
//   constexpr virtual ActionID get_type() const = 0;
//   constexpr virtual bool takes_options() const = 0;
//   constexpr bool takes_squares() const { return
//   act_takes_squares(get_type()); } virtual ActionPossible other_cond(const
//   Pion& actor) const {
//     return actor.get_fat() < 1;
//   }
//   virtual std::vector<OptionData> get_opts(const Pion&) const { return {}; }
//   virtual std::vector<Coord> get_dsts(Coord) const { return {}; }
//   virtual ActionStatus perform(const UserInput& in) = 0;
//   virtual ~Action() = default;
//
//  protected:
//   Player& get_owner(const Pion& piece) {
//     return game.get_player(piece.get_player_i());
//   }
//   const Player& get_owner(const Pion& piece) const {
//     return game.get_player(piece.get_player_i());
//   }
//   ActionStatus helper_create_new_unit(Coord src, PieceType type, Coord dst);
//   ActionStatus helper_turn_into_unit(Coord src, PieceType type);
//
//   std::vector<Coord> helper_get_free_neighbours(Coord src) const;
//   std::vector<Coord> helper_get_foe_neighbours(Coord src) const;
//   bool helper_can_afford(const Pion& actor, PieceType type) const;
// };
//
// class EndTurn : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::end_turn; }
//   constexpr bool takes_options() const override { return false; }
//
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class Idle : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::idle; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return false; }
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class Farm : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::farm; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return false; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class PlaceUnit : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::place_unit; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return true; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class BuildUnit : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::build_unit; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return true; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
//
// class ToUnit : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::to_unit; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return true; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class Move : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::move; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return false; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class Attack : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::attack; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return false; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class Cut : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::cut; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return false; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class Convert : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::convert; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return false; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
// class Explode : public Action {
//  public:
//   using Action::Action;
//   ActionID get_type() const override { return ActionID::explode; }
//   std::vector<OptionData> get_opts(const Pion& actor) const override;
//   constexpr bool takes_options() const override { return false; }
//
//   ActionPossible other_cond(const Pion&) const override {
//     return game.get_turn() < 0;
//   }
//   ActionStatus perform(const UserInput& in) override;
// };
