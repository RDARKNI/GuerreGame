#ifndef PION_HPP
#define PION_HPP
#include <array>

#include "../helpers/action_data.hpp"
#include "../helpers/piece_data.hpp"

class Pion {
 public:
  explicit constexpr Pion(PieceType type, unsigned char player_i)
      : type{type}, player_i{player_i}, pv{get_max_pv()}, fatigue{2} {}
  constexpr Pion() : type{0}, player_i{0}, pv{0}, fatigue{0} {}

  PieceType get_type() const { return type; }
  unsigned char get_player_i() const { return player_i; }
  unsigned char get_pv() const { return pv; }
  unsigned char get_fat() const { return fatigue; }
  const char* get_name() const { return pieces_data[index(type)].name; }
  const std::array<ActionID, 8>& get_actions() const { return pieces_data[index(type)].actions; }
  unsigned char get_sign() const { return pieces_data[index(type)].sign; }
  unsigned char get_max_pv() const { return pieces_data[index(type)].max_hp; }
  unsigned char get_depl() const { return pieces_data[index(type)].depl; }
  unsigned char get_puiss() const { return pieces_data[index(type)].puiss; }
  unsigned char get_atkran() const { return pieces_data[index(type)].atk_ran; }
  unsigned char get_prod() const { return pieces_data[index(type)].prod; }
  unsigned char get_cout() const { return pieces_data[index(type)].cout; }

  explicit operator bool() const { return type != PieceType::Sentinel; }

  void set_player(unsigned char other) { player_i = other; }
  void start_turn() { change_fatigue(-3); }
  void change_fatigue(int i) { fatigue = fatigue + i > 0 ? fatigue + i : 0; }
  void change_pv(int val) {
    int new_pv{pv + val};  // int for overflow handling
    if (new_pv > get_max_pv()) {
      pv = get_max_pv();
    } else if (new_pv <= 0) {
      pv = 0;
    } else {
      pv = new_pv;
    }
  }

 private:
  PieceType type;
  unsigned char player_i;
  unsigned char pv;
  unsigned char fatigue;
};

#endif
