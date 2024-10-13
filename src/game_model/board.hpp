#ifndef BOARD_HPP
#define BOARD_HPP

// #include <array>
#include <algorithm>
#include <vector>

#include "../helpers/coords.hpp"
#include "../signalhandler.hpp"
#include "pion.hpp"
class Board {
 public:
  explicit constexpr Board(Coord dims)
      : board{std::vector<Pion>(std::size_t(dims.y * dims.x), Pion{})}, B_H{dims.y}, B_W{dims.x} {}

  constexpr auto&& operator[](this auto&& self, Coord coords) {
    return std::forward<decltype(self)>(self).board[coords.y * self.B_W + coords.x];
  }
  constexpr auto&& operator[](this auto&& self, int y, int x) {
    return std::forward<decltype(self)>(self).board[y * self.B_W + x];
  }

  coordval get_height() const { return B_H; };
  coordval get_width() const { return B_W; };

  bool in_bounds(Coord c) const { return c.y < B_H && c.x < B_W; }  // since unsigned

  std::vector<Coord> get_neighbours(Coord square, int range) const {
    return get_neighbours(square, range, [](Pion) { return true; });
  }

  std::vector<Coord> get_neighbours(Coord square, int range, std::predicate<Pion> auto pred) const {
    std::vector<Coord> neighbours;
    int y{square.y};
    int x{square.x};
    for (int dy = 0; dy <= range; ++dy) {
      for (int dx = dy == 0; dx + dy <= range; ++dx) {
        int y_pos = y + dy;
        int y_neg = y - dy;
        int x_pos = x + dx;
        int x_neg = x - dx;
        if (y_pos < B_H) {
          if (x_pos < B_W) {
            if (pred((*this)[y_pos, x_pos])) { neighbours.emplace_back(y_pos, x_pos); }
          }
          if (x_neg != x_pos) {
            if (x_neg >= 0) {
              if (pred((*this)[y_pos, x_neg])) { neighbours.push_back(Coord(y_pos, x_neg)); }
            }
          }
        }
        if (y_neg == y_pos) { continue; }
        if (y_neg >= 0) {
          if (x_pos < B_W) {
            if (pred((*this)[y_neg, x_pos])) { neighbours.push_back(Coord(y_neg, x_pos)); }
          }
          if (x_neg != x_pos) {
            if (x_neg >= 0) {
              if (pred((*this)[y_neg, x_neg])) { neighbours.push_back(Coord(y_neg, x_neg)); }
            }
          }
        }
      }
    }
    std::ranges::sort(neighbours);
    //  auto dup = std::ranges::adjacent_find(neighbours);
    //  if (dup != neighbours.end()) { fprintf(stderr, "%d,%d found twice\n\n", dup->y, dup->x); }
    //  massert(std::ranges::adjacent_find(neighbours) == neighbours.end());
    return neighbours;
  }

  // template <typename Pred>
  // std::vector<Coord> get_neighbours(Coord square, unsigned int range, Pred pred) const
  //   requires(std::predicate<Pred, const Pion&>)
  //{
  //   std::vector<Coord> squares;
  //   int y{square.y};
  //   int x{square.x};
  //   /*
  //   fix this shit
  //
  //  */
  //  for (unsigned int dy = 0; dy <= range; ++dy) {
  //    for (unsigned int dx = dy == 0; dx + dy <= range; ++dx) {
  //      Coord c;
  //      int ny = y + dy;
  //      int nx = x + dx;
  //      c = {coordval(ny), coordval(nx)};
  //      if (ny < B_H && nx < B_W && pred((*this)[c])) { squares.push_back(c); }
  //      ny = y - dy;
  //      c = {coordval(ny), coordval(nx)};
  //      if (c.y >= 0 && c.x < B_W && pred((*this)[c])) { squares.push_back(c); }
  //      nx = x - dx;
  //      c = {coordval(ny), coordval(nx)};
  //      if (c.y >= 0 && c.x >= 0 && pred((*this)[c])) { squares.push_back(c); }
  //      ny = y + dy;
  //      c = {coordval(ny), coordval(nx)};
  //      if (c.y < B_H && c.x >= 0 && pred((*this)[c])) { squares.push_back(c); }
  //    }
  //  }
  //  fprintf(stderr, "Size neighbours: %lu\n", squares.size());
  //  std::ranges::sort(squares);
  //  massert(std::ranges::adjacent_find(squares) != squares.end());
  //  return squares;
  //}
  // std::vector<int> encode() {
  //  std::vector<int> encoded;
  //  // encoded.reserve(static_cast<std::size_t>(B_H * B_W));
  //  // for (int y = 0; y < B_H; ++y) {
  //  //   for (int x = 0; x < B_W; ++x) {
  //  //     encoded.push_back(*reinterpret_cast<int*>(&board[y * B_W + x]));
  //  //   }
  //  // }
  //  return encoded;
  //}
  const auto& get_raw() const { return board; }

 private:
  // std::array<Pion, 1024> board{};

  std::vector<Pion> board;
  coordval B_H;
  coordval B_W;
};

#endif
