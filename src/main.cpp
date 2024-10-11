#include <signal.h>

#include <csignal>

#include "controller/controller.hpp"
#include "game_config.hpp"
#include "network/network_helper.hpp"
#include "save/save.hpp"
#include "signalhandler.hpp"
#include "ui/menu_ui.hpp"
int main() {
#ifndef _WIN32
  signal(SIGINT, sigint_handler);  // Register signal handler for Ctrl+C
  signal(SIGSEGV, sigint_handler);
#endif
  init_ncurses();
  while (true) {
    MenuUI menu;
    std::vector<UserInput> hist = menu.menu_loop();
    if (hist.empty()) {
      return 0;
    }
    // assert(0 <= B_H && B_H < max_h && 0 <= B_W && B_W < max_w && );

    hist = Controller{hist, role}.game_loop();
    save_game(hist);
    close_all_sockets();
  }
  shutdown_network();
  endwin();
  return 0;
}

// zig c++ -o ./bin/windows/guerre.exe -I./extern/ ./src/main.cpp
// ./extern/libpdcurses.a ./src/input/input_parser.cpp ./src/ui/menu_ui.cpp
// ./src/ui/ncurses_setup.cpp ./src/game_model/actions/action.cpp
// ./src/game_model/game.cpp ./src/controller/controller.cpp
// ./src/controller/data_handler.cpp ./src/controller/network_helper.cpp
// ./src/save/save.cpp -std=c++23 -target x86_64-windows -lws2_32
