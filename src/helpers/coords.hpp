#ifndef COORDS_H
#define COORDS_H
#include <compare>
using coordval = unsigned char;
struct Coord {
  coordval y{}, x{};
  constexpr auto operator<=>(const Coord& other) const = default;
  inline static constexpr coordval sentinelval{255};
  inline static constexpr int max_bits{8};
  static const Coord sentinel;
};
inline constexpr Coord Coord::sentinel = {sentinelval, sentinelval};

#endif

// constexpr operator
// std::pair<char, char>() {
// return std::pair{y, x}; }
//  friend std::ostream&
//  operator<<(std::ostream&
//  os, const Coord& coord) {
//    return os << "(" <<
//    coord.y << "," <<
//    coord.x << ")";
//  }
//   #ifdef __clang__
//   #ifdef
//   __apple_build_version__
// constexpr inline Coord(int
// y, int x) noexcept : y(y),
// x(x) {} constexpr inline
// Coord(char y, char x)
// noexcept : y(y), x(x) {}

// constexpr inline Coord()
// noexcept {}
//    #endif
//    #endif
// inline static constexpr
// coordval
// coord_senitel{255}; inline
// constexpr int
// max_coord_bits{8};
