#include "menu_ui.hpp"

#include <curses.h>

#include <cassert>
#include <stack>

#include "../controller/network_helper.hpp"
#include "../game_config.hpp"
#include "../input_parser.hpp"
size_t PLAYER_INDEX;
size_t PLAYER_COUNT;
size_t B_H;
size_t B_W;
Role role;
// char ip_address[64]{"000.000.000.000"};
char ip_address[64]{"127.000.000.001"};

const char* player_names[9]{"Gaia",     "Player 1", "Player 2",
                            "Player 3", "Player 4", "Player 5",
                            "Player 6", "Player 7", "Player 8"};

MenuUI::MenuUI() {
  PLAYER_INDEX = 0;
  PLAYER_COUNT = 2;
  B_H = 20;
  B_W = 20;
  role = LOCAL;
}

int MenuUI::menu_loop() {
  // for (int i = 0; i < screen_width; i += 2) {
  //   int j = i;
  //   int s = 0;
  //   while (s < screen_height && j < screen_width) {
  //     mvaddch(s, j, ' ' | A_STANDOUT);
  //     napms(10);
  //     refresh();
  //     ++s;
  //     j++;
  //   }
  //   napms(100);
  //   // for (int j = 0; j < screen_width; ++j) {
  //   //   mvaddch(i, j, '-' | COLOR_PAIR(16));
  //   //   napms(10);
  //   //   refresh();
  //   // }
  //   //  for (int j = 0; j < B_W; ++j) {
  //   //    mvaddch(i, j, ' ' | COLOR_PAIR(0));
  //   //    // napms(50);
  //   //  }
  // }
  menu_stack = std::stack<main_menu_id>{{MAIN}};
  curs_set(0);
  while (1) {
    switch (menu_stack.top()) {
      case MAIN:
        role = LOCAL;
        main_menu();
        break;
      case LOCAL_RULES:
        PLAYER_INDEX = 0;
        settings_menu();
        break;
      case HOST_RULES:
        PLAYER_INDEX = 1;
        settings_menu();
        break;
      case HOST_WAIT:
        role = HOST;
        init_connection();
        // todo
        break;
      case PCOUNT:
        pcount_menu();
        break;
      case DIMS:
        dim_menu();
        break;
      case CLIENT_CONNECT:
        client_connect_menu();
        break;
      case CLIENT_IP:
        ip_input();
        break;
      case CLIENT_WAIT:
        role = CLIENT;
        init_connection();
        break;
      case CLOSE_GAME:
        return 1;
      case FINISH:
        clear();
        return 0;
      default:
        assert(0);
    }
  }
}
const char* client_connect_options[4]{"Local", "Confirm"};

void render_client_connect_menu(int selected = -1) {
  clear();
  static int off = screen_width / 4 - 5;
  int pos = 0;
  mvprintw(5, off, "Host IP-Address:  ");
  if (pos == selected) {
    attron(A_BLINK | A_STANDOUT);
  }
  printw("%s", ip_address);
  attroff(A_BLINK | A_STANDOUT);
  pos++;
  mvprintw(6, off + 5, "     ");
  if (pos == selected) {
    attron(A_BLINK | A_STANDOUT);
  }
  printw("CONFIRM");
  attroff(A_BLINK | A_STANDOUT);
  refresh();
}
void MenuUI::client_connect_menu() {
  static int y{0};
  curs_set(0);
  render_client_connect_menu(y);
  while (1) {
    switch (input_handler()) {
      case UP:
        render_client_connect_menu((y = (y - 1 + 2) % 2));
        break;
      case DOWN:
        render_client_connect_menu((y = (y + 1) % 2));
        break;
      case CONF:
        menu_stack.push((main_menu_id)(!y ? CLIENT_IP : CLIENT_WAIT));
        return;
      case BACK:
        return menu_stack.pop();
      default:
        break;
    }
  }
}

