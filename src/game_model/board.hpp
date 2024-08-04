#ifndef BOARD_HPP
#define BOARD_HPP
#include <cassert>
#include <vector>

#include "../game_config.hpp"
#include "../helpers/coords.hpp"
#include "pion.hpp"
class Board {
 public:
  Pion*& operator[](Coord coords) {
    assert(in_bounds(coords));
    return board_[coords.y * B_W + coords.x];
  }
  const Pion* const& operator[](Coord coords) const {
    return board_[coords.y * B_W + coords.x];
  }
  bool in_bounds(Coord c) const {
    return c.y >= 0 && c.y < B_H && c.x >= 0 && c.x < B_W;
  }

  std::vector<Coord> get_neighbours(Coord square, int range = 1) const {
    std::vector<Coord> neighbours;
    for (int i = 1; i <= range; ++i) {
      if (square.y - i >= 0) {
        neighbours.emplace_back(square.y - i, square.x);
      }
      if (square.y + i < B_H) {
        neighbours.emplace_back(square.y + i, square.x);
      }
    }
    // case i=0
    for (int j = 1; j <= range; ++j) {
      if (square.x - j >= 0) {
        neighbours.emplace_back(square.y, square.x - j);
      }
      if (square.x + j < B_W) {
        neighbours.emplace_back(square.y, square.x + j);
      }
    }
    for (int i = 1; i <= range; ++i) {
      for (int j = 1; j <= range - i; ++j) {
        if (square.y - i >= 0 && square.x - j >= 0) {
          neighbours.emplace_back(square.y - i, square.x - j);
        }
        if (square.y - i >= 0 && square.x + j < B_W) {
          neighbours.emplace_back(square.y - i, square.x + j);
        }
        if (square.y + i < B_H && square.x - j >= 0) {
          neighbours.emplace_back(square.y + i, square.x - j);
        }
        if (square.y + i < B_H && square.x + j < B_W) {
          neighbours.emplace_back(square.y + i, square.x + j);
        }
      }
    }
    return neighbours;
  }

 private:
  std::vector<Pion*> board_{B_H * B_W, nullptr};
};
#endif
