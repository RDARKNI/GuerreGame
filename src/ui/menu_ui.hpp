#ifndef __MENU_UI_H__
#define __MENU_UI_H__

#include <curses.h>

#include <stack>

#include "../input_parser.hpp"
#include "./ncurses_setup.hpp"

enum main_menu_id {
  MAIN = 0,
  LOCAL_RULES = 1,
  CLIENT_CONNECT = 2,
  HOST_RULES = 3,
  CLOSE_GAME = 4,
  HOST_WAIT = 5,
  CLIENT_WAIT = 6,
  PCOUNT = 7,
  DIMS = 8,
  FINISH = 9,
  CLIENT_IP = 10
};

class MenuUI {
 public:
  MenuUI();
  int menu_loop();

 private:
  std::stack<main_menu_id> menu_stack;

  void render_main_menu(int selected = 0);
  void render_client_ip_menu(int selected = -1);
  void settings_menu();
  void main_menu();
  void render_settings_menu(int selected = 0);
  void render_host_wait_menu();
  void dim_menu();
  void pcount_menu();
  void client_connect_menu();
  void setting_input(int menu_height, int& y);
  void playercount_input();
  void board_dims_input();

  void ip_input();
};
#endif  // __MENU_UI_H__