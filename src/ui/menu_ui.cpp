#include "menu_ui.hpp"

#include <cassert>
#include <utility>

#include "../controller/controller.hpp"
#include "../game_config.hpp"
#include "../input/input_parser.hpp"
#include "../network/network_helper.hpp"
#include "../save/save.hpp"
using enum ParsedInput;

MenuUI::MenuUI() {
  xo = 60;
  seed = 0;
  role = LOCAL;
  settings.act = forestgen_default;
}
std::vector<UserInput> MenuUI::menu_loop() {
  menu_stack = std::stack<main_menu_id>{{main_menu_id::MAIN}};
  while (true) {
    switch (menu_stack.top()) {
      case main_menu_id::MAIN:
        main_menu();
        break;
      case main_menu_id::SETTINGS:
        settings_menu();
        break;
      case main_menu_id::CLIENT_CONNECT:
        client_connect_menu();
        break;
      case main_menu_id::CLOSE_GAME:
        return {};
      case main_menu_id::FINISH:
        if (history.empty()) {
          if (role == LOCAL) {
            settings.dst.x = 0;
          }
          history = {settings};
        }
        if (role == HOST) {
          history[0].dst.x = 1;
        }
        if (role == HOST) {
          if (init_connection_host(history[0])) {
            menu_stack = std::stack<main_menu_id>({main_menu_id::MAIN});
          }
        } else if (role == CLIENT) {
          if (init_connection_client(history[0])) {
            menu_stack = std::stack<main_menu_id>({main_menu_id::MAIN});
          }
        }
        clear();
        refresh();
        xo = 0;
        return history;
      default:
        std::unreachable();
    }
  }
}

/*
        y = (y - 1 + 4) % 4;
        mainwindow.move_cursor(y);
        break;
      case down:
        y = (y + 1) % 4;
        mainwindow.move_cursor(y);
        break;
*/
void MenuUI::main_menu() {
  static int y{0};
  mainwindow.move_cursor(y);
  constexpr int siz{arr_size(main_options)};
  while (true) {
    switch (input_handler()) {
      case up:
        y = (y - 1 + siz) % siz;
        mainwindow.move_cursor(y);
        break;
      case down:
        y = (y + 1) % siz;
        mainwindow.move_cursor(y);
        break;
      case conf:
        if (y == 3) {
          return menu_stack.push(main_menu_id::CLOSE_GAME);
        }
        role = static_cast<Role>(y);
        if (y == 1) {
          mainwindow.hide();
          return menu_stack.push(main_menu_id::CLIENT_CONNECT);
        }
        mainwindow.hide();
        return menu_stack.push(main_menu_id::SETTINGS);
      default:
        break;
    }
  }
}

void MenuUI::settings_menu() {
  settingswindow.show();
  Controller{{settings}, LOCAL};
  constexpr int siz{5};
  static int y{0};
  settingswindow.move_cursor(y, 0);
  while (true) {
    switch (input_handler()) {
      case up:
        y = (y - 1 + siz) % siz;
        settingswindow.move_cursor(y, 0);
        break;
      case down:
        y = (y + 1) % siz;
        settingswindow.move_cursor(y, 0);
        break;
      case conf:
        switch (y) {
          case 0:
            return pcount_menu();
          case 1:
            return dim_menu();
          case 2:
            return forestationmenu();
          case 3:
            settingswindow.hide();
            return savefilemenu();
          default:
            return menu_stack.push(main_menu_id::FINISH);
        }
        break;
      case back:
        settingswindow.hide();
        return menu_stack.pop();
      default:
        break;
    }
  }
}
void MenuUI::forestationmenu() {
  const char prev_forestation{settings.act};
  settingswindow.move_cursor(2, 1);
  while (true) {
    switch (input_handler()) {
      case right:
        settings.act = settings.act < forestgen_maximum ? settings.act + 1 : 0;
        settingswindow.move_cursor(2, 1);
        break;
      case left:
        settings.act = settings.act == 0 ? forestgen_maximum : settings.act - 1;
        settingswindow.move_cursor(2, 1);
        break;
      case conf:
        return;
      case back:
        settings.act = prev_forestation;
        return;
      default:
        break;
    }
  }
}

void MenuUI::pcount_menu() {
  const char pcount{settings.dst.y};
  settingswindow.move_cursor(0, 1);
  while (true) {
    switch (input_handler()) {
      case right:
        settings.dst.y =
            settings.dst.y < MAX_PLAYERS ? settings.dst.y + 1 : MIN_PLAYERS;
        settingswindow.move_cursor(0, 1);
        break;
      case left:
        if (--settings.dst.y < MIN_PLAYERS) {
          settings.dst.y = MAX_PLAYERS;
        }
        settingswindow.move_cursor(0, 1);
        break;
      case conf:
        return;
      case back:
        settings.dst.y = pcount;
        return;
      default:
        break;
    }
  }
}

