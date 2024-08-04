#ifndef ACTION_ENUMS
#define ACTION_ENUMS

// All <0 are special actions
enum ActionID {
  CREATE_UNIT = -1,
  END_TURN = -2,
  ACTION = 0,
  MOVE = 1,
  ATTACK = 2,
  BUILD_UNIT = 3,
  FARM = 4,
  TO_CASTLE = 5,
  IDLE = 6,
  CONVERT = 7,
  TO_MONK = 8,
  CUT = 9,
  EXPLODE = 10,
  BUILD_TOWER = 11,
  ACTIONCOUNT,
};

enum OptionID {  // todo
  DUMMY = -1,
  CREATE_CHATEAU = 0,
  CREATE_PAYSAN = 1,
  CREATE_SEIGNEUR = 2,
  CREATE_GUERRIER = 3,
  CREATE_FORET = 4,
  CREATE_MINE = 5,
  OPTIONCOUNT = 6,
};
static const char* option_names[OPTIONCOUNT]{"Chateau",  "Paysan", "Seigneur",
                                             "Guerrier", "Foret",  "Mine"};

static constexpr const char* action_names[ACTIONCOUNT] = {
    "",     "Move",    "Attack",        "Build Unit", "Harvest", "Build Castle",
    "Idle", "Convert", "Become a Monk", "Cut",        "Explode", "Build Tower"};

constexpr const OptionID action_options[ACTIONCOUNT][5]{
    {CREATE_CHATEAU, CREATE_PAYSAN, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {CREATE_PAYSAN, CREATE_SEIGNEUR, CREATE_GUERRIER, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY},
    {DUMMY, DUMMY, DUMMY, DUMMY, DUMMY}};

static const bool action_takes_options[ACTIONCOUNT]{
    true, false, false, true, false, false, false, false, false, false, false};

static const bool action_takes_squares[ACTIONCOUNT]{false, true,  true,  true,
                                                    false, false, false, true,
                                                    false, true,  false, true};

#endif