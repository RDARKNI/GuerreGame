#ifndef COORDS
#define COORDS

using coordval = unsigned char;
struct Coord {
  coordval y{}, x{};
  constexpr bool operator==(const Coord& other) const = default;
  // constexpr operator std::pair<char, char>() { return std::pair{y, x}; }
  //  friend std::ostream& operator<<(std::ostream& os, const Coord& coord) {
  //    return os << "(" << coord.y << "," << coord.x << ")";
  //  }
  //   #ifdef __clang__
  //   #ifdef __apple_build_version__
  // constexpr inline Coord(int y, int x) noexcept : y(y), x(x) {}
  // constexpr inline Coord(char y, char x) noexcept : y(y), x(x) {}

  // constexpr inline Coord() noexcept {}
  //    #endif
  //    #endif
};
inline constexpr unsigned char coord_senitel{255};
inline constexpr int max_coord_bits{8};

#endif
