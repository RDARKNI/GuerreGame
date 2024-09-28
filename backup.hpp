// #ifndef __CURSEWINDOW_H__
// #define __CURSEWINDOW_H__
//
// #include <curses.h>
//
// #include <vector>
//
// #include "../controller/data_handler.hpp"
// #include "../game_config.hpp"
// #include "../helpers/coords.hpp"
// #include "../helpers/user_input.hpp"
// #include "../input_parser.hpp"
// #include "../ui/ncurses_setup.hpp"
///*
// Todo: Make work without datahandler to use these for the Menu UI
// Make the design more logical
//*/
//
// class SubWindow {
//  public:
//   SubWindow(const DataHandler& dh, Coord size, Coord position);
//   ~SubWindow();
//   WINDOW*& get_win();
//   void hide();
//
//  protected:
//   const DataHandler& dh;
//   WINDOW* win;
// };
//
// class BoardWindow : public SubWindow {
//  public:
//   BoardWindow(const DataHandler& dh, Coord position);
//   void move_cursor(Coord coords);
//
//   void show();
//   void redraw_square(Coord coords);
//
//   void mark_squares();
//   void unmark_squares();
//
//  private:
//   std::vector<Coord> marked_squares;
// };
//
///*
// Windows can have a selected field or not
// If they don't, they're Info Windows, if they are they are MenuWindows
// Each Type of InfoWindow has different data it needs to get and different ways
// of displaying the data: InfoWindows:
//- InfoWindow (to show pieces): Piece Stats -> Gets Piece Data. || Holds square
// NOT INFO WINODWS:
//- PlayerWindow: Player, Gold -> Gets current Player Data
//- TurnWindow: Turn -> Gets current Turn data
// MenuWIndows:
//- ActionWindow -> Gets Action Data
//- OptionWinwod -> Gets Option Data
//
//
//
//
//*/
//
// class InfoWindow : public SubWindow {
//  public:
//   InfoWindow(const DataHandler& dh, Coord position, const Coord& square);
//   void show();
//
//  private:
//   const Coord& square;
// };
//
//// Player and InfoWindow should be InfoWindows?
// class PlayerWindow : public SubWindow {
//  public:
//   PlayerWindow(const DataHandler& dh, Coord position);
//   void show();
// };
//
// class TurnWindow : public SubWindow {
//  public:
//   TurnWindow(const DataHandler& dh, Coord position);
//   void show();
// };
//
///*
// Maybe have each MenuWindow have a const ref to the data directly
//*/
//// Why are Action and OptionWindwow different classes?
// class MenuWindow : public SubWindow {
//  public:
//   MenuWindow(const DataHandler& dh, Coord position, const char* const*
//   strings); virtual std::vector<Dat> get_data() = 0; void move_cursor(int pos
//   = -1); void show();
//
//  private:
//   const char* const* strings;
//   void render(int sel = -1);
// };
//
// class ActionWindow : public MenuWindow {
//  public:
//   using MenuWindow::MenuWindow;
//   std::vector<Dat> get_data() override { return dh.get_curr_actions(); }
// };
//
// class OptionWindow : public MenuWindow {
//  public:
//   using MenuWindow::MenuWindow;
//   std::vector<Dat> get_data() override { return dh.get_curr_options(); }
// };
//
// class NewWindow {
//  public:
//   NewWindow(const DataHandler& dh, Coord size, Coord position)
//       : dh{dh}, win{newwin(size.y, size.x, position.y, position.x)} {}
//   ~NewWindow() {
//     delwin(win);
//     touchwin(stdscr);
//     refresh();
//   }
//   WINDOW*& get_win() { return win; }
//
//  protected:
//   const DataHandler& dh;
//   WINDOW* win;
// };
//
// #define HEIGHTA 20
// #define WIDTHA 70
// class WinScreen : public NewWindow {
//  public:
//   WinScreen(const DataHandler& dh, Coord pos)
//       : NewWindow{dh, {HEIGHTA, WIDTHA}, pos} {  // todo adjust size
//     int colour = dh.get_current_player_data().index;
//     curs_set(2);
//     nodelay(win, TRUE);
//     wattron(win, COLOR_PAIR(colour) | A_BLINK | A_BOLD | A_STANDOUT);
//     int z = 0;
//     while (1) {
//       napms(500);
//       werase(win);
//       if (!SEIZURE_REDUCTION) {
//         z = 1 - z;
//         chtype ch = z ? '^' : '@';
//         for (int i = 0; i < HEIGHTA; i++) {
//           for (int j = 0; j < WIDTHA; j++) {
//             if (i % 2 + j % 2 + z % 2) {
//               mvwaddch(win, i, j, ch);
//             }
//           }
//         }
//       }
//       mvwprintw(win, HEIGHTA / 2 - 1, WIDTHA / 3 + 4, "!!!Player %d wins!!!",
//                 colour);
//       if (wgetch(win) != ERR) {
//         return;
//       }
//       wrefresh(win);
//     }
//   }
// };
//
// class ManualWindow : public NewWindow {
//  public:
//   ManualWindow(const DataHandler& dh, Coord pos)
//       : NewWindow{dh, {20, 20}, pos} {
//     wborder(win, '+', '+', '+', '+', '+', '+', '+', '+');
//     for (int i = 0; const char* str : manual_items) {
//       mvwaddstr(win, 2 + i++, 4, str);
//     }
//     wrefresh(win);
//   }
//
//  private:
//   inline static const char* manual_items[]{
//       "          MANUAL",
//       "",
//       "WASD/ARROWS  :  MOVE CURSOR",
//       "ENTER/C      :  SELECT",
//       "B            :  GO BACK",
//       "N/TAB        :  NEXT TURN",
//       "SPACE        :  NEXT PIECE",
//       "M            :  MANUAL",
//       "ESC          :  QUIT",
//       "",
//       "DESTROY FOE CASTLES",
//   };
// };
//
//// static void flash_screen() {
////   chtype traits = A_REVERSE;
////   WINDOW* copy = newwin(screen_height, screen_width, 0, 0);
////   overlay(stdscr, copy);
////   for (int rep = 0; rep < 5; ++rep) {
////     attron(traits);
////     for (int i = 0; i < screen_height; ++i) {
////       for (int j = 0; j < screen_width; ++j) {
////         mvaddch(i, j, mvinch(i, j));
////       }
////     }
////     refresh();
////     napms(100);
////     attroff(traits);
////     for (int i = 0; i < screen_height; ++i) {
////       for (int j = 0; j < screen_width; ++j) {
////         mvaddch(i, j, mvinch(i, j) - traits);
////       }
////     }
////     refresh();
////     napms(100);
////   }
////   overlay(copy, stdscr);
////   delwin(copy);
//// }
////
//// static void convert_anim(const DataHandler& dh, const UserInput&) {
////   int cp = dh.get_current_player_data().index;
////   chtype traits = A_STANDOUT | A_DIM | COLOR_PAIR(cp);
////   WINDOW* copy = newwin(screen_height, screen_width, 0, 0);
////   overlay(stdscr, copy);
////   attron(traits);
////   for (int i = 0; i < screen_height; ++i) {
////     for (int j = 0; j < screen_width; ++j) {
////       mvaddch(i, j, mvinch(i, j));
////     }
////   }
////   refresh();
////   napms(300);
////   attroff(traits);
////   for (int i = 0; i < screen_height; ++i) {
////     for (int j = 0; j < screen_width; ++j) {
////       mvaddch(i, j, mvinch(i, j) - traits);
////     }
////   }
////   refresh();
////   overlay(copy, stdscr);
////   delwin(copy);
//// }
//
// #endif
