#ifndef NETWORKHELPER
#define NETWORKHELPER

#define PORT 8746
#define MAXTRIES 3
#include "../helpers/user_input.hpp"

// 1-8 for players, socket 0 reserved for connection
int init_connection_host(UserInput settings);
int init_connection_client(UserInput& settings);
int receive_inputs(int i, UserInput& input, int nonblock = 0);
int send_inputs(int& sock, UserInput input);
int broadcast_inputs(UserInput input, int except = -1);

void shutdown();

#endif
