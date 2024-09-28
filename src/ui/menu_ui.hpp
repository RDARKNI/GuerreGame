#ifndef __MENU_UI_H__
#define __MENU_UI_H__
#define arr_size(arr) (sizeof(arr) / sizeof(arr[0]))

#include <array>
#include <stack>
#include <vector>

#include "../helpers/user_input.hpp"
#include "cursewindow.hpp"

enum class main_menu_id {
  MAIN,
  SETTINGS,
  CLIENT_CONNECT,
  CLOSE_GAME,
  FINISH,
};

inline std::array<const char*, 4> main_options{"Local", "Client", "Host",
                                               "Quit"};
inline std::array<const char*, 5> setting_strings{
    "Player Count", "Board Size", "Forestation", "Load Savefile", "Confirm"};

/*
For settings window:


*/
class MenuUI {
 public:
  MenuUI();
  std::vector<UserInput> menu_loop();

 private:
  std::stack<main_menu_id> menu_stack;
  std::vector<UserInput> history{};
  UserInput settings = {{14, 14}, forestgen_default, -1, {2, 0}};
  MenuWindow<void, main_options.size()> mainwindow{
      {18, 14}, {5, 5}, main_options};
  MenuWindow<UserInput, setting_strings.size()> settingswindow{
      {5, 45}, {5, 5}, setting_strings, settings};
  /*
  51
  */
  IPWindow ipwindow{{4, 40}, {5, 5}};

  void settings_menu();
  void main_menu();
  void dim_menu();
  void pcount_menu();
  void client_connect_menu();
  void forestationmenu();
  void savefilemenu();
  void board_dims_input();
  void ip_input();
};
#endif  // __MENU_UI_H__