const char* main_options[4]{"Local", "Client", "Host", "Quit"};
void MenuUI::main_menu() {
  static int y{0};
  render_main_menu(y);
  napms(500);
  while (1) {
    switch (input_handler()) {
      case UP:
        render_main_menu((y = (y - 1 + 4) % 4));
        break;
      case DOWN:
        render_main_menu((y = (y + 1) % 4));
        break;
      case CONF:
        menu_stack.push((main_menu_id)(y + 1));
        return;
      default:
        break;
    }
  }
}
void MenuUI::render_main_menu(int selected) {
  clear();
  static int offset = screen_width / 4 - 3;
  int i{0};
  for (const char* option : main_options) {
    if (i == selected) {
      attron(A_BLINK | A_STANDOUT);
    }
    mvaddstr(i + 5, offset, option);
    i++;
    attroff(A_BLINK | A_STANDOUT);
  }
  refresh();
}

void MenuUI::settings_menu() {
  static int y{0};
  render_settings_menu(y);
  while (1) {
    switch (input_handler()) {
      case UP:
        render_settings_menu((y = (y - 1 + 3) % 3));
        break;
      case DOWN:
        render_settings_menu((y = (y + 1) % 3));
        break;
      case CONF:
        switch (y) {
          case 0:
            return menu_stack.push(PCOUNT);
          case 1:
            return menu_stack.push(DIMS);
          default:
            return menu_stack.push(menu_stack.top() == LOCAL_RULES ? FINISH
                                                                   : HOST_WAIT);
        }
      case BACK:
        return menu_stack.pop();
      default:
        break;
    }
  }
}

void MenuUI::render_settings_menu(int selected) {
  clear();
  static int off = screen_width / 4 - 5;
  int pos = 0;

  mvprintw(5, off, "PLAYERS:  ");
  if (pos == selected) {
    attron(A_BLINK | A_STANDOUT);
  }
  printw("%d", PLAYER_COUNT);
  attroff(A_BLINK | A_STANDOUT);
  pos++;

  mvprintw(6, off, "BOARD:    ");
  if (pos == selected) {
    attron(A_BLINK | A_STANDOUT);
  }
  printw("%dx%d", B_H, B_W);
  attroff(A_BLINK | A_STANDOUT);
  pos++;

  mvprintw(7, off, "     ");
  if (pos == selected) {
    attron(A_BLINK | A_STANDOUT);
  }
  printw("CONFIRM");
  attroff(A_BLINK | A_STANDOUT);
  refresh();
}

void MenuUI::pcount_menu() {
  const size_t pcount{PLAYER_COUNT};
  while (1) {
    switch (input_handler()) {
      case RIGHT:
        if (++PLAYER_COUNT > MAX_PLAYERS) {
          PLAYER_COUNT = MIN_PLAYERS;
        }
        render_settings_menu(0);
        break;
      case LEFT:
        if (--PLAYER_COUNT < MIN_PLAYERS) {
          PLAYER_COUNT = MAX_PLAYERS;
        }
        render_settings_menu(0);
        break;
      case CONF:
        menu_stack.pop();
        return;
      case BACK:
        PLAYER_COUNT = pcount;
        menu_stack.pop();
        return;
      default:
        break;
    }
  }
}

void MenuUI::dim_menu() {
  const int height = B_H;
  const int width = B_W;
  while (1) {
    switch (input_handler()) {
      case RIGHT:
        if (++B_W > MAX_W) {
          B_W = MIN_W;
        }
        render_settings_menu(1);
        break;
      case LEFT:
        if (--B_W < MIN_W) {
          B_W = MAX_W;
        }
        render_settings_menu(1);
        break;
      case UP:
        if (++B_H > MAX_H) {
          B_H = MIN_H;
        }
        render_settings_menu(1);
        break;
      case DOWN:
        if (--B_H < MIN_H) {
          B_H = MAX_H;
        }
        render_settings_menu(1);
        break;
      case CONF:
        menu_stack.pop();
        return;
      case BACK:
        B_H = height;
        B_W = width;
        menu_stack.pop();
        return;
      default:
        break;
    }
  }
}

