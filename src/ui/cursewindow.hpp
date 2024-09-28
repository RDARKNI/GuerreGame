#ifndef __CURSEWINDOW_H__
#define __CURSEWINDOW_H__

#include <curses.h>

#include <vector>

#include "../game_config.hpp"
#include "../helpers/coords.hpp"
#include "../helpers/dtos.hpp"
#include "../helpers/user_input.hpp"
#include "../ui/ncurses_setup.hpp"

inline int yo = 0;
inline int xo = 0;

enum class rel_pos {
  above,
  below,
  right,
  left,
};
/*
Todo: Make work without datahandler to use these for the Menu UI
Make the design more logical
*/

inline chtype to_chtype(SquareData sqdat, bool mark = false) {
  return pieces_data[static_cast<size_t>(sqdat.type())].sign |
         COLOR_PAIR(sqdat.player() + (mark ? 16 : 0)) |
         (WA_DIM * static_cast<int>(sqdat.player() != 0 && sqdat.cannot_act()));
}
class SubWindow {
 public:
  ~SubWindow() {
    hide();
    delwin(win);
  }
  void hide() {
    wclear(win);
    wrefresh(win);
  }
  Coord size;
  Coord pos;

 protected:
  SubWindow(Coord size, Coord pos)
      : win{subwin(stdscr, size.y, size.x, pos.y, pos.x)},
        size{size},
        pos{pos} {}

  /*
  offs describe distance to window
  */
  SubWindow(Coord size, const SubWindow& other, rel_pos dir, int v_off = 0,
            int h_off = 0)
      : size{size} {
    using enum rel_pos;
    switch (dir) {
      case above:
        pos = {char(other.pos.y - size.y - v_off), char(other.pos.x + h_off)};
        break;
      case below:
        pos = {char(other.pos.y + other.size.y + v_off),
               char(other.pos.x + h_off)};
        break;
      case right:
        pos = {char(other.pos.y + v_off),
               char(other.pos.x + other.size.x + h_off)};
        break;
      case left:
        pos = {char(other.pos.y + v_off), char(other.pos.x + size.x - h_off)};
        break;
    }
    win = subwin(stdscr, size.y, size.x, pos.y, pos.x);
  }
  WINDOW* win;
};

class BoardWindow : public SubWindow {
 public:
  BoardWindow(Coord size, Coord position,
              const std::vector<Coord>& curr_targets)
      : SubWindow{size, position},
        curr_targets{curr_targets},
        inner_window{subwin(win, size.y - 2, size.x - 2, position.y + 1,
                            position.x + 1)} {
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(win);
  }
  ~BoardWindow() { delwin(inner_window); }
  void move_cursor(Coord coords) {
    curs_set(2);
    wmove(inner_window, coords.y, coords.x);
    wrefresh(inner_window);
  }

  void refresh_square(Coord coords, SquareData sqdat) {
    mvwaddch(inner_window, coords.y, coords.x, to_chtype(sqdat, false));
  }
  void refresh_win() { wrefresh(inner_window); }

  void redraw_square(Coord coords, SquareData sqdat) {
    mvwaddch(inner_window, coords.y, coords.x, to_chtype(sqdat, false));
    wrefresh(inner_window);
  }

  void mark_squares(std::vector<SquareData>&& sqdats) {
    unmark_squares();
    prevs = sqdats;
    marked_squares = curr_targets;
    for (int i = 0; auto sq : marked_squares) {
      mvwaddch(inner_window, sq.y, sq.x, to_chtype(prevs[i++], true));
    }
    wrefresh(inner_window);
  }
  void unmark_squares() {
    for (int i = 0; auto sq : marked_squares) {
      mvwaddch(inner_window, sq.y, sq.x, to_chtype(prevs[i++], false));
    }
    marked_squares.clear();
    prevs.clear();
    wrefresh(inner_window);
  }
  void discard_markings() {
    marked_squares.clear();
    prevs.clear();
  }

