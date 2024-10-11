// #ifndef __CURSEWINDOW_H__
// #define __CURSEWINDOW_H__
//
// #include <curses.h>
//
// #include <cassert>
// #include <span>
// #include <vector>
//
// #include "../game_config.hpp"
// #include "../game_model/board.hpp"
// #include "../game_model/pion.hpp"
// #include "../helpers/coords.hpp"
// #include "../helpers/user_input.hpp"
// #include "../ui/ncurses_setup.hpp"
// #define yoffs 3
// #define xoffs 5
//
// enum class rel_pos {
//   above,
//   below,
//   right,
//   left,
// };
//
// class SubWin {
//  public:
//   ~SubWin() {
//     if (win) {
//       wclear(win);
//       wrefresh(win);
//       delwin(win);
//     }
//     wrefresh(stdscr);
//   }
//   void refresh() { wrefresh(win); }
//
//   SubWin(const SubWin& other) = delete;
//   SubWin& operator=(const SubWin& other) = delete;
//   SubWin(SubWin&& other) noexcept = delete;
//
//   /*
//   instead of making the other delete everything, just delete the difference
//   in size
//   */
//   SubWin& operator=(SubWin&& other) noexcept {
//     if (this != &other) {
//       if (win) {
//         // WINDOW* copy =
//         //     newwin(other.size.y, other.size.x, other.pos.y, other.pos.x);
//         // overlay()
//
//         if (other.size.y < size.y) {
//           for (int i{other.size.y}; i <= size.y; ++i) {
//             wmove(win, i, 0);
//             wclrtoeol(win);
//           }
//           wrefresh(win);
//         }
//         if (other.size.x < size.x) {
//           for (int i{0}; i <= size.y; ++i) {
//             wmove(win, i, other.size.x);
//             wclrtoeol(win);
//           }
//           wrefresh(win);
//         }
//         delwin(win);
//       }
//       this->win = other.win;
//       this->pos = other.pos;
//       this->size = other.size;
//       other.win = nullptr;
//       other.pos = {};
//       other.size = {};
//       wrefresh(win);
//       wrefresh(stdscr);
//     }
//     return *this;
//   }
//   void hide() {
//     wclear(win);
//     wrefresh(win);
//   }
//   auto show(this auto&& self) {
//     werase(self.win);
//     curs_set(0);
//     self.display();
//     wattrset(self.win, A_NORMAL);
//     wrefresh(self.win);
//   }
//
//   WINDOW* win;
//   Coord size;
//   Coord pos;
//
//   SubWin(Coord size, Coord pos)
//       : win{subwin(stdscr, size.y, size.x, pos.y, pos.x)},
//         size{size},
//         pos{pos} {}
//   SubWin(Coord size, const SubWin& other, rel_pos dir, int v_off = 0,
//          int h_off = 0)
//       : size{size} {
//     switch (dir) {
//       using enum rel_pos;
//       case above:
//         pos = Coord(other.pos.y - size.y - v_off, other.pos.x + h_off);
//         break;
//       case below:
//         pos = Coord(other.pos.y + other.size.y + v_off, other.pos.x + h_off);
//         break;
//       case right:
//         pos = {Coord(other.pos.y + v_off, other.pos.x + other.size.x +
//         h_off)}; break;
//       case left:
//         pos = {Coord(other.pos.y + v_off, other.pos.x + size.x - h_off)};
//         break;
//     }
//     win = subwin(stdscr, size.y, size.x, pos.y, pos.x);
//   }
// };
// template <typename T>
// class SubWindow : public SubWin {
//  public:
//   SubWindow(Coord size, Coord pos, std::span<const char* const> strings,
//             const T* data)
//       : SubWin{size, pos}, strings{strings}, data{data} {}
//   SubWindow(Coord size, const SubWin& other, rel_pos dir,
//             std::span<const char* const> strings, const T* data, int v_off =
//             0, int h_off = 0)
//       : SubWin{size, other, dir, v_off, h_off}, strings{strings}, data{data}
//       {}
//   std::span<const char* const> strings;
//   const T* data;
// };
//
// class PlayerInfoWindow : public SubWindow<Coord> {
//  public:
//   using SubWindow::SubWindow;
//
//  public:
//   void display() {
//     wattrset(win, COLOR_PAIR(data->y));
//     mvwaddstr(win, 0, 0, strings[data->y]);
//     wattrset(win, A_NORMAL);
//     mvwprintw(win, 1, 0, "%d Gold", data->x);
//   }
// };
//
///*
// Previewing changes:
// Second object that shows previews
//
//
//
//*/
// class PieceInfoWindow : public SubWindow<Pion> {
//  public:
//   using SubWindow::SubWindow;
//
//  public:
//   void display() {
//     if (*data) {
//       const PieceConstants& cdata = pieces_data[size_t(data->get_type())];
//       wattrset(win, COLOR_PAIR(data->get_player_index()));
//       mvwaddstr(win, 0, 0, cdata.name);
//       wattrset(win, A_NORMAL);
//       mvwprintw(win, 1, 0, strings[0], data->get_pv(), cdata.max_hp);
//       mvwprintw(win, 2, 0, strings[1], data->get_fatigue());
//       mvwprintw(win, 3, 0, strings[2], cdata.depl);
//       mvwprintw(win, 4, 0, strings[3], cdata.puiss);
//       mvwprintw(win, 5, 0, strings[4], cdata.atk_ran);
//       mvwprintw(win, 6, 0, strings[5], cdata.prod);
//     }
//   }
//   // void display_preview(SquareData pdata) {
//   //   const PieceConstants& cdata = pieces_data[size_t(pdata.type())];
//   //   if (pdata.player() != data->player()) {
//   //     wattrset(win, COLOR_PAIR(data->player()));
//   //     mvwaddstr(win, 0, 0, cdata.name);
//   //     wattrset(win, A_NORMAL);
//   //   }
//   //   //"HP    %d/%d", "FAT   %d", "DEPL  %d", "PUIS  %d", "RANGE %d", "PROD
//   //   %d", if (pdata.hp()) {
//   //     mvwprintw(win, 1, 10, "%+d", pdata.hp());
//   //   }
//   //
//   //  mvwprintw(win, 2, 10, "%+d", strings[1], data->fatigue());
//   //  mvwprintw(win, 3, 10, "%+d", strings[2], cdata.depl);
//   //  mvwprintw(win, 4, 10, "%+d", strings[3], cdata.puiss);
//   //  mvwprintw(win, 5, 10, "%+d", strings[4], cdata.atk_ran);
//   //  mvwprintw(win, 6, 10, "%+d", strings[5], cdata.prod);
//   //}
// };
// class TurnInfoWindow : public SubWindow<int> {
//  public:
//   using SubWindow::SubWindow;
//
//  public:
//   void display() { mvwprintw(win, 0, 0, strings[0], *data); }
// };
// class SaveFileMenu : public SubWindow<std::vector<const char*>> {
//  public:
//   using SubWindow::SubWindow;
//   void move_cursor(size_t new_sely) {
//     sely = new_sely;
//     show();
//   }
//
//  public:
//   void display() {
//     size_t offs = 0;
//     if (sely >= size.y) {
//       offs = (sely - size.y) + 1;
//     }
//     for (size_t i = 0; i + offs < strings.size(); ++i) {
//       wattrset(win, (i + offs == sely) * A_STANDOUT);
//       mvwaddstr(win, i, 0, strings[i + offs]);
//     }
//   }
//   size_t sely;
// };
//
// class MainMenuWindow : public SubWindow<void> {
//  public:
//   using SubWindow::SubWindow;
//   void move_cursor(size_t new_sely) {
//     wattrset(win, A_NORMAL);
//     mvwaddstr(win, sely, 0, strings[sely]);
//     sely = new_sely;
//     wattrset(win, A_STANDOUT);
//     mvwaddstr(win, sely, 0, strings[sely]);
//     wattrset(win, A_NORMAL);
//     wrefresh(win);
//   }
//
//  public:
//   void display() {
//     for (size_t i = 0; i < strings.size(); ++i) {
//       mvwaddstr(win, i, 0, strings[i]);
//     }
//   }
//   size_t sely;
// };
//
// class ActionOptionWindow : public SubWindow<std::vector<Coord>> {
//  public:
//   using SubWindow::SubWindow;
//   void move_cursor(size_t new_sely) {
//     if (sely < data->size()) {
//       wattrset(win, !(*data)[sely].x ? A_DIM : 0);
//       mvwaddstr(win, sely, 0, strings[(*data)[sely].y]);
//     }
//     sely = new_sely;
//     wattrset(win, (!(*data)[sely].x * A_DIM) | A_STANDOUT);
//     mvwaddstr(win, sely, 0, strings[(*data)[sely].y]);
//     wattrset(win, A_NORMAL);
//     wrefresh(win);
//   }
//
//  public:
//   void display() {
//     for (size_t i = 0; i < data->size(); ++i) {
//       wattrset(win, !(*data)[i].x ? A_DIM : 0);
//       mvwaddstr(win, i, 0, strings[(*data)[i].y]);
//     }
//   }
//   size_t sely;
// };
//
// class IPWindow : public SubWindow<char[32]> {
//  public:
//   using SubWindow::SubWindow;
//   void move_cursor(size_t new_sely, size_t new_selx) {
//     wattrset(win, A_NORMAL);
//     if (selx == 1) {
//       mvwaddstr(win, 0, 17, ip_address);
//     } else {
//       mvwaddstr(win, sely, 0, strings[sely]);
//     }
//     sely = new_sely;
//     selx = new_selx;
//     if (selx == 1) {
//       mvwaddstr(win, 0, 17, ip_address);
//       curs_set(2);
//       wmove(win, 0, 17 + sely);
//     } else {
//       wattrset(win, A_STANDOUT);
//       mvwaddstr(win, sely, 0, strings[sely]);
//       curs_set(0);
//     }
//     wattrset(win, A_NORMAL);
//     wrefresh(win);
//   }
//
//  public:
//   void display() {
//     for (size_t i = 0; i < strings.size(); ++i) {
//       mvwaddstr(win, i, 0, strings[i]);
//     }
//     mvwaddstr(win, 0, 17, ip_address);
//   }
//   size_t sely;
//   size_t selx;
// };
//
// class SettingsWindow : public SubWindow<SettingsInput> {
//  public:
//   using SubWindow::SubWindow;
//   void move_cursor(size_t new_sely, size_t new_selx) {
//     wattrset(win, A_NORMAL);
//     if (selx == 0) {
//       mvwaddstr(win, sely, 0, strings[sely]);
//     } else {
//       if (sely == 0) {
//         mvwprintw(win, 0, 17, "  %d  ", data->dst.y);
//         for (char i = 0; i < data->dst.y; ++i) {
//           mvwaddch(win, 0, 26 + i * 2, 'P' | COLOR_PAIR(i + 1));
//         }
//       } else if (sely == 1) {
//         mvwprintw(win, 1, 17, "%2dx%-2d", data->src.y, data->src.x);
//       } else if (sely == 2) {
//         mvwprintw(win, 2, 17, " %2d%%  ", data->act);
//       }
//     }
//     sely = new_sely;
//     selx = new_selx;
//     wattrset(win, A_STANDOUT);
//     if (selx == 0) {
//       mvwaddstr(win, sely, 0, strings[sely]);
//     } else {
//       if (sely == 0) {
//         mvwprintw(win, 0, 17, "  %d  ", data->dst.y);
//         wattrset(win, A_NORMAL);
//         mvwprintw(win, 0, 26, "%16s", " ");
//         for (char i = 0; i < data->dst.y; ++i) {
//           mvwaddch(win, 0, 26 + i * 2, 'P' | COLOR_PAIR(i + 1));
//         }
//       } else if (sely == 1) {
//         mvwprintw(win, 1, 17, "%2dx%-2d", data->src.y, data->src.x);
//       } else if (sely == 2) {
//         mvwprintw(win, 2, 17, " %2d%%  ", data->act);
//       }
//     }
//     wattrset(win, A_NORMAL);
//     wrefresh(win);
//   }
//
//  public:
//   void display() {
//     for (size_t i = 0; i < strings.size(); ++i) {
//       mvwaddstr(win, i, 0, strings[i]);
//     }
//     mvwprintw(win, 0, 17, "  %d  ", data->dst.y);
//     for (char i = 0; i < data->dst.y; ++i) {
//       mvwaddch(win, 0, 26 + i * 2, 'P' | COLOR_PAIR(i + 1));
//     }
//     mvwprintw(win, 1, 17, "%2dx%-2d", data->src.y, data->src.x);
//     mvwprintw(win, 2, 17, " %2d%%  ", data->act);
//   }
//   size_t sely;
//   size_t selx;
// };
//
// class BoardWindow : public SubWin {
//  public:
//   BoardWindow(Coord size, Coord position,
//               const std::vector<Coord>& curr_targets, const Board& board,
//               const coordval& curr_player)
//       : SubWin{size, position},
//         curr_targets{&curr_targets},
//         board{&board},
//         curr_player{&curr_player} {
//     wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
//     wrefresh(win);
//   }
//   void move_cursor(Coord coords) {
//     curs_set(2);
//     wmove(win, 1 + coords.y, 1 + coords.x);
//     wrefresh(win);
//   }
//
//   void refresh_square(Coord coords) {
//     mvwaddch(win, 1 + coords.y, 1 + coords.x,
//              to_chtype((*board)[coords], false));
//   }
//   void redraw_square(Coord coords) {
//     refresh_square(coords);
//     wrefresh(win);
//   }
//   inline chtype to_chtype(Pion sqdat, bool mark = false) {
//     return sqdat.get_sign() |
//            COLOR_PAIR(sqdat.get_player_index() + (mark ? 16 : 0)) |
//            (WA_DIM * int(sqdat.get_player_index() != 0 &&
//                          (sqdat.get_fatigue() ||
//                           sqdat.get_player_index() != *curr_player)));
//   }
//
//   void mark_squares() {
//     unmark_squares();
//     marked_squares = *curr_targets;
//     for (size_t i{}; i < marked_squares.size(); ++i) {
//       Coord c = marked_squares[i];
//       mvwaddch(win, 1 + c.y, 1 + c.x, to_chtype((*board)[c], true));
//     }
//     wrefresh(win);
//   }
//   void unmark_squares() {
//     for (size_t i{}; i < marked_squares.size(); ++i) {
//       Coord& c = marked_squares[i];
//       mvwaddch(win, 1 + c.y, 1 + c.x, to_chtype((*board)[c], false));
//     }
//     marked_squares.clear();
//     wrefresh(win);
//   }
//   void discard_markings() { marked_squares.clear(); }
//
//  private:
//   const std::vector<Coord>* curr_targets;
//   std::vector<Coord> marked_squares;
//   const Board* board;
//   const coordval* curr_player;
// };
//
// class NewWindow {
//  protected:
//   NewWindow(Coord size, Coord position)
//       : win{newwin(size.y, size.x, position.y, position.x)} {}
//   ~NewWindow() {
//     delwin(win);
//     touchwin(stdscr);
//     refresh();
//   }
//   WINDOW* win;
// };
//
// constexpr inline Coord man_dims{17, 50};
//
// class ManualWindow : public NewWindow {
//  public:
//   explicit ManualWindow(Coord size, Coord pos) : NewWindow{size, pos} {
//     mvwaddstr(win, 2, 6,
//               "               MANUAL             \n"
//               "                                  \n"
//               "     WASD / ARROWS  :  MOVE CURSOR\n"
//               "     C / ENTER      :  CONFIRM    \n"
//               "     B / BACKSPACE  :  BACK       \n"
//               "     N / TAB        :  NEXT TURN  \n"
//               "     SPACE          :  NEXT PIECE \n"
//               "     M              :  MANUAL     \n"
//               "     Z              :  QUIT       \n"
//               "                                  \n"
//               "     DESTROY FOE CASTLES          \n");
//     wborder(win, '+', '+', '+', '+', '+', '+', '+', '+');
//     wrefresh(win);
//   }
// };
//
// inline void winscr(unsigned char winner) {
//   curs_set(0);
//   nodelay(stdscr, TRUE);
//   attron(COLOR_PAIR(winner) | A_BLINK | A_BOLD | A_STANDOUT);
//   int z = 0;
//   int y;
//   int x;
//   getmaxyx(stdscr, y, x);
//   while (true) {
//     napms(500);
//     erase();
//     if (!SEIZURE_REDUCTION) {
//       z = 1 - z;
//       chtype ch = z ? '^' : '@';
//       for (int i = 0; i < y; i++) {
//         for (int j = 0; j < x; j++) {
//           if (i % 2 + j % 2 + z % 2) {
//             mvaddch(i, j, ch);
//           }
//         }
//       }
//     }
//     mvprintw(y / 2 - 1, x / 3 + 4, "!!!Player %d wins!!!", winner);
//     if (getch() != ERR) {
//       break;
//     }
//     refresh();
//   }
//   clear();
//   attroff(COLOR_PAIR(winner) | A_BLINK | A_BOLD | A_STANDOUT);
//   nodelay(stdscr, false);
// }
//
// inline void flash_screen() {
//   curs_set(0);
//   for (int rep = 0; rep < 2; ++rep) {
//     bkgd(A_REVERSE);
//     refresh();
//     napms(20);
//     bkgd(A_NORMAL);
//     refresh();
//     napms(20);
//   }
//   refresh();
// }
//
// inline void convert_anim(int cp) {
//   chtype traits = A_STANDOUT | COLOR_PAIR(cp);
//   WINDOW* copy = newwin(screen_height, screen_width, 0, 0);
//   overlay(stdscr, copy);
//   wbkgd(stdscr, traits);
//   refresh();
//   napms(100);
//   wbkgd(stdscr, A_NORMAL);
//   overlay(copy, stdscr);
//   delwin(copy);  // Delete the flash window
//   refresh();     // Refresh stdscr to restore the original content
// }
//
// #endif
