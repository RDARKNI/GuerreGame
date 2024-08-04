#include "network_helper.hpp"

#include <curses.h>

#include <cassert>

#include "../game_config.hpp"
int socks[9]{-1, -1, -1, -1, -1, -1, -1, -1, -1};
int input_buffer[16]{0};

int init_connection() {
  if (role == HOST) {
    startup();  // for windows compatability
    if ((socks[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      pterror("creating socket");
      shutdown();
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
#ifdef _WIN32
    const char opt = 1;
#else
    const int opt = 1;
#endif

    if (setsockopt(socks[0], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
        -1) {
      pterror("Error socket options");
      shutdown();
    }
    if (bind(socks[0], (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      pterror("Error bind");
      shutdown();
    }
    if (listen(socks[0], PLAYER_COUNT - 1) == -1) {
      pterror("Error listen");
      shutdown();
    }
    size_t send_data[2]{PLAYER_COUNT, 0};
    for (size_t i = 2; i < PLAYER_COUNT + 1; ++i) {
      if ((socks[i] = accept(socks[0], nullptr, nullptr)) == -1) {
        endwin();
        pterror("Error accept");
        shutdown();
        i--;
        continue;
      }
      send_data[1] = i;
      if (send(send_data, socks[i]) <= 0) {
        assert(0);
        close(socks[i]);
        socks[i--] = -1;  // something went wrong, find new connection for slot
      }
    }
    if (close(socks[0])) {
      pterror("Error close listening socket\n");
      shutdown();
    }
    socks[0] = -1;
  } else if (role == CLIENT) {
    endwin();  // todo debug
    startup();
    if ((socks[0] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
    int tries = MAXTRIES;
    while (connect(socks[0], (sockaddr*)&server_address,
                   sizeof(server_address)) == -1) {
      if (tries-- == 0) {
        pterror("Connect failed Client");
        shutdown();
      }
    }
    shutdown();
    tries = MAXTRIES;
    size_t confirm_data[2]{0};
    int res;
    while ((res = recv(socks[0], (char*)&confirm_data, size(confirm_data), 0) <=
                  0)) {
      if (res == 0) {
        printf("Connection terminated by server\n");
        shutdown();
      }
      if (tries-- == 0) {
        pterror("Receive failed");
        shutdown();
      }
    }
    PLAYER_INDEX = confirm_data[0];
    PLAYER_COUNT = confirm_data[1];
  }
  return 0;
}

int shutdown() {
  for (int i = 0; i < size(socks); ++i) {
    if (socks[i] == -1) {
      continue;
    }
    // if (close(socks[i])) {
    //   pterror("Error Close\n");
    // }
    socks[i] = -1;
  }
  cleanup();
  exit(-1);
}