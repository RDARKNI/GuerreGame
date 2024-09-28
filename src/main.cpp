#include "controller/controller.hpp"
#include "game_config.hpp"
#include "save/save.hpp"
#include "ui/menu_ui.hpp"

int main() {
  init_ncurses();
  while (true) {
    MenuUI menu;
    std::vector<UserInput> hist = menu.menu_loop();
    if (hist.empty()) {
      return 0;
    }
    Controller game_session{hist, role};  // here
    save_game(game_session.game_loop());
  }
  endwin();
  return 0;
}
