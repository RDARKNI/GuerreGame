#ifndef PION_HPP
#define PION_HPP
#include <array>
#include <cassert>

#include "../helpers/action_data.hpp"
#include "../helpers/coords.hpp"
#include "../helpers/piece_data.hpp"

class Player;
/*
Every Pion is owned by a Player via a vector of unique_ptrs (not just a vector
of Pions to prevent Object Slicing in case I want to give some Pions more
attributes). Every square on the game board also has a non-owning Pion* (raw
pointer) to keep track of where the Pions are. Every Pion puts itself on the
destructor)
*/
class Pion {
 public:
  inline static std::vector<std::unique_ptr<Player>>* players;
  Pion(PieceType type, Player& p, Coord coords);
  constexpr Pion() : type{0}, player_index{0}, pv{0}, fatigue{0} {}
  operator bool() const { return type != PieceType::Nullpion; }
  Player& get_player() const { return *(*players)[player_index]; }
  PieceType get_type() const { return type; }
  unsigned char get_fatigue() const { return fatigue; }
  unsigned char get_pv() const { return pv; }
  unsigned char get_puiss() const { return pieces_data[char(type)].puiss; }
  unsigned char get_depl() const { return pieces_data[char(type)].depl; }
  unsigned char get_prod() const { return pieces_data[char(type)].prod; }
  unsigned char get_atkran() const { return pieces_data[char(type)].atk_ran; }
  unsigned char get_max_pv() const { return pieces_data[char(type)].max_hp; }
  unsigned char get_max_depl() const { return pieces_data[char(type)].depl; }
  unsigned char get_cout() const { return pieces_data[char(type)].cout; }

  const std::array<ActionID, 8>& get_actions() const {
    return pieces_data[char(type)].actions;
  }
  void set_player(Player& new_player);
  void change_fatigue(int i);
  void change_pv(int val);

  void start_turn() { this->change_fatigue(-3); }

 private:
  PieceType type;
  unsigned char player_index;
  unsigned char pv;
  unsigned char fatigue;
  // unsigned char reserved[4]{};
};

#endif
