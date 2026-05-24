#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <exception>
#include <iostream>

#define SOCKET_ALREADY_BOUND "Socket is already connected"
#define SOCKET_CREATION_FAILED "Failed to create socket"
#define SOCKET_BIND_FAILED "Failed to bind socket"
#define SOCKET_NOT_BOUND "Socket is not bound"
#define SOCKET_ACCEPT_FAILED "Failed to accept connection"

namespace Communication {
  class Socket {
    public:
      class SocketException : public std::exception {
        public:
          SocketException(const std::string& message) : message(message) {}
          const char* what() const noexcept override {
            return message.c_str();
          }
        private:
          std::string message;
      };

      Socket();
      ~Socket() {
        if (_sockfd != -1) {
          close(_sockfd);
        }
      }

      int bind(const std::string& ip, unsigned short port);
      int accept();

    private:
      int _sockfd;
  };
}