 private:
  std::vector<Coord> marked_squares;
  std::vector<SquareData> prevs;
  WINDOW* inner_window;
  const std::vector<Coord>& curr_targets;
};

/*
exists for SquareDat (Piece Info), PlayerInfo and Turn Window
*/
#include <cassert>
class IPWindow : public SubWindow {
 public:
  IPWindow(Coord size, Coord position) : SubWindow{size, position} {}
  IPWindow(Coord size, const SubWindow& other, rel_pos dir, int v_off = 0,
           int h_off = 0)
      : SubWindow(size, other, dir, v_off, h_off) {}
  void set_inner(bool i) { in_inner = i; }
  void move_cursor(int index) {
    if (in_inner) {
      curs_set(2);
      mvwaddstr(win, 1, 0, ip_address);
      wmove(win, 1, index);
    } else {
      curs_set(0);
      mvwaddstr(win, 0, 0, "Enter Host IP Address");
      wattrset(win, (index == 0) * A_STANDOUT);
      mvwaddstr(win, 1, 0, ip_address);
      wattrset(win, (index == 1) * A_STANDOUT);
      mvwaddstr(win, 2, 0, "Confirm");
      wattrset(win, (index == 2) * A_STANDOUT);
      mvwaddstr(win, 3, 0, "Back");
      wattrset(win, A_NORMAL);
    }
    wrefresh(win);
  }
  void show() { move_cursor(-1); }

 private:
  bool in_inner{false};
};

template <typename T>
class InfoWindow : public SubWindow {
 public:
  InfoWindow(Coord size, Coord position, const T& sqdat)
      : SubWindow{size, position}, data{sqdat} {}
  InfoWindow(Coord size, const SubWindow& other, rel_pos dir, const T& sqdat,
             int v_off = 0, int h_off = 0)
      : SubWindow(size, other, dir, v_off, h_off), data{sqdat} {}
  void show() {
    werase(win);
    curs_set(0);
    if constexpr (std::same_as<T, SquareData>) {
      show_sqdat();
    } else if constexpr (std::same_as<T, Coord>) {
      show_pldat();
    } else if constexpr (std::is_integral_v<T>) {
      show_turn();
    }
    wrefresh(win);
  }

 private:
  const T& data;

  inline void show_sqdat()
    requires(std::same_as<T, SquareData>)
  {
    if (data.data) {
      const auto& cdata = pieces_data[static_cast<size_t>(data.type())];
      wattrset(win, COLOR_PAIR(data.player()));
      mvwaddstr(win, 0, 0, cdata.name);
      wattrset(win, A_NORMAL);
      mvwprintw(win, 1, 0,
                "HP    %d/%d\n"
                "FAT   %d\n"
                "DEPL  %d\n"
                "PUIS  %d\n"
                "RANGE %d\n"
                "PROD  %d\n",
                data.hp(), cdata.max_hp, data.fatigue(), cdata.depl,
                cdata.puiss, cdata.atk_ran, cdata.prod);
    }
  }
  inline void show_pldat()
    requires(std::same_as<T, Coord>)
  {
    wattrset(win, COLOR_PAIR(data.y));
    mvwaddstr(win, 0, 0, player_names[data.y]);
    wattrset(win, A_NORMAL);
    mvwprintw(win, 1, 0, "%d Gold", data.x);
  }
  inline void show_turn()
    requires(std::is_integral_v<T>)
  {
    mvwprintw(win, 0, 0, "Turn %3d", data);
  }
};
/*
THIS

*/

/*
Windows either have:
Fixed/non-fixed strings size
Coords/void, input


*/
class SaveFileMenu : public SubWindow {
 public:
  SaveFileMenu(Coord size, Coord position,
               const std::vector<const char*>& strings)
      : SubWindow{size, position}, strings{strings} {}
  SaveFileMenu(Coord size, const SubWindow& other, rel_pos dir,
               const std::vector<const char*>& strings, int v_off = 0,
               int h_off = 0)
      : SubWindow(size, other, dir, v_off, h_off), strings{strings} {}
  void move_cursor(int sely) { render(sely); }

