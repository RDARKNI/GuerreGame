#ifndef ERROR_ENUMS
#define ERROR_ENUMS
// not used anymore, the controller should make sure that no errors occur
enum ErrorID {
  EX_OUT_OF_BOUNDS = 1,
  EX_SQUARE_OCCUPIED = 2,
  EX_UNIT_TYPE = 3,
  EX_WRONG_ACTION = 4,
  EX_WRONG_OPTION = 5,
  EX_NO_ACTOR = 6,
  EX_WRONG_PLAYER = 7,
};

#endif
