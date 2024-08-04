#ifndef NETWORKHELPER
#define NETWORKHELPER

#define PORT 8746
#define MAXTRIES 3

/*
Currently broken
*/
#include <array>
#include <cassert>

#include "winix.hpp"

extern int socks[9];
extern int input_buffer[16];

int init_connection();
int shutdown();

template <typename T>
static constexpr size_t size(const T& data) {
  if (std::is_array_v<T>) {
    return std::extent_v<T> * sizeof(std::remove_extent_t<T>);
  }
  return sizeof(data);
}

template <typename T>
int send(T& data, int sock = 0) {
  int tries = MAXTRIES;
  int sentbytes;
  while ((sentbytes = send(sock, (const char*)&data, size(data), 0)) <= 0) {
    if (sentbytes == 0) {
      return 0;
    }
    pterror("Send\n");
    if (tries-- == 0) {
      return -1;
    }
  }
  return sentbytes;
}
template <typename T>
int receive(T& buff, int sock = 0) {
  int tries = MAXTRIES;
  int recvbytes;
  while ((recvbytes = recv(socks[sock], (char*)&buff, size(buff), 0)) <= 0) {
    if (recvbytes == 0) {
      return 1;
    }
    pterror("Recv\n");
    if (tries-- == 0) {
      return -1;
    }
  }
  return 0;
}

template <typename T>
int broadcast(T data, int but = -1) {
  for (int i = 2; i < size(socks); ++i) {
    if (socks[i] < 0 || socks[i] == but) {
      continue;
    }
    int res;
    if ((res = send(data, socks[i]))) {
      socks[i] = -1;
      if (res < 0) {
        // let bot play?
        pterror("Error Host send\n");
      }
      return i;
    }
  }
  return 0;
}

#endif
