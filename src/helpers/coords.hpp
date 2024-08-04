#ifndef COORDS
#define COORDS
#include <ostream>
struct Coord {
  short y;
  short x;
  Coord() : y{0}, x{0} {}
  Coord(int y, int x) : y{static_cast<char>(y)}, x{static_cast<char>(x)} {}
  constexpr bool operator==(const Coord& other) const = default;
  friend std::ostream& operator<<(std::ostream& os, const Coord& c) {
    return os << "(" << c.y << "," << c.x << ")";
  }
};

#endif