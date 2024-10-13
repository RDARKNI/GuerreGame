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
  seed = 0;
  role = Role::local;
  settings.act = forestgen_default;
}
std::vector<UserInput> MenuUI::menu_loop() {
  menu_stack = std::stack<main_menu_id>{{main_menu_id::main_menu}};
  while (true) {
    switch (menu_stack.top()) {
      case main_menu_id::main_menu     : main_menu(); break;
      case main_menu_id::setting_menu  : settings_menu(); break;
      case main_menu_id::client_connect: client_connect_menu(); break;
      case main_menu_id::close_game    : return {};
      case main_menu_id::finish:
        if (history.empty()) {
          if (role == Role::local) { settings.dst.x = 0; }
          history = {*reinterpret_cast<UserInput*>(&settings)};
        }

        if (role == Role::host) {
          history[0].dst.x = 1;
          if (init_connection_host(history[0])) {
            menu_stack = std::stack<main_menu_id>({main_menu_id::main_menu});
          }
        } else if (role == Role::client) {
          if (init_connection_client(history[0])) {
            menu_stack = std::stack<main_menu_id>({main_menu_id::main_menu});
          }
        }
        clear();
        refresh();
        return history;
      default: std::unreachable();
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
  mainwindow.show();
  static int y{0};
  mainwindow.move_cursor(y);
  constexpr int siz(main_options.size());
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
      case confirm:
        if (y == 3) { return menu_stack.push(main_menu_id::close_game); }
        role = static_cast<Role>(y);
        if (y == 1) {
          mainwindow.hide();
          return menu_stack.push(main_menu_id::client_connect);
        }
        mainwindow.hide();
        return menu_stack.push(main_menu_id::setting_menu);
      default: break;
    }
  }
}

void MenuUI::settings_menu() {
  settingswindow.show();
  Controller con = Controller({*reinterpret_cast<UserInput*>(&settings)}, Role::local);
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
      case confirm:
        switch (y) {
          case 0:
            pcount_menu();
            settingswindow.move_cursor(y, 0);
            break;
          case 1:
            dim_menu(con);
            settingswindow.move_cursor(y, 0);
            break;
          case 2:
            forestationmenu(con);
            settingswindow.move_cursor(y, 0);
            break;
          case 3:
            settingswindow.hide();
            savefilemenu(con);
            return;
          default: return menu_stack.push(main_menu_id::finish);
        }
        break;
      case back: settingswindow.hide(); return menu_stack.pop();
      default  : break;
    }
  }
}
void MenuUI::pcount_menu() {
  const auto pcount{settings.dst.y};
  settingswindow.move_cursor(0, 1);
  while (true) {
    switch (input_handler()) {
      case right:
        if (++settings.dst.y > max_players) { settings.dst.y = min_players; }
        settingswindow.move_cursor(0, 1);
        break;
      case left:
        if (--settings.dst.y < min_players) { settings.dst.y = max_players; }
        settingswindow.move_cursor(0, 1);
        break;
      case confirm: return;
      case back   : settings.dst.y = pcount; return;
      default     : break;
    }
  }
}
void MenuUI::forestationmenu(Controller& con) {
  const auto prev_forestation{settings.act};
  settingswindow.move_cursor(2, 1);
  con = Controller({*reinterpret_cast<UserInput*>(&settings)}, Role::local);
  while (true) {
    switch (input_handler()) {
      case right:
        if (++settings.act > forestgen_maximum) { settings.act = 0; }
        settingswindow.move_cursor(2, 1);
        con = Controller({*reinterpret_cast<UserInput*>(&settings)}, Role::local);
        break;
      case left:
        if (--settings.act < 0) { settings.act = forestgen_maximum; }
        settingswindow.move_cursor(2, 1);
        con = Controller({*reinterpret_cast<UserInput*>(&settings)}, Role::local);
        break;
      case confirm: return;
      case back   : settings.act = prev_forestation; return;
      default     : break;
    }
  }
}

void MenuUI::dim_menu(Controller& con) {
  const Coord dims{settings.src};
  settingswindow.move_cursor(1, 1);
  while (true) {
    switch (input_handler()) {
      case right:
        if (++settings.src.x > max_w) { settings.src.x = min_w; }
        settingswindow.move_cursor(1, 1);
        con = Controller({*reinterpret_cast<UserInput*>(&settings)}, Role::local);
        break;
      case left:
        if (--settings.src.x < min_w) { settings.src.x = max_w; }
        settingswindow.move_cursor(1, 1);
        con = Controller({*reinterpret_cast<UserInput*>(&settings)}, Role::local);
        break;
      case up:
        if (++settings.src.y > max_h) { settings.src.y = min_h; }
        settingswindow.move_cursor(1, 1);
        con = Controller({*reinterpret_cast<UserInput*>(&settings)}, Role::local);
        break;
      case down:
        if (--settings.src.y < min_h) { settings.src.y = max_h; }
        settingswindow.move_cursor(1, 1);
        con = Controller({*reinterpret_cast<UserInput*>(&settings)}, Role::local);
        break;
      case confirm: return;
      case back   : settings.src = dims; return;
      default     : break;
    }
  }
}

