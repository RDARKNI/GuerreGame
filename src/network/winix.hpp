#ifndef WINIX
#define WINIX
/*
Simple wrapper for platform-compatibility with windows
*/
#include <cstdio>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

// macro remappings
#ifdef _WIN32
#define SHUT_RD SD_RECEIVE
#define SHUT_RDWR SD_SEND
#define SHUT_WR SD_BOTH

#define EAGAIN WSAEWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINTR WSAEINTR
#define ECONNRESET WSAECONNRESET

#else
#define SOCKET unsigned int
#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_RDWR
#define SD_BOTH SHUT_WR
#endif

#ifdef _WIN32
inline int mysockopt(int a, int b, int c, const int* d, socklen_t e) {
  const char opt = 0 + *d;  // ENDIANNESS FIX
  return setsockopt(a, b, c, &opt, sizeof(opt));
}
#define setsockopt(a, b, c, d, e)                         \
  e == sizeof(int) ? mysockopt(a, b, c, (const int*)d, e) \
                   : setsockopt(a, b, c, (const char*)d, (int)e)
#else
inline int mysockopt(int a, int b, int c, const char* d,
                     [[maybe_unused]] socklen_t e) {
  const int opt = *d;  // will only be called for const char(e==1)
  return setsockopt(a, b, c, &opt, sizeof(opt));
}
#define setsockopt(a, b, c, d, e) \
  e == 1 ? mysockopt(a, b, c, (const char*)(d), e) : setsockopt(a, b, c, d, e)
#endif

inline int startup() {
#ifdef _WIN32
  WSADATA wsaData;
  return WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
  return 0;
#endif
}
#ifndef _WIN32
inline int WSACleanup() { return 0; }
#endif

#ifdef _WIN32
#define close(socket) closesocket(socket)
// static int close(SOCKET socket) { return closesocket(socket); }
#else
#define closesocket(socket) close(socket)
// static int closesocket(int socket) { return close(socket); }
#endif

#ifdef _WIN32
#define LAST_ERR WSAGetLastError()
inline void pterror(const char* s) {
  DWORD error = WSAGetLastError();  // Use GetLastError() for Winsock errors

  // Print the provided string followed by a colon and space
  if (s != NULL) {
    fprintf(stderr, "%s: ", s);
  }

  // Print the error message corresponding to the error code
  LPSTR messageBuffer = NULL;
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR)&messageBuffer, 0, NULL);

  // Print the error message to stderr
  if (messageBuffer != NULL) {
    fprintf(stderr, "%s\n", messageBuffer);
    LocalFree(messageBuffer);  // Free the buffer allocated by FormatMessage()
  } else {
    fprintf(stderr, "Unknown error (code %lu)\n", error);
  }
}
#else
#define LAST_ERR errno
inline void pterror(const char* s) { perror(s); }
#endif

#endif