void MenuUI::dim_menu() {
  const Coord dims{settings.src};
  settingswindow.move_cursor(1, 1);
  while (true) {
    switch (input_handler()) {
      case right:
        if (++settings.src.x > MAX_W) {
          settings.src.x = MIN_W;
        }
        settingswindow.move_cursor(1, 1);
        break;
      case left:
        if (--settings.src.x < MIN_W) {
          settings.src.x = MAX_W;
        }
        settingswindow.move_cursor(1, 1);
        break;
      case up:
        if (++settings.src.y > MAX_H) {
          settings.src.y = MIN_H;
        }
        settingswindow.move_cursor(1, 1);
        break;
      case down:
        if (--settings.src.y < MIN_H) {
          settings.src.y = MAX_H;
        }
        settingswindow.move_cursor(1, 1);
        break;
      case conf:
        return;
      case back:
        settings.src = dims;
        return;
      default:
        break;
    }
  }
}

void MenuUI::savefilemenu() {  // todo debug segfault
  std::vector<std::string> filenames{get_savefiles()};
  const int size{(int)filenames.size()};
  if (!size) {
    return;
  }
  std::vector<const char*> strings;
  strings.reserve(size);
  for (const auto& file : filenames) {
    strings.push_back(file.data());
  }
  SaveFileMenu savefilewindow{{10, 15}, {4, 4}, strings};

  int selected{0};
  std::vector<UserInput> tmp_hist;
  auto mov{[&]() {
    tmp_hist = load_game(filenames[selected]);
    savefilewindow.move_cursor(selected);
  }};
  mov();
  while (true) {
    Controller test = Controller(tmp_hist, LOCAL);
    switch (input_handler()) {
      case up:
        selected = selected > 0 ? selected - 1 : size - 1;
        mov();
        break;
      case down:
        selected = selected < size - 1 ? selected + 1 : 0;
        mov();
        break;
      case conf:
        history = tmp_hist;
        menu_stack.push(main_menu_id::FINISH);
        return;
      case back:
        return;
      default:
        break;
    }
  }
}

void MenuUI::client_connect_menu() {
  static int index{0};
  ipwindow.set_inner(false);
  ipwindow.move_cursor(index);
  while (true) {
    switch (input_handler()) {
      case up:
        index = index > 0 ? index - 1 : 2;
        ipwindow.move_cursor(index);
        break;
      case down:
        index = index < 2 ? index + 1 : 0;
        ipwindow.move_cursor(index);
        break;
      case conf:
        switch (index) {
          case 0:
            ip_input();
            return;
          case 1:
            return menu_stack.push(main_menu_id::FINISH);
          case 2:
            ipwindow.hide();
            return menu_stack.pop();
          default:
            std::unreachable();
        }
        break;
      case back:
        ipwindow.hide();
        return menu_stack.pop();
      default:
        break;
    }
  }
}

char max_digit(char* str, int pos) {  //  refactor
  constexpr int max_ip_segment{255};
  str += (pos / 4) * 4;
  pos %= 4;
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
    if (sum + i * tpot <= max_ip_segment) {
      return i + '0';
    }
  }
  return '0';
}

void MenuUI::ip_input() {
  ipwindow.set_inner(true);
  char tmp_ip[sizeof(ip_address)]{};
  constexpr int max_index{14};
  memcpy(tmp_ip, ip_address, sizeof(ip_address));
  int index = 0;
  ipwindow.move_cursor(index);
  while (true) {
    int ch = getch();
    switch (parse_input(ch)) {
      case left:
        index = !index ? max_index : index - (1 + !(index % 4));
        ipwindow.move_cursor(index);
        break;
      case right:
        index = index == max_index ? 0 : index + (1 + !((index + 2) % 4));
        ipwindow.move_cursor(index);
        break;
      case up:
        if (ip_address[index] < max_digit(ip_address, index)) {
          ip_address[index]++;
        } else {
          ip_address[index] = '0';
        }
        ipwindow.move_cursor(index);
        break;
      case down:
        if (ip_address[index] > '0') {
          ip_address[index]--;
        } else {
          ip_address[index] = max_digit(ip_address, index);
        }
        ipwindow.move_cursor(index);
        break;
      case number:
        if (ch > max_digit(ip_address, index)) {
          if (index % 4 < 2) {  // either of first two digits
            ip_address[index] = '0';
            ip_address[index + 1] = ch;
            index += 2;
            ipwindow.move_cursor(index);
            // 135.9.20.9
          }
          break;
        }
        ip_address[index] = ch;
        if (index < max_index) {
          index += 1 + !((index + 2) % 4);
        }
        ipwindow.move_cursor(index);
        continue;
      case dot:
        if (index >= 12) {
          break;
        }
        if (index % 4 == 1) {
          ip_address[index + 1] = ip_address[index - 1];
          ip_address[index - 1] = '0';
          ip_address[index] = '0';
          index += 3;
          ipwindow.move_cursor(index);
        } else if (index % 4 == 2) {
          ip_address[index] = ip_address[index - 1];
          ip_address[index - 1] = ip_address[index - 2];
          ip_address[index - 2] = '0';
          index += 2;
        }
        ipwindow.move_cursor(index);
        break;
      case back:
        memcpy(ip_address, tmp_ip, sizeof(ip_address));
        ipwindow.set_inner(false);
        return;
      case conf:
        ipwindow.set_inner(false);
        return;
      default:
        break;
    }
  }
}
