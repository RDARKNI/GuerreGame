#ifndef ACTION_HPP
#define ACTION_HPP

#include <queue>
#include <unordered_set>
#include <vector>

#include "../../helpers/action_data.hpp"
#include "../../helpers/coords.hpp"
#include "../board.hpp"
#include "../pion.hpp"
#include "../player.hpp"

class Action {
 public:
  inline static Board* board;
  virtual ~Action() = default;
  constexpr virtual ActionID get_type() const = 0;
  constexpr bool takes_options() const {
    return action_takes_options(get_type());
  }
  constexpr bool takes_squares() const {
    return action_takes_squares[get_type()];
  }
  virtual bool get_fatigue_condition(Coord source) const {
    return !board->operator[](source).get_fatigue();
  }
  virtual void perform(Coord source, int opt, Coord dst) const = 0;
  virtual std::vector<Coord> get_poss_options(Coord source) const { return {}; }

  virtual const std::vector<Coord> get_poss_squares(Coord source) const {
    return {};
  };
};

class Attack final : public Action {
 public:
  constexpr ActionID get_type() const override { return ATTACK; }
  void perform(Coord source, int, Coord dst) const override {
    Player& target_player{(*board)[dst].get_player()};
    (*board)[dst].change_pv(-(*board)[source].get_puiss());
    if (!(*board)[dst]) {
      target_player.remove_piece(dst);
    }
    (*board)[source].change_fatigue(2);
  }
  bool get_fatigue_condition(Coord source) const override {
    return (*board)[source].get_fatigue() < 2;
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    const Pion& actor = (*board)[source];
    const Player& player = actor.get_player();
    for (const Coord square :
         board->get_neighbours(source, actor.get_atkran())) {
      const Pion& occ = (*board)[square];
      if (occ && occ.get_player().get_colour() && occ.get_player() != player) {
        squares.push_back(square);
      }
    }
    return squares;
  }
};

class BuildTower final : public Action {
 public:
  constexpr ActionID get_type() const override { return BUILD_TOWER; }
  void perform(Coord source, int, Coord dst) const override {
    Player& player = (*board)[source].get_player();
    (*board)[dst] = Pion(PieceType::Tour, player, dst);
    player.add_piece(dst);
    player.change_gold(-pieces_data[static_cast<size_t>(PieceType::Tour)].cout);
  }

  bool get_fatigue_condition(Coord source) const override {
    const Pion& actor = (*board)[source];
    return !actor.get_fatigue() &&
           actor.get_player().get_gold() >=
               pieces_data[static_cast<size_t>(PieceType::Tour)].cout;
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    for (const Coord square : board->get_neighbours(source)) {
      if (!(*board)[square]) {
        squares.push_back(square);
      }
    }
    return squares;
  }
};

class BuildUnit final : public Action {
 public:
  constexpr ActionID get_type() const override { return BUILD_UNIT; }

  void perform(Coord source, int opt, Coord dst) const override {
    Player& player = (*board)[source].get_player();
    PieceType type;
    switch (opt) {
      case 0:
        type = PieceType::Paysan;
        break;
      case 1:
        type = PieceType::Seigneur;
        break;
      case 2:
        type = PieceType::Guerrier;
        break;
    }
    (*board)[dst] = Pion(type, player, dst);
    player.add_piece(dst);
    player.change_gold(-pieces_data[static_cast<size_t>(type)].cout);
    (*board)[source].change_fatigue(+2);
  }

  std::vector<Coord> get_poss_options(Coord source) const override {
    std::vector<Coord> opts = {
        {CREATE_PAYSAN, 0}, {CREATE_SEIGNEUR, 0}, {CREATE_GUERRIER, 0}};
    int gold = (*board)[source].get_player().get_gold();
    if (gold >= pieces_data[static_cast<size_t>(PieceType::Paysan)].cout) {
      opts[0].x = 1;
    }
    if (gold >= pieces_data[static_cast<size_t>(PieceType::Seigneur)].cout) {
      opts[1].x = 1;
    }
    if (gold >= pieces_data[static_cast<size_t>(PieceType::Guerrier)].cout) {
      opts[2].x = 1;
    }
    return opts;
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    for (const Coord square : board->get_neighbours(source)) {
      if (!(*board)[square]) {
        squares.push_back(square);
      }
    }
    return squares;
  }
};

class Convert final : public Action {
 public:
  constexpr ActionID get_type() const override { return CONVERT; }

  void perform(Coord source, int, Coord dst) const override {
    Player& source_player{(*board)[source].get_player()};
    Player& target_player{(*board)[dst].get_player()};
    (*board)[dst].set_player((*board)[source].get_player());
    target_player.remove_piece(dst);
    source_player.add_piece(dst);
    (*board)[source].change_fatigue(+2);
    (*board)[dst].change_fatigue(+2);
  }

