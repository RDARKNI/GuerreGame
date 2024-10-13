#ifndef CONFIG_HPP
#define CONFIG_HPP

inline constexpr int min_h{5};
inline constexpr int max_h{32};
inline constexpr int min_w{5};
inline constexpr int max_w{32};
inline constexpr int min_players{2};
inline constexpr int max_players{8};
inline constexpr int forestgen_default{20};
inline constexpr int forestgen_maximum{90};

inline unsigned int seed;
inline unsigned int goldgeneration;

enum class Role { local, client, host, preview };

inline char ip_address[32]{"127.000.000.001"};

inline Role role;
inline const char* player_names[9]{"Gaia",     "Player 1", "Player 2", "Player 3", "Player 4",
                                   "Player 5", "Player 6", "Player 7", "Player 8"};

#define GOLDDIST 0.000  // probability of each square having gold (inaccurate)

#define SEIZURE_REDUCTION 0

#endif
