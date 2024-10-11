#include <cstdlib>
#ifndef __WIN32

#ifndef SIGNNN
#define SIGNNN
#include <execinfo.h>
#include <signal.h>

#include "network/network_helper.hpp"
#include "ui/ncurses_setup.hpp"

inline void sigint_handler(int signum = 0) {
  endwin();
  shutdown_network();
#ifdef DDEBUG
  if (signum == SIGSEGV) {
    printf("\nCaught SEGFAULT\n");
    void *array[10];
    size_t size = backtrace(array, 10);  // Capture the call stack (10 frames)
    char **symbols =
        backtrace_symbols(array, size);  // Translate to readable symbols
    fprintf(stderr, "%s\n", "Call Stack:");
    for (size_t i = 0; i < size; i++) {
      fprintf(stderr, "%s\n", symbols[i]);
    }
    free(symbols);  // Free the memory allocated by backtrace_symbols
  } else {
    printf("\nCaught SIGINT (Ctrl+C). Cleaning up...\n");
  }
#endif
  exit(signum);
}

inline void massert(bool cond) {
  if (!cond) {
    sigint_handler(0);
  }
}
#endif
#endif