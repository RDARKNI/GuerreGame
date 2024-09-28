#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <vector>

#include "../helpers/piece_data.hpp"
#include "board.hpp"
#include "pion.hpp"
class Player {
 public:
  inline static Board *board;

  explicit Player(int colour) : colour(colour) { pieces_.reserve(30); };

  unsigned char get_colour() const { return colour; }
  int get_gold() const { return gold_; }
  void change_gold(int g) { gold_ += g; }

  void start_turn() {
    for (Coord &piece : pieces_) {
      if ((*board)[piece].get_type() == PieceType::Chateau) {
        change_gold(+pieces_data[static_cast<size_t>(PieceType::Chateau)].prod);
      }
      (*board)[piece].start_turn();
    }
  }

  bool operator==(const Player &other) const = default;

  const std::vector<Coord> &get_pieces() const { return pieces_; }

  void add_piece(Coord piece_coords) {
    for (Coord c : pieces_) {
      assert(piece_coords != c);
    }
    pieces_.push_back(piece_coords);
  }
  void remove_piece(Coord piece_coords) {
    for (auto it = pieces_.begin(); it != pieces_.end(); ++it) {
      if (*it == piece_coords) {
        pieces_.erase(it);
        return;
      }
    }
    assert(0);
    std::unreachable();
  }

 private:
  /*
  A vector of Pions will NOT work since the board needs pointers to pions and we
  cannot have pointers to elements in vector (unstable)
  */
  std::vector<Coord> pieces_;
  int gold_{20};
  unsigned char colour;
};

#endif
