#ifndef __NCURSES_SETUP_H__
#define __NCURSES_SETUP_H__
#include <curses.h>

inline int screen_height{0};
inline int screen_width{0};

void init_ncurses();

#endif  // __NCURSES_SETUP_H__