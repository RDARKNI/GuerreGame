#ifndef CONFIG_HPP
#define CONFIG_HPP

#define MIN_H 5
#define MIN_W 5
#define MAX_H 30
#define MAX_W 30
#define MIN_PLAYERS 2
#define MAX_PLAYERS 8
#define DEBUGB
enum Role { LOCAL = -1, HOST = 0, CLIENT = 1 };

extern char ip_address[64];
extern size_t PLAYER_INDEX;
extern size_t PLAYER_COUNT;
extern size_t B_H;
extern size_t B_W;
extern Role role;
extern const char* player_names[9];
#define FORESTATION 0.1  // in %, probability of each square being forrested
#define GOLDDIST 0.000   // probability of each square having gold (inaccurate)

#define SEIZURE_REDUCTION 0

#endif
