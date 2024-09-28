// #include "data_handler.hpp"
//
// DataHandler::DataHandler(UserInput init_settings) : game{{init_settings}} {}
//
// SquareData DataHandler::get_square_data(Coord coords) const {
//   const Pion* const occ = board[coords];
//   if (!occ) {
//     return SquareData{0};
//   }
//   return SquareData{
//       static_cast<int>(occ->get_type()) +
//       (occ->get_player().get_colour() << 4) + (occ->get_fatigue() << 8) +
//       ((!occ->get_player().get_is_their_turn() || occ->get_fatigue()) << 12)
//       + (occ->get_pv() << 13)};
// }
// std::vector<SquareData> DataHandler::get_squares_data(
//     const std::vector<Coord>& coords) const {
//   std::vector<SquareData> data;
//   data.reserve(coords.size());
//   for (auto coord : coords) {
//     data.push_back(get_square_data(coord));
//   }
//   return data;
// }
// const Coord& DataHandler::get_curr_action(int act) const {
//   // assert(act<actions_data.size());
//   return actions_data[act];
// }
// const std::vector<Coord>& DataHandler::get_curr_actions() const {
//   return actions_data;
// }
// const std::vector<Coord>& DataHandler::get_curr_options() const {
//   return curr_options;
// }
// const std::vector<Coord>& DataHandler::get_curr_targets() const {
//   return curr_targets;
// }
// void DataHandler::select_source(Coord source) {
//   curr_source = get_square_data(source);
//   actions_data.clear();
//   options_data.clear();
//   targets_data.clear();
//   const Pion* const occptr = board[source];
//   if (!occptr || !(occptr->get_player().get_is_their_turn())) {
//     return;
//   }
//   const Pion& occ = *occptr;
//   const auto& ac = occ.get_actions();
//   size_t ac_size{ac.size()};
//   for (size_t i = 0; i < ac_size && ac[i] != ACTION; ++i) {
//     Action& act = *(game.actions[ac[i]]);
//     actions_data.emplace_back(act.get_type(),
//                               act.get_fatigue_condition(source));
//     options_data.emplace_back();
//     targets_data.emplace_back();
//     if (actions_data.back().x && act.takes_options()) {
//       std::vector<OptionID> possible_options = act.get_poss_options(source);
//       if (possible_options.size() == 0) {
//         actions_data[i].x = 0;
//         continue;
//       }
//       for (OptionID opt : action_options[act.get_type()]) {
//         if (opt < 0) {
//           break;
//         }
//         options_data[i].push_back({opt, 0});
//         for (OptionID possopt : possible_options) {
//           if (possopt == opt) {
//             options_data[i].back().x = 1;
//           }
//         }
//       }
//     }
//     if (actions_data.back().x && act.takes_squares()) {
//       targets_data.back() = act.get_poss_squares(source);
//       if (targets_data.back().empty()) {
//         actions_data.back().x = false;
//       }
//     }
//   }
// }
//
// void DataHandler::select_action(int act) {
//   curr_options = options_data[act];
//   curr_targets = targets_data[act];
// }
// void DataHandler::select_target(Coord target) {
//   curr_target = get_square_data(target);
// }
//
// const int& DataHandler::get_curr_turn() const { return curr_turn; }
//
// const Coord& DataHandler::get_curr_player_data() const {
//   return curr_player_data;
// }
//
// Coord DataHandler::get_next_piece_coords(Coord src) const {
//   bool start_found = false;
//   for (const auto& piece : game.get_current_player().get_pieces()) {
//     if (piece->get_coords() == src) {
//       start_found = true;
//     } else if (!piece->get_fatigue() && start_found) {
//       return piece->get_coords();
//     }
//   }
//   for (const auto& piece : game.get_current_player().get_pieces()) {
//     if (!piece->get_fatigue()) {
//       return piece->get_coords();
//     }
//   }
//   return src;
// }
// Coord DataHandler::get_next_target_coords(Coord dst) const {
//   bool start_found = false;
//   for (const auto& sq : curr_targets) {
//     if (sq == dst) {
//       start_found = true;
//     } else if (start_found) {
//       return sq;
//     }
//   }
//   return curr_targets[0];
// }
// void DataHandler::start_selection() {
//   std::vector<Coord> new_options = {{Coord{1, 1}, Coord{2, 2}}};
//   const auto& pieces{game.get_current_player().get_pieces()};
//   if (pieces.empty()) {
//     curr_options = {{CREATE_CHATEAU, 1}, {CREATE_PAYSAN, 1}};
//   } else if (pieces.size() == 2) {
//     curr_options = {};
//   } else if (pieces[0]->get_type() == PieceType::Chateau) {
//     curr_options = {{CREATE_CHATEAU, 0}, {CREATE_PAYSAN, 1}};
//   } else {
//     curr_options = {{CREATE_CHATEAU, 1}, {CREATE_PAYSAN, 0}};
//   }
// }
