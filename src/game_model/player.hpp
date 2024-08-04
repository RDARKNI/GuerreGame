#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <memory>
#include <vector>

#include "../helpers/piece_data.hpp"
class Pion;
class Player {
 public:
  static inline Board *board;

  Player(size_t colour) : colour_{colour} { pieces_.reserve(B_H); };
  int get_colour() const { return colour_; }

  int get_gold() const { return gold_; }
  void change_gold(int g) { gold_ += g; }

  bool get_is_their_turn() const { return is_their_turn_; }

  void start_turn() {
    is_their_turn_ = true;
    for (auto &piece : pieces_) {
      if (piece->get_type() == CHATEAU) {
        change_gold(+piece_constants[CHATEAU].prod);
      }
      piece->start_turn();
    }
  }

  void end_turn() { is_their_turn_ = false; }

  bool operator==(const Player &other) const = default;

  const std::vector<std::unique_ptr<Pion>> &get_pieces() const {
    return pieces_;
  }

  void add_piece(std::unique_ptr<Pion> piece) {
    pieces_.push_back(std::move(piece));
    (*board)[pieces_.back()->get_coords()] = pieces_.back().get();
  }

  std::unique_ptr<Pion> remove_piece(Pion &piece) {
    for (auto it = pieces_.begin(); it != pieces_.end(); ++it) {
      if ((*it).get() == &piece) {
        std::unique_ptr<Pion> tmp = std::move(*it);
        (*board)[tmp->get_coords()] = nullptr;
        pieces_.erase(it);
        return tmp;
      }
    }
    return nullptr;
  }

 private:
  std::vector<std::unique_ptr<Pion>> pieces_;
  size_t colour_;
  int gold_{20};
  bool is_their_turn_{false};
};

#endif
