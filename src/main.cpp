
#include <format>
#include <iostream>
#include <memory>
#include <print>
#include <type_traits>

#include "controller/controller.hpp"
#include "controller/network_helper.hpp"
#include "helpers/coords.hpp"
#include "ui/menu_ui.hpp"
#include "ui/ncurses_setup.hpp"

int main(int argc, char** argv) {
  static_assert(std::is_move_constructible_v<Pion>);
  static_assert(std::is_move_constructible_v<Player>);
  static_assert(std::is_move_assignable_v<Pion>);
  static_assert(std::is_move_assignable_v<Player>);
  init_ncurses();
  MenuUI menu;
  while (!menu.menu_loop()) {
    Controller game_session{""};  // here
    game_session.game_loop();
  }
  endwin();
  return 0;
}
