#ifndef __MENU_UI_H__
#define __MENU_UI_H__
#include <array>
#include <cassert>
#include <stack>
#include <vector>

#include "../controller/controller.hpp"
#include "../helpers/user_input.hpp"
#include "cursewindow.hpp"

enum class main_menu_id {
  main_menu,
  setting_menu,
  client_connect,
  close_game,
  finish,
};

inline std::array<const char*, 4> main_options{"Local", "Client", "Host", "Quit"};

inline std::array<const char*, 5> setting_strings{"Player Count", "Board Size", "Forestation",
                                                  "Load Savefile", "Confirm"};

static inline constexpr std::array<const char*, 3> ip_strings{"Host IP Address", "Confirm", "Back"};

class MenuUI {
 public:
  MenuUI();
  std::vector<UserInput> menu_loop();

 private:
  std::stack<main_menu_id> menu_stack;
  std::vector<UserInput> history{};
  SettingsInput settings = {{14, 14}, forestgen_default, -1, {2, 0}};

  MainMenuWindow mainwindow{{18, 14}, {yoffs, xoffs}, main_options, nullptr};
  SettingsWindow settingswindow{{18, 45}, {yoffs, xoffs}, setting_strings, &settings};
  IPWindow ipwindow{{18, 40}, {yoffs, xoffs}, ip_strings, &ip_address};

  void main_menu();
  void settings_menu();
  void dim_menu(Controller& con);
  void forestationmenu(Controller& con);
  void savefilemenu(Controller& con);
  void pcount_menu();

  void client_connect_menu();
  void ip_input();
};
#endif  // __MENU_UI_H__