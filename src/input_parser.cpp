#include "input_parser.hpp"

#include <curses.h>

#include "controller/cursewindow.hpp"
#include "controller/network_helper.hpp"
#include "game_config.hpp"
PARSED_INPUTS parse_input(int ch) {
  switch (ch) {
    case 'w':
    case 'W':
    case KEY_UP:
      return UP;
    case 'd':
    case 'D':
    case KEY_RIGHT:
      return RIGHT;
    case 's':
    case 'S':
    case KEY_DOWN:
      return DOWN;
    case 'a':
    case 'A':
    case KEY_LEFT:
      return LEFT;
    case 'c':
    case 'C':
    case '\n':
      return CONF;
    case ' ':
      return NEXT_VALID;
    case 'b':
    case 'B':
      return BACK;
    case 'n':
    case 'N':
    case '\t':
      return NEXT_TURN;
    case 'm':
    case 'M':
      return MANUAL;
    case 'z':
    case 'Z':
      return QUIT;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return NUM;
    case '.':
      return DOT;
    default:
      return OTHER;
  }
}

int getch_mod() {
  int ch = getch();
  if (ch == 'M' || ch == 'm') {
    nodelay(stdscr, FALSE);
    // ManualWindow man;
    ch = getch();
  }
  return ch;
}
// 127.0.0.001
PARSED_INPUTS input_handler(int current_player) {
  switch (role) {
    case LOCAL:
      input_buffer[0] = getch_mod();
      break;
    case HOST:
      if (current_player == PLAYER_INDEX) {
        input_buffer[0] = getch_mod();
        broadcast(input_buffer[0]);
      } else {
        receive(input_buffer[0], current_player);
        broadcast(input_buffer[0], current_player);
      }
      break;
    case CLIENT:
      if (current_player == PLAYER_INDEX) {
        input_buffer[0] = getch_mod();
        send(input_buffer[0]);
      } else {
        receive(input_buffer[0]);
      }
      break;
    default:
      assert(0);
  }
  return parse_input(input_buffer[0]);
}

/*
    case '0':
      return NUM_ZERO;
    case '1':
      return NUM_ONE;
    case '2':
      return NUM_TWO;
    case '3':
      return NUM_THREE;
    case '4':
      return NUM_FOUR;
    case '5':
      return NUM_FIVE;
    case '6':
      return NUM_SIX;
    case '7':
      return NUM_SEVEN;
    case '8':
      return NUM_EIGHT;
    case '9':
      return NUM_NINE;*/
