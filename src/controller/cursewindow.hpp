#ifndef __CURSEWINDOW_H__
#define __CURSEWINDOW_H__

#include <curses.h>

#include <vector>

#include "../controller/data_handler.hpp"
#include "../game_config.hpp"
#include "../helpers/coords.hpp"
#include "../helpers/user_input.hpp"
#include "../input_parser.hpp"
#include "../ui/ncurses_setup.hpp"
/*
Todo: Make work without datahandler to use these for the Menu UI
Make the design more logical
*/
class SubWindow {
 public:
  SubWindow(const DataHandler& dh, Coord size, Coord position)
      : dh{dh}, win{subwin(stdscr, size.y, size.x, position.y, position.x)} {}
  ~SubWindow() { hide(); }
  WINDOW*& get_win() { return win; }
  void hide() {
    wclear(win);
    wrefresh(win);
  }

 protected:
  const DataHandler& dh;
  WINDOW* win;
};

class BoardWindow : public SubWindow {
 public:
  BoardWindow(const DataHandler& dh, Coord position)
      : SubWindow{dh, {(int)B_H + 2, (int)B_W + 2}, position} {
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
  }
  void redraw_square(Coord coords) {
    mvwaddch(win, coords.y + 1, coords.x + 1, dh.get_sqdat(coords));
    wrefresh(win);
  }

  void unmark_squares() {
    for (auto sq : marked_squares) {
      mvwaddch(win, sq.y + 1, sq.x + 1, dh.get_sqdat(sq));
    }
    marked_squares.clear();
    wrefresh(win);
  }
  void mark_squares() {
    unmark_squares();
    marked_squares = dh.get_curr_targets();
    for (auto sq : marked_squares) {
      chtype ch = mvwinch(win, sq.y + 1, sq.x + 1);
      chtype pnum = COLOR_PAIR(PAIR_NUMBER(ch) + 16);
      mvwaddch(win, sq.y + 1, sq.x + 1,
               (ch & A_CHARTEXT) | pnum | (A_DIM & ch));
    }
    wrefresh(win);
  }

  void move_cursor(Coord coords) {
    curs_set(2);
    wmove(win, coords.y + 1, coords.x + 1);
    wrefresh(win);
  }

  void render() {
    for (int i = 0; i < B_H; ++i) {
      for (int j = 0; j < B_W; ++j) {
        redraw_square({i, j});
      }
    }
  }

 private:
  std::vector<Coord> marked_squares;
};

class InfoWindow : public SubWindow {
 public:
  InfoWindow(const DataHandler& dh, Coord position, const Coord& square)
      : SubWindow{dh, {8, 20}, position}, square{square} {}
  void show() {
    werase(win);
    curs_set(0);
    SquareData data = dh.get_piece_data(square);
    if (data.data) {  // todo
      const PieceConstants& pdata = piece_constants[data.type()];
      wattron(win, COLOR_PAIR(data.player()));
      mvwaddstr(win, 0, 1, pdata.name);
      wattroff(win, COLOR_PAIR(data.player()));
      mvwprintw(win, 1, 1, "HP    %d/%d", data.hp(), pdata.max_hp);
      mvwprintw(win, 2, 1, "FAT   %d", data.fatigue());
      mvwprintw(win, 3, 1, "DEPL  %d", pdata.depl);
      mvwprintw(win, 4, 1, "PUIS  %d", pdata.puiss);
      mvwprintw(win, 5, 1, "RANGE %d", pdata.atk_ran);
      mvwprintw(win, 6, 1, "PROD  %d", pdata.prod);
    }
    wrefresh(win);
  };

 private:
  const Coord& square;
};

class PlayerWindow : public SubWindow {
 public:
  PlayerWindow(const DataHandler& dh, Coord position)
      : SubWindow{dh, {3, 20}, position} {}
  void show() {
    curs_set(0);
    werase(win);
    PlayerData pdata = dh.get_current_player_data();
    wattron(win, COLOR_PAIR(pdata.index));
    mvwaddstr(win, 0, 1, player_names[pdata.index]);
    wattroff(win, COLOR_PAIR(pdata.index));
    mvwprintw(win, 1, 1, "%d Gold", pdata.gold);
    wrefresh(win);
  };
};

class TurnWindow : public SubWindow {
 public:
  TurnWindow(const DataHandler& dh, Coord position)
      : SubWindow{dh, {1, 15}, position} {}
  void show() {
    werase(win);
    mvwprintw(win, 0, 1, "Turn %3d", dh.get_current_turn());
    wrefresh(win);
  };
};

class MenuWindow : public SubWindow {
 public:
  MenuWindow(const DataHandler& dh, Coord position, const int& curs)
      : SubWindow{dh, {5, 20}, position}, curs{curs} {}
  virtual std::vector<MenuItem> get_data() = 0;
  void move_cursor() {
    wmove(win, curs, 1);
    render(curs);
  }
  void show() { render(); }

