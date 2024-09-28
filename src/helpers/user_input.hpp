#ifndef DTOS
#define DTOS
#include "coords.hpp"

struct UserInput {
  Coord src{};
  char act{0};
  char opt{};
  Coord dst{};
  friend std::ostream& operator<<(std::ostream& os, const UserInput& inp) {
    return os << "{" << inp.src << "," << inp.act << "," << inp.opt << ","
              << inp.dst << "}";
  }
  // #ifdef __clang__
  // #ifdef __apple_build_version__
  inline UserInput(Coord src, int act, int opt, Coord dst) noexcept
      : src{src}, act(act), opt(opt), dst{dst} {}
  inline UserInput() noexcept {}
  // #endif
  // #endif
};

#endif