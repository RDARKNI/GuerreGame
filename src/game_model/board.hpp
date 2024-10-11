#ifndef BOARD_HPP
#define BOARD_HPP

// #include <array>
#include <vector>

#include "../helpers/coords.hpp"
#include "../signalhandler.hpp"
#include "pion.hpp"
class Board {
 public:
  explicit constexpr Board(Coord dims)
      : board{std::vector<Pion>(std::size_t(dims.y * dims.x), Pion{})},
        B_H{dims.y},
        B_W{dims.x} {
        }

  constexpr auto &&operator[](this auto &&self, Coord coords) {
    return std::forward<decltype(self)>(self)
        .board[coords.y * self.B_W + coords.x];
  }

  Pion &operator[](Coord coords) { return board[coords.y * B_W + coords.x]; }
  const Pion &operator[](Coord coords) const {
    return board[coords.y * B_W + coords.x];
  }
  // since unsigned
  bool in_bounds(Coord c) const { return c.y < B_H && c.x < B_W; }

  std::vector<Coord> get_neighbours(Coord square, int range) const {
    return get_neighbours(square, range, [](const Pion &) { return true; });
  }
  template <typename Pred>
  std::vector<Coord> get_neighbours(Coord square, unsigned int range,
                                    Pred pred) const
    requires(std::predicate<Pred, const Pion &>)
  {
    std::vector<Coord> neighbours;
    int y{square.y};
    int x{square.x};
    for (unsigned int dy = 0; dy <= range; ++dy) {
      for (unsigned int dx = dy == 0; dx + dy <= range; ++dx) {
        Coord c{coordval(y + dy), coordval(x + dx)};
        fprintf(stderr, "neighbour: %d, %d\n", c.y, c.x);
        massert(in_bounds(c));
        if (c.y < B_H && c.x < B_W && pred((*this)[c])) {
          neighbours.push_back(c);
        }
        c.y = y - dy;
        if (c.y < B_H && c.x < B_W && pred((*this)[c])) {
          neighbours.push_back(c);
        }
        c.x = x - dx;
        if (c.y < B_H && c.x < B_W && pred((*this)[c])) {
          neighbours.push_back(c);
        }
        c.y = y + dy;
        if (c.y < B_H && c.x < B_W && pred((*this)[c])) {
          neighbours.push_back(c);
        }
      }
    }
    return neighbours;
  }
  std::vector<int> encode() {
    std::vector<int> encoded;
    // encoded.reserve(static_cast<std::size_t>(B_H * B_W));
    // for (int y = 0; y < B_H; ++y) {
    //   for (int x = 0; x < B_W; ++x) {
    //     encoded.push_back(*reinterpret_cast<int*>(&board[y * B_W + x]));
    //   }
    // }
    return encoded;
  }

 private:
  std::vector<Pion> board;
  // std::array<Pion, 1024> board{};
  coordval B_H;
  coordval B_W;
};

#endif
