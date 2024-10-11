#include "network_helper.hpp"

#include <cassert>

#include "../game_config.hpp"
#include "winix.hpp"
/*
returns 0 on success
1 if not received yet
panics if connection was closed
*/

size_t receive_inputs(int i, UserInput &input, int nonblock) {
label:
#ifndef _WIN32
  size_t recvd =
      recv(sockets[i], &input, sizeof(input), MSG_DONTWAIT * nonblock);
#else
  int recvd = 0;  // todo
#endif
  if (recvd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    } else if (errno == EINTR) {
      printf("Signal Interrupt\n");
      goto label;
    } else if (errno == ECONNRESET) {
      printf("Connection reset by peer\n");
      shutdown_network();
      return -2;
    } else {
      perror("recv");
      shutdown_network();
      return -3;
    }
  } else if (recvd == 0) {
    printf("Connection closed by peer\n");
    return -1;
  }
  return recvd;
}
size_t send_inputs(int &sock, UserInput input) {
  assert(sock != -1);
  size_t res = send(sock, (const char *)&input, sizeof(input), 0);
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
      shutdown_network();
      return 1;
    }
  }
  return 0;
}

int init_connection_host(UserInput settings) {
  startup();  // for windows compatability
  sockets[0] = socket(AF_INET, SOCK_STREAM, 0);
  if (sockets[0] == -1) {
    pterror("creating socket");
    shutdown_network();
  }
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr = {INADDR_ANY};
  const int opt = 1;
  if (setsockopt(sockets[0], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    pterror("Error socket options");
    shutdown_network();
  }
  if (bind(sockets[0], reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) ==
      -1) {
    pterror("Error bind");
    shutdown_network();
  }
  if (listen(sockets[0], settings.dst.y - 1) == -1) {
    pterror("Error listen");
    shutdown_network();
  }
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
    shutdown_network();
  }
  sockets[0] = -1;
  return 0;
}
int init_connection_client(UserInput &settings) {
  startup();
  sockets[0] = socket(AF_INET, SOCK_STREAM, 0);
  if (sockets[0] < 0) {
    pterror("Error creating socket");
    shutdown_network();
  }
  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  // "127.0.0.1"
  if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) <= 0) {
    pterror("Invalid IP address format");
    shutdown_network();
  }
  if (connect(sockets[0], reinterpret_cast<sockaddr *>(&server_address),
              sizeof(server_address)) == -1) {
    pterror("Connect failed Client");
    shutdown_network();
  }
  if (receive_inputs(0, settings)) {
    // todo error handling
  }
  return 0;
}

int close_all_sockets() {
  for (int socket : sockets) {
    if (socket == -1) {
      continue;
    }
    if (close(socket)) {
      pterror("Error Close\n");
      return 1;
    }
    socket = -1;
  }
  return 0;
}
void shutdown_network() {
  close_all_sockets();
  WSACleanup();
}