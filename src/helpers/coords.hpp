#ifndef COORDS
#define COORDS
#include <ostream>

inline constexpr char max_coord_bits{8};
struct Coord {
  char y{0};
  char x{0};
  constexpr bool operator==(const Coord& other) const = default;
  friend std::ostream& operator<<(std::ostream& os, const Coord& coord) {
    return os << "(" << coord.y << "," << coord.x << ")";
  }
  // #ifdef __clang__
  // #ifdef __apple_build_version__
  // constexpr inline Coord(int y, int x) noexcept : y(y), x(x) {}
  // constexpr inline Coord() noexcept {}
  // #endif
  // #endif
};

#endif