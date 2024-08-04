#ifndef DTOS
#define DTOS
#include "coords.hpp"

struct UserInput {
  Coord acteur;
  int action;
  int option;
  Coord target;
};

#endif