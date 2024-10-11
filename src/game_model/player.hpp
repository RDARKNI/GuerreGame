#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <vector>

#include "../helpers/coords.hpp"
enum class PlayerStatus : unsigned char {
  gaia,
  uninit,
  alive,
  dead,
};
class Player {
  using enum PlayerStatus;

 public:
  explicit Player(unsigned char colour, PlayerStatus state = uninit)
      : colour{colour}, status{state} {};
  explicit Player() : colour{0}, status{uninit} {}
  bool operator==(const Player &other) const = default;

  const std::vector<Coord> &get_pieces() const { return pieces; }
  PlayerStatus get_status() const { return status; }
  unsigned char get_colour() const { return colour; }
  int get_gold() const { return gold; }
  bool has_research(int i) const { return research & (1 << i); }

  void add_piece(Coord piece_coords) { pieces.push_back(piece_coords); }
  void remove_piece(Coord piece_coords) {
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
      if (*it == piece_coords) {
        pieces.erase(it);
        return;
      }
    }
    std::unreachable();
  }
  void set_status(PlayerStatus new_status) { status = new_status; }
  void change_gold(int g) { gold += g; }
  void add_research(int i) { research += 1 << i; }

 private:
  std::vector<Coord> pieces;
  unsigned char colour;
  PlayerStatus status;
  int gold{20};
  int research{0};
};

#endif