  void show() { render(); }

 private:
  const std::vector<const char*>& strings;
  void render(int sely = -1) {
    werase(win);
    curs_set(0);
    int offs = 0;
    if (sely >= size.y) {
      offs = (sely - size.y) + 1;
    }
    for (int i = 0; i < strings.size(); ++i) {
      wattrset(win, (i + offs == sely) * A_STANDOUT);
      mvwaddstr(win, i, 0, strings[i + offs]);
    }
    wattrset(win, A_NORMAL);

    wrefresh(win);
  }
};

template <typename T, std::size_t N>
class MenuWindow : public SubWindow {
 public:
  MenuWindow(Coord size, Coord position,
             const std::array<const char*, N>& strings, const T& data)
      : SubWindow{size, position}, strings{strings}, data{data} {}
  MenuWindow(Coord size, const SubWindow& other, rel_pos dir,
             const std::array<const char*, N>& strings, const T& data,
             int v_off = 0, int h_off = 0)
      : SubWindow(size, other, dir, v_off, h_off),
        strings{strings},
        data{data} {}
  void move_cursor(int sely)
    requires(!std::same_as<T, UserInput>)
  {
    render(sely);
  }

  void move_cursor(int sely, int selx)
    requires(std::same_as<T, UserInput>)
  {
    render(sely, selx);
  }
  void show() { render(); }

 private:
  const std::array<const char*, N>& strings;
  const T& data;
  void render(int sely = -1, int selx = -1) {
    werase(win);
    curs_set(0);
    int offs = 0;
    if (sely >= size.y) {
      offs = (sely - size.y) + 1;
    }

    if constexpr (std::same_as<T, UserInput>) {
      render_settings(sely, selx, offs);
    } else if constexpr (std::same_as<T, std::vector<Coord>>) {
      render_coord(sely, offs);
    }
    wrefresh(win);
  }

  inline void render_settings(int sely, int selx, int offs)
    requires(std::same_as<T, UserInput>)
  {
    for (int i = 0; i < strings.size(); ++i) {
      wattrset(win, (sely == i && !selx) * A_STANDOUT);
      mvwaddstr(win, i, 0, strings[i]);
    }
    wattrset(win, (selx == 1 && sely == 0) * A_STANDOUT);
    mvwprintw(win, 0, 17, "  %d  ", data.dst.y);
    wattrset(win, A_NORMAL);
    for (char i = 0; i < data.dst.y; ++i) {
      mvwaddch(win, 0, 26 + i * 2, 'P' | COLOR_PAIR(i + 1));
    }
    wattrset(win, (selx == 1 && sely == 1) * A_STANDOUT);
    mvwprintw(win, 1, 17, "%2dx%-2d", data.src.y, data.src.x);

    wattrset(win, (selx == 1 && sely == 2) * A_STANDOUT);
    mvwprintw(win, 2, 17, " %2d%%  ", data.act);
    wattrset(win, A_NORMAL);
    wrefresh(win);
  }
  inline void render_coord(int sel, int offs)
    requires(std::same_as<T, std::vector<Coord>>)
  {
    for (int i = 0; i < data.size(); ++i) {
      wattrset(win, ((i + offs == sel) * A_STANDOUT) | (!data[i].x * A_DIM));
      mvwaddstr(win, i, 0, strings[data[i + offs].y]);
    }
    wattrset(win, A_NORMAL);
  }
};

template <std::size_t N>
class MenuWindow<void, N> : public SubWindow {
 public:
  MenuWindow(Coord size, Coord position,
             const std::array<const char*, N>& strings)
      : SubWindow{size, position}, strings{strings} {}
  MenuWindow(Coord size, const SubWindow& other, rel_pos dir,
             const std::array<const char*, N>& strings, int v_off = 0,
             int h_off = 0)
      : SubWindow(size, other, dir, v_off, h_off), strings{strings} {}
  void move_cursor(int pos) { render(pos); }

