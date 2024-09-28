// #include "cursewindow.hpp"
//
// SubWindow::SubWindow(const DataHandler& dh, Coord size, Coord position)
//     : dh{dh}, win{subwin(stdscr, size.y, size.x, position.y, position.x)} {}
//
// SubWindow::~SubWindow() { hide(); }
// WINDOW*& SubWindow::get_win() { return win; }
// void SubWindow::hide() {
//   wclear(win);
//   wrefresh(win);
// }
//
// BoardWindow::BoardWindow(const DataHandler& dh, Coord position)
//     : SubWindow{dh, {B_H + 2, B_W + 2}, position} {
//   wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
// }
// void BoardWindow::move_cursor(Coord coords) {
//   curs_set(2);
//   wmove(win, coords.y + 1, coords.x + 1);
//   wrefresh(win);
// }
//
// void BoardWindow::show() {
//   for (int i = 0; i < B_H; ++i) {
//     for (int j = 0; j < B_W; ++j) {
//       redraw_square({i, j});
//     }
//   }
// }
// void BoardWindow::redraw_square(Coord coords) {
//   mvwaddch(win, coords.y + 1, coords.x + 1, dh.get_sqdat(coords));
//   wrefresh(win);
// }
//
// void BoardWindow::mark_squares() {
//   unmark_squares();
//   marked_squares = dh.get_curr_targets();
//   for (auto sq : marked_squares) {
//     chtype ch = mvwinch(win, sq.y + 1, sq.x + 1);
//     chtype pnum = COLOR_PAIR(PAIR_NUMBER(ch) + 16);
//     mvwaddch(win, sq.y + 1, sq.x + 1, (ch & A_CHARTEXT) | pnum | (A_DIM &
//     ch));
//   }
//   wrefresh(win);
// }
// void BoardWindow::unmark_squares() {
//   for (auto sq : marked_squares) {
//     mvwaddch(win, sq.y + 1, sq.x + 1, dh.get_sqdat(sq));
//   }
//   marked_squares.clear();
//   wrefresh(win);
// }
//
// InfoWindow::InfoWindow(const DataHandler& dh, Coord position,
//                        const Coord& square)
//     : SubWindow{dh, {8, 20}, position}, square{square} {}
//
// void InfoWindow::show() {
//   werase(win);
//   curs_set(0);
//   SquareData data = dh.get_piece_data(square);
//   if (data.data) {  // todo
//     const PieceConstants& pdata =
//         piece_constants[static_cast<size_t>(data.type())];
//     wattron(win, COLOR_PAIR(data.player()));
//     mvwaddstr(win, 0, 1, pdata.name);
//     wattroff(win, COLOR_PAIR(data.player()));
//     mvwprintw(win, 1, 1, "HP    %d/%d", data.hp(), pdata.max_hp);
//     mvwprintw(win, 2, 1, "FAT   %d", data.fatigue());
//     mvwprintw(win, 3, 1, "DEPL  %d", pdata.depl);
//     mvwprintw(win, 4, 1, "PUIS  %d", pdata.puiss);
//     mvwprintw(win, 5, 1, "RANGE %d", pdata.atk_ran);
//     mvwprintw(win, 6, 1, "PROD  %d", pdata.prod);
//   }
//   wrefresh(win);
// };
//
// PlayerWindow::PlayerWindow(const DataHandler& dh, Coord position)
//     : SubWindow{dh, {3, 20}, position} {}
// void PlayerWindow::show() {
//   curs_set(0);
//   werase(win);
//   PlayerData pdata = dh.get_current_player_data();
//   wattron(win, COLOR_PAIR(pdata.index));
//   mvwaddstr(win, 0, 1, player_names[pdata.index]);
//   wattroff(win, COLOR_PAIR(pdata.index));
//   mvwprintw(win, 1, 1, "%d Gold", pdata.gold);
//   wrefresh(win);
// };
//
// TurnWindow::TurnWindow(const DataHandler& dh, Coord position)
//     : SubWindow{dh, {1, 15}, position} {}
// void TurnWindow::show() {
//   werase(win);
//   mvwprintw(win, 0, 1, "Turn %3d", dh.get_current_turn());
//   wrefresh(win);
// }
//
// MenuWindow::MenuWindow(const DataHandler& dh, Coord position,
//                        const char* const* strings)
//     : SubWindow{dh, {5, 20}, position}, strings{strings} {}
//
// void MenuWindow::move_cursor(int pos) {
//   wmove(win, pos, 1);
//   render(pos);
// }
// void MenuWindow::show() { render(); }
//
// void MenuWindow::render(int sel) {
//   werase(win);
//   curs_set(0);
//   std::vector<Dat> data = get_data();
//   int size = data.size();
//   for (int i = 0; i < size; ++i) {
//     int selval = (i == sel) * A_STANDOUT;
//     wattron(win, (!data[i].value * A_DIM) | selval);
//     mvwaddstr(win, i, 1, strings[data[i].type]);
//     wattroff(win, (!data[i].value * A_DIM) | selval);
//   }
//   wrefresh(win);
// }
