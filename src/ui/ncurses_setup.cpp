#include "ncurses_setup.hpp"

#define COLOR_ORANGE 8
#define COLOR_GREY 9
// process attach --name guerre --waitfor

inline constexpr short player_colours[9] = {
    COLOR_WHITE, COLOR_BLUE,    COLOR_RED,    COLOR_GREEN, COLOR_YELLOW,
    COLOR_CYAN,  COLOR_MAGENTA, COLOR_ORANGE, COLOR_GREY,
};

void init_ncurses() {
  initscr();
  noecho();
  start_color();
  init_color(COLOR_GREY, 500, 500, 500);
  init_color(COLOR_ORANGE, 1000, 647, 0);
  for (size_t i = 0; i < 9; ++i) {
    init_pair(i, player_colours[i], COLOR_BLACK);
    init_pair(i + 16, player_colours[i], COLOR_GREY);  // marked squares
  }

  getmaxyx(stdscr, screen_height, screen_width);
  while (screen_height < 10 || screen_width < 20) {
    mvprintw(0, 0, "INCREASE TERMINAL SIZE\n");
    napms(100);
    refresh();
    getmaxyx(stdscr, screen_height, screen_width);
  }
  keypad(stdscr, TRUE);
}