char max_digit(char* str, int pos) {  // TODO
  int sum = 0;
  int tpot = 0;
  int pot = 1;
  for (int i = 2; i >= 0; --i) {
    if (pos != i) {
      sum += (str[i] - '0') * pot;
    } else {
      tpot = pot;
    }
    pot *= 10;
  }
  for (int i = 9; i >= 0; --i) {
    if (sum + i * tpot <= 255) {
      return i + '0';
    }
  }
  return '0';
}

void MenuUI::ip_input() {
  clear();
  static int offset = screen_width / 4 - 5;
  char tmp_ip[sizeof(ip_address)]{};
  for (int i = 0; i < sizeof(ip_address); ++i) {
    tmp_ip[i] = ip_address[i];
  }

  render_client_connect_menu(-1);
  const int ip_start = offset + 17 + 1;
  refresh();
  move(5, ip_start);
  curs_set(1);
  int index = 0;
  char tmp;
  while (true) {
    int ch = getch();
    switch (parse_input(ch)) {
      case LEFT:
        if (!index) {
          index = 14;
          move(5, ip_start + index);
          refresh();
          break;
        }

        index--;
        if (!((index + 1) % 4)) {
          index--;
        }
        move(5, ip_start + index);
        refresh();

        break;
      case RIGHT:
        if (index == 14) {
          index = 0;
          move(5, ip_start + index);
          refresh();
          break;
        }
        index++;
        if (!((index + 1) % 4)) {
          index++;
        }
        move(5, ip_start + index);
        refresh();
        break;

      case UP:
        if (ip_address[index] <
            max_digit(ip_address + (index / 4) * 4, index % 4)) {
          ip_address[index]++;
        } else {
          ip_address[index] = '0';
        }
        addch(ip_address[index]);
        move(5, ip_start + index);
        refresh();
        break;
        // 135.90.0.300
      case DOWN:
        if (ip_address[index] > '0') {
          ip_address[index]--;
        } else {
          ip_address[index] =
              max_digit(ip_address + (index / 4) * 4, index % 4);
        }
        addch(ip_address[index]);
        move(5, ip_start + index);
        refresh();  // 135.90.20.80
        break;
      case NUM:

        if (ch > max_digit(ip_address + (index / 4) * 4, index % 4)) {
          if (index % 4 < 2) {
            ip_address[index] = '0';
            addch(ip_address[index]);
            index++;
            ip_address[index] = ch;
            addch(ip_address[index]);
            index++;
            move(5, ip_start + index);
            refresh();  // 135.9.20.9
          }
          break;
        }

        ip_address[index] = ch;
        if (index < 14) {
          index++;
          if (!((index + 1) % 4)) {
            index++;
          }
        }
        addch(ch);
        move(5, ip_start + index);
        refresh();
        continue;

      case DOT:
        if (index >= 12) {
          break;
        }
        if ((index % 4) == 0) {
          break;
        }

        if ((index % 4) == 1) {
          ip_address[index + 1] = ip_address[index - 1];
          ip_address[index - 1] = '0';
          ip_address[index] = '0';
          move(5, ip_start + index - 1);
          addch('0');
          addch('0');
          addch(ip_address[index + 1]);
          index += 3;
          move(5, ip_start + index);
          refresh();
          break;
        }
        if ((index % 4) == 2) {
          ip_address[index] = ip_address[index - 1];
          ip_address[index - 1] = ip_address[index - 2];
          ip_address[index - 2] = '0';
          move(5, ip_start + index - 2);
          addch(ip_address[index - 2]);
          addch(ip_address[index - 1]);
          addch(ip_address[index]);
          index += 2;
          move(5, ip_start + index);
          refresh();
        }
        break;
      case BACK:
        index = 0;
        for (int i = 0; i < sizeof(ip_address); ++i) {
          ip_address[i] = tmp_ip[i];
          addch(ip_address[i]);
          index++;
          move(5, ip_start + index);
        }
        refresh();
        curs_set(0);
        menu_stack.pop();
        return;
      case CONF:
        menu_stack.pop();
        return;
      default:
        break;
    }
  }
}

/*

3 positions:
If dot: if previous three are all numbers, just jump
if previous two are numbers, add a zero, move previous two to the right and then
jump

*/
