#ifndef PION_HPP
#define PION_HPP
#include <cassert>
#include <vector>

#include "../helpers/action_data.hpp"
#include "../helpers/coords.hpp"
#include "../helpers/piece_data.hpp"
class Board;
class Player;
/*
Every Pion is owned by a Player via a vector of unique_ptrs (not just a vector
of Pions to prevent Object Slicing in case I want to give some Pions more
attributes). Every square on the game board also has a non-owning Pion* (raw
pointer) to keep track of where the Pions are. Every Pion puts itself on the
board when it's created and deletes itself off it when it's destroyed (in its
destructor)
*/
class Pion {
 public:
  inline static Board* board;

  Pion(Player& p, Piece_ID type, Coord coords);

  Player& get_player() const { return *player_; }
  int get_fatigue() const { return fatigue_; }
  int get_pv() const { return pv_; }
  Coord get_coords() const { return coords_; }
  Piece_ID get_type() const { return type; }
  int get_puiss() const { return piece_constants[type].puiss; }
  int get_depl() const { return piece_constants[type].depl; }
  int get_prod() const { return piece_constants[type].prod; }
  int get_atkran() const { return piece_constants[type].atk_ran; }
  int get_max_pv() const { return piece_constants[type].max_hp; }
  int get_max_depl() const { return piece_constants[type].depl; }
  int get_cout() const { return piece_constants[type].cout; }

  const std::array<ActionID, 5>& get_actions() const;
  void set_player(Player& new_player);
  void set_square(Coord coords);
  void change_fatigue(int i);
  void change_pv(int val);

  void start_turn();

  bool operator==(const Pion&) const = default;

 protected:
  Player* player_;
  Coord coords_;
  Piece_ID type;
  int pv_;
  char fatigue_{2};
  void die();
};

#endif
