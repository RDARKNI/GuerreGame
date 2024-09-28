#ifndef CONFIG_HPP
#define CONFIG_HPP

#define MIN_H 5
#define MIN_W 5
#define MAX_H 32
#define MAX_W 32
#define MIN_PLAYERS 2
#define MAX_PLAYERS 8
#define DEBUGB 1
enum Role { LOCAL, CLIENT, HOST, ROLES };

inline char ip_address[32]{"127.000.000.001"};

inline Role role;
inline const char* player_names[9]{"Gaia",     "Player 1", "Player 2",
                                   "Player 3", "Player 4", "Player 5",
                                   "Player 6", "Player 7", "Player 8"};
inline unsigned int seed;
inline unsigned int goldgeneration;
inline constexpr int forestgen_default{20};
inline constexpr int forestgen_maximum{90};
#define GOLDDIST 0.000  // probability of each square having gold (inaccurate)

#define SEIZURE_REDUCTION 0

#endif