  bool get_fatigue_condition(Coord source) const override {
    return (*board)[source].get_fatigue() < 2;
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    const Pion& actor = (*board)[source];
    const Player& player = actor.get_player();
    for (const Coord square :
         board->get_neighbours(source, actor.get_atkran())) {
      const Pion& occ = (*board)[square];
      if (occ && occ.get_player().get_colour() && occ.get_player() != player) {
        squares.push_back(square);
      }
    }
    return squares;
  }
};

class Cut final : public Action {
 public:
  constexpr ActionID get_type() const override { return CUT; }
  void perform(Coord source, int, Coord dst) const override {
    Pion& actor = (*board)[source];
    int harvest = (*board)[dst].get_prod();
    Player& target_player = (*board)[dst].get_player();
    (*board)[dst].change_pv(-actor.get_puiss());
    actor.get_player().change_gold(+harvest);
    actor.change_fatigue(+2);
    if (!(*board)[dst]) {
      actor.get_player().change_gold(+harvest * 2);
      target_player.remove_piece(dst);
    }
  }
  bool get_fatigue_condition(Coord source) const override {
    return (*board)[source].get_fatigue() < 2;
  }
  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    for (const Coord square : board->get_neighbours(source)) {
      const Pion& actee{(*board)[square]};
      if (actee && !actee.get_player().get_colour()) {
        squares.push_back(square);
      }
    }
    return squares;
  }
};

class DoNothing final : public Action {
 public:
  constexpr ActionID get_type() const override { return IDLE; }

  void perform(Coord source, int, Coord) const override {
    (*board)[source].change_pv(+2);  // healing effect
    (*board)[source].change_fatigue(+2);
  }
};

class Explode final : public Action {
 public:
  ActionID get_type() const override { return EXPLODE; }
  void perform(Coord source, int, Coord) const override {
    Pion& actor = (*board)[source];
    for (Coord sq : board->get_neighbours(source)) {
      if ((*board)[sq]) {
        Player& target_player{(*board)[sq].get_player()};
        (*board)[sq].change_pv(-actor.get_puiss());
        if (!(*board)[sq]) {
          target_player.remove_piece(sq);
        }
      }
    }
    actor.change_pv(-100);
  }
};

class Harvest final : public Action {
 public:
  constexpr ActionID get_type() const override { return FARM; }
  void perform(Coord source, int, Coord) const override {
    Pion& actor = (*board)[source];
    actor.get_player().change_gold(actor.get_prod());
    actor.change_fatigue(2);
  }
};

template <>
struct std::hash<Coord> {
  std::size_t operator()(const Coord& c) const {
    return (c.y << max_coord_bits) + c.x;
  }
};

class Move final : public Action {
 public:
  constexpr ActionID get_type() const override { return MOVE; }

  void perform(Coord source, int, Coord dst) const override {
    Player& player = (*board)[source].get_player();
    (*board)[dst] = (*board)[source];
    player.add_piece(dst);
    (*board)[source] = Pion();
    player.remove_piece(source);
    (*board)[dst].change_fatigue(+1);
  }

  const std::vector<Coord> get_poss_squares(Coord source) const override {
    std::vector<Coord> squares;
    const Player& player = (*board)[source].get_player();
    std::queue<Coord> visited{{source}};  // squares we can move through
    std::unordered_set<Coord> bord{source};
    int mov = (*board)[source].get_depl();
    while (mov) {
      for (size_t i{visited.size()}; i; --i) {
        for (const Coord sq : board->get_neighbours(visited.front())) {
          if (bord.count(sq)) {
            continue;
          }
          bord.insert(sq);
          const Pion& piece{(*board)[sq]};
          if (!piece) {
            squares.push_back(sq);
            visited.push(sq);
          } else if (player == piece.get_player()) {
            visited.push(sq);
          }
        }
        visited.pop();
      }
      --mov;
    }
    return squares;
  }
};

class ToCastle final : public Action {
 public:
  constexpr ActionID get_type() const override { return TO_CASTLE; }

  void perform(Coord source, int, Coord) const override {
    Player& player = (*board)[source].get_player();
    (*board)[source] = Pion(PieceType::Chateau, player, source);
    player.change_gold(
        -pieces_data[static_cast<size_t>(PieceType::Chateau)].cout);
  }
  bool get_fatigue_condition(Coord source) const override {
    const Pion& actor = (*board)[source];
    return !actor.get_fatigue() &&
           actor.get_player().get_gold() >=
               pieces_data[static_cast<size_t>(PieceType::Chateau)].cout;
  }
};

class ToMonk final : public Action {
 public:
  constexpr ActionID get_type() const override { return TO_MONK; }

  bool get_fatigue_condition(Coord source) const override {
    const Pion& actor = (*board)[source];
    return !actor.get_fatigue() &&
           actor.get_player().get_gold() >=
               pieces_data[static_cast<size_t>(PieceType::Moine)].cout;
  }
  void perform(Coord source, int, Coord) const override {
    Player& player = (*board)[source].get_player();
    (*board)[source] = Pion(PieceType::Moine, player, source);
    player.change_gold(
        -pieces_data[static_cast<size_t>(PieceType::Moine)].cout);
  }
};

#endif
