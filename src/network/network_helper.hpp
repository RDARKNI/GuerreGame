#ifndef NETWORKHELPER
#define NETWORKHELPER

#define PORT 8746
#define MAXTRIES 3
#include <array>

#include "../helpers/user_input.hpp"

inline std::array<int, 9> sockets{-1, -1, -1, -1, -1, -1, -1, -1, -1};

// 1-8 for players, socket 0 reserved for connection
int init_connection_host(UserInput settings);
int init_connection_client(UserInput &settings);
size_t receive_inputs(int i, UserInput &input, int nonblock = 0);
size_t send_inputs(int &sock, UserInput input);
int broadcast_inputs(UserInput input, int except = -1);
int close_all_sockets();
void shutdown_network();

#endif
