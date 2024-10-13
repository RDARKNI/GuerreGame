#include "input_parser.hpp"

#include <curses.h>

#include "../ui/cursewindow.hpp"
ParsedInput parse_input(int ch) {
  using enum ParsedInput;
  switch (ch) {
    case 'w':
    case 'W':
    case KEY_UP   : return up;
    case 'd'      :
    case 'D'      :
    case KEY_RIGHT: return right;
    case 's'      :
    case 'S'      :
    case KEY_DOWN : return down;
    case 'a'      :
    case 'A'      :
    case KEY_LEFT : return left;
    case 'c'      :
    case 'C'      :
    case '\n'     : return confirm;
    case ' '      : return next_valid;
    case 'b'      :
    case 'B'      : return back;
    case 'n'      :
    case 'N'      :
    case '\t'     : return next_turn;
    case 'm'      :
    case 'M'      : return manual;
    case 'z'      :
    case 'Z'      : return quit;
    case '0'      :
    case '1'      :
    case '2'      :
    case '3'      :
    case '4'      :
    case '5'      :
    case '6'      :
    case '7'      :
    case '8'      :
    case '9'      : return number;
    case '.'      : return dot;
    default       : return other;
  }
}

ParsedInput input_handler() {
  int ch = getch();
  if (ch == 'M' || ch == 'm') {
    nodelay(stdscr, FALSE);
    ManualWindow man{man_dims, {2, 2}};
    ch = getch();
  }
  return parse_input(ch);
}
