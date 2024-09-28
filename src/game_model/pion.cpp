#include "pion.hpp"

#include "player.hpp"

Pion::Pion(PieceType type, Player& p, Coord coords)
    : type{type},
      player_index{p.get_colour()},
      pv{pieces_data[char(type)].max_hp},
      fatigue{2} {}

void Pion::set_player(Player& new_player) {
  player_index = new_player.get_colour();
}

void Pion::change_fatigue(int i) {
  if (fatigue + i > 0) {
    fatigue += i;
  } else {
    fatigue = 0;
  }
}
void Pion::change_pv(int val) {
  int max = get_max_pv();
  int new_pv{pv + val};
  if (new_pv > max) {
    pv = max;
  } else if (new_pv <= 0) {
    *this = Pion();
  } else {
    pv = new_pv;
  }
}
