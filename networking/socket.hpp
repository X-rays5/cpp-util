//
// Created by X-ray on 2/15/2022.
//

#pragma once

#ifndef LIBSOCK_SOCKET_HPP
#define LIBSOCK_SOCKET_HPP

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#define WINDOWS
#elif defined(__unix__) || defined(__unix) || defined(unix) || defined(__APPLE__) || defined(__MACH__) || defined(__gnu_linux__) || defined(__linux__) || defined(linux) || defined(__linux)
#define UNIX
#endif

#include <stdexcept>
#include <string>
#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#elif defined(UNIX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#else
#error "Unsupported platform"
#endif

namespace libsock {
#if defined(WINDOWS)
    typedef SOCKET socket_t;
    typedef SOCKET& socket_ref;
    typedef int size_res_t;
#elif defined(UNIX)
    typedef int socket_t;
    typedef int socket_ref;
    typedef ssize_t size_res_t;
#endif

  inline void init() {
#if defined(WINDOWS)
    WSAData data;
    int res = WSAStartup(MAKEWORD(2,2), &data);
    if (res != 0) {
      throw std::runtime_error("init failed with error: "+std::to_string(res));
    }
#endif
  }

  inline void deinit() {
#if defined(WINDOWS)
    WSACleanup();
#endif
  }

  inline int getaddrinfo(const char* host, const char* service, const addrinfo* hints, addrinfo** res) {
    return ::getaddrinfo(host, service, hints, res);
  }

  inline void freeaddrinfo(addrinfo* res) {
    ::freeaddrinfo(res);
  }

  inline socket_t socket(int domain, int type, int protocol) {
    return ::socket(domain, type, protocol);
  }

  inline int bind(socket_ref socket, const sockaddr* addr, socklen_t addrlen) {
    return ::bind(socket, addr, addrlen);
  }

  inline int listen(socket_ref socket, int backlog) {
    return ::listen(socket, backlog);
  }

  inline socket_t accept(socket_ref socket, sockaddr* addr, socklen_t* addrlen) {
    return ::accept(socket, addr, addrlen);
  }

  inline int connect(socket_ref socket, const sockaddr* addr, socklen_t addrlen) {
    return ::connect(socket, addr, addrlen);
  }

  inline int shutdown(socket_ref socket, int how) {
    return ::shutdown(socket, how);
  }

  inline socket_t close(socket_ref socket) {
#if defined(WINDOWS)
    return ::closesocket(socket);
#elif defined(UNIX)
    return ::close(socket);
#endif
  }

  inline int getpeername(socket_ref socket, sockaddr* addr, socklen_t* addrlen) {
    return ::getpeername(socket, addr, addrlen);
  }

  inline int getsockname(socket_ref socket, sockaddr* addr, socklen_t* addrlen) {
    return ::getsockname(socket, addr, addrlen);
  }

  inline int getsockopt(socket_ref socket, int level, int optname, void* optval, socklen_t* optlen) {
    return ::getsockopt(socket, level, optname, static_cast<char*>(optval), optlen);
  }

  inline int setsockopt(socket_ref socket, int level, int optname, const void* optval, socklen_t optlen) {
    return ::setsockopt(socket, level, optname, static_cast<const char*>(optval), optlen);
  }

  inline size_res_t recv(socket_ref socket, void* buf, size_t len, int flags) {
    return ::recv(socket, static_cast<char*>(buf), len, flags);
  }

  inline size_res_t recvfrom(socket_ref socket, void* buf, size_t len, int flags, sockaddr* src_addr, socklen_t* addrlen) {
    return ::recvfrom(socket, static_cast<char*>(buf), len, flags, src_addr, addrlen);
  }

  inline size_res_t send(socket_ref socket, const void* buf, size_t len, int flags) {
    return ::send(socket, static_cast<const char*>(buf), len, flags);
  }

  inline size_res_t sendto(socket_ref socket, const void* buf, size_t len, int flags, const sockaddr* dest_addr, socklen_t addrlen) {
    return ::sendto(socket, static_cast<const char*>(buf), len, flags, dest_addr, addrlen);
  }

  inline int ioctl(socket_ref socket, int request, void* argp) {
#if defined(WINDOWS)
    return ::ioctlsocket(socket, request, static_cast<u_long*>(argp));
#elif defined(UNIX)
    return ::ioctl(socket, request, argp);
#endif
  }

  namespace util {
    size_t available(socket_ref socket) {
      size_t len{0};
      ioctl(socket, FIONREAD, &len);
      return len;
    }
  }
}
#endif //LIBSOCK_SOCKET_HPP