 private:
  void render(int sel = -1) {
    werase(win);
    curs_set(0);
    std::vector<MenuItem> data = get_data();
    for (int i = 0; i < data.size(); ++i) {
      int selval = (i == sel) * A_STANDOUT;
      wattron(win, (!data[i].value * A_DIM) | selval);
      mvwaddstr(win, i, 1, data[i].str);
      wattroff(win, (!data[i].value * A_DIM) | selval);
    }
    wrefresh(win);
  }
  const int& curs;
};

class ActionWindow : public MenuWindow {
 public:
  using MenuWindow::MenuWindow;
  std::vector<MenuItem> get_data() override { return dh.get_action_items(); }
};

class OptionWindow : public MenuWindow {
 public:
  using MenuWindow::MenuWindow;
  std::vector<MenuItem> get_data() override { return dh.get_option_items(); }
};

class NewWindow {
 public:
  NewWindow(const DataHandler& dh, Coord size, Coord position)
      : dh{dh}, win{newwin(size.y, size.x, position.y, position.x)} {}
  ~NewWindow() {
    delwin(win);
    touchwin(stdscr);
    refresh();
  }
  WINDOW*& get_win() { return win; }

 protected:
  const DataHandler& dh;
  WINDOW* win;
};

#define HEIGHTA 20
#define WIDTHA 70
class WinScreen : public NewWindow {
 public:
  WinScreen(const DataHandler& dh, Coord pos)
      : NewWindow{dh, {HEIGHTA, WIDTHA}, pos} {  // todo adjust size
    int colour = dh.get_current_player_data().index;
    curs_set(2);
    nodelay(win, TRUE);
    wattron(win, COLOR_PAIR(colour) | A_BLINK | A_BOLD | A_STANDOUT);
    int z = 0;
    while (1) {
      napms(500);
      werase(win);
      if (!SEIZURE_REDUCTION) {
        z = 1 - z;
        chtype ch = z ? '^' : '@';
        for (int i = 0; i < HEIGHTA; i++) {
          for (int j = 0; j < WIDTHA; j++) {
            if (i % 2 + j % 2 + z % 2) {
              mvwaddch(win, i, j, ch);
            }
          }
        }
      }
      mvwprintw(win, HEIGHTA / 2 - 1, WIDTHA / 3 + 4, "!!!Player %d wins!!!",
                colour);
      if (wgetch(win) != ERR) {
        return;
      }
      wrefresh(win);
    }
  }
};

class ManualWindow : public NewWindow {
 public:
  ManualWindow(const DataHandler& dh, Coord pos)
      : NewWindow{dh, {20, 20}, pos} {
    wborder(win, '+', '+', '+', '+', '+', '+', '+', '+');
    for (int i = 0; const char* str : manual_items) {
      mvwaddstr(win, 2 + i++, 4, str);
    }
    wrefresh(win);
  }

 private:
  inline static const char* manual_items[]{
      "          MANUAL",
      "",
      "WASD/ARROWS  :  MOVE CURSOR",
      "ENTER/C      :  SELECT",
      "B            :  GO BACK",
      "N/TAB        :  NEXT TURN",
      "SPACE        :  NEXT PIECE",
      "M            :  MANUAL",
      "ESC          :  QUIT",
      "",
      "DESTROY FOE CASTLES",
  };
};

static void flash_screen() {
  return;
  chtype traits = A_REVERSE;
  WINDOW* copy = newwin(screen_height, screen_width, 0, 0);
  overlay(stdscr, copy);
  for (int rep = 0; rep < 5; ++rep) {
    attron(traits);
    for (int i = 0; i < screen_height; ++i) {
      for (int j = 0; j < screen_width; ++j) {
        mvaddch(i, j, mvinch(i, j));
      }
    }
    refresh();
    napms(100);
    attroff(traits);
    for (int i = 0; i < screen_height; ++i) {
      for (int j = 0; j < screen_width; ++j) {
        mvaddch(i, j, mvinch(i, j) - traits);
      }
    }
    refresh();
    napms(100);
  }
  overlay(copy, stdscr);
  delwin(copy);
}

static void convert_anim(const DataHandler& dh, const UserInput& data) {
  int cp = dh.get_current_player_data().index;
  chtype traits = A_STANDOUT | A_DIM | COLOR_PAIR(cp);
  WINDOW* copy = newwin(screen_height, screen_width, 0, 0);
  overlay(stdscr, copy);
  attron(traits);
  for (int i = 0; i < screen_height; ++i) {
    for (int j = 0; j < screen_width; ++j) {
      mvaddch(i, j, mvinch(i, j));
    }
  }
  refresh();
  napms(300);
  attroff(traits);
  for (int i = 0; i < screen_height; ++i) {
    for (int j = 0; j < screen_width; ++j) {
      mvaddch(i, j, mvinch(i, j) - traits);
    }
  }
  refresh();
  overlay(copy, stdscr);
  delwin(copy);
}

#endif
