#include "network_helper.hpp"

#include <cassert>

#include "../game_config.hpp"
#include "winix.hpp"
/*
returns 0 on success
1 if not received yet
panics if connection was closed
*/
int sockets[9]{-1, -1, -1, -1, -1, -1, -1, -1, -1};
int receive_inputs(int i, UserInput& input, int nonblock) {
  assert(sockets[i] != -1);
  int recvd = recv(sockets[i], &input, sizeof(input), MSG_DONTWAIT * nonblock);
  if (recvd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // No data available at the moment, try again later
      return 1;
    } else {
      // Handle other errors
      perror("recv");
      shutdown();
      exit(1);
    }
  } else if (recvd == 0) {
    // The peer has closed the connection
    printf("Connection closed by peer\n");
    shutdown();
    exit(1);
  }
  return 0;
}
int send_inputs(int& sock, UserInput input) {
  assert(sock != -1);
  int res = send(sock, &input, sizeof(input), 0);
  if (res <= 0) {
    pterror("send failed");
    close(sock);
    sock = -1;
    return 1;
  }
  return 0;
}

int broadcast_inputs(UserInput input, int except) {
  for (int i = 0; i < 9; ++i) {
    if (sockets[i] < 0 || i == except) {
      continue;
    }
    if (send_inputs(sockets[i], input)) {
      shutdown();
      exit(1);
    }
  }
  return 0;
}

int init_connection_host(UserInput settings) {
  startup();  // for windows compatability
  if ((sockets[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    pterror("creating socket");
    shutdown();
  }

  sockaddr_in addr{
      .sin_family = AF_INET, .sin_port = htons(PORT), .sin_addr = {INADDR_ANY}};
  const int opt = 1;
  if (setsockopt(sockets[0], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    pterror("Error socket options");
    shutdown();
  }
  if (bind(sockets[0], reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) ==
      -1) {
    pterror("Error bind");
    shutdown();
  }
  if (listen(sockets[0], settings.dst.y - 1) == -1) {
    pterror("Error listen");
    shutdown();
  }
  // ADSF add unique identifier to dst
  // UserInput game_init_data{{B_H, B_W}, PLAYER_COUNT, 0, {0, 0}};
  for (int i = 2; i < settings.dst.y + 1; ++i) {  // host is always player 1
    sockets[i] = accept(sockets[0], nullptr, nullptr);
    if (sockets[i] == -1) {
      pterror("Error accept");
      --i;
      continue;
    }
    settings.dst.x = i;
    if (send_inputs(sockets[i], settings)) {
      pterror("Error accept");
      --i;
      continue;
    }
  }
  if (close(sockets[0])) {
    pterror("Error close listening socket\n");
    shutdown();
  }
  sockets[0] = -1;
  return 0;
}
int init_connection_client(UserInput& settings) {
  startup();
  if ((sockets[0] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    pterror("Error creating socket");
    shutdown();
  }
  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  // "127.0.0.1"
  if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) <= 0) {
    pterror("Invalid IP address format");
    shutdown();
  }
  if (connect(sockets[0], reinterpret_cast<sockaddr*>(&server_address),
              sizeof(server_address)) == -1) {
    pterror("Connect failed Client");
    shutdown();
  }
  if (receive_inputs(0, settings)) {
    // todo error handling
  }
  return 0;
}

void shutdown() {
  for (size_t i = 0; i < 9; ++i) {
    if (sockets[i] == -1) {
      continue;
    }
    if (close(sockets[i])) {
      pterror("Error Close\n");
      exit(-1);
    }
    sockets[i] = -1;
  }
  WSACleanup();
}