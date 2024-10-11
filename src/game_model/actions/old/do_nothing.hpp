// #ifndef DO_NOTHING_ACTION_HPP
// #define DO_NOTHING_ACTION_HPP
//
// #include "../../helpers/action_data.hpp"
// #include "action.hpp"
//
// class DoNothing final : public Action {
//  public:
//   ActionID get_type() const override { return IDLE; }
//
//   void perform(Coord source, int, Coord) const override {
//     (*board)[source]->change_pv(+2);  // healing effect
//     (*board)[source]->change_fatigue(+2);
//   }
// };
//
// #endif