void MenuUI::savefilemenu(Controller& con) {  // todo debug segfault
  std::vector<std::string> filenames{get_savefiles()};
  const size_t size{filenames.size()};
  if (!size) { return; }
  std::vector<const char*> strings;
  strings.reserve(size);
  for (const auto& file : filenames) {
    strings.push_back(file.data());
  }

  SaveFileMenu savefilewindow{{15, 15}, {yoffs, xoffs}, strings, nullptr};
  savefilewindow.show();

  size_t selected{0};
  std::vector<UserInput> tmp_hist;
  auto mov{[&]() {
    tmp_hist = load_game(filenames[selected]);
    savefilewindow.move_cursor(selected);
    con = Controller(tmp_hist, Role::local);
  }};
  mov();
  while (true) {
    switch (input_handler()) {
      case up:
        selected = selected > 0 ? selected - 1 : size - 1;
        mov();
        break;
      case down:
        selected = selected < size - 1 ? selected + 1 : 0;
        mov();
        break;
      case confirm:
        history = tmp_hist;
        menu_stack.push(main_menu_id::finish);
        return;
      case back: return;
      default  : break;
    }
  }
}

void MenuUI::client_connect_menu() {
  static int index{0};
  ipwindow.show();
  ipwindow.move_cursor(index, 0);
  while (true) {
    switch (input_handler()) {
      case up:
        index = index > 0 ? index - 1 : 2;
        ipwindow.move_cursor(index, 0);
        break;
      case down:
        index = index < 2 ? index + 1 : 0;
        ipwindow.move_cursor(index, 0);
        break;
      case confirm:
        switch (index) {
          case 0 : ip_input(); return;
          case 1 : return menu_stack.push(main_menu_id::finish);
          case 2 : ipwindow.hide(); return menu_stack.pop();
          default: std::unreachable();
        }
        break;
      case back: ipwindow.hide(); return menu_stack.pop();
      default  : break;
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
    if (sum + i * tpot <= max_ip_segment) { return static_cast<char>(i + '0'); }
  }
  return '0';
}

void MenuUI::ip_input() {
  int index = 0;
  ipwindow.move_cursor(index, 1);
  char tmp_ip[sizeof(ip_address)]{};
  constexpr int max_index{14};
  memcpy(tmp_ip, ip_address, sizeof(ip_address));
  while (true) {
    int ch = getch();
    switch (parse_input(ch)) {
      case left:
        index = !index ? max_index : index - (1 + !(index % 4));
        ipwindow.move_cursor(index, 1);
        break;
      case right:
        index = index == max_index ? 0 : index + (1 + !((index + 2) % 4));
        ipwindow.move_cursor(index, 1);
        break;
      case up:
        if (ip_address[index] < max_digit(ip_address, index)) {
          ip_address[index]++;
        } else {
          ip_address[index] = '0';
        }
        ipwindow.move_cursor(index, 1);
        break;
      case down:
        if (ip_address[index] > '0') {
          ip_address[index]--;
        } else {
          ip_address[index] = max_digit(ip_address, index);
        }
        ipwindow.move_cursor(index, 1);
        break;
      case number:
        if (ch > max_digit(ip_address, index)) {
          if (index % 4 < 2) {  // either of first two digits
            ip_address[index] = '0';
            ip_address[index + 1] = char(ch);
            index += 2;
            ipwindow.move_cursor(index, 1);
          }
          break;
        }
        ip_address[index] = char(ch);
        if (index < max_index) { index += 1 + !((index + 2) % 4); }
        ipwindow.move_cursor(index, 1);
        continue;
      case dot:
        if (index >= 12) { break; }
        if (index % 4 == 1) {
          ip_address[index + 1] = ip_address[index - 1];
          ip_address[index - 1] = '0';
          ip_address[index] = '0';
          index += 3;
          ipwindow.move_cursor(index, 1);
        } else if (index % 4 == 2) {
          ip_address[index] = ip_address[index - 1];
          ip_address[index - 1] = ip_address[index - 2];
          ip_address[index - 2] = '0';
          index += 2;
        }
        ipwindow.move_cursor(index, 1);

        break;
      case back:
        memcpy(ip_address, tmp_ip, sizeof(ip_address));
        ipwindow.move_cursor(index, 1);
        return;
      case confirm: ipwindow.move_cursor(index, 1); return;
      default     : break;
    }
  }
}
