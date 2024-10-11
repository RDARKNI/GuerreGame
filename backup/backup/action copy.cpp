
// #include "action.hpp"
//
// #include <queue>
// #include <unordered_set>
//
// inline bool can_afford(const Player& player, PieceType type) {
//   return player.get_gold() >= pieces_data[to_index(type)].cout;
// }
//
// ActionStatus Action::helper_create_new_unit(Coord src, PieceType type,
//                                             Coord dst) {
//   Pion& actor{board[src]};
//   Pion& target{board[dst]};
//   unsigned char player_index{actor.get_player_index()};
//   Player& player{game.get_player(player_index)};
//   target = Pion(type, player_index);
//   if (src != dst) {
//     player.add_piece(dst);
//   }
//   player.change_gold(-pieces_data[to_index(type)].cout);
//   actor.change_fatigue(+2);
//   return ActionStatus::created_unit;
// }
//
// std::vector<Coord> Action::helper_get_free_neighbours(Coord src) const {
//   return board.get_neighbours(src, 1,
//                               [](const Pion& target) { return !target; });
// }
// std::vector<Coord> Action::helper_get_foe_neighbours(Coord src) const {
//   const Pion& actor{board[src]};
//   unsigned char index{board[src].get_player_index()};
//   auto is_foe = [index](const Pion& target) {
//     unsigned char targ_index{target.get_player_index()};
//     return targ_index && targ_index != index;
//   };
//   return board.get_neighbours(src, actor.get_atkran(), is_foe);
// }
// bool Action::helper_can_afford(const Pion& actor, PieceType type) const {
//   const Player& player{game.get_player(actor.get_player_index())};
//   return !actor.get_fatigue() && can_afford(player, PieceType::Moine);
// }
//
// bool ResearchUpgrade::get_fatigue_condition(const Pion& actor) const {
//   return !actor.get_fatigue() && !get_owner(actor).has_research(0);
// }
// std::vector<Coord> ResearchUpgrade::get_opts(const Pion& actor) const {
//   return {{to_index(OptionID::res_fortress),
//            static_cast<coordval>(get_owner(actor).get_gold() >= 15 ? 1 :
//            0)}};
// }
// ActionStatus ResearchUpgrade::perform(const UserInput& in) {
//   Pion& actor{board[in.src]};
//   Player& player{get_owner(actor)};
//   if (in.opt == 0) {
//     player.change_gold(-15);
//     player.add_research(0);
//     actor.change_fatigue(+2);
//   }
//   return ActionStatus::ok;
// }
//
// template <>
// struct std::hash<Coord> {
//   std::size_t operator()(const Coord& c) const {
//     return (c.y << max_coord_bits) + c.x;
//   }
// };
// std::vector<Coord> Move::get_dsts(Coord source) const {
//   const Pion& actor{board[source]};
//   unsigned char index{actor.get_player_index()};
//   auto mov{actor.get_depl()};
//   auto not_foe = [index](const Pion& target) {
//     return !target || target.get_player_index() == index;
//   };
//   std::vector<Coord> squares;
//   std::queue<Coord> visited{{source}};  // squares we can move through
//   std::unordered_set<Coord> bord{source};
//   while (mov) {
//     for (size_t i{visited.size()}; i; --i) {
//       for (Coord sq : board.get_neighbours(visited.front(), 1, not_foe)) {
//         if (bord.count(sq)) {
//           continue;
//         }
//         bord.insert(sq);
//         visited.push(sq);
//         const Pion& target{board[sq]};
//         if (!target) {
//           squares.push_back(sq);
//         }
//       }
//       visited.pop();
//     }
//     --mov;
//   }
//   return squares;
// }
// ActionStatus Move::perform(const UserInput& in) {
//   Pion& actor{board[in.src]};
//   Player& player{get_owner(actor)};
//   Pion& target{board[in.dst]};
//   player.remove_piece(in.src);
//   player.add_piece(in.dst);
//   std::swap(actor, target);
//   target.change_fatigue(+1);
//   return ActionStatus::ok;
// }
// bool ToMonk::get_fatigue_condition(const Pion& actor) const {
//   return helper_can_afford(actor, PieceType::Moine);
// }
// ActionStatus ToMonk::perform(const UserInput& in) {
//   return helper_create_new_unit(in.src, PieceType::Moine, in.src);
// }
// bool ToCastle::get_fatigue_condition(const Pion& actor) const {
//   return helper_can_afford(actor, PieceType::Chateau);
// }
// ActionStatus ToCastle::perform(const UserInput& in) {
//   return helper_create_new_unit(in.src, PieceType::Chateau, in.src);
// }
// ActionStatus Harvest::perform(const UserInput& in) {
//   Pion& actor{board[in.src]};
//   Player& player{get_owner(actor)};
//   player.change_gold(+actor.get_prod());
//   actor.change_fatigue(+2);
//   return ActionStatus::ok;
// }
// ActionStatus Idle::perform(const UserInput& in) {
//   Pion& actor{board[in.src]};
//   actor.change_pv(+2);
//   actor.change_fatigue(+2);
//   return ActionStatus::ok;
// }
// std::vector<Coord> Convert::get_dsts(Coord source) const {
//   return helper_get_foe_neighbours(source);
// }
// bool Convert::get_fatigue_condition(const Pion& actor) const {
//   return actor.get_fatigue() < 2;
// }
// ActionStatus Convert::perform(const UserInput& in) {
//   Pion& actor{board[in.src]};
//   Player& player{get_owner(actor)};
//   Pion& target{board[in.dst]};
//   Player& target_player{get_owner(target)};
//   target.set_player(player.get_colour());
//   target_player.remove_piece(in.dst);
//   player.add_piece(in.dst);
//   actor.change_fatigue(+4);
//   target.change_fatigue(+2);
//   if (target.get_type() == PieceType::Chateau) {
//     if (game.kill_player_if(target_player.get_colour())) {
//       return ActionStatus::defeated_player;
//     }
//   }
//   return ActionStatus::ok;
// }
// ActionStatus Attack::perform(const UserInput& in) {
//   Pion& actor{board[in.src]};
//   Pion& target{board[in.dst]};
//   Player& target_player{get_owner(target)};
//   target.change_pv(-actor.get_puiss());
//   fprintf(stderr, "%d,%d\n", actor.get_type(), actor.get_puiss());
//   actor.change_fatigue(+2);
//   if (!target.get_pv()) {
//     if (target.get_type() == PieceType::Bombardier) {
//       return Explode{game, board}.perform({in.dst});
//     }
//     target_player.remove_piece(in.dst);
//     PieceType type{target.get_type()};
//     target = Pion();
//     if (type == PieceType::Chateau) {
//       if (game.kill_player_if(target_player.get_colour())) {
//         return ActionStatus::defeated_player;
//       }
//     }
//   }
//   return ActionStatus::ok;
// }
// bool Attack::get_fatigue_condition(const Pion& actor) const {
//   const Player& player{get_owner(actor)};
//   if (actor.get_type() == PieceType::Chateau && !player.has_research(0)) {
//     return false;
//   }
//   return actor.get_fatigue() < 2;
// }
// std::vector<Coord> Attack::get_dsts(Coord source) const {
//   return helper_get_foe_neighbours(source);
// }
// ActionStatus Cut::perform(const UserInput& in) {
//   Pion& actor{board[in.src]};
//   Player& player{get_owner(actor)};
//   Pion& target{board[in.dst]};
//   target.change_pv(-actor.get_puiss());
//   player.change_gold(+target.get_prod());
//   if (!target.get_pv()) {
//     player.change_gold(+target.get_prod());
//     game.get_player(0).remove_piece(in.dst);
//     target = Pion();
//   }
//   actor.change_fatigue(+2);
//   return ActionStatus::ok;
// }
// std::vector<Coord> Cut::get_dsts(Coord source) const {
//   constexpr auto is_res = [](const Pion& target) {
//     PieceType type{target.get_type()};
//     return type == PieceType::Arbre || type == PieceType::Mine;
//   };
//   return board.get_neighbours(source, board[source].get_atkran(), is_res);
// }
// bool Cut::get_fatigue_condition(const Pion& actor) const {
//   return actor.get_fatigue() < 2;
// }
// ActionStatus Explode::perform(const UserInput& in) {
//   Pion& actor{board[in.src]};
//   int puiss{actor.get_puiss()};
//   Player& player{get_owner(actor)};
//   ActionStatus state{ActionStatus::ok};
//   for (Coord square : board.get_neighbours(in.src, actor.get_atkran())) {
//     Pion& target{board[square]};
//     if (target) {
//       Player& target_player{get_owner(target)};
//       PieceType target_type{target.get_type()};
//       target.change_pv(-puiss);
//       if (!target.get_pv()) {
//         target = Pion();
//         target_player.remove_piece(square);
//         if (target_type == PieceType::Chateau) {
//           game.kill_player_if(target_player.get_colour());
//         }
//       }
//     }
//   }
//   if (player.get_status() == PlayerStatus::alive) {
//     actor = Pion();
//     player.remove_piece(in.src);
//   }
//   return ActionStatus::defeated_player;
// }
// ActionStatus BuildBombardier::perform(const UserInput& in) {
//   return helper_create_new_unit(in.src, PieceType::Bombardier, in.dst);
// }
// std::vector<Coord> BuildBombardier::get_dsts(Coord source) const {
//   return helper_get_free_neighbours(source);
// }
// bool BuildBombardier::get_fatigue_condition(const Pion& actor) const {
//   return helper_can_afford(actor, PieceType::Bombardier);
// }
// ActionStatus BuildTower::perform(const UserInput& in) {
//   return helper_create_new_unit(in.src, PieceType::Tour, in.dst);
// }
// std::vector<Coord> BuildTower::get_dsts(Coord source) const {
//   return helper_get_free_neighbours(source);
// }
// bool BuildTower::get_fatigue_condition(const Pion& actor) const {
//   return helper_can_afford(actor, PieceType::Tour);
// }
// ActionStatus BuildUnit::perform(const UserInput& in) {
//   using enum PieceType;
//   PieceType type;
//   switch (in.opt) {
//     case OptionID::create_paysan:
//       type = Paysan;
//       break;
//     case OptionID::create_seigneur:
//       type = Seigneur;
//       break;
//     case OptionID::create_guerrier:
//       type = Guerrier;
//       break;
//     case OptionID::create_archer:
//       type = Archer;
//       break;
//     default:
//       std::unreachable();
//   }
//   return helper_create_new_unit(in.src, type, in.dst);
// }
// std::vector<Coord> BuildUnit::get_dsts(Coord source) const {
//   return helper_get_free_neighbours(source);
// }
// std::vector<Coord> BuildUnit::get_opts(const Pion& actor) const {
//   using enum OptionID;
//   int gold{game.get_player(actor.get_player_index()).get_gold()};
//   std::vector<Coord> opts = {
//       {to_index(create_paysan),
//        gold >= pieces_data[to_index(PieceType::Paysan)].cout},
//       {to_index(create_seigneur),
//        gold >= pieces_data[to_index(PieceType::Seigneur)].cout},
//       {to_index(create_guerrier),
//        gold >= pieces_data[to_index(PieceType::Guerrier)].cout},
//       {to_index(create_archer),
//        gold >= pieces_data[to_index(PieceType::Archer)].cout}};
//   return opts;
// }
// ActionStatus CreateUnit::perform(const UserInput& in) {
//   PieceType type;
//   switch (in.opt) {
//     case OptionID::create_arbre:
//       type = PieceType::Arbre;
//       break;
//     case OptionID::create_mine:
//       type = PieceType::Mine;
//       break;
//     case OptionID::create_chateau:
//       type = PieceType::Chateau;
//       break;
//     case OptionID::create_paysan:
//       type = PieceType::Paysan;
//       break;
//     default:
//       // assert(0);
//   }
//   Player& player{game.get_player(in.src.y)};
//   Pion& target{board[in.dst]};
//   target = Pion(type, player.get_colour());
//   player.add_piece(in.dst);
//   if (player.get_status() == PlayerStatus::alive) {
//     game.next_player();
//     if (player.get_colour() == game.get_initial_player_count() &&
//         player.get_pieces().size() == 2) {
//       game.next_turn();
//       return ActionStatus::new_turn;
//     }
//   }
//   return ActionStatus::created_unit;
// }
// bool CreateUnit::get_fatigue_condition(const Pion&) const {
//   return game.get_turn() < 1;
// }
// ActionStatus EndTurn::perform(const UserInput&) {
//   game.next_player();
//   game.next_turn();
//   return ActionStatus::new_turn;
// }
// Player& Action::get_owner(const Pion& piece) {
//   return game.get_player(piece.get_player_index());
// }
// const Player& Action::get_owner(const Pion& piece) const {
//   return game.get_player(piece.get_player_index());
// }
