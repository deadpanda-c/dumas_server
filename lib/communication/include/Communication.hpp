/**
 * @file Communication.hpp
 * @brief Socket abstractions for basic TCP server operations.
 */
#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <exception>
#include <iostream>

/** @brief Error message for binding an already-initialized socket. */
#define SOCKET_ALREADY_BOUND "Socket is already connected"
/** @brief Error message when socket creation fails. */
#define SOCKET_CREATION_FAILED "Failed to create socket"
/** @brief Error message when binding the socket fails. */
#define SOCKET_BIND_FAILED "Failed to bind socket"
/** @brief Error message for operations on an unbound socket. */
#define SOCKET_NOT_BOUND "Socket is not bound"
/** @brief Error message when accepting a client connection fails. */
#define SOCKET_ACCEPT_FAILED "Failed to accept connection"
/** @brief Error message when socket initialization fails. */
#define SOCKET_INITIALIZATION_FAILED "Failed to initialize socket"
/** @brief Error message when listening on the socket fails. */
#define SOCKET_LISTEN_FAILED "Failed to listen on socket"

namespace Communication {
  /**
   * @brief TCP server socket wrapper.
   *
   * Creates and binds a socket to an address and accepts incoming connections.
   * The socket is closed on destruction if it is open.
   */
  class Socket {
    public:
      /**
       * @brief Exception type thrown by socket operations.
       */
      class SocketException : public std::exception {
        public:
          /**
           * @brief Construct a new SocketException.
           * @param message Error message describing the failure.
           */
          SocketException(const std::string& message) : message(message) {}
          /**
           * @brief Return the error message.
           * @return Null-terminated error message string.
           */
          const char* what() const noexcept override {
            return message.c_str();
          }
        private:
          std::string message;
      };

      /**
       * @brief Construct a new Socket with no underlying descriptor.
       */
      Socket();
      /**
       * @brief Destroy the socket and close the descriptor if open.
       */
      ~Socket() {
        if (_sockfd != -1) {
          close(_sockfd);
        }
      }

      int init(const std::string& ip, unsigned short port);

      void run();

      /**
       * @brief Create and bind the socket to the given address.
       * @param ip IPv4 address to bind to.
       * @param port Port to bind to.
       * @return 0 on success.
       * @throws SocketException on socket creation or bind failure.
       */
      int _bind(const std::string& ip, unsigned short port);
      /**
       * @brief Accept an incoming connection.
       * @return File descriptor for the accepted client socket.
       * @throws SocketException if the socket is not bound or accept fails.
       */
      int _accept();

    private:
      int _sockfd;
  };
}
