#ifndef __NCURSES_SETUP_H__
#define __NCURSES_SETUP_H__

#include <curses.h>

#define COLOR_GREY 8
#define COLOR_ORANGE 9

extern int screen_height;
extern int screen_width;
extern const short player_colours[9];

void init_ncurses();

#endif  // __NCURSES_SETUP_H__