  void show() { render(); }

 private:
  const std::array<const char*, N>& strings;

  void render(int sely = -1) {
    werase(win);
    curs_set(0);
    int offs = 0;
    if (sely >= size.y) {
      offs = (sely - size.y) + 1;
    }
    for (int i = 0; i < strings.size(); ++i) {
      wattrset(win, (i + offs == sely) * A_STANDOUT);
      mvwaddstr(win, i, 0, strings[i + offs]);
    }
    wattrset(win, A_NORMAL);
    wrefresh(win);
  }
};

class NewWindow {
 public:
  NewWindow(const NewWindow&) = delete;
  NewWindow& operator=(const NewWindow&) = delete;
  NewWindow(NewWindow&&) = delete;
  NewWindow& operator=(NewWindow&&) = delete;

 protected:
  NewWindow(Coord size, Coord position)
      : win{newwin(size.y, size.x, position.y, position.x)} {}
  ~NewWindow() {
    delwin(win);
    touchwin(stdscr);
    refresh();
  }
  WINDOW* win;
};

constexpr inline Coord man_dims{17, 50};

class ManualWindow : public NewWindow {
 public:
  explicit ManualWindow(Coord size, Coord pos) : NewWindow{size, pos} {
    mvwaddstr(win, 2, 6, manual);
    wborder(win, '+', '+', '+', '+', '+', '+', '+', '+');
    wrefresh(win);
  }

 private:
  constexpr inline static const char* manual{
      "               MANUAL             \n"
      "                                  \n"
      "     WASD / ARROWS  :  MOVE CURSOR\n"
      "     C / ENTER      :  CONFIRM    \n"
      "     B / BACKSPACE  :  BACK       \n"
      "     N / TAB        :  NEXT TURN  \n"
      "     SPACE          :  NEXT PIECE \n"
      "     M              :  MANUAL     \n"
      "     Z              :  QUIT       \n"
      "                                  \n"
      "     DESTROY FOE CASTLES          \n"};
};

inline void winscr(unsigned char winner) {
  curs_set(0);
  nodelay(stdscr, TRUE);
  attron(COLOR_PAIR(winner) | A_BLINK | A_BOLD | A_STANDOUT);
  int z = 0;
  int y;
  int x;
  getmaxyx(stdscr, y, x);
  while (true) {
    napms(500);
    erase();
    if (!SEIZURE_REDUCTION) {
      z = 1 - z;
      chtype ch = z ? '^' : '@';
      for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
          if (i % 2 + j % 2 + z % 2) {
            mvaddch(i, j, ch);
          }
        }
      }
    }
    mvprintw(y / 2 - 1, x / 3 + 4, "!!!Player %d wins!!!", winner);
    if (getch() != ERR) {
      break;
    }
    refresh();
  }
  clear();
  attroff(COLOR_PAIR(winner) | A_BLINK | A_BOLD | A_STANDOUT);
  nodelay(stdscr, false);
}

inline void flash_screen() {
  curs_set(0);
  for (int rep = 0; rep < 2; ++rep) {
    bkgd(A_REVERSE);
    refresh();
    napms(20);
    bkgd(A_NORMAL);
    refresh();
    napms(20);
  }
  refresh();
}

inline void convert_anim(int cp) {
  chtype traits = A_STANDOUT | COLOR_PAIR(cp);
  WINDOW* copy = newwin(screen_height, screen_width, 0, 0);
  overlay(stdscr, copy);
  wbkgd(stdscr, traits);
  refresh();
  napms(100);
  wbkgd(stdscr, A_NORMAL);
  overlay(copy, stdscr);
  delwin(copy);  // Delete the flash window
  refresh();     // Refresh stdscr to restore the original content
}

#endif
